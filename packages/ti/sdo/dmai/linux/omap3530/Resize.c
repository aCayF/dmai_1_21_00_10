/* --COPYRIGHT--,BSD
 * Copyright (c) 2009, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#include <ti/sdo/dmai/Dmai.h>
#include <ti/sdo/dmai/Buffer.h>
#include <ti/sdo/dmai/Resize.h>

#include <linux/omap_resizer.h>
#include <linux/videodev.h>

/*
 * If PRECALC is set, a lookup table will be used for calculating sinus
 * and cosinus values instead of calling the math library. If you change this
 * define, note that you will have to set the define in
 * linux/dm6446/Resize_precalc.c accordingly.
 */
#define PRECALC

#ifdef PRECALC
#define _PI 3.14159265358979323846
extern Float Resize_sinFloat[];

/******************************************************************************
 * _sin
 ******************************************************************************/
static inline Float _sin(Float x)
{
    Int d;

    /* Scale and round up for positive values and down for negative values */
    d = x < 0 ? ((Float) x * 256 / _PI - 0.5) : ((Float) x * 256 / _PI + 0.5);

    /* Make "degrees" positive by adding 2 * pi */
    while (d < 0) d += 512;

    /* Cut "degrees" at 2 * pi */
    d &= 0x1ff;

    return Resize_sinFloat[d];
}

/******************************************************************************
 * _cos
 ******************************************************************************/
static inline Float _cos(Float x)
{
    Int d;

    /* Scale and round up for positive values and down for negative values */
    d = x < 0 ? ((Float) x * 256 / _PI - 0.5) : ((Float) x * 256 / _PI + 0.5);

    /* Make "degrees" positive by adding 2 * pi */
    while (d < 0) d += 512;

    /* Cut "degrees" at 2 * pi */
    d &= 0x1ff;

    /* Cosinus is "90 degrees" = 128 phase shifted from the sinus value */
    return Resize_sinFloat[d + 128];
}

/******************************************************************************
 * _abs
 ******************************************************************************/
static inline Int _abs(Int x)
{
    return x < 0 ? -x : x;
}

#else
#include <math.h>
#define _PI M_PI
#define _cos(x) cos(x)
#define _sin(x) sin(x)
#define _abs(x) abs(x)
#endif

#define MODULE_NAME     "Resize"

#define RESIZER_DEVICE  "/dev/omap-resizer"
#define NUM_COEFS       32

typedef struct Resize_Object {
    Int                 fd;
    Resize_WindowType   hWindowType;
    Resize_WindowType   vWindowType;
    Resize_FilterType   hFilterType;
    Resize_FilterType   vFilterType;
    Int32               inSize;
    Int32               outSize;
} Resize_Object;

const Resize_Attrs Resize_Attrs_DEFAULT = {
    Resize_FilterType_LOWPASS,
    Resize_FilterType_LOWPASS,
    Resize_WindowType_BLACKMAN,
    Resize_WindowType_BLACKMAN,
    0xe
};

static short copyCoeffs[NUM_COEFS] = {
    256, 0, 0, 0,
    256, 0, 0, 0,
    256, 0, 0, 0,
    256, 0, 0, 0,
    256, 0, 0, 0,
    256, 0, 0, 0,
    256, 0, 0, 0,
    256, 0, 0, 0
};


static short upsamplingCoeffs[NUM_COEFS] = {
    0, 256, 0, 0,
    -6, 246, 16, 0,
    -7, 219, 44, 0,
    -5, 179, 83, -1,
    -3, 131, 131, -3,
    -1, 83, 179, -5,
    0, 44, 219, -7,
    0, 16, 246, -6
};

/******************************************************************************
 * bicubicCore
 ******************************************************************************/
static Float bicubicCore(Float s)
{
    Float   w = s < 0 ? -s : s;
    Float   u;

    if (w >= 0 && w <= 1) {
        u = 1.5 * w * w * w - 2.5 * w * w + 1;
    }
    else if (w <= 2) {
        u = -0.5 * w * w * w + 2.5 * w * w - 4 * w + 2;
    }
    else {
        u = 0;
    }
    return u;

}

/******************************************************************************
 * calcCoeffs
 ******************************************************************************/
static void calcCoeffs(Int rsz, Resize_WindowType window,
                       Resize_FilterType filter, __u16 *coeffs)
{
    Int   i, j, w;
    Float t, gain;
    Float alpha, fc;
    Float win[NUM_COEFS+1];
    Float iCoeffs[NUM_COEFS+1];
    Float dCoeffs[NUM_COEFS+1];
    Int   phaseOffset;
    Int   nphases, ntaps;
    Int   max, max2, idx, idx2;
    Int   totalGain, delta;

    if (rsz == 256) {
        for (i=0; i<NUM_COEFS; i++) {
            coeffs[i] = copyCoeffs[i];
        }
        return;
    }

    if (rsz > 512) {
        ntaps = 7;
        nphases = 4;
        phaseOffset = 8;
    }
    else {
        ntaps = 4;
        nphases = 8;
        phaseOffset = 4;
    }

    /* Window length = 32 or 28 (8 phases x 4 taps or 4 phases x 7 taps) */
    w = nphases * ntaps;

    /* Calculating the cut-off frequency, normalized by fs/2 */
    fc = (rsz < 256) ? (1.0 / nphases) : 256.0 / (rsz * nphases);

    switch (window) {
        case Resize_WindowType_HANN:
            for (i = 0; i <= w; i++) {
                win[i] = 0.5 - 0.5 * _cos(2 * _PI * i / w);
            }
            break;
        case Resize_WindowType_TRIANGULAR:
            for (i = 0; i <= w; i++) {
                win[i] = i < (w / 2) ? i : (w - i);
            }
            break;
        case Resize_WindowType_RECTANGULAR:
            for (i = 0; i <= w; i++) {
                win[i] = i;
            }
            break;
        case Resize_WindowType_BLACKMAN:
            for (i = 0; i <= w; i++) {
                win[i] = 0.42 - 0.5 * _cos(2 * _PI * i / w) +
                                0.08 * _cos(4 * _PI * i / w);
            }
            break;
    }

    switch (filter) {
        case Resize_FilterType_LOWPASS:
            /* Optimize upscaling as it always produces the same coeffs */
            if (rsz < 256) {
                for (i=0; i<NUM_COEFS; i++) {
                    coeffs[i] = upsamplingCoeffs[i];
                }
                return;
            }

            /* Calculating the sinc function coefficients */
            for (i = 0; i <= w; i++) {
                t = ((Float) i - w / 2);

                iCoeffs[i] = (i == w / 2) ? win[i] :
                                win[i] * _sin(_PI * fc * t) / (_PI * fc * t);
            }
            /* de-interleave into phases */
            for (i = 0; i < nphases; i++) {
                for (j = 0; j < ntaps; j++) {
                    dCoeffs[phaseOffset * i + j] =
                        iCoeffs[(nphases - i) + nphases * j];
                }
            }

            break;

        case Resize_FilterType_BICUBIC:
            /* always 8 phases and 4 taps */
            nphases = 8;
            ntaps = 4;
            phaseOffset = 4;

            /* calculating the bi-cubic coefficients */
            for (i = 0; i < nphases; i++) {
                alpha = ((Float) i) / nphases;

                dCoeffs[phaseOffset * i] = bicubicCore(alpha + 1);
                dCoeffs[phaseOffset * i + 1] = bicubicCore(alpha);
                dCoeffs[phaseOffset * i + 2] = bicubicCore(1 - alpha);
                dCoeffs[phaseOffset * i + 3] = bicubicCore(2 - alpha);
            }
            break;

        case Resize_FilterType_BILINEAR:
            nphases = 8;
            ntaps = 4;
            phaseOffset = 4;
            /* calculating the bi-linear coefficients */
            for (i = 0; i < nphases; i++) { /* always 8 phases and 4 taps */
                alpha = ((Float) i) / nphases;

                dCoeffs[phaseOffset * i] = 1 - alpha;
                dCoeffs[phaseOffset * i + 1] = alpha;
                dCoeffs[phaseOffset * i + 2] = 0;
                dCoeffs[phaseOffset * i + 3] = 0;
            }
            break;
    }
    /* Normalize for each phase */
    for (i = 0; i < nphases; i++) {
        gain = 0;

        for (j = 0; j < ntaps; j++) {
            gain += dCoeffs[phaseOffset * i + j];
        }

        for (j = 0; j < ntaps; j++) {
            coeffs[phaseOffset * i + j] =
                (short) (dCoeffs[phaseOffset * i + j] * 256 / gain + 0.5);
        }

        if (ntaps == 7) {
            coeffs[phaseOffset * i + 7] = 0;
        }
    }

    /*
     * Sum up each phase and adjust largest coef to comprehend rounding errors
     * in previous pass.
     */
    for (i = 0; i < nphases; i++) {
        max = totalGain = coeffs[phaseOffset * i];
        max2 = idx = idx2 = 0;

        for (j = 1; j < ntaps; j++) {
            totalGain += coeffs[phaseOffset * i + j];

            if (_abs(coeffs[phaseOffset * i + j]) >= max) {
                max2 = max;
                idx2 = idx;
                idx = j;
                max = _abs(coeffs[phaseOffset * i + j]);
            }
        }

        delta = 256 - totalGain;

        if (max - max2 < 10) {
            coeffs[phaseOffset * i + idx2] += (delta >> 1);
            coeffs[phaseOffset * i + idx] += (delta - (delta >> 1));
        }
        else {
            coeffs[phaseOffset * i + idx] += delta;
        }
    }
}
/******************************************************************************
 * Resize_create
 ******************************************************************************/
Resize_Handle Resize_create(Resize_Attrs *attrs)
{
    Resize_Handle hResize;

    hResize = calloc(1, sizeof(Resize_Object));

    if (hResize == NULL) {
        Dmai_err0("Failed to allocate space for Resize Object\n");
        return NULL;
    }

    /* Open resizer device */
    hResize->fd = open(RESIZER_DEVICE, O_RDWR);

    if (hResize->fd == -1) {
        Dmai_err1("Failed to open %s\n", RESIZER_DEVICE);
        Resize_delete(hResize);
        return NULL;
    }

    hResize->hWindowType = attrs->hWindowType;
    hResize->vWindowType = attrs->vWindowType;
    hResize->hFilterType = attrs->hFilterType;
    hResize->vFilterType = attrs->vFilterType;

    return hResize;
}

/******************************************************************************
 * estimateRsz
 ******************************************************************************/
static Int estimateRsz(Int src, Int dst)
{
    Int rsz;

    /* Estimate rsz */
    rsz = ((src - 7) * 256 - 16) / (dst - 1);

    /* Recalculate rsz if 8-phase 4-tap */
    if (rsz <= 512) {
        rsz = ((src - 4) * 256 - 16) / (dst - 1);

        if (rsz > 512) {
            rsz = 512;
        }
    }

    return rsz;
}

/******************************************************************************
 * calcSrcSize
 ******************************************************************************/
static Int calcSrcSize(Int dst, Int rsz)
{
    Int rszTemp;
    Int src, srcOrig;

    /* Estimate src */
    if (rsz <= 512) {
        src = (((dst - 1) * rsz + 16) >> 8) + 4;
    }
    else {
        src = (((dst - 1) * rsz + 32) >> 8) + 7;
    }

    srcOrig = src;

    /* Fine tune src */
    while (1) {
        rszTemp = estimateRsz(src, dst);

        if (rszTemp < rsz) {
            if (src < srcOrig) break;
            src++;
        }
        else if (rszTemp > rsz) {
            if (src > srcOrig) break;
            src--;
        }
        else {
            break;
        }
    }

    return src;
}
/******************************************************************************
 * Resize_config
 ******************************************************************************/
Int Resize_config(Resize_Handle hResize,
                  Buffer_Handle hSrcBuf, Buffer_Handle hDstBuf)
{
    BufferGfx_Dimensions        srcDim;
    BufferGfx_Dimensions        dstDim;
    struct rsz_params           params;
    struct v4l2_requestbuffers  reqbuf;
    Int                         hrsz, vrsz;
    Int                         width, height;
    Int                         rszRate;

    BufferGfx_getDimensions(hSrcBuf, &srcDim);
    BufferGfx_getDimensions(hDstBuf, &dstDim);

    Dmai_dbg4("Configuring resizer job resizing %dx%d to %dx%d\n",
              srcDim.width, srcDim.height, dstDim.width, dstDim.height);

    memset (&params, 0, sizeof(struct rsz_params));

    /* Source and destination pitch must be 32 bytes aligned */
    if (srcDim.lineLength % 32 || dstDim.lineLength % 32) {
        Dmai_err2("Src (%ld) and dst (%ld) must be aligned on 32 bytes\n",
                  srcDim.lineLength, dstDim.lineLength);
        return Dmai_EINVAL;
    }

    /* Estimate the hrsz value */
    hrsz = srcDim.width == dstDim.width ? 256 :
        estimateRsz(srcDim.width, dstDim.width);

   /* Estimate the vrsz value */
    vrsz = srcDim.height == dstDim.height ? 256 :
        estimateRsz(srcDim.height, dstDim.height);

    width = calcSrcSize(dstDim.width, hrsz);
    height = calcSrcSize(dstDim.height, vrsz);

    /* Set up the resizer job */
    params.in_hsize            = srcDim.width;
    params.in_vsize            = srcDim.height;
    params.in_pitch            = srcDim.lineLength;
    params.cbilin              = 0;
    params.pix_fmt             = RSZ_PIX_FMT_UYVY;
    params.out_hsize           = dstDim.width;
    params.out_vsize           = dstDim.height;
    params.out_pitch           = dstDim.lineLength;
    params.vert_starting_pixel = 0;
    params.horz_starting_pixel = 0;
    params.inptyp              = RSZ_INTYPE_YCBCR422_16BIT;
    params.hstph               = 0;
    params.vstph               = 0;
    params.yenh_params.type    = 0;
    params.yenh_params.gain    = 0;
    params.yenh_params.slop    = 0;
    params.yenh_params.core    = 0;

    calcCoeffs(hrsz, hResize->hWindowType, hResize->hFilterType,
               params.tap4filt_coeffs);
    calcCoeffs(vrsz, hResize->vWindowType, hResize->vFilterType,
               params.tap7filt_coeffs);

    if (ioctl(hResize->fd, RSZ_S_PARAM, &params) == -1) {
        Dmai_err0("Resize setting parameters failed.\n");
        return Dmai_EFAIL;
    }

    rszRate = 0;
    if (ioctl(hResize->fd, RSZ_S_EXP, &rszRate) == -1) {
            Dmai_err0("Resize setting read cycle failed.\n");
            return Dmai_EFAIL;
    }

    hResize->inSize = srcDim.lineLength * srcDim.height;
    hResize->outSize = dstDim.lineLength * dstDim.height;

    /* Request the resizer buffers */
    reqbuf.type     = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    reqbuf.memory   = V4L2_MEMORY_USERPTR;
    reqbuf.count    = 2;

    if (ioctl(hResize->fd, RSZ_REQBUF, &reqbuf) == -1) {
        Dmai_err0("Request buffer failed.\n");
        return Dmai_EFAIL;
    }

    return Dmai_EOK;
}
/******************************************************************************
 * Resize_execute
 ******************************************************************************/
Int Resize_execute(Resize_Handle hResize,
                   Buffer_Handle hSrcBuf, Buffer_Handle hDstBuf)
{
    Int                         i;
    struct v4l2_buffer          qbuf[2];
    UInt32       srcOffset, dstOffset;
    BufferGfx_Dimensions srcDim, dstDim;

    assert(hResize);
    assert(hSrcBuf);
    assert(hDstBuf);

    BufferGfx_getDimensions(hSrcBuf, &srcDim);
    BufferGfx_getDimensions(hDstBuf, &dstDim);

    /* Because resulting pointers must be a multiple of 32 bytes */
    assert((srcDim.x & 0xF) == 0);
    assert((dstDim.x & 0xF) == 0);

    srcOffset = srcDim.y * srcDim.lineLength + srcDim.x * 2;
    dstOffset = dstDim.y * dstDim.lineLength + dstDim.x * 2;

    /* Input and output buffers must be 4096 bytes aligned */
    assert(((Buffer_getPhysicalPtr(hDstBuf) + srcOffset) & 0xFFF) == 0);
    assert(((Buffer_getPhysicalPtr(hSrcBuf) + dstOffset) & 0xFFF) == 0);

    /* Queue the resizer buffers */
    for (i=0; i < 2; i++) { 

        qbuf[i].type         = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        qbuf[i].memory       = V4L2_MEMORY_USERPTR;
        qbuf[i].index        = i;

        if (ioctl (hResize->fd, RSZ_QUERYBUF, &qbuf[i]) == -1) {
            Dmai_err1("Failed to query buffer index %d\n", i);
            return Dmai_EFAIL;
        }

        if (i == 0) {
            qbuf[i].m.userptr = (unsigned long) Buffer_getUserPtr(hSrcBuf) + 
                                            srcOffset;
        }
        else {
            qbuf[i].m.userptr = (unsigned long) Buffer_getUserPtr(hDstBuf) +
                                              dstOffset;
        }

        if (ioctl (hResize->fd, RSZ_QUEUEBUF, &qbuf[i]) == -1) {
            Dmai_err1("Failed to queue buffer index %d\n",i);
            return Dmai_EFAIL;
        }
    }

    if (ioctl(hResize->fd, RSZ_RESIZE, NULL) == -1) {
        Dmai_err0("Failed to execute resize job\n");
        return Dmai_EFAIL;
    }

    Buffer_setNumBytesUsed(hDstBuf, hResize->outSize);

    return Dmai_EOK;
}

/******************************************************************************
 * Resize_delete
 ******************************************************************************/
Int Resize_delete(Resize_Handle hResize)
{
    if (hResize) {
        if (hResize->fd != -1) {
            close(hResize->fd);
        }
    }

    free(hResize);

    return Dmai_EOK;
}
