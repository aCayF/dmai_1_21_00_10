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
/*
 * This simple application shows a pattern on the display device. The pattern
 * looks different depending on the color format of the display.
 */

#include <stdio.h>
#include <string.h>

#include <xdc/std.h>

#include <ti/sdo/dmai/Cpu.h>
#include <ti/sdo/dmai/Dmai.h>
#include <ti/sdo/dmai/Time.h>
#include <ti/sdo/dmai/Display.h>
#include <ti/sdo/dmai/VideoStd.h>
#include <ti/sdo/dmai/BufferGfx.h>

#include "appMain.h"

/******************************************************************************
 * appMain
 ******************************************************************************/
Void appMain(Args * args)
{
    Time_Attrs           tAttrs    = Time_Attrs_DEFAULT;
    Display_Handle       hDisplay  = NULL;
    Time_Handle          hTime     = NULL;
    Int                  numFrame  = 0;
    Display_Attrs        dAttrs;
    Buffer_Handle        hDispBuf;
    Int                  y, x, pos, color;
    Cpu_Device           device;
    UInt32               time;
    BufferGfx_Dimensions dim;

    /* Initialize DMAI */
    Dmai_init();

    if (args->benchmark) {
        hTime = Time_create(&tAttrs);

        if (hTime == NULL) {
            printf("Failed to create Time object\n");
            goto cleanup;
        }
    }

    /* Determine which device the application is running on */
    if (Cpu_getDevice(NULL, &device) < 0) {
        printf("Failed to determine target board\n");
        goto cleanup;
    }

    switch (device) {
        case Cpu_Device_DM6467:
            dAttrs = Display_Attrs_DM6467_VID_DEFAULT;
            break;
        case Cpu_Device_OMAP3530:
            dAttrs = Display_Attrs_O3530_VID_DEFAULT;
            break;
        case Cpu_Device_DM365:
            dAttrs = Display_Attrs_DM365_VID_DEFAULT;
            break;            
        default:
            dAttrs = Display_Attrs_DM6446_DM355_VID_DEFAULT;
            break;
    }

    /* Create the video display */
    dAttrs.videoStd = args->videoStd;
    dAttrs.videoOutput = args->videoOutput;
    hDisplay = Display_create(NULL, &dAttrs);

    if (hDisplay == NULL) {
        printf("Failed to open display device\n");
        goto cleanup;
    }

#define HEIGHT 240
#define WIDTH 320
#define YUV420SP
#ifdef UYVY
    x = color = 0;
    pos = y = 0;
    int tmpr[HEIGHT*WIDTH/2]={0};
    int tmpg[HEIGHT*WIDTH/2]={0};
    int tmpb[HEIGHT*WIDTH/2]={0};
    int tmph[HEIGHT*WIDTH/2]={0};
    for (x = 0; x < WIDTH; x++){
       for (y = 0; y < HEIGHT/2; y++){
            pos = x * HEIGHT/2 + y ;
            tmpr[pos]=(92<<24) + (255<<16) + (92<<8) + 85;
        }
    }
    for (x = 0; x < WIDTH; x++){
       for (y = 0; y < HEIGHT/2; y++){
            pos = x * HEIGHT/2 + y ;
            tmpg[pos]=(165<<24) + (19<<16) + (165<<8) + 42;
        }
    }
    for (x = 0; x < WIDTH; x++){
       for (y = 0; y < HEIGHT/2; y++){
            pos = x * HEIGHT/2 + y ;
            tmpb[pos]=(45<<24) + (107<<16) + (45<<8) + 255;
        }
    }
    for (y = 0; y < HEIGHT/2; y++){
       for (x = 0; x < WIDTH; x++){
            pos = x * HEIGHT/2 + y ;
            if(y<WIDTH/2/3)
               tmph[pos]=tmpr[pos];
            else{
               if(y<WIDTH/2/3*2)
                  tmph[pos]=tmpg[pos];
               else
                  tmph[pos]=tmpb[pos];
            }
        }
    }
#endif
#ifdef YUV420SP
    x = color = 0;
    pos = y = 0;
    char tmpr[WIDTH*HEIGHT*3/2]={0};
    char tmpg[WIDTH*HEIGHT*3/2]={0};
    char tmpb[WIDTH*HEIGHT*3/2]={0};
    char tmph[WIDTH*HEIGHT*3/2]={0};
    for (y = 0; y < HEIGHT; y++) {
       for (x = 0; x < WIDTH; x++) {
            pos = y * WIDTH + x ;
            tmpr[pos]=92;//Y
            if (y < HEIGHT/2) {
                if (x%2 == 0)
                    tmpr[pos+WIDTH*HEIGHT]=85;//U
                else
                    tmpr[pos+WIDTH*HEIGHT]=255;//V
            }
        }
    }
    for (y = 0; y < HEIGHT; y++) {
       for (x = 0; x < WIDTH; x++) {
            pos = y * WIDTH + x ;
            tmpb[pos]=45;
            if (y < HEIGHT/2) {
                if (x%2 == 0)
                    tmpb[pos+WIDTH*HEIGHT]=255;
                else
                    tmpb[pos+WIDTH*HEIGHT]=107;
            }
        }
    }
    for (y = 0; y < HEIGHT; y++) {
       for (x = 0; x < WIDTH; x++) {
            pos = y * WIDTH + x ;
            tmpg[pos]=165;
            if (y < HEIGHT/2) {
                if (x%2 == 0)
                    tmpg[pos+WIDTH*HEIGHT]=42;
                else
                    tmpg[pos+WIDTH*HEIGHT]=16;
            }
        }
    }
    for (x = 0; x < WIDTH; x++) {
       for (y = 0; y < HEIGHT*3/2; y++) {
            pos = y * WIDTH + x ;
            if(x<WIDTH/3)
               tmph[pos]=tmpr[pos];
            else{
               if(x<WIDTH/3*2)
                  tmph[pos]=tmpg[pos];
               else
                  tmph[pos]=tmpb[pos];
            }
        }
    }

#endif

    x = color = 0;

    while (numFrame++ < args->numFrames) {
        if (args->benchmark) {
            if (Time_reset(hTime) < 0) {
                printf("Failed to reset timer\n");
                goto cleanup;
            }
        }

        /* Get a buffer from the display driver */
        if (Display_get(hDisplay, &hDispBuf) < 0) {
            printf("Failed to get display buffer\n");
            goto cleanup;
        }

        /* Retrieve the dimensions of the display buffer */
        BufferGfx_getDimensions(hDispBuf, &dim);

        printf("Display size %dx%d pitch %d x = %d color %d\n", (Int) dim.width,
               (Int) dim.height, (Int) dim.lineLength, x, color);
#define DEBUG
#ifndef DEBUG
        /* Draw a vertical bar of a color */
        for (y = 0; y < dim.height; y++) {
            pos = y * dim.lineLength + x * 2;
            memset(Buffer_getUserPtr(hDispBuf) + pos, color, 2);
        }

        x = (x + 1) % dim.width;
        color = (color + 1) % 0xff;
#endif
#ifdef DEBUG
       if (numFrame%500<125)
           memcpy(Buffer_getUserPtr(hDispBuf), tmpr, WIDTH*HEIGHT*3/2);
       else{
           if(numFrame%500<250)
              memcpy(Buffer_getUserPtr(hDispBuf), tmpg, WIDTH*HEIGHT*3/2);
           else{
               if(numFrame%500<375)
                  memcpy(Buffer_getUserPtr(hDispBuf), tmpb, WIDTH*HEIGHT*3/2);
               else
                  memcpy(Buffer_getUserPtr(hDispBuf), tmph, WIDTH*HEIGHT*3/2);
           }
        }
#endif
#ifndef DEBUG
           memcpy(Buffer_getUserPtr(hDispBuf), in_buf, WIDTH*HEIGHT*3/2);
#endif
       

        /* Give the display buffer back to be displayed */
        if (Display_put(hDisplay, hDispBuf) < 0) {
            printf("Failed to put display buffer\n");
            goto cleanup;
        }

        if (args->benchmark) {
            if (Time_total(hTime, &time) < 0) {
                printf("Failed to get timer total\n");
                goto cleanup;
            }

            printf("[%d] Frame time: %uus\n", numFrame, (Uns) time);
        }
    }

cleanup:
    /* Clean up the application */
    if (hDisplay) {
        Display_delete(hDisplay);
    }

    if (hTime) {
        Time_delete(hTime);
    }

    return;
}
