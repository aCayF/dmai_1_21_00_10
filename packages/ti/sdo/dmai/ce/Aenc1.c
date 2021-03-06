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

#include <stdio.h>
#include <stdlib.h>

#include <xdc/std.h>
#include <ti/sdo/ce/Engine.h>
#include <ti/sdo/ce/audio1/audenc1.h>

#include <ti/sdo/dmai/Dmai.h>
#include <ti/sdo/dmai/Buffer.h>
#include <ti/sdo/dmai/ce/Aenc1.h>

#define MODULE_NAME     "Aenc1"

typedef struct Aenc1_Object {
    AUDENC1_Handle      hEncode;
    Int32               minInBufSize;
    Int32               minOutBufSize;
    Int32               bytesPerSample;
    Int32               numChannels;
} Aenc1_Object;

const AUDENC1_Params Aenc1_Params_DEFAULT = {
    sizeof(AUDENC1_Params),
    48000,
    288000,
    IAUDIO_2_0,
    XDM_LE_16,
    IAUDIO_CBR,
    IAUDIO_INTERLEAVED,
    16,
    192000,
    IAUDIO_DUALMONO_LR,
    XDAS_FALSE,
    XDAS_FALSE,
    XDAS_FALSE
};

const AUDENC1_DynamicParams Aenc1_DynamicParams_DEFAULT = {
    sizeof(AUDENC1_DynamicParams),
    288000,
    48000,
    IAUDIO_2_0,
    XDAS_FALSE,
    IAUDIO_DUALMONO_LR,
    16
};

/******************************************************************************
 * cleanup
 ******************************************************************************/
static Int cleanup(Aenc1_Handle hAe)
{
    if (hAe->hEncode) {
        AUDENC1_delete(hAe->hEncode);
    }

    free(hAe);

    return Dmai_EOK;
}

/******************************************************************************
 * Aenc1_process
 ******************************************************************************/
Int Aenc1_process(Aenc1_Handle hAe, Buffer_Handle hInBuf, Buffer_Handle hOutBuf)
{
    XDM1_BufDesc            inBufDesc;
    XDM1_BufDesc            outBufDesc;
    XDAS_Int32              status;
    AUDENC1_InArgs          inArgs;
    AUDENC1_OutArgs         outArgs;

    assert(hAe);
    assert(hInBuf);
    assert(hOutBuf);
    assert(Buffer_getUserPtr(hInBuf));
    assert(Buffer_getUserPtr(hOutBuf));
    assert(Buffer_getNumBytesUsed(hInBuf));
    assert(Buffer_getSize(hOutBuf));
    assert(hAe->bytesPerSample);
    assert(hAe->numChannels);

    inBufDesc.numBufs           = 1;
    inBufDesc.descs[0].bufSize  = Buffer_getSize(hInBuf);
    inBufDesc.descs[0].buf      = Buffer_getUserPtr(hInBuf);

    outBufDesc.numBufs          = 1;
    outBufDesc.descs[0].bufSize = Buffer_getSize(hOutBuf);
    outBufDesc.descs[0].buf     = Buffer_getUserPtr(hOutBuf);

    inArgs.size                 = sizeof(AUDENC1_InArgs);
    inArgs.numInSamples         = Buffer_getNumBytesUsed(hInBuf) /
                                    hAe->bytesPerSample;
    inArgs.numInSamples         /= hAe->numChannels;
    inArgs.ancData.bufSize      = 0L;
    inArgs.ancData.buf          = (XDAS_Int8 *) NULL;

    outArgs.size                = sizeof(AUDENC1_OutArgs);

    /* Encode the audio buffer */
    status = AUDENC1_process(hAe->hEncode, &inBufDesc, &outBufDesc, &inArgs,
                             &outArgs);

    if (status != AUDENC1_EOK) {
        Dmai_err2("AUDENC1_process() failed with error (%d ext: 0x%x)\n",
                  (Int)status, (Uns) outArgs.extendedError);
        return Dmai_EFAIL;
    }

    Buffer_setNumBytesUsed(hOutBuf, outArgs.bytesGenerated);

    return Dmai_EOK;
}

/******************************************************************************
 * Aenc1_create
 ******************************************************************************/
Aenc1_Handle Aenc1_create(Engine_Handle hEngine,
                        Char *codecName,
                        AUDENC1_Params *params,
                        AUDENC1_DynamicParams *dynParams)
{
    Aenc1_Handle        hAe;
    AUDENC1_Handle      hEncode;
    AUDENC1_Status      encStatus;
    XDAS_Int32          status;

    if (hEngine == NULL || codecName == NULL ||
        params == NULL || dynParams == NULL) {
        Dmai_err0("Cannot pass null for engine, codec name, params or "
                  "dynamic params\n");
        return NULL;
    }

    /* Allocate space for the object */
    hAe = (Aenc1_Handle)calloc(1, sizeof(Aenc1_Object));

    if (hAe == NULL) {
        Dmai_err0("Failed to allocate space for Aenc1 Object\n");
        return NULL;
    }

    /* Create audio encoder */
    hEncode = AUDENC1_create(hEngine, codecName, params);

    if (hEncode == NULL) {
        Dmai_err1("Failed to open audio encode algorithm: %s\n", codecName);
        cleanup(hAe);
        return NULL;
    }

    /* Set dynamic parameters */
    encStatus.size = sizeof(AUDENC1_Status);
    encStatus.data.buf = NULL;
    status = AUDENC1_control(hEncode, XDM_SETPARAMS, dynParams, &encStatus);

    if (status != AUDENC1_EOK) {
        Dmai_err1("XDM_SETPARAMS failed, status=%d\n", status);
        cleanup(hAe);
        return NULL;
    }

    /* Get buffer requirements */
    status = AUDENC1_control(hEncode, XDM_GETBUFINFO, dynParams, &encStatus);

    if (status != AUDENC1_EOK) {
        Dmai_err1("XDM_GETBUFINFO failed, status=%d\n", status);
        cleanup(hAe);
        return NULL;
    }

    /* channelMode supported IAUDIO_1_0,IAUDIO_2_0,IAUDIO_11_0  */
    hAe->numChannels = dynParams->channelMode == IAUDIO_1_0 ? 1 : 2;
    hAe->minInBufSize = encStatus.bufInfo.minInBufSize[0];
    hAe->minOutBufSize = encStatus.bufInfo.minOutBufSize[0];
    hAe->bytesPerSample = dynParams->inputBitsPerSample >> 3;

    hAe->hEncode = hEncode;

    return hAe;
}

/******************************************************************************
 * Aenc1_delete
 ******************************************************************************/
Int Aenc1_delete(Aenc1_Handle hAe)
{
    if (hAe) {
        return cleanup(hAe);
    }

    return Dmai_EOK;
}

/******************************************************************************
 * Aenc1_getVisaHandle
 ******************************************************************************/
AUDENC1_Handle Aenc1_getVisaHandle(Aenc1_Handle hAe)
{
    assert(hAe);

    return hAe->hEncode;
}

/******************************************************************************
 * Aenc1_getInBufSize
 ******************************************************************************/
Int32 Aenc1_getInBufSize(Aenc1_Handle hAe)
{
    assert(hAe);

    return hAe->minInBufSize;
}

/******************************************************************************
 * Aenc1_getOutBufSize
 ******************************************************************************/
Int32 Aenc1_getOutBufSize(Aenc1_Handle hAe)
{
    assert(hAe);

    return hAe->minOutBufSize;
}

