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
#include <string.h>

#include <xdc/std.h>

#include <ti/sdo/dmai/Dmai.h>

#include "priv/_SysFs.h"

#define MODULE_NAME     "Dmai"

/******************************************************************************
 * _Dmai_writeSysFs
 ******************************************************************************/
Int _Dmai_writeSysFs(Char *fileName, Char *val)
{
    FILE *fp;
    Char *valString;

    valString = malloc(strlen(val) + 1);

    if (valString == NULL) {
        Dmai_err0("Failed to allocate memory for temporary string\n");
        return Dmai_ENOMEM;
    }

    fp = fopen(fileName, "w");

    if (fp == NULL) {
        Dmai_err1("Failed to open %s for writing\n", fileName);
        free(valString);
        return Dmai_EIO;
    }

    if (fwrite(val, strlen(val) + 1, 1, fp) != 1) {
        Dmai_err2("Failed to write sysfs variable %s to %s\n",
                  fileName, val);
        fclose(fp);
        free(valString);
        return Dmai_EIO;
    }

    fclose(fp);

    if (_Dmai_readSysFs(fileName, valString, strlen(val) + 1) < 0) {
        free(valString);
        return Dmai_EIO;
    }

    if (strcmp(valString, val) != 0) {
        Dmai_err3("Failed to verify sysfs variable %s to %s (is %s)\n",
                  fileName, val, valString);
        free(valString);
        return Dmai_EFAIL;
    }

    free(valString);

    return Dmai_EOK;
}

/******************************************************************************
 * _Dmai_readSysFs
 ******************************************************************************/
Int _Dmai_readSysFs(Char *fileName, Char *val, int length)
{
    FILE *fp;
    int ret;
    int len;
    char *tok;

    fp = fopen(fileName, "r");

    if (fp == NULL) {
        Dmai_err1("Failed to open %s for reading\n", fileName);
        return Dmai_EIO;
    }

    memset(val, '\0', length);

    ret = fread(val, 1, length, fp);

    if (ret < 1) {
        Dmai_err1("Failed to read sysfs variable from %s\n", fileName);
        return Dmai_EIO;
    }

    tok = strtok(val, "\n");
    len = tok ? strlen(tok) : strlen(val);
    val[len] = '\0';

    fclose(fp);

    return Dmai_EOK;
}
