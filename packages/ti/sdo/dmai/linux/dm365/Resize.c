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

#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include <asm/arch/imp_resizer.h>
#include <asm/arch/imp_previewer.h>
#include <asm/arch/dm365_ipipe.h>

#include <ti/sdo/dmai/Dmai.h>
#include <ti/sdo/dmai/BufferGfx.h>
#include <ti/sdo/dmai/Resize.h>

#define MODULE_NAME   "Resize"
#define RESIZER_DEVICE   "/dev/davinci_resizer"
#define PREVIEWER_DEVICE "/dev/davinci_previewer"

typedef struct Resize_Object {
    int   fd;
} Resize_Object;

const Resize_Attrs Resize_Attrs_DEFAULT = {
    Resize_FilterType_LOWPASS,
    Resize_FilterType_LOWPASS,
    Resize_WindowType_BLACKMAN,
    Resize_WindowType_BLACKMAN,
    0xe
};

/******************************************************************************
 * pixFormatConversion
 ******************************************************************************/
static enum ipipe_pix_formats pixFormatConversion(ColorSpace_Type cs)
{
    switch(cs) {
        case ColorSpace_YUV420PSEMI:
            return IPIPE_YUV420SP;
        case ColorSpace_UYVY:
            return IPIPE_UYVY;
        default:
            return -1;
    };
}

/******************************************************************************
 * Resize_create
 ******************************************************************************/
Resize_Handle Resize_create(Resize_Attrs *attrs)
{
    Resize_Handle hResize;
    unsigned long oper_mode;    

    assert(attrs);

    hResize = (Resize_Handle)calloc(1, sizeof(Resize_Object));

    if (hResize == NULL) {
        Dmai_err0("Failed to allocate space for Framecopy Object\n");
        return NULL;
    }

    /* Open resizer device */
    hResize->fd = open(RESIZER_DEVICE, O_RDWR);

    if (hResize->fd == -1) {
        Dmai_err1("Failed to open %s\n", RESIZER_DEVICE);
        return NULL;
    }

    oper_mode = IMP_MODE_SINGLE_SHOT;

	if (ioctl(hResize->fd,RSZ_S_OPER_MODE, &oper_mode) < 0) {
        Dmai_err0("Resizer can't set operation mode\n");        
        goto fac_error;
	}

	if (ioctl(hResize->fd,RSZ_G_OPER_MODE, &oper_mode) < 0) {
        Dmai_err0("Resizer can't get operation mode\n");        
        goto fac_error;
	}
    
	if (oper_mode != IMP_MODE_SINGLE_SHOT) {
        Dmai_err1("Resizer can't set operation mode single shot, the mode is %d\n",
            oper_mode);        
        goto fac_error;
	}

    return hResize;

fac_error:
	close(hResize->fd);
    hResize->fd = 0;
    return NULL;
}

/******************************************************************************
 * Resize_config
 ******************************************************************************/
Int Resize_config(Resize_Handle hResize,
                  Buffer_Handle hSrcBuf, Buffer_Handle hDstBuf, Buffer_Handle hsDstBuf)
{
    BufferGfx_Dimensions srcDim, dstDim, sdstDim;
    UInt                 hDif;
    UInt                 vDif;    
    UInt                 shDif;
    UInt                 svDif;    
	struct rsz_channel_config rsz_chan_config;
	struct rsz_single_shot_config rsz_ss_config;

    /* Make sure our input parameters are valid */
    if (!hResize) {
        Dmai_err0("Resize_Handle parameter must not be NULL\n");
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
    
    if (!hsDstBuf) {
        Dmai_err0("Second destination buffer parameter is NULL\n");
    }
    
    /* Buffer needs to be graphics buffers */
    if (Buffer_getType(hSrcBuf) != Buffer_Type_GRAPHICS ||
        Buffer_getType(hDstBuf) != Buffer_Type_GRAPHICS ||
        hsDstBuf?(Buffer_getType(hsDstBuf) != Buffer_Type_GRAPHICS):FALSE) {
        Dmai_err0("Src and dst buffers need to be graphics buffers\n");
        return Dmai_EINVAL;
    }

    BufferGfx_getDimensions(hSrcBuf, &srcDim);
    BufferGfx_getDimensions(hDstBuf, &dstDim);
    hsDstBuf?BufferGfx_getDimensions(hsDstBuf, &sdstDim):NULL;

    if (dstDim.width <= 0) {
        Dmai_err0("Destination buffer width must be greater than zero\n");
        return Dmai_EINVAL;
    }

    if (dstDim.height <= 0) {
        Dmai_err0("Destination buffer height must be greater than zero\n");
        return Dmai_EINVAL;
    }

    if (hsDstBuf?(sdstDim.width <= 0):FALSE) {
        Dmai_err0("Second destination buffer width must be greater than zero\n");
        return Dmai_EINVAL;
    }

    if (hsDstBuf?(sdstDim.height <= 0):FALSE) {
        Dmai_err0("Second destination buffer height must be greater than zero\n");
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

    if (hsDstBuf?((sdstDim.lineLength & 0x1F) != 0):FALSE) {
        Dmai_err0("Second destination buffer pitch must be a multiple of 32 bytes\n");
        return Dmai_EINVAL;
    }


    /* Check for valid buffer scaling */
    hDif = srcDim.width  * 256 / dstDim.width;
    vDif = srcDim.height * 256 / dstDim.height;

    if (hDif < 32) {
        Dmai_err0("Horizontal up-scaling must not exceed 8x\n");
        return Dmai_EINVAL;
    }

    if (hDif > 4096) {
        Dmai_err0("Horizontal down-scaling must not exceed 1/16x\n");
        return Dmai_EINVAL;
    }

    if (vDif < 32) {
        Dmai_err0("Vertical up-scaling must not exceed 8x\n");
        return Dmai_EINVAL;
    }

    if (vDif > 4096) {
        Dmai_err0("Vertical down-scaling must not exceed 1/16x\n");
        return Dmai_EINVAL;
    }    

    shDif = hsDstBuf?(srcDim.width  * 256 / sdstDim.width):0;
    svDif = hsDstBuf?(srcDim.height * 256 / sdstDim.height):0;

    if (shDif && shDif < 32) {
        Dmai_err0("Second horizontal up-scaling must not exceed 8x\n");
        return Dmai_EINVAL;
    }

    if (shDif && shDif > 4096) {
        Dmai_err0("Second horizontal down-scaling must not exceed 1/16x\n");
        return Dmai_EINVAL;
    }

    if (svDif && svDif < 32) {
        Dmai_err0("Second vertical up-scaling must not exceed 8x\n");
        return Dmai_EINVAL;
    }

    if (svDif && svDif > 4096) {
        Dmai_err0("Second vertical down-scaling must not exceed 1/16x\n");
        return Dmai_EINVAL;
    }    

    /* Configure for ColorSpace_UYVY_*/
    if (BufferGfx_getColorSpace(hSrcBuf) == ColorSpace_UYVY) {
        /* Setting default configuration in Resizer */
	    Dmai_clear(rsz_ss_config);
	    rsz_chan_config.oper_mode = IMP_MODE_SINGLE_SHOT;
	    rsz_chan_config.chain = 0;
	    rsz_chan_config.len = 0;
	    rsz_chan_config.config = NULL; /* to set defaults in driver */
	    if (ioctl(hResize->fd, RSZ_S_CONFIG, &rsz_chan_config) < 0) {
	    	Dmai_err0("Error in setting default configuration for single shot mode\n");
            goto faco_error;
	    }

	    /* default configuration setting in Resizer successfull */
	    Dmai_clear(rsz_ss_config);
	    rsz_chan_config.oper_mode = IMP_MODE_SINGLE_SHOT;
	    rsz_chan_config.chain = 0;
	    rsz_chan_config.len = sizeof(struct rsz_single_shot_config);
	    rsz_chan_config.config = &rsz_ss_config;

	    if (ioctl(hResize->fd, RSZ_G_CONFIG, &rsz_chan_config) < 0) {
	    	Dmai_err0("Error in getting resizer channel configuration from driver\n");
            goto faco_error;
	    }

	    /* input params are set at the resizer */
	    rsz_ss_config.input.image_width  = srcDim.width;
	    rsz_ss_config.input.image_height = srcDim.height;
	    rsz_ss_config.input.ppln = rsz_ss_config.input.image_width + 8;
	    rsz_ss_config.input.lpfr = rsz_ss_config.input.image_height + 10;
	    rsz_ss_config.input.pix_fmt = pixFormatConversion(BufferGfx_getColorSpace(hSrcBuf));
	    rsz_ss_config.output1.pix_fmt = pixFormatConversion(BufferGfx_getColorSpace(hDstBuf));
	    rsz_ss_config.output1.enable = 1;
	    rsz_ss_config.output1.width = dstDim.width;
	    rsz_ss_config.output1.height = dstDim.height;
	    rsz_ss_config.output2.pix_fmt = hsDstBuf?pixFormatConversion(BufferGfx_getColorSpace(hsDstBuf)):0;
	    rsz_ss_config.output2.enable = hsDstBuf?1:0;
	    rsz_ss_config.output2.width = hsDstBuf?sdstDim.width:0;
	    rsz_ss_config.output2.height = hsDstBuf?sdstDim.height:0;

	    rsz_chan_config.oper_mode = IMP_MODE_SINGLE_SHOT;
	    rsz_chan_config.chain = 0;
	    rsz_chan_config.len = sizeof(struct rsz_single_shot_config);
	    if (ioctl(hResize->fd, RSZ_S_CONFIG, &rsz_chan_config) < 0) {
	    	Dmai_err0("Error in setting default configuration for single shot mode\n");
            goto faco_error;
	    }
	    rsz_chan_config.oper_mode = IMP_MODE_SINGLE_SHOT;
	    rsz_chan_config.chain = 0;
	    rsz_chan_config.len = sizeof(struct rsz_single_shot_config);

	    /* read again and verify */
	    if (ioctl(hResize->fd, RSZ_G_CONFIG, &rsz_chan_config) < 0) {
	    	Dmai_err0("Error in getting configuration from driver\n");
            goto faco_error;
	    }
    }

    return Dmai_EOK;
    
faco_error:
	close(hResize->fd);
    hResize->fd = 0;    
    return Dmai_EFAIL;
    
}

/******************************************************************************
 * Resize_execute
 ******************************************************************************/
Int Resize_execute(Resize_Handle hResize,
                   Buffer_Handle hSrcBuf, Buffer_Handle hDstBuf, Buffer_Handle hsDstBuf)
{
    struct imp_convert  rsz;
	struct rsz_channel_config rsz_chan_config;
	struct rsz_single_shot_config rsz_ss_config;
    BufferGfx_Dimensions srcDim;
    BufferGfx_Dimensions dstDim;
    BufferGfx_Dimensions sdstDim;
    UInt32               srcOffset;
    UInt32               dstOffset;
    UInt32               sdstOffset;
    UInt32               srcSize;
    UInt32               dstSize;
    
    assert(hResize);
    assert(hSrcBuf);
    assert(hDstBuf);

    Dmai_clear(rsz);

    BufferGfx_getDimensions(hSrcBuf, &srcDim);
    BufferGfx_getDimensions(hDstBuf, &dstDim);
    hsDstBuf?BufferGfx_getDimensions(hsDstBuf, &sdstDim):NULL;
    srcSize = srcDim.width * srcDim.height;
    dstSize = dstDim.width * dstDim.height;

    /* the resize operation to ColorSpace_UYVY is different from ColorSpace_YUV420PSEMI*/
    if (BufferGfx_getColorSpace(hSrcBuf) == ColorSpace_UYVY) {
        srcOffset = srcDim.y * srcDim.lineLength + (srcDim.x << 1);
        dstOffset = dstDim.y * dstDim.lineLength + (dstDim.x << 1);
        sdstOffset = hsDstBuf?(sdstDim.y * sdstDim.lineLength + (sdstDim.x << 1)):0;

        rsz.in_buff.index     = -1;
        rsz.in_buff.buf_type  = IMP_BUF_IN;
        rsz.in_buff.offset    = Buffer_getPhysicalPtr(hSrcBuf) + srcOffset;
        rsz.in_buff.size      = Buffer_getSize(hSrcBuf);

        rsz.out_buff1.index    = -1;
        rsz.out_buff1.buf_type = IMP_BUF_OUT1;
        rsz.out_buff1.offset   = Buffer_getPhysicalPtr(hDstBuf) + dstOffset;
        rsz.out_buff1.size     = Buffer_getSize(hDstBuf);
        
        rsz.out_buff2.index    = -1;
        rsz.out_buff2.buf_type = IMP_BUF_OUT2;
        rsz.out_buff2.offset   = hsDstBuf?(Buffer_getPhysicalPtr(hsDstBuf) + sdstOffset):0;
        rsz.out_buff2.size     = hsDstBuf?Buffer_getSize(hsDstBuf):0;
        
        /* 
         * The IPIPE requires that the memory offsets of the input and output
         * buffers start on 32-byte boundaries.
         */
        assert((rsz.in_buff.offset  & 0x1F) == 0);
        assert((rsz.out_buff1.offset & 0x1F) == 0);
        assert((rsz.out_buff2.offset & 0x1F) == 0);

        /* Start IPIPE operation */
        if (ioctl(hResize->fd, RSZ_RESIZE, &rsz) == -1) {
            Dmai_err0("Failed RSZ_RESIZE\n");
            return Dmai_EFAIL;
        }

        Buffer_setNumBytesUsed(hDstBuf, Buffer_getSize(hDstBuf));
        hsDstBuf?Buffer_setNumBytesUsed(hsDstBuf, Buffer_getSize(hsDstBuf)):NULL;
    } else {
        /* configure for the ColorSpace_YUV420PSEMI*/
	    Dmai_clear(rsz_ss_config);
	    rsz_chan_config.oper_mode = IMP_MODE_SINGLE_SHOT;
	    rsz_chan_config.chain = 0;
	    rsz_chan_config.len = 0;
	    rsz_chan_config.config = NULL; /* to set defaults in driver */
	    if (ioctl(hResize->fd, RSZ_S_CONFIG, &rsz_chan_config) < 0) {
	    	Dmai_err0("Error in setting default configuration for single shot mode\n");
            return Dmai_EFAIL;
	    }

	    /* default configuration setting in Resizer successfull */
	    Dmai_clear(rsz_ss_config);
	    rsz_chan_config.oper_mode = IMP_MODE_SINGLE_SHOT;
	    rsz_chan_config.chain = 0;
	    rsz_chan_config.len = sizeof(struct rsz_single_shot_config);
	    rsz_chan_config.config = &rsz_ss_config;

	    if (ioctl(hResize->fd, RSZ_G_CONFIG, &rsz_chan_config) < 0) {
	    	Dmai_err0("Error in getting configuration from driver\n");
            return Dmai_EFAIL;
	    }

	    /* input params are set at the resizer */
	    rsz_ss_config.input.image_width  = srcDim.width;
	    rsz_ss_config.input.image_height = srcDim.height;
	    rsz_ss_config.input.ppln = rsz_ss_config.input.image_width + 8;
	    rsz_ss_config.input.lpfr = rsz_ss_config.input.image_height + 10;
	    rsz_ss_config.input.pix_fmt = IPIPE_420SP_Y;
	    rsz_ss_config.output1.pix_fmt = pixFormatConversion(BufferGfx_getColorSpace(hDstBuf));
        rsz_ss_config.output1.enable = 1;
	    rsz_ss_config.output1.width = dstDim.width;
	    rsz_ss_config.output1.height = dstDim.height;
	    rsz_ss_config.output2.enable = 0;

	    rsz_chan_config.oper_mode = IMP_MODE_SINGLE_SHOT;
	    rsz_chan_config.chain = 0;
	    rsz_chan_config.len = sizeof(struct rsz_single_shot_config);
	    if (ioctl(hResize->fd, RSZ_S_CONFIG, &rsz_chan_config) < 0) {
	    	Dmai_err0("Error in setting default configuration for single shot mode\n");
            return Dmai_EFAIL;
	    }
	    rsz_chan_config.oper_mode = IMP_MODE_SINGLE_SHOT;
	    rsz_chan_config.chain = 0;
	    rsz_chan_config.len = sizeof(struct rsz_single_shot_config);

	    /* read again and verify */
	    if (ioctl(hResize->fd, RSZ_G_CONFIG, &rsz_chan_config) < 0) {
	    	Dmai_err0("Error in getting configuration from driver\n");
            return Dmai_EFAIL;
	    }

        /* execute for ColorSpace_YUV420PSEMI*/
        Dmai_clear(rsz);
        //srcOffset = srcDim.y * srcDim.lineLength + (srcDim.x << 1);
        //dstOffset = dstDim.y * dstDim.lineLength + (dstDim.x << 1);
        rsz.in_buff.buf_type  = IMP_BUF_IN;
        rsz.in_buff.index     = -1;
        rsz.in_buff.offset    = Buffer_getPhysicalPtr(hSrcBuf);
        //rsz.in_buff.size      = Buffer_getSize(hSrcBuf);
        rsz.in_buff.size      = srcSize;

        rsz.out_buff1.buf_type = IMP_BUF_OUT1;
        rsz.out_buff1.index    = -1;
        rsz.out_buff1.offset    = Buffer_getPhysicalPtr(hDstBuf);
        //rsz.out_buff1.size     = Buffer_getSize(hDstBuf);
        rsz.out_buff1.size     = dstSize;
        
        /* 
         * The IPIPE requires that the memory offsets of the input and output
         * buffers start on 32-byte boundaries.
         */
        assert((rsz.in_buff.offset  & 0x1F) == 0);
        assert((rsz.out_buff1.offset & 0x1F) == 0);
        /* Start IPIPE operation */
        if (ioctl(hResize->fd, RSZ_RESIZE, &rsz) == -1) {
            Dmai_err0("Failed RSZ_RESIZE\n");
            return Dmai_EFAIL;
        }

	    /* input params are set at the resizer */
	    rsz_ss_config.input.image_width  = srcDim.width;
	    rsz_ss_config.input.image_height = srcDim.height>>1;
	    rsz_ss_config.input.ppln = rsz_ss_config.input.image_width + 8;
	    rsz_ss_config.input.lpfr = rsz_ss_config.input.image_height + 10;
	    rsz_ss_config.input.pix_fmt = IPIPE_420SP_C;
        rsz_ss_config.output1.pix_fmt = pixFormatConversion(BufferGfx_getColorSpace(hDstBuf));
	    rsz_ss_config.output1.enable = 1;
	    rsz_ss_config.output1.width = dstDim.width;
	    rsz_ss_config.output1.height = dstDim.height;
        rsz_ss_config.output2.enable = 0;

	    rsz_chan_config.oper_mode = IMP_MODE_SINGLE_SHOT;
	    rsz_chan_config.chain = 0;
	    rsz_chan_config.len = sizeof(struct rsz_single_shot_config);
	    if (ioctl(hResize->fd, RSZ_S_CONFIG, &rsz_chan_config) < 0) {
	    	Dmai_err0("Error in setting default configuration for single shot mode\n");
            return Dmai_EFAIL;
	    }
	    rsz_chan_config.oper_mode = IMP_MODE_SINGLE_SHOT;
	    rsz_chan_config.chain = 0;
	    rsz_chan_config.len = sizeof(struct rsz_single_shot_config);

	    /* read again and verify */
	    if (ioctl(hResize->fd, RSZ_G_CONFIG, &rsz_chan_config) < 0) {
	    	Dmai_err0("Error in getting configuration from driver\n");
            return Dmai_EFAIL;
	    }

        Dmai_clear(rsz);
        //srcOffset = srcDim.y * srcDim.lineLength + (srcDim.x << 1);
        //dstOffset = dstDim.y * dstDim.lineLength + (dstDim.x << 1);
        rsz.in_buff.buf_type  = IMP_BUF_IN;
        rsz.in_buff.index     = -1;
        rsz.in_buff.offset    = Buffer_getPhysicalPtr(hSrcBuf)  + srcSize;
        rsz.in_buff.size      = srcSize>>1;

        rsz.out_buff1.buf_type = IMP_BUF_OUT1;
        rsz.out_buff1.index    = -1;
        rsz.out_buff1.offset   = Buffer_getPhysicalPtr(hDstBuf)  + dstSize;
        rsz.out_buff1.size     = dstSize>>1;
        
        /* 
         * The IPIPE requires that the memory offsets of the input and output
         * buffers start on 32-byte boundaries.
         */
        assert((rsz.in_buff.offset  & 0x1F) == 0);
        assert((rsz.out_buff1.offset & 0x1F) == 0);
        /* Start IPIPE operation */
        if (ioctl(hResize->fd, RSZ_RESIZE, &rsz) == -1) {
            Dmai_err0("Failed RSZ_RESIZE\n");
            return Dmai_EFAIL;
        }

        Buffer_setNumBytesUsed(hDstBuf, Buffer_getNumBytesUsed(hSrcBuf));

    }
    return Dmai_EOK;
}

/******************************************************************************
 * Resize_delete
 ******************************************************************************/
Int Resize_delete(Resize_Handle hResize)
{
    if (hResize) {
        close(hResize->fd);
        free(hResize);
    }

    return Dmai_EOK;
}

/******************************************************************************
 * Initialize & configure resizer in on-the-fly (continous) mode
 ******************************************************************************/
Int Resizer_continous_config(void)
{
	Int rsz_fd;
	unsigned int oper_mode, user_mode;
	struct rsz_channel_config rsz_chan_config;
	struct rsz_continuous_config rsz_cont_config;

    user_mode = IMP_MODE_CONTINUOUS;
	rsz_fd = open((const char *)RESIZER_DEVICE, O_RDWR);
	if(rsz_fd <= 0) {
		Dmai_err0("Cannot open resize device \n");
		return NULL;
	}
    
	if (ioctl(rsz_fd, RSZ_S_OPER_MODE, &user_mode) < 0) {
		Dmai_err1("Can't set operation mode (%s)\n", strerror(errno));
		close(rsz_fd);
		return Dmai_EFAIL;
	}

	if (ioctl(rsz_fd, RSZ_G_OPER_MODE, &oper_mode) < 0) {
		Dmai_err1("Can't get operation mode (%s)\n", strerror(errno));
		close(rsz_fd);
		return Dmai_EFAIL;
	}

	if (oper_mode == user_mode) {
		Dmai_dbg0("Successfully set mode to continuous in resizer\n");
	} else {
		Dmai_err0("Failed to set mode to continuous in resizer\n");
		close(rsz_fd);
		return Dmai_EFAIL;
	}
		
	/* set configuration to chain resizer with preview */
	rsz_chan_config.oper_mode = user_mode;
	rsz_chan_config.chain  = 1;
	rsz_chan_config.len = 0;
	rsz_chan_config.config = NULL; /* to set defaults in driver */
	if (ioctl(rsz_fd, RSZ_S_CONFIG, &rsz_chan_config) < 0) {
		Dmai_err1("Error in setting default configuration in resizer (%s)\n", 
            strerror(errno));
		close(rsz_fd);
		return Dmai_EFAIL;
	}
	
	bzero(&rsz_cont_config, sizeof(struct rsz_continuous_config));
	rsz_chan_config.oper_mode = user_mode;
	rsz_chan_config.chain = 1;
	rsz_chan_config.len = sizeof(struct rsz_continuous_config);
	rsz_chan_config.config = &rsz_cont_config;

	if (ioctl(rsz_fd, RSZ_G_CONFIG, &rsz_chan_config) < 0) {
		Dmai_err1("Error in getting channel configuration from resizer (%s)\n",
            strerror(errno));
		close(rsz_fd);
		return Dmai_EFAIL;
	}
		
	/* we can ignore the input spec since we are chaining. So only
	   set output specs */
	rsz_cont_config.output1.enable = 1;
	rsz_cont_config.output2.enable = 0;
	rsz_chan_config.len = sizeof(struct rsz_continuous_config);
	rsz_chan_config.config = &rsz_cont_config;
	if (ioctl(rsz_fd, RSZ_S_CONFIG, &rsz_chan_config) < 0) {
		Dmai_err1("Error in setting resizer configuration (%s)\n", 
            strerror(errno));
		close(rsz_fd);
		return Dmai_EFAIL;
	}
	Dmai_dbg0("Resizer A initialized\n");
	return rsz_fd;
}

/******************************************************************************
* Initialize & configure resizer B in on-the-fly (continous) mode
******************************************************************************/
int Resizer_B_config(int rsz_fd, unsigned int width, unsigned int height)
{
    unsigned int user_mode;
    struct rsz_channel_config rsz_chan_config;
    struct rsz_continuous_config rsz_cont_config;

    user_mode = IMP_MODE_CONTINUOUS;
    if(rsz_fd <= 0) {
       Dmai_err0("Cannot use resize device \n");
       return NULL;
    }
                                        
    bzero(&rsz_cont_config, sizeof(struct rsz_continuous_config));
    rsz_chan_config.oper_mode = user_mode;
    rsz_chan_config.chain = 1;
    rsz_chan_config.len = sizeof(struct rsz_continuous_config);
    rsz_chan_config.config = &rsz_cont_config;

    if (ioctl(rsz_fd, RSZ_G_CONFIG, &rsz_chan_config) < 0) {
        Dmai_err1("Error in getting channel configuration from resizer (%s)\n",
                   strerror(errno));
        return Dmai_EFAIL;
    }
                                                                                                      
    /* we can ignore the input spec since we are chaining. So only
       set output specs */
    rsz_cont_config.output2.width = width;
    rsz_cont_config.output2.height= height;
    rsz_cont_config.output2.pix_fmt = IPIPE_YUV420SP;
    rsz_cont_config.output2.enable = 1;
    rsz_chan_config.len = sizeof(struct rsz_continuous_config);
    rsz_chan_config.config = &rsz_cont_config;
    if (ioctl(rsz_fd, RSZ_S_CONFIG, &rsz_chan_config) < 0) {
        Dmai_err1("Error in setting resizer configuration (%s)\n", 
                   strerror(errno));
        return Dmai_EFAIL;
    }
    Dmai_dbg0("Resizer B initialized\n");
    return Dmai_EOK;
}

/******************************************************************************
 * Initialize & configure previewer in on-the-fly (continous) mode
 ******************************************************************************/
Int Previewer_continous_config(void)
{
	Int preview_fd;
	unsigned int oper_mode, user_mode;
	struct prev_channel_config prev_chan_config;
	struct prev_continuous_config prev_cont_config;
	
    user_mode = IMP_MODE_CONTINUOUS;
    
	preview_fd = open((const char *)PREVIEWER_DEVICE, O_RDWR);
	if(preview_fd <= 0) {
		Dmai_err0("Cannot open previewer device \n");
		return NULL;
	}

	if (ioctl(preview_fd,PREV_S_OPER_MODE, &user_mode) < 0) {
		Dmai_err1("Can't set operation mode in previewer (%s)\n", strerror(errno));
		close(preview_fd);        
		return Dmai_EFAIL;
	}

	if (ioctl(preview_fd,PREV_G_OPER_MODE, &oper_mode) < 0) {
		Dmai_err1("Can't get operation mode from previewer (%s)\n", strerror(errno));
		close(preview_fd);        
		return Dmai_EFAIL;
	}

	if (oper_mode == user_mode) {
		Dmai_dbg0("Operating mode changed successfully to continuous in previewer\n");
	} else {
		Dmai_err0("failed to set mode to continuous in previewer\n");
		close(preview_fd);
		return Dmai_EFAIL;
	}

	prev_chan_config.oper_mode = oper_mode;
	prev_chan_config.len = 0;
	prev_chan_config.config = NULL; /* to set defaults in driver */
	if (ioctl(preview_fd, PREV_S_CONFIG, &prev_chan_config) < 0) {
		Dmai_err1("Error in setting default previewer configuration (%s)\n", 
            strerror(errno));
		close(preview_fd);
		return Dmai_EFAIL;
	}

	prev_chan_config.oper_mode = oper_mode;
	prev_chan_config.len = sizeof(struct prev_continuous_config);
	prev_chan_config.config = &prev_cont_config;

	if (ioctl(preview_fd, PREV_G_CONFIG, &prev_chan_config) < 0) {
		Dmai_err1("Error in getting configuration from previewer (%s)\n", 
            strerror(errno));
		close(preview_fd);
		return Dmai_EFAIL;
	}
	
	prev_chan_config.oper_mode = oper_mode;
	prev_chan_config.len = sizeof(struct prev_continuous_config);
	prev_chan_config.config = &prev_cont_config;
	
	if (ioctl(preview_fd, PREV_S_CONFIG, &prev_chan_config) < 0) {
		Dmai_err1("Error in setting previewer configuration (%s)\n", 
            strerror(errno));
		close(preview_fd);
		return Dmai_EFAIL;
	}

	Dmai_dbg0("Previewer initialized\n");
	return preview_fd;
}

/******************************************************************************
 * Resizer_continous_delete
 ******************************************************************************/
Int Resizer_continous_delete(Int fd)
{
    if (fd) {
        if (close(fd) == -1) {
            Dmai_err1("Failed to close resizer device (%s)\n", strerror(errno));
            return Dmai_EIO;
        }        
    }
    return Dmai_EOK;
}

/******************************************************************************
 * Previewer_continous_delete
 ******************************************************************************/
Int Previewer_continous_delete(Int fd)
{
    if (fd) {
        if (close(fd) == -1) {
            Dmai_err1("Failed to close previewer device (%s)\n", strerror(errno));
            return Dmai_EIO;
        }        
    }
    return Dmai_EOK;
}

