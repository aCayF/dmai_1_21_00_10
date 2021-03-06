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

/**
 * @defgroup   ti_sdo_dmai_Display   Display
 *
 * @brief Creates an interface instance to the display device driver. This
 *        module will allow you to get and return display buffers from the 
 *        capture device. Currently all platforms support the driver allocating
 *        it's own buffers, and some (like dm6467) support the user allocating
 *        contiguous buffers themselves and passing them in at creation. Typical
 *        use case for driver allocated buffers (dm6446 video window 1,
 *        no error checking):
 *
 * @code
 *   #include <xdc/std.h>
 *   #include <ti/sdo/dmai/Dmai.h>
 *   #include <ti/sdo/dmai/Display.h>
 *   #include <ti/sdo/dmai/BufferGfx.h>
 *   BufferGfx_Handle dBuf;
 *   Display_Attrs dattrs = Display_Attrs_DM6446_DM355_VID_DEFAULT;
 *   Display_Handle hDisplay;
 *
 *   Dmai_init();
 *   hDisplay = Display_create(NULL, &dAttrs);
 *   while (1) {
 *       Display_get(hDisplay, &dBuf);
 *       // Write data in to display buffer "dBuf" for display..
 *       Display_put(hDisplay, dBuf);
 *   }
 *   Display_delete(hDisplay);
 * @endcode
 */
 
#ifndef ti_sdo_dmai_Display_h_
#define ti_sdo_dmai_Display_h_

#include <xdc/std.h>

#include <ti/sdo/dmai/Dmai.h>
#include <ti/sdo/dmai/BufTab.h>
#include <ti/sdo/dmai/VideoStd.h>
#include <ti/sdo/dmai/ColorSpace.h>

/** @ingroup    ti_sdo_dmai_Display */
/*@{*/

/**
 * @brief       Display standards supported on Linux (v4l2 and fbdev).
 */
typedef enum {
    /** @brief v4l2 video standard */
    Display_Std_V4L2 = 0,

    /** @brief Fbdev video standard */
    Display_Std_FBDEV,

    Display_Std_COUNT
} Display_Std;

/**
 * @brief       Video display outputs.
 */
typedef enum {
    /** @brief S-Video video output */
    Display_Output_SVIDEO = 0,

    /** @brief Composite video output */
    Display_Output_COMPOSITE,

    /** @brief Component video output */
    Display_Output_COMPONENT,

    /** @brief LCD video output */
    Display_Output_LCD,

    /** @brief DVI video output */
    Display_Output_DVI,

    /** @brief Select video output by reading sysfs */
    Display_Output_SYSTEM,

    Display_Output_COUNT
} Display_Output;

/**
 * @brief       Attributes used to create a Display device driver instance.
 *
 * @see         Display_Attrs_DM6446_DM355_ATTR_DEFAULT
 * @see         Display_Attrs_DM6446_DM355_OSD_DEFAULT
 * @see         Display_Attrs_DM6446_DM355_VID_DEFAULT
 * @see         Display_Attrs_DM6467_VID_DEFAULT
 */
typedef struct Display_Attrs {
    /** @brief Number of buffers the driver is to use, either allocated
      * from the device driver or the supplied #BufTab_Handle. */
    Int                 numBufs;

    /** @brief Device driver standard to use. (see #Display_Std). 
      * @remarks Only applicable on Linux.
      */
    Display_Std         displayStd;

    /** @brief Video standard to be used by driver. If Display_Attrs.videoStd 
      * is set to VideoStd_AUTO then video standard is determined by sysfs.
      */
    VideoStd_Type       videoStd;

    /** @brief Which video output to use. If Display_Attrs.videoOutput is set 
      * to Display_Output_SYSTEM then video output device is determind by sysfs.
      */
    Display_Output      videoOutput;  

    /** @brief Name of fbdev or v4l2 display device to use.
      * @remarks Only applicable on Linux.
      */
    Char               *displayDevice;

    /** @brief Degrees to rotate the display, valid values are 0, 90, 180
      * and 270.
      * @remarks Only applicable on omap3530.
      */
    Int                 rotation;

    /** @brief Color space type for display driver.
      * @remarks Only applicable on dm365.
      */
    ColorSpace_Type     colorSpace;
} Display_Attrs;

/**
 * @brief Handle through which to reference a Display device instance.
 */
typedef struct Display_Object *Display_Handle;

/**
 * @brief       Default parameters for the Attribute window on DM6446 / DM355.
 * @code
 * numBufs          = 1,
 * displayStd       = Display_Std_FBDEV,
 * videoStd         = VideoStd_D1_NTSC,
 * videoOutput      = Display_Output_COMPOSITE,
 * displayDevice    = "/dev/fb/2"
 * rotation         = 0
 * colorSpace       = ColorSpace_NOTSET
 * @endcode
 */
extern const Display_Attrs Display_Attrs_DM6446_DM355_ATTR_DEFAULT;

/**
 * @brief       Default parameters for the OSD window on DM6446 / DM355.
 * @code
 * numBufs          = 2,
 * displayStd       = Display_Std_FBDEV,
 * videoStd         = VideoStd_D1_NTSC,
 * videoOutput      = Display_Output_COMPOSITE,
 * displayDevice    = "/dev/fb/0"
 * rotation         = 0
 * colorSpace       = ColorSpace_NOTSET 
 * @endcode
 */
extern const Display_Attrs Display_Attrs_DM6446_DM355_OSD_DEFAULT;

/**
 * @brief       Default parameters for Video window 1 on DM6446 / DM355.
 * @code
 * numBufs          = 3,
 * displayStd       = Display_Std_V4L2,
 * videoStd         = VideoStd_D1_NTSC,
 * videoOutput      = Display_Output_COMPOSITE,
 * displayDevice    = "/dev/video2"
 * rotation         = 0
 * colorSpace       = ColorSpace_NOTSET 
 * @endcode
 */
extern const Display_Attrs Display_Attrs_DM6446_DM355_VID_DEFAULT;

/**
 * @brief       Default parameters for Video window on DM6467.
 * @code
 * numBufs          = 3,
 * displayStd       = Display_Std_V4L2,
 * videoStd         = VideoStd_720P_60,
 * videoOutput      = Display_Output_COMPONENT,
 * displayDevice    = "/dev/video2"
 * rotation         = 0
 * colorSpace       = ColorSpace_NOTSET 
 * @endcode
 */
extern const Display_Attrs Display_Attrs_DM6467_VID_DEFAULT;

/**
 * @brief       Default parameters for Video window on DM365.
 * @code
 * numBufs          = 3,
 * displayStd       = Display_Std_V4L2,
 * videoStd         = VideoStd_720P_60,
 * videoOutput      = Display_Output_COMPONENT,
 * displayDevice    = "/dev/video2"
 * rotation         = 0
 * colorSpace       = ColorSpace_YUV420PSEMI 
 * @endcode
 */
extern const Display_Attrs Display_Attrs_DM365_VID_DEFAULT;

/**
 * @brief       Default parameters for OSD window on DM365.
 * @code
 * numBufs          = 2,
 * displayStd       = Display_Std_FBDEV,
 * videoStd         = VideoStd_480P,
 * videoOutput      = Display_Output_COMPONENT,
 * displayDevice    = "/dev/fb/0"
 * rotation         = 0
 * colorSpace       = ColorSpace_NOTSET 
 * @endcode
 */
extern const Display_Attrs Display_Attrs_DM365_OSD_DEFAULT;

/**
 * @brief       Default parameters for Attribute window on DM365.
 * @code
 * numBufs          = 1,
 * displayStd       = Display_Std_FBDEV,
 * videoStd         = VideoStd_480P,
 * videoOutput      = Display_Output_COMPONENT,
 * displayDevice    = "/dev/fb/2"
 * rotation         = 0
 * colorSpace       = ColorSpace_NOTSET 
 * @endcode
 */
extern const Display_Attrs Display_Attrs_DM365_ATTR_DEFAULT;

/**
 * @brief       Default parameters for Video window on OMAP3530.
 * @code
 * frameSkip        = FALSE,
 * numBufs          = 3,
 * displayStd       = Display_Std_V4L2,
 * videoStd         = VideoStd_VGA,
 * videoOutput      = Display_Output_LCD,
 * displayDevice    = "/dev/video1"
 * rotation         = 0
 * colorSpace       = ColorSpace_NOTSET 
 * @endcode
 */
extern const Display_Attrs Display_Attrs_O3530_VID_DEFAULT;

/**
 * @brief       Default parameters for OSD window on OMAP3530.
 * @code
 * frameSkip        = FALSE,
 * numBufs          = 1,
 * displayStd       = Display_Std_FBDEV,
 * videoStd         = VideoStd_VGA,
 * videoOutput      = Display_Output_LCD,
 * displayDevice    = "/dev/fb0"
 * rotation         = 0
 * colorSpace       = ColorSpace_NOTSET 
 * @endcode
 */
extern const Display_Attrs Display_Attrs_O3530_OSD_DEFAULT;

#if defined (__cplusplus)
extern "C" {
#endif

/**
 * @brief       Creates a Display driver instance.
 *
 * @param[in]   hBufTab     The BufTab with buffers to use with the display
 *                          device. If NULL is passed, the display driver will
 *                          be used to allocate the buffers.
 * @param[in]   attrs       #Display_Attrs to use for creating the Display
 *                          driver instance.
 *
 * @retval      #Display_Handle for use in subsequent operations.
 * @retval      NULL for failure.
 */
extern Display_Handle Display_create(BufTab_Handle hBufTab,
                                     Display_Attrs *attrs);

/**
 * @brief       Deletes a Display driver instance.
 *
 * @param[in]   hDisplay    The #Display_Handle to delete.
 *
 * @retval      Dmai_EOK for success.
 * @retval      "Negative value" for failure, see Dmai.h.
 *
 * @remarks     #Display_create must be called before this function.
 */
extern Int Display_delete(Display_Handle hDisplay);

/**
 * @brief       Receive a buffer from the display device driver instance to be
 *              filled with data.
 *
 * @param[in]   hDisplay    The #Display_Handle to get a buffer from.
 * @param[out]  hBuf        A pointer to the #Buffer_Handle received.
 *
 * @retval      Dmai_EOK for success.
 * @retval      "Negative value" for failure, see Dmai.h.
 *
 * @remarks     #Display_create must be called before this function.
 */
extern Int Display_get(Display_Handle hDisplay, Buffer_Handle *hBuf);

/**
 * @brief       Send a buffer with data to the display device driver instance
 *              to be shown on the display.
 *
 * @param[in]   hDisplay    The #Display_Handle to send a buffer to.
 * @param[in]   hBufPtr     A #Buffer_Handle to send to the display device.
 *
 * @retval      Dmai_EOK for success.
 * @retval      "Negative value" for failure, see Dmai.h.
 *
 * @remarks     #Display_create must be called before this function.
 */
extern Int Display_put(Display_Handle hDisplay, Buffer_Handle hBufPtr);

/**
 * @brief       Get the display standard used by a Display device driver
 *              instance.
 *
 * @param[in]   hDisplay    The #Display_Handle to get the video standard of.
 *
 * @retval      Type of video standard used (see #VideoStd_Type).
 *
 * @remarks     #Display_create must be called before this function.
 */
extern VideoStd_Type Display_getVideoStd(Display_Handle hDisplay);


/**
 * @brief       Get a handle to the BufTab used by the Display device driver
 *              instance.
 *
 * @param[in]   hDisplay    The #Display_Handle to get the BufTab of.
 *
 * @retval      Handle to the BufTab used (see #BufTab_Handle).
 *
 * @remarks     #Display_create must be called before this function.
 */
extern BufTab_Handle Display_getBufTab(Display_Handle hDisplay);

#if defined (__cplusplus)
}
#endif

/*@}*/

#endif /* ti_sdo_dmai_Display_h_ */
