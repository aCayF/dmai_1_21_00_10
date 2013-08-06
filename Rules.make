#============================================================================== 
# DMAI Stand-Alone Build Configuration
#============================================================================== 
ifndef DMAI_INSTALL_DIR
   $(error DMAI_INSTALL_DIR must be set to the DMAI installation directory prior to building DMAI)
endif

#------------------------------------------------------------------------------ 
# If this DMAI installation is part of a DVSDK, include the top level DVSDK
# Rules.make.  Otherwise, configure the build environment with reasonable
# values for a stand-alone build.
#------------------------------------------------------------------------------ 
-include $(DMAI_INSTALL_DIR)/../Rules.make

ifndef DVSDK_INSTALL_DIR

#------------------------------------------------------------------------------ 
# DVSDK SOFTWARE COMPONENTS:
#
#    Set the locations of the DVSDK installations and all required software
#    components.  Software component locations are specified with an
#    environment variable (e.g. CE_INSTALL_DIR).  This variable is typically
#    set to be the most recent version of the software supported by DMAI.  If a
#    platform requires an older version of a software component, this variable
#    is overridden on a per-platform basis.
#
#    Example:
#               CE_INSTALL_DIR=<path to most recent release supported by DMAI>
#    dm355_al:  CE_INSTALL_DIR=<path to older CE release needed by DM355>
#
#    Note that each platform override designation contains a suffix such as
#    "_al" or "_db".  This is to designate the operating system environment
#    for that platform.  The following operating system environments are
#    currently supported:
#
#      al:  ARM/Linux
#      db:  DSP/BIOS
#
#    This designation is required as some ARM+DSP platforms may support running
#    DMAI under multiple environments.
#------------------------------------------------------------------------------ 
           DVSDK_2_00_INSTALL_DIR=$(HOME)/dvsdk_2_00_00_19
           DVSDK_2_05_INSTALL_DIR=$(HOME)/dvsdk_2_05_00_14
           DVSDK_2_10_INSTALL_DIR=$(HOME)/dvsdk_2_10_00_14
           DVSDK_3_00_INSTALL_DIR=$(HOME)/dvsdk_3_00_00_29

# Where the Codec Engine package is installed.
           CE_INSTALL_DIR=$(DVSDK_2_00_INSTALL_DIR)/codec_engine_2_21
dm357_al:  CE_INSTALL_DIR=$(DVSDK_2_05_INSTALL_DIR)/codec_engine_2_21
dm365_al:  CE_INSTALL_DIR=$(DVSDK_2_10_INSTALL_DIR)/codec_engine_2_23_01
o3530_al:  CE_INSTALL_DIR=$(DVSDK_3_00_INSTALL_DIR)/codec_engine_2_20_01

# Where the DSP Link package is installed.
           LINK_INSTALL_DIR=$(DVSDK_2_00_INSTALL_DIR)/dsplink-1_60-prebuilt
dm357_al:  LINK_INSTALL_DIR=$(DVSDK_2_05_INSTALL_DIR)/dsplink-1_60-prebuilt
o3530_al:  LINK_INSTALL_DIR=$(DVSDK_3_00_INSTALL_DIR)/dsplink_1.51

# Where the CMEM (contiguous memory allocator) package is installed.
           CMEM_INSTALL_DIR=$(DVSDK_2_00_INSTALL_DIR)/linuxutils_2_22_01
dm357_al:  CMEM_INSTALL_DIR=$(DVSDK_2_05_INSTALL_DIR)/linuxutils_2_21
dm365_al:  CMEM_INSTALL_DIR=$(DVSDK_2_10_INSTALL_DIR)/linuxutils_2_24
o3530_al:  CMEM_INSTALL_DIR=$(DVSDK_3_00_INSTALL_DIR)/linuxutils_2_20

# Where Framework Components product is installed
           FC_INSTALL_DIR=$(DVSDK_2_00_INSTALL_DIR)/framework_components_2_21
dm357_al:  FC_INSTALL_DIR=$(DVSDK_2_05_INSTALL_DIR)/framework_components_2_21
dm365_al:  FC_INSTALL_DIR=$(DVSDK_2_10_INSTALL_DIR)/framework_components_2_23_02
o3530_al:  FC_INSTALL_DIR=$(DVSDK_3_00_INSTALL_DIR)/framework_components_2_20_01

# Where the BIOS utilities are installed.
           BIOSUTILS_INSTALL_DIR=$(DVSDK_2_00_INSTALL_DIR)/biosutils_1_01_00
dm357_al:  BIOSUTILS_INSTALL_DIR=$(DVSDK_2_05_INSTALL_DIR)/biosutils_1_01_00
o3530_al:  BIOSUTILS_INSTALL_DIR=$(DVSDK_3_00_INSTALL_DIR)/biosutils_1_01_00

# Where the Local Power Management product is installed
o3530_al:  LPM_INSTALL_DIR=$(DVSDK_3_00_INSTALL_DIR)/local_power_manager_1_20_01

# Where the XDAIS package is installed.
           XDAIS_INSTALL_DIR=$(DVSDK_2_00_INSTALL_DIR)/xdais_6_22_01
dm357_al:  XDAIS_INSTALL_DIR=$(DVSDK_2_05_INSTALL_DIR)/xdais_6_21
dm365_al:  XDAIS_INSTALL_DIR=$(DVSDK_2_10_INSTALL_DIR)/xdais_6_23
o3530_al:  XDAIS_INSTALL_DIR=$(DVSDK_3_00_INSTALL_DIR)/xdais_6_20

# Where the RTSC tools package is installed.
           XDC_INSTALL_DIR=$(HOME)/xdctools_3_10_03

# Where DSP/BIOS is installed
           BIOS_INSTALL_DIR=$(HOME)/bios_5_33_02
o3530_al:  BIOS_INSTALL_DIR=$(HOME)/bios_5_32_04

# Where RTDX is installed
           RTDX_INSTALL_DIR=$(HOME)/rtdx_2_00_01

# Where EDMA3 LLD is installed
           EDMA3_LLD_INSTALL_DIR=$(DVSDK_2_00_INSTALL_DIR)/edma3_lld_1_05_00
dm365_al:  EDMA3_LLD_INSTALL_DIR=$(DVSDK_2_10_INSTALL_DIR)/edma3_lld_1_06_00_01

# Linux support libraries installation directory
o3530_al:  LINUXLIBS_INSTALL_DIR=$(DVSDK_3_00_INSTALL_DIR)/linuxlibs

# Linux kernel installation directory
           LINUXKERNEL_INSTALL_DIR=$(HOME)/DaVinciLSP_02_00_00_100
dm365_al:  LINUXKERNEL_INSTALL_DIR=$(HOME)/LSP_02_10_00_14
o3530_al:  LINUXKERNEL_INSTALL_DIR=$(HOME)/OMAP35x_SDK_1.0.2/src/linux/kernel_org/2.6_kernel

# Where codecs are installed
dm6467_al: CODEC_INSTALL_DIR=$(DVSDK_2_00_INSTALL_DIR)/dm6467_dvsdk_combos_2_03
dm355_al:  CODEC_INSTALL_DIR=$(DVSDK_2_00_INSTALL_DIR)/dm355_codecs_1_13_000
dm6446_al: CODEC_INSTALL_DIR=$(DVSDK_2_00_INSTALL_DIR)/dm6446_dvsdk_combos_2_02
dm357_al:  CODEC_INSTALL_DIR=$(DVSDK_2_05_INSTALL_DIR)/dm357_codecs_1_00_004
dm365_al:  CODEC_INSTALL_DIR=$(DVSDK_2_10_INSTALL_DIR)/dm365_codecs_01_00_04
o3530_al:  CODEC_INSTALL_DIR=$(DVSDK_3_00_INSTALL_DIR)/omap3530_dvsdk_combos_3_16
dm6437_db: CODEC_INSTALL_DIR=$(DVSDK_2_00_INSTALL_DIR)/dm6446_dvsdk_combos_2_03
dm6446_db: CODEC_INSTALL_DIR=$(DVSDK_2_00_INSTALL_DIR)/dm6446_dvsdk_combos_2_03
dm6467_db: CODEC_INSTALL_DIR=$(DVSDK_2_00_INSTALL_DIR)/dm6467_dvsdk_combos_2_03

#------------------------------------------------------------------------------ 
# TI C6000 CODEGEN DEVELOPMENT TOOLS:
#    Set the location of the TI C6000 codegen tools.
#------------------------------------------------------------------------------ 
           CODEGEN_INSTALL_DIR=$(HOME)/cg6x_6_0_16

#------------------------------------------------------------------------------ 
# MONTAVISTA DEVELOPMENT TOOLS:
#    Set the location of the MontaVista Linux components.
#------------------------------------------------------------------------------ 
           MVTOOL_INSTALL_DIR=$(HOME)/mv_pro_5.0
           MVTOOL_DIR=$(MVTOOL_INSTALL_DIR)/montavista/pro/devkit/arm/v5t_le
           MVTOOL_PREFIX=$(MVTOOL_DIR)/bin/arm_v5t_le-

#------------------------------------------------------------------------------ 
# CODE SOURCERY DEVELOPMENT TOOLS:
#    Set the location of the Code Sourcery Linux components.
#------------------------------------------------------------------------------ 
           CSTOOL_DIR=$(HOME)/arm-2007q3
           CSTOOL_PREFIX=$(CSTOOL_DIR)/bin/arm-none-linux-gnueabi-

#------------------------------------------------------------------------------ 
# USER_XDC_PATH:
#    Pre-pend the following paths to the XDC path used when building the
#    DMAI sample applications.
#------------------------------------------------------------------------------ 
USER_XDC_PATH=

#------------------------------------------------------------------------------ 
# TARGET FILESYSTEM:
#     Where to copy the resulting executables and data to (when executing
#     'make install') in a proper file structure. This EXEC_DIR should either
#     be visible from the target, or you will have to copy this (whole)
#     directory onto the target filesystem.
#------------------------------------------------------------------------------ 
EXEC_DIR=$(HOME)/workdir/filesys/opt/dvsdk

endif
