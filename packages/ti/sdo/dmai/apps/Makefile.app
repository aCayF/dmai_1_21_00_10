# --COPYRIGHT--,BSD
#  Copyright (c) 2009, Texas Instruments Incorporated
#  All rights reserved.
# 
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions
#  are met:
# 
#  *  Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
# 
#  *  Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in the
#     documentation and/or other materials provided with the distribution.
# 
#  *  Neither the name of Texas Instruments Incorporated nor the names of
#     its contributors may be used to endorse or promote products derived
#     from this software without specific prior written permission.
# 
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
#  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
#  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
#  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
#  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
#  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
#  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
#  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
#  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
#  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
#  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
# --/COPYRIGHT--

# Use the main DMAI Rules.make for building items in thie directory
ifndef DMAI_INSTALL_DIR
    DMAI_INSTALL_DIR = ../../../../../..
endif
include $(DMAI_INSTALL_DIR)/Rules.make

# Should the full command be echoed to the console during build?
VERBOSE=false

ifeq ($(VERBOSE), true)
    override PRE	=
else
    override PRE	= @
endif

# Which compiler flags should be used?
BUILD_TYPE=debug

ifeq ($(BUILD_TYPE), release)
    CPP_FLAGS		=
    C_FLAGS		= -O2
else
    CPP_FLAGS		= -DNDEBUG
    C_FLAGS		= -g
endif

TARGET = $(notdir $(CURDIR))

# Package path for the XDC tools
XDC_PATH = $(USER_XDC_PATH);$(DMAI_INSTALL_DIR)/packages;$(CE_INSTALL_DIR)/packages;$(CE_INSTALL_DIR)/examples;$(FC_INSTALL_DIR)/packages;$(LINK_INSTALL_DIR)/packages;$(XDAIS_INSTALL_DIR)/packages;$(CMEM_INSTALL_DIR)/packages;$(CODEC_INSTALL_DIR)/packages;$(BIOS_INSTALL_DIR)/packages;$(BIOSUTILS_INSTALL_DIR)/packages;$(RTDX_INSTALL_DIR)/packages;$(EDMA3_LLD_INSTALL_DIR)/packages;$(LPM_INSTALL_DIR)/packages

DM6446_AL_TARGET	= linux/$(TARGET)_dm6446
DM6467_AL_TARGET	= linux/$(TARGET)_dm6467
DM355_AL_TARGET	    	= linux/$(TARGET)_dm355
DM357_AL_TARGET	    	= linux/$(TARGET)_dm357
DM365_AL_TARGET	    	= linux/$(TARGET)_dm365
O3530_AL_TARGET		= linux/$(TARGET)_omap3530
DM6437_DB_TARGET	= bios/$(TARGET)_dm6437
DM6446_DB_TARGET	= bios/$(TARGET)_dm6446
DM6467_DB_TARGET        = bios/$(TARGET)_dm6467

# Where to output configuration files
DM6446_AL_XDC_CFG	= linux/$(TARGET)_dm6446_config
DM6467_AL_XDC_CFG	= linux/$(TARGET)_dm6467_config
DM355_AL_XDC_CFG	= linux/$(TARGET)_dm355_config
DM357_AL_XDC_CFG	= linux/$(TARGET)_dm357_config
DM365_AL_XDC_CFG	= linux/$(TARGET)_dm365_config
O3530_AL_XDC_CFG	= linux/$(TARGET)_omap3530_config
DM6437_DB_XDC_CFG	= bios/$(TARGET)_dm6437_config
DM6446_DB_XDC_CFG	= bios/$(TARGET)_dm6446_config
DM6467_DB_XDC_CFG       = bios/$(TARGET)_dm6467_config

# Executable names
DM6446_AL_EXEC		= $(DM6446_AL_TARGET).x470MV
DM6467_AL_EXEC		= $(DM6467_AL_TARGET).x470MV
DM355_AL_EXEC		= $(DM355_AL_TARGET).x470MV
DM357_AL_EXEC		= $(DM357_AL_TARGET).x470MV
DM365_AL_EXEC		= $(DM365_AL_TARGET).x470MV
O3530_AL_EXEC		= $(O3530_AL_TARGET).x470MV
DM6437_DB_EXEC		= $(DM6437_DB_TARGET).x64P
DM6446_DB_EXEC		= $(DM6446_DB_TARGET).x64P
DM6467_DB_EXEC          = $(DM6467_DB_TARGET).x64P

# Map files
DM6446_AL_MAP		= $(DM6446_AL_EXEC).map
DM6467_AL_MAP		= $(DM6467_AL_EXEC).map
DM355_AL_MAP		= $(DM355_AL_EXEC).map
DM357_AL_MAP		= $(DM357_AL_EXEC).map
DM365_AL_MAP		= $(DM365_AL_EXEC).map
O3530_AL_MAP		= $(O3530_AL_EXEC).map
DM6437_DB_MAP		= $(DM6437_DB_EXEC).map
DM6446_DB_MAP		= $(DM6446_DB_EXEC).map
DM6467_DB_MAP           = $(DM6467_DB_EXEC).map

# DMAI libraries
DMAI_LIB_DIR		= $(DMAI_INSTALL_DIR)/packages/ti/sdo/dmai/lib
DM6446_AL_LIBS		= $(DMAI_LIB_DIR)/dmai_linux_dm6446.a470MV
DM6467_AL_LIBS		= $(DMAI_LIB_DIR)/dmai_linux_dm6467.a470MV
DM355_AL_LIBS		= $(DMAI_LIB_DIR)/dmai_linux_dm355.a470MV
DM357_AL_LIBS		= $(DMAI_LIB_DIR)/dmai_linux_dm357.a470MV
DM365_AL_LIBS		= $(DMAI_LIB_DIR)/dmai_linux_dm365.a470MV
O3530_AL_LIBS		= $(DMAI_LIB_DIR)/dmai_linux_omap3530.a470MV
DM6437_DB_LIBS		= $(DMAI_LIB_DIR)/dmai_bios_dm6437.a64P
DM6446_DB_LIBS		= $(DMAI_LIB_DIR)/dmai_bios_dm6446.a64P
DM6467_DB_LIBS		= $(DMAI_LIB_DIR)/dmai_bios_dm6467.a64P

# Output compiler options
DM6446_AL_XDC_CFLAGS	= $(DM6446_AL_XDC_CFG)/compiler.opt
DM6467_AL_XDC_CFLAGS	= $(DM6467_AL_XDC_CFG)/compiler.opt
DM355_AL_XDC_CFLAGS	= $(DM355_AL_XDC_CFG)/compiler.opt
DM357_AL_XDC_CFLAGS	= $(DM357_AL_XDC_CFG)/compiler.opt
DM365_AL_XDC_CFLAGS	= $(DM365_AL_XDC_CFG)/compiler.opt
O3530_AL_XDC_CFLAGS	= $(O3530_AL_XDC_CFG)/compiler.opt
DM6437_DB_XDC_CFLAGS	= $(DM6437_DB_XDC_CFG)/compiler.opt
DM6446_DB_XDC_CFLAGS	= $(DM6446_DB_XDC_CFG)/compiler.opt
DM6467_DB_XDC_CFLAGS    = $(DM6467_DB_XDC_CFG)/compiler.opt

# Output linker file
DM6446_AL_XDC_LFILE	= $(DM6446_AL_XDC_CFG)/linker.cmd
DM6467_AL_XDC_LFILE	= $(DM6467_AL_XDC_CFG)/linker.cmd
DM355_AL_XDC_LFILE	= $(DM355_AL_XDC_CFG)/linker.cmd
DM357_AL_XDC_LFILE	= $(DM357_AL_XDC_CFG)/linker.cmd
DM365_AL_XDC_LFILE	= $(DM365_AL_XDC_CFG)/linker.cmd
O3530_AL_XDC_LFILE	= $(O3530_AL_XDC_CFG)/linker.cmd
DM6437_DB_XDC_LFILE	= $(DM6437_DB_XDC_CFG)/linker.cmd
DM6446_DB_XDC_LFILE	= $(DM6446_DB_XDC_CFG)/linker.cmd
DM6467_DB_XDC_LFILE     = $(DM6467_DB_XDC_CFG)/linker.cmd

# Input configuration file
DM6446_AL_XDC_CFGFILE	= $(DM6446_AL_TARGET).cfg
DM6467_AL_XDC_CFGFILE	= $(DM6467_AL_TARGET).cfg
DM355_AL_XDC_CFGFILE	= $(DM355_AL_TARGET).cfg
DM357_AL_XDC_CFGFILE	= $(DM357_AL_TARGET).cfg
DM365_AL_XDC_CFGFILE	= $(DM365_AL_TARGET).cfg
O3530_AL_XDC_CFGFILE	= $(O3530_AL_TARGET).cfg
DM6437_DB_XDC_CFGFILE	= $(DM6437_DB_TARGET).cfg
DM6446_DB_XDC_CFGFILE	= $(DM6446_DB_TARGET).cfg
DM6467_DB_XDC_CFGFILE   = $(DM6467_DB_TARGET).cfg

# Platform (board) to build for
DM6446_AL_XDC_PLATFORM	= ti.platforms.evmDM6446
DM6467_AL_XDC_PLATFORM	= ti.platforms.evmDM6467
DM355_AL_XDC_PLATFORM	= ti.platforms.evmDM355
DM357_AL_XDC_PLATFORM	= ti.platforms.evmDM357
DM365_AL_XDC_PLATFORM	= ti.platforms.evmDM365
O3530_AL_XDC_PLATFORM	= ti.platforms.evm3530
DM6437_DB_XDC_PLATFORM	= ti.platforms.evmDM6437  
DM6446_DB_XDC_PLATFORM	= ti.platforms.evmDM6446  
DM6467_DB_XDC_PLATFORM  = ti.platforms.evmDM6467

# Target tools
MVL_XDC_TARGET 		= gnu.targets.MVArm9
CS_XDC_TARGET		= gnu.targets.arm.GCArmv5T
C64P_XDC_TARGET 	= ti.targets.C64P

# The XDC configuration tool command line
CONFIGURO		= $(XDC_INSTALL_DIR)/xs xdc.tools.configuro
CONFIG_BLD		= $(DMAI_INSTALL_DIR)/packages/config.bld

GNU_C_FLAGS 		= $(C_FLAGS) -Wall -Werror -g
C64P_C_FLAGS		= $(C_FLAGS) -g 

GNU_CPP_FLAGS  		= $(CPP_FLAGS) -I$(LINUXKERNEL_INSTALL_DIR)/include
C64P_CPP_FLAGS		= $(CPP_FLAGS) -pdse225 -I$(CODEGEN_INSTALL_DIR)/include -mv6400+ -I$(BIOS_INSTALL_DIR)/packages/ti/bios/include

GNU_LD_FLAGS		= $(LD_FLAGS) -lpthread -lm -L$(LINUXLIBS_INSTALL_DIR)/lib -lasound
C64P_LD_FLAGS		= $(LD_FLAGS) -z -w -x -c -i$(RTDX_INSTALL_DIR)/packages/ti/rtdx/iom/lib/debug -i$(RTDX_INSTALL_DIR)/packages/ti/rtdx/cio/lib/release -i$(RTDX_INSTALL_DIR)/packages/ti/rtdx/lib/c6000 

DM6437_DB_CPP_FLAGS  	= $(C64P_CPP_FLAGS) -eodm6437.o64P
DM6446_DB_CPP_FLAGS	= $(C64P_CPP_FLAGS) -eodm6446.o64P
DM6467_DB_CPP_FLAGS     = $(C64P_CPP_FLAGS) -DDmai_Device_dm6467 -eodm6467.o64P

SOURCES 		= $(wildcard *.c)
LINUX_SOURCES		= $(SOURCES) $(wildcard linux/*.c)
BIOS_SOURCES		= $(SOURCES) $(wildcard bios/*.c)

HEADERS			= $(wildcard *.h)
LINUX_HEADERS		= $(HEADERS) $(wildcard linux/*.h)
BIOS_HEADERS		= $(HEADERS) $(wildcard bios/*.h)

DM6446_LINUX_OBJFILES	= $(LINUX_SOURCES:%.c=%.dm6446.o470MV)
DM6467_LINUX_OBJFILES	= $(LINUX_SOURCES:%.c=%.dm6467.o470MV)
DM355_LINUX_OBJFILES	= $(LINUX_SOURCES:%.c=%.dm355.o470MV)
DM357_LINUX_OBJFILES	= $(LINUX_SOURCES:%.c=%.dm357.o470MV)
DM365_LINUX_OBJFILES	= $(LINUX_SOURCES:%.c=%.dm365.o470MV)
O3530_LINUX_OBJFILES	= $(LINUX_SOURCES:%.c=%.o3530.o470MV)
DM6437_BIOS_OBJFILES	= $(BIOS_SOURCES:%.c=%.dm6437.o64P)
DM6446_BIOS_OBJFILES	= $(BIOS_SOURCES:%.c=%.dm6446.o64P)
DM6467_BIOS_OBJFILES    = $(BIOS_SOURCES:%.c=%.dm6467.o64P)

MVL_COMPILE.c 		= $(PRE) $(MVTOOL_PREFIX)gcc $(GNU_C_FLAGS) $(GNU_CPP_FLAGS) -c
MVL_LINK.c 		= $(PRE) $(MVTOOL_PREFIX)gcc $(GNU_LD_FLAGS)

CS_COMPILE.c		= $(PRE) $(CSTOOL_PREFIX)gcc $(CS_CPP_FLAGS) $(GNU_C_FLAGS) -c
CS_LINK.c		= $(PRE) $(CSTOOL_PREFIX)gcc $(GNU_LD_FLAGS)

C64P_COMPILE.c 		= $(PRE) $(CODEGEN_INSTALL_DIR)/bin/cl6x $(C64P_C_FLAGS) $(C64P_CPP_FLAGS) -c
C64P_LINK.c 		= $(PRE) $(CODEGEN_INSTALL_DIR)/bin/cl6x $(C64P_LD_FLAGS)

.PHONY: clean install o3530_al dm357_al dm6446_al dm6467_al dm355_al dm365_al dm6437_db dm6446_db o3530_al_install dm350_al_install dm6446_al_install dm6467_al_install dm355_al_install dm365_al_install dm6446_db_install dm6437_db_install dm6467_db_install

help:
	@echo
	@echo "You must specify a build target. Available targets are:"
	@echo
	@echo "    dm6446_al   : ARM Linux dm6446"
	@echo "    dm6467_al   : ARM Linux dm6467"
	@echo "    dm355_al    : ARM Linux dm355"
	@echo "    dm365_al    : ARM Linux dm365"
	@echo "    dm357_al    : ARM Linux dm357"
	@echo "    o3530_al    : ARM Linux omap3530"
	@echo
	@echo "    dm6446_db   : DSP DSP/BIOS dm6446"
	@echo "    dm6467_db   : DSP DSP/BIOS dm6467"
	@echo "    dm6437_db   : DSP DSP/BIOS dm6437"
	@echo
	@echo "    all:        : build for all targets"
	@echo "    install:    : install binaries"
	@echo "    clean:      : remove all generated files"
	@echo

all:		o3530_al dm357_al dm6446_al dm6467_al dm355_al dm6437_db dm6446_db dm6467_db

dm6446_al:	$(if $(wildcard $(DM6446_AL_XDC_CFGFILE)), $(DM6446_AL_EXEC))

dm6467_al:	$(if $(wildcard $(DM6467_AL_XDC_CFGFILE)), $(DM6467_AL_EXEC))

dm355_al:	$(if $(wildcard $(DM355_AL_XDC_CFGFILE)), $(DM355_AL_EXEC))

dm357_al:	$(if $(wildcard $(DM357_AL_XDC_CFGFILE)), $(DM357_AL_EXEC))

dm365_al:	$(if $(wildcard $(DM365_AL_XDC_CFGFILE)), $(DM365_AL_EXEC))

o3530_al:	$(if $(wildcard $(O3530_AL_XDC_CFGFILE)), $(O3530_AL_EXEC))

dm6437_db:	$(if $(wildcard $(DM6437_DB_XDC_CFGFILE)), $(DM6437_DB_EXEC))

dm6446_db:	$(if $(wildcard $(DM6446_DB_XDC_CFGFILE)), $(DM6446_DB_EXEC))

dm6467_db:      $(if $(wildcard $(DM6467_DB_XDC_CFGFILE)), $(DM6467_DB_EXEC))

dm6446_al_install:
	$(PRE) install -d $(EXEC_DIR)
	$(PRE) install $(DM6446_AL_EXEC) $(EXEC_DIR)

dm6467_al_install:
	$(PRE) install -d $(EXEC_DIR)
	$(PRE) install $(DM6467_AL_EXEC) $(EXEC_DIR)

dm355_al_install:
	$(PRE) install -d $(EXEC_DIR)
	$(PRE) install $(DM355_AL_EXEC) $(EXEC_DIR)

dm357_al_install:
	$(PRE) install -d $(EXEC_DIR)
	$(PRE) install $(DM357_AL_EXEC) $(EXEC_DIR)

dm365_al_install:
	$(PRE) install -d $(EXEC_DIR)
	$(PRE) install $(DM365_AL_EXEC) $(EXEC_DIR)

o3530_al_install:
	$(PRE) install -d $(EXEC_DIR)
	$(PRE) install $(O3530_AL_EXEC) $(EXEC_DIR)

dm6437_db_install:
	$(PRE) install -d $(EXEC_DIR)
	$(PRE) install $(DM6437_DB_EXEC) $(EXEC_DIR)

dm6446_db_install:
	$(PRE) install -d $(EXEC_DIR)
	$(PRE) install $(DM6446_DB_EXEC) $(EXEC_DIR)
    	
dm6467_db_install:
	$(PRE) install -d $(EXEC_DIR)
	$(PRE) install $(DM6467_DB_EXEC) $(EXEC_DIR)

install:	$(if $(wildcard $(DM6446_AL_EXEC)), dm6446_al_install) $(if $(wildcard $(DM6467_AL_EXEC)), dm6467_al_install) $(if $(wildcard $(DM355_AL_EXEC)), dm355_al_install) $(if $(wildcard $(O3530_AL_EXEC)), o3530_al_install) $(if $(wildcard $(DM357_AL_EXEC)), dm357_al_install) $(if $(wildcard $(DM365_AL_EXEC)), dm365_al_install) $(if $(wildcard $(DM6437_DB_EXEC)), dm6437_db_install) $(if $(wildcard $(DM6446_DB_EXEC)), dm6446_db_install) $(if $(wildcard $(DM6467_DB_EXEC)), dm6467_db_install)
	@echo
	@echo Installed $(TARGET) binaries to $(EXEC_DIR)..

$(DM6446_AL_EXEC):	$(DM6446_LINUX_OBJFILES) $(DM6446_AL_XDC_LFILE) \
			$(DM6446_AL_LIBS)
	@echo
	@echo Linking $@ from $^..
	$(MVL_LINK.c) -Wl,-Map,$(DM6446_AL_MAP) -o $@ $^

$(DM6467_AL_EXEC):	$(DM6467_LINUX_OBJFILES) $(DM6467_AL_XDC_LFILE) \
			$(DM6467_AL_LIBS)
	@echo
	@echo Linking $@ from $^..
	$(MVL_LINK.c) -Wl,-Map,$(DM6467_AL_MAP) -o $@ $^

$(DM355_AL_EXEC):	$(DM355_LINUX_OBJFILES) $(DM355_AL_XDC_LFILE) \
			$(DM355_AL_LIBS)
	@echo
	@echo Linking $@ from $^..
	$(MVL_LINK.c) -Wl,-Map,$(DM355_AL_MAP) -o $@ $^

$(DM357_AL_EXEC):	$(DM357_LINUX_OBJFILES) $(DM357_AL_XDC_LFILE) \
			$(DM357_AL_LIBS)
	@echo
	@echo Linking $@ from $^..
	$(MVL_LINK.c) -Wl,-Map,$(DM357_AL_MAP) -o $@ $^

$(DM365_AL_EXEC):	$(DM365_LINUX_OBJFILES) $(DM365_AL_XDC_LFILE) \
			$(DM365_AL_LIBS)
	@echo
	@echo Linking $@ from $^..
	$(MVL_LINK.c) -Wl,-Map,$(DM365_AL_MAP) -o $@ $^

$(O3530_AL_EXEC):	$(O3530_LINUX_OBJFILES) $(O3530_AL_XDC_LFILE) \
			$(O3530_AL_LIBS)
	@echo
	@echo Linking $@ from $^..
	$(CS_LINK.c) -Wl,-Map,$(O3530_AL_MAP) -o $@ $^

$(DM6437_DB_EXEC):	$(DM6437_BIOS_OBJFILES) $(DM6437_DB_XDC_LFILE) \
			$(DM6437_DB_LIBS)
	@echo
	@echo Linking $@ from $^..
	$(C64P_LINK.c) -m$(DM6437_DB_MAP) -o $@ $^

$(DM6446_DB_EXEC):	$(DM6446_BIOS_OBJFILES) $(DM6446_DB_XDC_LFILE) \
			$(DM6446_DB_LIBS)
	@echo
	@echo Linking $@ from $^..
	$(C64P_LINK.c) -m$(DM6446_DB_MAP) -o $@ $^

$(DM6467_DB_EXEC):      $(DM6467_BIOS_OBJFILES) $(DM6467_DB_XDC_LFILE) \
			$(DM6467_DB_LIBS)
	@echo
	@echo Linking $@ from $^..
	$(C64P_LINK.c) -m$(DM6467_DB_MAP) -o $@ $^
    	
$(DM6446_LINUX_OBJFILES):	%.dm6446.o470MV: %.c $(LINUX_HEADERS) $(DM6446_AL_XDC_CFLAGS)
	@echo Compiling $@ from $<..
	$(MVL_COMPILE.c) $(shell cat $(DM6446_AL_XDC_CFLAGS)) $(GNU_CPP_FLAGS) -o $@ $<

$(DM6467_LINUX_OBJFILES):	%.dm6467.o470MV: %.c $(LINUX_HEADERS) $(DM6467_AL_XDC_CFLAGS)
	@echo Compiling $@ from $<..
	$(MVL_COMPILE.c) $(shell cat $(DM6467_AL_XDC_CFLAGS)) $(GNU_CPP_FLAGS) -o $@ $<

$(DM355_LINUX_OBJFILES):	%.dm355.o470MV: %.c $(LINUX_HEADERS) $(DM355_AL_XDC_CFLAGS)
	@echo Compiling $@ from $<..
	$(MVL_COMPILE.c) $(shell cat $(DM355_AL_XDC_CFLAGS)) $(GNU_CPP_FLAGS) -o $@ $<

$(DM357_LINUX_OBJFILES):	%.dm357.o470MV: %.c $(LINUX_HEADERS) $(DM357_AL_XDC_CFLAGS)
	@echo Compiling $@ from $<..
	$(MVL_COMPILE.c) $(shell cat $(DM357_AL_XDC_CFLAGS)) $(GNU_CPP_FLAGS) -o $@ $<

$(DM365_LINUX_OBJFILES):	%.dm365.o470MV: %.c $(LINUX_HEADERS) $(DM365_AL_XDC_CFLAGS)
	@echo Compiling $@ from $<..
	$(MVL_COMPILE.c) $(shell cat $(DM365_AL_XDC_CFLAGS)) $(GNU_CPP_FLAGS) -o $@ $<

$(O3530_LINUX_OBJFILES):	%.o3530.o470MV: %.c $(LINUX_HEADERS) $(O3530_AL_XDC_CFLAGS)
	@echo Compiling $@ from $<..
	$(CS_COMPILE.c) $(shell cat $(O3530_AL_XDC_CFLAGS)) $(GNU_CPP_FLAGS) -o $@ $<

$(DM6437_BIOS_OBJFILES):	%.dm6437.o64P: %.c $(BIOS_HEADERS) $(DM6437_DB_XDC_CFLAGS)
	@echo Compiling $@ from $<..
	$(C64P_COMPILE.c) $(shell cat $(DM6437_DB_XDC_CFLAGS)) $(DM6437_DB_CPP_FLAGS) $< -fr"$(dir $<)"

$(DM6446_BIOS_OBJFILES):	%.dm6446.o64P: %.c $(BIOS_HEADERS) $(DM6446_DB_XDC_CFLAGS)
	@echo Compiling $@ from $<..
	$(C64P_COMPILE.c) $(shell cat $(DM6446_DB_XDC_CFLAGS)) $(DM6446_DB_CPP_FLAGS) $< -fr"$(dir $<)"
	
$(DM6467_BIOS_OBJFILES):        %.dm6467.o64P: %.c $(BIOS_HEADERS) $(DM6467_DB_XDC_CFLAGS)
	@echo Compiling $@ from $<..
	$(C64P_COMPILE.c) $(shell cat $(DM6467_DB_XDC_CFLAGS)) $(DM6467_DB_CPP_FLAGS) $< -fr"$(dir $<)"

$(DM6446_AL_XDC_LFILE) $(DM6446_AL_XDC_CFLAGS):	$(DM6446_AL_XDC_CFGFILE)
	@echo
	@echo ======== Building $(DM6446_AL_TARGET) ========
	@echo Configuring application using $<
	@echo
	$(PRE) XDCPATH="$(XDC_PATH)" $(CONFIGURO) -c $(MVTOOL_DIR) -o $(DM6446_AL_XDC_CFG) -t $(MVL_XDC_TARGET) -p $(DM6446_AL_XDC_PLATFORM) -b $(CONFIG_BLD) $(DM6446_AL_XDC_CFGFILE)

$(DM6467_AL_XDC_LFILE) $(DM6467_AL_XDC_CFLAGS):	$(DM6467_AL_XDC_CFGFILE)
	@echo
	@echo ======== Building $(DM6467_AL_TARGET) ========
	@echo Configuring application using $<
	@echo
	$(PRE) XDCPATH="$(XDC_PATH)" $(CONFIGURO) -c $(MVTOOL_DIR) -o $(DM6467_AL_XDC_CFG) -t $(MVL_XDC_TARGET) -p $(DM6467_AL_XDC_PLATFORM) -b $(CONFIG_BLD) $(DM6467_AL_XDC_CFGFILE)

$(DM355_AL_XDC_LFILE) $(DM355_AL_XDC_CFLAGS):	$(DM355_AL_XDC_CFGFILE)
	@echo
	@echo ======== Building $(DM355_AL_TARGET) ========
	@echo Configuring application using $<
	@echo
	$(PRE) XDCPATH="$(XDC_PATH)" $(CONFIGURO) -c $(MVTOOL_DIR) -o $(DM355_AL_XDC_CFG) -t $(MVL_XDC_TARGET) -p $(DM355_AL_XDC_PLATFORM) -b $(CONFIG_BLD) $(DM355_AL_XDC_CFGFILE)

$(DM357_AL_XDC_LFILE) $(DM357_AL_XDC_CFLAGS):	$(DM357_AL_XDC_CFGFILE)
	@echo
	@echo ======== Building $(DM357_AL_TARGET) ========
	@echo Configuring application using $<
	@echo
	$(PRE) XDCPATH="$(XDC_PATH)" $(CONFIGURO) -c $(MVTOOL_DIR) -o $(DM357_AL_XDC_CFG) -t $(MVL_XDC_TARGET) -p $(DM357_AL_XDC_PLATFORM) -b $(CONFIG_BLD) $(DM357_AL_XDC_CFGFILE)

$(DM365_AL_XDC_LFILE) $(DM365_AL_XDC_CFLAGS):	$(DM365_AL_XDC_CFGFILE)
	@echo
	@echo ======== Building $(DM365_AL_TARGET) ========
	@echo Configuring application using $<
	@echo
	$(PRE) XDCPATH="$(XDC_PATH)" $(CONFIGURO) -c $(MVTOOL_DIR) -o $(DM365_AL_XDC_CFG) -t $(MVL_XDC_TARGET) -p $(DM365_AL_XDC_PLATFORM) -b $(CONFIG_BLD) $(DM365_AL_XDC_CFGFILE)

$(O3530_AL_XDC_LFILE) $(O3530_AL_XDC_CFLAGS):	$(O3530_AL_XDC_CFGFILE)
	@echo
	@echo ======== Building $(O3530_AL_TARGET) ========
	@echo Configuring application using $<
	@echo
	$(PRE) XDCPATH="$(XDC_PATH)" $(CONFIGURO) -c $(CSTOOL_DIR) -o $(O3530_AL_XDC_CFG) -t $(CS_XDC_TARGET) -p $(O3530_AL_XDC_PLATFORM) -b $(CONFIG_BLD) $(O3530_AL_XDC_CFGFILE)

$(DM6437_DB_XDC_LFILE) $(DM6437_DB_XDC_CFLAGS):	$(DM6437_DB_XDC_CFGFILE)
	@echo
	@echo ======== Building $(DM6437_DB_TARGET) ========
	@echo Configuring application using $<
	@echo
	$(PRE) XDCPATH="$(XDC_PATH)" $(CONFIGURO) -c $(CODEGEN_INSTALL_DIR) -o $(DM6437_DB_XDC_CFG) -t $(C64P_XDC_TARGET) -p $(DM6437_DB_XDC_PLATFORM) -b $(CONFIG_BLD) --tcf $(DM6437_DB_XDC_CFGFILE)

$(DM6446_DB_XDC_LFILE) $(DM6446_DB_XDC_CFLAGS):	$(DM6446_DB_XDC_CFGFILE)
	@echo
	@echo ======== Building $(DM6446_DB_TARGET) ========
	@echo Configuring application using $<
	@echo
	$(PRE) XDCPATH="$(XDC_PATH)" $(CONFIGURO) -c $(CODEGEN_INSTALL_DIR) -o $(DM6446_DB_XDC_CFG) -t $(C64P_XDC_TARGET) -p $(DM6446_DB_XDC_PLATFORM) -b $(CONFIG_BLD) --tcf $(DM6446_DB_XDC_CFGFILE)
    	
$(DM6467_DB_XDC_LFILE) $(DM6467_DB_XDC_CFLAGS): $(DM6467_DB_XDC_CFGFILE)
	@echo
	@echo ======== Building $(DM6467_DB_TARGET) ========
	@echo Configuring application using $<
	@echo
	$(PRE) XDCPATH="$(XDC_PATH)" $(CONFIGURO) -c $(CODEGEN_INSTALL_DIR) -o $(DM6467_DB_XDC_CFG) -t $(C64P_XDC_TARGET) -p $(DM6467_DB_XDC_PLATFORM) -b $(CONFIG_BLD) --tcf $(DM6467_DB_XDC_CFGFILE)

clean:
	@echo Removing generated files..
	$(PRE) -$(RM) -rf $(DM6446_AL_XDC_CFG) $(DM6467_AL_XDC_CFG) $(DM355_AL_XDC_CFG) $(O3530_AL_XDC_CFG) $(DM357_AL_XDC_CFG) $(DM365_AL_XDC_CFG) $(DM6437_DB_XDC_CFG) $(DM6446_DB_XDC_CFG) $(DM6467_DB_XDC_CFG) $(DM6446_LINUX_OBJFILES) $(DM6467_LINUX_OBJFILES) $(DM355_LINUX_OBJFILES) $(O3530_LINUX_OBJFILES) $(DM357_LINUX_OBJFILES) $(DM365_LINUX_OBJFILES) $(DM6437_BIOS_OBJFILES) $(DM6446_BIOS_OBJFILES) $(DM6467_BIOS_OBJFILES) $(DM6446_AL_EXEC) $(DM6467_AL_EXEC) $(DM355_AL_EXEC) $(O3530_AL_EXEC) $(DM357_AL_EXEC) $(DM365_AL_EXEC) $(DM6437_DB_EXEC) $(DM6446_DB_EXEC) $(DM6467_DB_EXEC) $(DM6446_AL_MAP) $(DM6467_AL_MAP) $(DM355_AL_MAP) $(DM357_AL_MAP) $(DM365_AL_MAP) $(O3530_AL_MAP) $(DM6437_DB_MAP) $(DM6446_DB_MAP) $(DM6467_DB_MAP) *~ *.d .dep
