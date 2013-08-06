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
#include <string.h>

#include <xdc/std.h>

#include <ti/sdo/dmai/Ccv.h>
#include <ti/sdo/dmai/BufferGfx.h>

#include "priv/_Ccv.h"

#define MODULE_NAME     "Ccv"

/* Accelerated function prototypes */
extern Int Ccv_accel_init(Ccv_Handle hCcv, Ccv_Attrs *attrs);
extern Int Ccv_accel_config(Ccv_Handle hCcv,
                            Buffer_Handle hSrcBuf, Buffer_Handle hDstBuf);
extern Int Ccv_accel_execute(Ccv_Handle hCcv, Buffer_Handle hSrcBuf,
                             Buffer_Handle hDstBuf);
extern Int Ccv_accel_exit(Ccv_Handle hCcv);

const Ccv_Attrs Ccv_Attrs_DEFAULT = {
    FALSE,
};

/******************************************************************************
 * cleanup
 ******************************************************************************/
static Int cleanup(Ccv_Handle hCcv)
{
    Int ret = Dmai_EOK;

    if (hCcv->accel) {
        ret = Ccv_accel_exit(hCcv);
    }

    free(hCcv);

    return ret;
}

/******************************************************************************
 * ccv_Yuv420semi_Yuv422semi
 ******************************************************************************/
static Void ccv_Yuv420semi_Yuv422semi(Ccv_Handle hCcv,
                                  Buffer_Handle hSrcBuf, Buffer_Handle hDstBuf)
{
    BufferGfx_Dimensions srcDim;
    BufferGfx_Dimensions dstDim;
    UInt32 srcOffset, dstOffset;
    Int8 *src, *dst;
    Int i;

    BufferGfx_getDimensions(hSrcBuf, &srcDim);
    BufferGfx_getDimensions(hDstBuf, &dstDim);

    assert((srcDim.x & 0x1) == 0);
    assert((dstDim.x & 0x1) == 0);

    srcOffset = srcDim.y * srcDim.lineLength + srcDim.x;
    dstOffset = dstDim.y * dstDim.lineLength + dstDim.x;

    src = Buffer_getUserPtr(hSrcBuf) + srcOffset;
    dst = Buffer_getUserPtr(hDstBuf) + dstOffset;

    /* Copy Y if necessary */
    if (dst != src) {
        for(i = 0; i < srcDim.height; i++) {
            memcpy(dst, src, srcDim.width);
            dst += dstDim.lineLength;
            src += srcDim.lineLength;
        }
    }

    dst = Buffer_getUserPtr(hDstBuf) + dstOffset + Buffer_getSize(hDstBuf) / 2;
    src = Buffer_getUserPtr(hSrcBuf) +
          srcDim.y * srcDim.lineLength / 2 + srcDim.x +
          Buffer_getSize(hSrcBuf) * 2 / 3;

    for(i = 0; i < srcDim.height; i += 2) {
        memcpy(dst, src, srcDim.width);
        dst += dstDim.lineLength;
        memcpy(dst, src, srcDim.width);
        src += srcDim.lineLength;
        dst += dstDim.lineLength;
    }

    Buffer_setNumBytesUsed(hDstBuf, srcDim.width * srcDim.height * 2);
}

/******************************************************************************
 * ccv_Yuv422semi_Yuv420semi
 ******************************************************************************/
static Void ccv_Yuv422semi_Yuv420semi(Ccv_Handle hCcv,
                                  Buffer_Handle hSrcBuf, Buffer_Handle hDstBuf)
{
    BufferGfx_Dimensions srcDim;
    BufferGfx_Dimensions dstDim;
    UInt32 srcOffset, dstOffset;
    Int8 *src, *dst;
    Int i;

    BufferGfx_getDimensions(hSrcBuf, &srcDim);
    BufferGfx_getDimensions(hDstBuf, &dstDim);

    assert((srcDim.x & 0x1) == 0);
    assert((dstDim.x & 0x1) == 0);

    srcOffset = srcDim.y * srcDim.lineLength + srcDim.x;
    dstOffset = dstDim.y * dstDim.lineLength + dstDim.x;

    src = Buffer_getUserPtr(hSrcBuf) + srcOffset;
    dst = Buffer_getUserPtr(hDstBuf) + dstOffset;

    /* Copy Y if necessary */
    if (dst != src) {
        for(i = 0; i < srcDim.height; i++) {
            memcpy(dst, src, srcDim.width);
            dst += dstDim.lineLength;
            src += srcDim.lineLength;
        }
    }

    src = Buffer_getUserPtr(hSrcBuf) + srcOffset + Buffer_getSize(hSrcBuf) / 2;
    dst = Buffer_getUserPtr(hDstBuf) +
          dstDim.y * dstDim.lineLength / 2 + dstDim.x +
          Buffer_getSize(hDstBuf) * 2 / 3;

    for(i = 0; i < srcDim.height; i += 2) {
        memcpy(dst, src, srcDim.width);
        src += srcDim.lineLength * 2;
        dst += dstDim.lineLength;
    }

    Buffer_setNumBytesUsed(hDstBuf, srcDim.width * srcDim.height * 3 / 2);
}

/* Unaccelerated color conversion function pointers */
static Void (*ccvFxns[Ccv_Mode_COUNT])(Ccv_Handle hCcv, Buffer_Handle hSrcBuf,
                                      Buffer_Handle hDstBuf) = {
    ccv_Yuv420semi_Yuv422semi,
    ccv_Yuv422semi_Yuv420semi,
};

/******************************************************************************
 * Ccv_create
 ******************************************************************************/
Ccv_Handle Ccv_create(Ccv_Attrs *attrs)
{
    Int ret = Dmai_EOK;
    Ccv_Handle hCcv;

    if (attrs == NULL) {
        Dmai_err0("Need valid attrs\n");
        return NULL;
    }

    hCcv = (Ccv_Handle)calloc(1, sizeof(Ccv_Object));

    if (hCcv == NULL) {
        Dmai_err0("Failed to allocate space for Ccv Object\n");
        return NULL;
    }

    hCcv->accel = attrs->accel;

    if (attrs->accel) {
        ret = Ccv_accel_init(hCcv, attrs);

        if (ret == Dmai_ENOTIMPL) {
            Dmai_err0("No accelerated color conversion available\n");
        }

        if (ret < 0) {
            cleanup(hCcv);
            return NULL;
        }
    }

    return hCcv;
}

/******************************************************************************
 * Ccv_config
 ******************************************************************************/
Int Ccv_config(Ccv_Handle hCcv, Buffer_Handle hSrcBuf, Buffer_Handle hDstBuf)
{
    Int ret = Dmai_EOK;
    BufferGfx_Dimensions srcDim, dstDim;
    Int width;

    assert(hCcv);
    assert(hSrcBuf);
    assert(hDstBuf);
    assert(Buffer_getType(hSrcBuf) == Buffer_Type_GRAPHICS);
    assert(Buffer_getType(hDstBuf) == Buffer_Type_GRAPHICS);

    BufferGfx_getDimensions(hSrcBuf, &srcDim);
    BufferGfx_getDimensions(hDstBuf, &dstDim);

    /* Select the smallest width */
    width = srcDim.width < dstDim.width ? srcDim.width : dstDim.width;

    /* Use device specific accelerated configuration function? */
    if (hCcv->accel) {
        ret = Ccv_accel_config(hCcv, hSrcBuf, hDstBuf);

        if (ret < 0) {
            return ret;
        }
    }
    else {
        switch (BufferGfx_getColorSpace(hSrcBuf)) {
            case ColorSpace_YUV422PSEMI:
                /* Two adjacent pixels are dependent, hence need even numbers */
                if (width & 1) {
                    Dmai_err1("Width needs to be even (%d)\n", width);
                    return Dmai_EINVAL;
                }

                if (BufferGfx_getColorSpace(hDstBuf) == ColorSpace_YUV420PSEMI){
                    hCcv->mode = Ccv_Mode_YUV422SEMI_YUV420SEMI;
                }
                else {
                    Dmai_err0("Color conversion mode not supported\n");
                    return Dmai_ENOTIMPL;
                }
                break;
            case ColorSpace_YUV420PSEMI:
                /* Two adjacent pixels are dependent, hence need even numbers */
                if (width & 1) {
                    Dmai_err1("Width needs to be even (%d)\n", width);
                    return Dmai_EINVAL;
                }

                if (BufferGfx_getColorSpace(hDstBuf) == ColorSpace_YUV422PSEMI){
                    hCcv->mode = Ccv_Mode_YUV420SEMI_YUV422SEMI;
                }
                else {
                    Dmai_err0("Color conversion mode not supported\n");
                    return Dmai_ENOTIMPL;
                }
                break;
            default:
                Dmai_err0("Color conversion mode not supported\n");
                return Dmai_ENOTIMPL;
        }
    }

    return Dmai_EOK;
}

/******************************************************************************
 * Ccv_execute
 ******************************************************************************/
Int Ccv_execute(Ccv_Handle hCcv, Buffer_Handle hSrcBuf, Buffer_Handle hDstBuf)
{
    assert(hCcv);
    assert(hSrcBuf);
    assert(hDstBuf);
    assert(Buffer_getUserPtr(hSrcBuf));
    assert(Buffer_getUserPtr(hDstBuf));
    assert(Buffer_getNumBytesUsed(hSrcBuf));
    assert(Buffer_getSize(hDstBuf));
    assert(Buffer_getType(hSrcBuf) == Buffer_Type_GRAPHICS);
    assert(Buffer_getType(hDstBuf) == Buffer_Type_GRAPHICS);

    /* Call device specific accelerated execute or generic function? */
    if (hCcv->accel) {
        return Ccv_accel_execute(hCcv, hSrcBuf, hDstBuf);
    }

    if (ccvFxns[hCcv->mode]) {
        ccvFxns[hCcv->mode](hCcv, hSrcBuf, hDstBuf);
    }
    else {
        return Dmai_ENOTIMPL;
    }

    return Dmai_EOK;
}

/******************************************************************************
 * Ccv_getMode
 ******************************************************************************/
Ccv_Mode Ccv_getMode(Ccv_Handle hCcv)
{
    return hCcv->mode;
}

/******************************************************************************
 * Ccv_delete
 ******************************************************************************/
Int Ccv_delete(Ccv_Handle hCcv)
{
    Int ret = Dmai_EOK;

    if (hCcv) {
        ret = cleanup(hCcv);
    }

    return ret;
}

