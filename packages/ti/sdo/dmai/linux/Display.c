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
#include <string.h>

#include <xdc/std.h>
#include <ti/sdo/dmai/Dmai.h>
#include <ti/sdo/dmai/Display.h>

#include "priv/_Display.h"
#include "priv/_SysFs.h"

#define MODULE_NAME     "Display"

const Display_Attrs Display_Attrs_DM6446_DM355_ATTR_DEFAULT = {
    1,
    Display_Std_FBDEV,
    VideoStd_D1_NTSC,
    Display_Output_COMPOSITE,
    "/dev/fb/2",
    0,
    ColorSpace_NOTSET
};

const Display_Attrs Display_Attrs_DM6446_DM355_OSD_DEFAULT = {
    2,
    Display_Std_FBDEV,
    VideoStd_D1_NTSC,
    Display_Output_COMPOSITE,
    "/dev/fb/0",
    0,
    ColorSpace_NOTSET
};

const Display_Attrs Display_Attrs_DM6446_DM355_VID_DEFAULT = {
    3,
    Display_Std_V4L2,
    VideoStd_D1_NTSC,
    Display_Output_COMPOSITE,
    "/dev/video2",
    0,
    ColorSpace_NOTSET
};

const Display_Attrs Display_Attrs_DM6467_VID_DEFAULT = {
    3,
    Display_Std_V4L2,
    VideoStd_720P_60,
    Display_Output_COMPONENT,
    "/dev/video2",
    0,
    ColorSpace_NOTSET
};

const Display_Attrs Display_Attrs_DM365_VID_DEFAULT = {
    3,
    Display_Std_V4L2,
    VideoStd_720P_60,
    Display_Output_LCD,
    "/dev/video2",
    0,
    ColorSpace_YUV420PSEMI
};

const Display_Attrs Display_Attrs_DM365_OSD_DEFAULT = {
    2,
    Display_Std_FBDEV,
    VideoStd_480P,
    Display_Output_COMPONENT,
    "/dev/fb/0",
    0,
    ColorSpace_NOTSET
};

const Display_Attrs Display_Attrs_DM365_ATTR_DEFAULT = {
    1,
    Display_Std_FBDEV,
    VideoStd_480P,
    Display_Output_COMPONENT,
    "/dev/fb/2",
    0,
    ColorSpace_NOTSET
};

const Display_Attrs Display_Attrs_O3530_VID_DEFAULT = {
    3,
    Display_Std_V4L2,
    VideoStd_VGA,
    Display_Output_LCD,
    "/dev/video1",
    0,
    ColorSpace_NOTSET
};

const Display_Attrs Display_Attrs_O3530_OSD_DEFAULT = {
    1,
    Display_Std_FBDEV,
    VideoStd_VGA,
    Display_Output_LCD,
    "/dev/fb0",
    0,
    ColorSpace_NOTSET
};

/* Functions defined in Display_fbdev.c */
extern Display_Handle Display_fbdev_create(BufTab_Handle hBufTab,
                                           Display_Attrs *attrs);
extern Int Display_fbdev_get(Display_Handle hDisplay, Buffer_Handle *hBufPtr);
extern Int Display_fbdev_put(Display_Handle hDisplay, Buffer_Handle hBuf);
extern Int Display_fbdev_delete(Display_Handle hDisplay);


/* Functions defined in Display_v4l2.c */
extern Display_Handle Display_v4l2_create(BufTab_Handle hBufTab,
                                          Display_Attrs *attrs);
extern Int Display_v4l2_get(Display_Handle hDisplay, Buffer_Handle *hBuf);
extern Int Display_v4l2_put(Display_Handle hDisplay, Buffer_Handle hBuf);
extern Int Display_v4l2_delete(Display_Handle hDisplay);

/* Function tables for run time lookup */
static Display_Handle (*createFxns[Display_Std_COUNT])(BufTab_Handle hBufTab,
                                                     Display_Attrs *attrs) = {
    Display_v4l2_create,
    Display_fbdev_create,
};

static Int (*deleteFxns[Display_Std_COUNT])(Display_Handle hDisplay) = {
    Display_v4l2_delete,
    Display_fbdev_delete,
};

static Int (*getFxns[Display_Std_COUNT])(Display_Handle hDisplay,
                                       Buffer_Handle *hBufPtr) = {
    Display_v4l2_get,
    Display_fbdev_get,
};

static Int (*putFxns[Display_Std_COUNT])(Display_Handle hDisplay,
                                       Buffer_Handle hBuf) = {
    Display_v4l2_put,
    Display_fbdev_put,
};

/******************************************************************************
 * Display_create
 ******************************************************************************/
Display_Handle Display_create(BufTab_Handle hBufTab, Display_Attrs *attrs)
{
    return createFxns[attrs->displayStd](hBufTab, attrs);
}

/******************************************************************************
 * Display_getVideoStd
 ******************************************************************************/
VideoStd_Type Display_getVideoStd(Display_Handle hDisplay)
{
    return hDisplay->videoStd;
}

/******************************************************************************
 * Display_getBufTab
 ******************************************************************************/
BufTab_Handle Display_getBufTab(Display_Handle hDisplay)
{
    return hDisplay->hBufTab;
}

/******************************************************************************
 * Display_delete
 ******************************************************************************/
Int Display_delete(Display_Handle hDisplay)
{
    return deleteFxns[hDisplay->displayStd](hDisplay);
}

/******************************************************************************
 * Display_get
 ******************************************************************************/
Int Display_get(Display_Handle hDisplay, Buffer_Handle *hBufPtr)
{
    return getFxns[hDisplay->displayStd](hDisplay, hBufPtr);
}

/******************************************************************************
 * Display_put
 ******************************************************************************/
Int Display_put(Display_Handle hDisplay, Buffer_Handle hBuf)
{
    return putFxns[hDisplay->displayStd](hDisplay, hBuf);
}

/* Strings for sysfs video output variables */
static Char *outputStrings[Display_Output_COUNT] = {
    "SVIDEO",
    "COMPOSITE",
    "COMPONENT",
    "LCD",
    "DVI",
    NULL
};

/* Strings for sysfs video mode variables */
static Char *modeStrings[VideoStd_COUNT] = {
    NULL,
    NULL,
    NULL,
    NULL,
    "VGA",
    "NTSC",
    "PAL",
#ifdef Dmai_Device_omap3530
    "480P",
#else
    "480P-60",
#endif
    "576P-50",
#ifdef Dmai_Device_omap3530
    "720P",
#else
    "720P-60",
#endif
    "720P-50",
    "720P-30",
    "1080I-30",
    "1080I-25",
    "1080P-30",
    "1080P-25",
    "1080P-24",
    "640x480",
};

#ifdef Dmai_Device_omap3530
#define SYSFS_FMT             "/sys/class/display_control/omap_disp_control/ch%d_output"
#define SYSFS_FMT_GRAPHICS    "/sys/class/display_control/omap_disp_control/graphics"
#define SYSFS_FMT_VIDEO1      "/sys/class/display_control/omap_disp_control/video1"
#define SYSFS_FMT_VIDEO2      "/sys/class/display_control/omap_disp_control/video2"
#else
#define SYSFS_FMT             "/sys/class/davinci_display/ch%d/output"
#endif

/******************************************************************************
 * _Display_sysfsSetup (INTERNAL)
 ******************************************************************************/
Int _Display_sysfsSetup(Display_Attrs *attrs, Int channel)
{
    Char sysfsFileName[MAX_SYSFS_PATH_LEN];
    Char sysfsValue[MAX_SYSFS_VALUE_LEN];
    Int  sysfsVideoStd;
    Int  sysfsDisplayOut;

    sprintf(sysfsFileName, SYSFS_FMT, channel);

    /* Select the output by reading from sysfs */
    if (attrs->videoOutput == Display_Output_SYSTEM) {
        if (_Dmai_readSysFs(sysfsFileName, sysfsValue, MAX_SYSFS_VALUE_LEN)) {
            return Dmai_EIO;
        }

        /* Map sysfs string to the display output enum */
        for (sysfsDisplayOut=0; sysfsDisplayOut < Display_Output_COUNT; 
                sysfsDisplayOut++) {
            if (outputStrings[sysfsDisplayOut] == NULL)
                continue;
            if (strcmp(outputStrings[sysfsDisplayOut],sysfsValue) == 0) {
                attrs->videoOutput = sysfsDisplayOut;
                Dmai_dbg1("Setting video output to '%s'\n", 
                            outputStrings[attrs->videoOutput]);
                break;
            }
        }   
    }
    else {
        /* Sanity check the video output passed by user */
        if (attrs->videoOutput < 0 ||
            attrs->videoOutput > Display_Output_COUNT) {

            Dmai_err1("Unsupported video output %d\n", attrs->videoOutput);
            return Dmai_EINVAL; 
        }

        /* Select the output by writing to sysfs */
        if (_Dmai_writeSysFs(sysfsFileName, outputStrings[attrs->videoOutput]) 
            < 0) {
            return Dmai_EIO;
        }
    }

    sysfsFileName[strlen(SYSFS_FMT) - 7] = 0;
    strcat(sysfsFileName, "mode");

    /* Select the video standard by reading from sysfs */
    if (attrs->videoStd == VideoStd_AUTO) {        
        if(_Dmai_readSysFs(sysfsFileName, sysfsValue, MAX_SYSFS_VALUE_LEN)) {
            return Dmai_EIO;
        }

        /* Map sysfs string to the video standard enum */
        for(sysfsVideoStd=0; sysfsVideoStd < VideoStd_COUNT; sysfsVideoStd++) {
            if (modeStrings[sysfsVideoStd] == NULL)
                continue;
            if (strcmp(modeStrings[sysfsVideoStd], sysfsValue) == 0) {
                attrs->videoStd = sysfsVideoStd;
                Dmai_dbg1("Setting video standard to '%s'\n", 
                            modeStrings[attrs->videoStd]);
                break;
            }
        }
    }
    else {
        /* Sanity check the display mode passed by user */
        if (attrs->videoStd < 0 ||
            attrs->videoStd > VideoStd_COUNT ||
            modeStrings[attrs->videoStd] == NULL ) { 

            Dmai_err1("Unsupported video standard %d\n", attrs->videoStd);
            return Dmai_EINVAL; 
        }   

        /* Select the video standard by writing to sysfs */
	if(attrs->videoOutput == Display_Output_LCD)
	{
		if (_Dmai_writeSysFs(sysfsFileName, modeStrings[VideoStd_LCD]) < 0) {
		return Dmai_EIO;
		}
	}
	else {
		if (_Dmai_writeSysFs(sysfsFileName, modeStrings[attrs->videoStd]) < 0) {
		return Dmai_EIO;
		}
	}
    }

#ifdef Dmai_Device_omap3530

    sprintf(sysfsValue, "channel%d", channel);

    /* Select graphics path by writing to sysfs */
    if (_Dmai_writeSysFs(SYSFS_FMT_GRAPHICS, sysfsValue) <0) {
        return Dmai_EIO;
    }

    /* Select video1 path by writing to sysfs */
    if (_Dmai_writeSysFs(SYSFS_FMT_VIDEO1, sysfsValue) <0) {
        return Dmai_EIO;
    }

    /* Select video2 path by writing to sysfs */
    if (_Dmai_writeSysFs(SYSFS_FMT_VIDEO2, sysfsValue) <0) {
        return Dmai_EIO;
    }
#endif

    return Dmai_EOK;
}

