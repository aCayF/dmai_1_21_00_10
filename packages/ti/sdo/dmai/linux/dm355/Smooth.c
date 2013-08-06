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

#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>

#include <asm/arch/dm355_ipipe.h>

#include <ti/sdo/dmai/Dmai.h>
#include <ti/sdo/dmai/BufferGfx.h>
#include <ti/sdo/dmai/Smooth.h>

#define MODULE_NAME    "Smooth"
#define IPIPE_DEVICE   "/dev/dm355_ipipe"
#define HALF_HEIGHT(x) (x >> 1)
#define TWO_LINES(x)   (x << 1)

typedef struct Smooth_Object {
    int   fd;
    int   channel;
} Smooth_Object;

const Smooth_Attrs Smooth_Attrs_DEFAULT = {
    0xe,
};

extern Bool            Dmai_DM355_ipipeChannelInUse[2];
extern pthread_mutex_t Dmai_DM355_ipipeChannelLock;

/******************************************************************************
 * Smooth_create
 ******************************************************************************/
Smooth_Handle Smooth_create(Smooth_Attrs *attrs)
{
    Smooth_Handle hSmooth;

    assert(attrs);

    hSmooth = calloc(1, sizeof(Smooth_Object));

    if (hSmooth == NULL) {
        Dmai_err0("Failed to allocate memory space for handle.\n");
        return NULL;
    }

    /* Open resizer device */
    hSmooth->fd = open(IPIPE_DEVICE, O_RDWR, 0);

    if (hSmooth->fd == -1) {
        Dmai_err1("Failed to open %s\n", IPIPE_DEVICE);
        free(hSmooth);
        return NULL;
    }

    /* Set the channel number to -1 until one has been assigned */
    hSmooth->channel = -1;

    return hSmooth;
}

/******************************************************************************
 * Smooth_config
 ******************************************************************************/
Int Smooth_config(Smooth_Handle hSmooth,
                  Buffer_Handle hSrcBuf, Buffer_Handle hDstBuf)
{
    BufferGfx_Dimensions srcDim;
    BufferGfx_Dimensions dstDim;
    UInt                 hDif;
    UInt                 vDif;
    struct ipipe_params  params;

    /* Make sure our input parameters are valid */
    if (!hSmooth) {
        Dmai_err0("Smooth_Handle parameter must not be NULL\n");
        return Dmai_EINVAL;
    }

    if (!hSrcBuf) {
        Dmai_err0("Source buffer parameter must not be NULL\n");
        return Dmai_EINVAL;
    }

    if (!hDstBuf) {
        Dmai_err0("Destination buffer parameter must not be NULL\n");
        return Dmai_EINVAL;
    }

    /* Check for valid buffer dimensions */
    BufferGfx_getDimensions(hSrcBuf, &srcDim);
    BufferGfx_getDimensions(hDstBuf, &dstDim);

    if (dstDim.width <= 0) {
        Dmai_err0("Destination buffer width must be greater than zero\n");
        return Dmai_EINVAL;
    }

    if (dstDim.height <= 0) {
        Dmai_err0("Destination buffer height must be greater than zero\n");
        return Dmai_EINVAL;
    }

    if ((srcDim.lineLength & 0x1F) != 0) {
        Dmai_err0("Source buffer pitch must be a multiple of 32 bytes\n");
        return Dmai_EINVAL;
    }

    if ((dstDim.lineLength & 0x1F) != 0) {
        Dmai_err0("Destination buffer pitch must be a multiple of 32 bytes\n");
        return Dmai_EINVAL;
    }

    if (srcDim.width > 1344) {
        Dmai_err0("Source buffer width must not exceed 1344 pixels\n");
        return Dmai_EINVAL;
    }

    if (dstDim.width > 1344) {
        Dmai_err0("Destination buffer width must not exceed 1344 pixels\n");
        return Dmai_EINVAL;
    }

    /* Check for valid buffer scaling */
    hDif = srcDim.width               * 256 / dstDim.width;
    vDif = HALF_HEIGHT(srcDim.height) * 256 / dstDim.height;

    if (hDif < 32) {
        Dmai_err0("Horizontal up-scaling must not exceed 8x\n");
        return Dmai_EINVAL;
    }

    if (hDif > 4096) {
        Dmai_err0("Horizontal down-scaling must not exceed 1/16x\n");
        return Dmai_EINVAL;
    }

    if (vDif < 32) {
        Dmai_err0("Vertical up-scaling must not exceed 4x\n");
        return Dmai_EINVAL;
    }

    if (vDif > 4096) {
        Dmai_err0("Vertical down-scaling must not exceed 1/32x\n");
        return Dmai_EINVAL;
    }

    /* Set the driver default parameters and retrieve what was set */
    if (ioctl(hSmooth->fd, IPIPE_SET_PARAM, NULL) == -1) {
        Dmai_err0("IPIPE setting default parameters failed\n");
        return Dmai_EFAIL;
    }

    if (ioctl(hSmooth->fd, IPIPE_GET_PARAM, &params) == -1) {
        Dmai_err0("Failed IPIPE_GET_PARAM\n");
        return Dmai_EFAIL;
    }

    /* Look for an available IPIPE channel to use */
    pthread_mutex_lock(&Dmai_DM355_ipipeChannelLock);
    if (dstDim.width <= 640 && Dmai_DM355_ipipeChannelInUse[1] == FALSE) {
       hSmooth->channel = 1;
       Dmai_DM355_ipipeChannelInUse[1] = TRUE;
    }
    else if (Dmai_DM355_ipipeChannelInUse[0] == FALSE) {
       hSmooth->channel = 0;
       Dmai_DM355_ipipeChannelInUse[0] = TRUE;
    }
    pthread_mutex_unlock(&Dmai_DM355_ipipeChannelLock);

    if (hSmooth->channel == -1) {
        Dmai_err0("No available IPIPE channel for smoothing\n");
        return Dmai_EFAIL;
    }

    /*
     * Configure the IPIPE such that its input looks like a buffer half the
     * height of the input buffer, and consisting of every other line in the
     * input buffer.  Smoothing will be done by resizing this half-height
     * buffer to a full size frame.
     */
    params.ipipeif_param.source           = SDRAM_YUV;
    params.ipipeif_param.mode             = ONE_SHOT;
    params.ipipeif_param.hnum             = srcDim.width;
    params.ipipeif_param.vnum             = HALF_HEIGHT(srcDim.height);
    params.ipipeif_param.glob_hor_size    = srcDim.width + TWO_LINES(8);
    params.ipipeif_param.glob_ver_size    = HALF_HEIGHT(srcDim.height) +
                                            TWO_LINES(10);
    params.ipipeif_param.adofs            = TWO_LINES(srcDim.lineLength);
    params.ipipeif_param.gain             = 0x200;
    params.ipipeif_param.clk_div          = DIVIDE_SIXTH;

    params.ipipe_dpaths_fmt               = YUV2YUV;
    params.ipipe_vst                      = 0;
    params.ipipe_hst                      = 0;
    params.ipipe_vsz                      = HALF_HEIGHT(srcDim.height) - 1;
    params.ipipe_hsz                      = srcDim.width - 1;

    params.rsz_rsc_param[hSmooth->channel].rsz_h_dif     = hDif;
    params.rsz_rsc_param[hSmooth->channel].rsz_v_dif     = vDif;
    params.rsz_rsc_param[hSmooth->channel].rsz_o_hsz     = dstDim.width  - 1;
    params.rsz_rsc_param[hSmooth->channel].rsz_o_vsz     = dstDim.height - 1;
    params.ext_mem_param[hSmooth->channel].rsz_sdr_oft   = dstDim.lineLength;
    params.ext_mem_param[hSmooth->channel].rsz_sdr_ptr_e = dstDim.height;

    params.rsz_en[0]                = DISABLE;
    params.rsz_en[1]                = DISABLE;
    params.rsz_en[hSmooth->channel] = ENABLE;

    params.prefilter.pre_en               = DISABLE;
    params.false_color_suppresion.fcs_en  = DISABLE;
    params.edge_enhancer.yee_emf          = DISABLE;

    if(ioctl(hSmooth->fd, IPIPE_SET_PARAM, &params) == -1) {
        Dmai_err0("IPIPE setting parameters failed\n");
        return Dmai_EFAIL;
    }

    return Dmai_EOK;
}

/******************************************************************************
 * Smooth_execute
 ******************************************************************************/
Int Smooth_execute(Smooth_Handle hSmooth,
                   Buffer_Handle hSrcBuf, Buffer_Handle hDstBuf)
{
    struct ipipe_convert convert;
    BufferGfx_Dimensions srcDim;
    BufferGfx_Dimensions dstDim;
    UInt32               srcOffset;
    UInt32               dstOffset;

    assert(hSmooth);
    assert(hSrcBuf);
    assert(hDstBuf);

    BufferGfx_getDimensions(hSrcBuf, &srcDim);
    BufferGfx_getDimensions(hDstBuf, &dstDim);

    srcOffset = srcDim.y * srcDim.lineLength + (srcDim.x << 1);
    dstOffset = dstDim.y * dstDim.lineLength + (dstDim.x << 1);

    /*
     * The IPIPE is configured such that its input looks like a buffer half the
     * height of the input buffer, and consists of every other line in the
     * input buffer.  We now re-size this half-height buffer to a full size
     * frame to perform smoothing.
     */
    convert.in_buff.index     = -1;
    convert.in_buff.buf_type  = IPIPE_BUF_IN;
    convert.in_buff.offset    = Buffer_getPhysicalPtr(hSrcBuf) + srcOffset;
    convert.in_buff.size      = srcDim.lineLength * HALF_HEIGHT(srcDim.height);

    convert.out_buff.index    = -1;
    convert.out_buff.buf_type = IPIPE_BUF_OUT;
    convert.out_buff.offset   = Buffer_getPhysicalPtr(hDstBuf) + dstOffset;
    convert.out_buff.size     = dstDim.lineLength * dstDim.height;

    /* 
     * The IPIPE requires that the memory offsets of the input and output
     * buffers start on 32-byte boundaries.
     */
    assert((convert.in_buff.offset  & 0x1F) == 0);
    assert((convert.out_buff.offset & 0x1F) == 0);

    /* Start IPIPE operation */
    if (ioctl(hSmooth->fd, IPIPE_START, &convert) == -1) {
        Dmai_err0("Failed IPIPE_START\n");
        return Dmai_EFAIL;
    }

    Buffer_setNumBytesUsed(hDstBuf, Buffer_getNumBytesUsed(hSrcBuf));

    return Dmai_EOK;
}

/******************************************************************************
 * Smooth_delete
 ******************************************************************************/
Int Smooth_delete(Smooth_Handle hSmooth)
{
    if (hSmooth) {
        if (hSmooth->channel >= 0) {
           Dmai_DM355_ipipeChannelInUse[hSmooth->channel] = FALSE;
        }
        close(hSmooth->fd);
        free(hSmooth);
    }

    return Dmai_EOK;
}
