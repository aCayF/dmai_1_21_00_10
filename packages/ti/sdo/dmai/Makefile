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

DMAI_INSTALL_DIR = ../../../..
TARGET = dmai

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
    CPP_FLAGS		= -DNDEBUG
    C_FLAGS		= -O2
else
    CPP_FLAGS		=
    C_FLAGS		= -g
endif

CPP_FLAGS		+= -I$(XDC_INSTALL_DIR)/packages -I$(DMAI_INSTALL_DIR)/packages -I$(CE_INSTALL_DIR)/packages -I$(FC_INSTALL_DIR)/packages -I$(XDAIS_INSTALL_DIR)/packages

XDC_PATH		+= $(XDC_INSTALL_DIR)/packages;$(DMAI_INSTALL_DIR)/packages

GNU_CPP_FLAGS		= $(CPP_FLAGS) -I$(LINUXKERNEL_INSTALL_DIR)/include -I$(CMEM_INSTALL_DIR)/packages -I$(LINK_INSTALL_DIR)/packages -DDmai_BuildOs_linux -Dxdc_target_types__=gnu/targets/std.h -I$(LINUXLIBS_INSTALL_DIR)/include
C64P_CPP_FLAGS		= $(CPP_FLAGS) -I$(BIOS_INSTALL_DIR)/packages/ti/bios/include -I$(BIOS_INSTALL_DIR)/packages -DDmai_BuildOs_bios -Dxdc_target__='<ti/targets/std.h>' -I$(CODEGEN_INSTALL_DIR)/include -mv6400+ -pdse225
MVL_CPP_FLAGS		= $(GNU_CPP_FLAGS) -Dxdc_target_name__=MVArm9
CS_CPP_FLAGS		= $(GNU_CPP_FLAGS) -Dxdc_target_name__=codesourcery/GCArmv5T

GNU_C_FLAGS		= $(C_FLAGS) -Wall
C64P_C_FLAGS		= $(C_FLAGS)

GNU_AR_FLAGS		= $(AR_FLAGS)
C64P_AR_FLAGS		= $(AR_FLAGS)

SOURCES			= $(wildcard *.c)
CE_SOURCES		= $(wildcard ce/*.c)
BIOS_SOURCES		= $(SOURCES) $(CE_SOURCES) $(wildcard bios/*.c)
LINUX_SOURCES		= $(SOURCES) $(CE_SOURCES) $(wildcard linux/*.c)

LINUX_DM6446_SOURCES	= $(LINUX_SOURCES) $(wildcard linux/dm6446/*.c)
LINUX_DM6467_SOURCES	= $(LINUX_SOURCES) $(wildcard linux/dm6467/*.c)
LINUX_DM355_SOURCES	= $(LINUX_SOURCES) $(wildcard linux/dm355/*.c)
LINUX_DM357_SOURCES	= $(LINUX_SOURCES) $(wildcard linux/dm6446/*.c)
LINUX_DM365_SOURCES	= $(LINUX_SOURCES) $(wildcard linux/dm365/*.c)
LINUX_O3530_SOURCES	= $(LINUX_SOURCES) $(wildcard linux/omap3530/*.c)
BIOS_DM6437_SOURCES	= $(BIOS_SOURCES) $(wildcard bios/dm6437/*.c)
BIOS_DM6446_SOURCES	= $(BIOS_SOURCES) $(wildcard bios/dm6446/*.c)
BIOS_DM6467_SOURCES	= $(BIOS_SOURCES) $(wildcard bios/dm6467/*.c)

HEADERS			= $(wildcard *.h)
CE_HEADERS		= $(wildcard ce/*.h)
LINUX_HEADERS		= $(HEADERS) $(CE_HEADERS) $(wildcard linux/priv/*.h)
BIOS_HEADERS		= $(HEADERS) $(CE_HEADERS) $(wildcard bios/*.h)

MVL_DM6446_TARGET	= lib/$(TARGET)_linux_dm6446.a470MV
MVL_DM6467_TARGET	= lib/$(TARGET)_linux_dm6467.a470MV
MVL_DM355_TARGET	= lib/$(TARGET)_linux_dm355.a470MV
MVL_DM357_TARGET	= lib/$(TARGET)_linux_dm357.a470MV
MVL_DM365_TARGET	= lib/$(TARGET)_linux_dm365.a470MV
CS_O3530_TARGET		= lib/$(TARGET)_linux_omap3530.a470MV
BIOS_DM6437_TARGET	= lib/$(TARGET)_bios_dm6437.a64P
BIOS_DM6446_TARGET	= lib/$(TARGET)_bios_dm6446.a64P
BIOS_DM6467_TARGET	= lib/$(TARGET)_bios_dm6467.a64P

MVL_DM6446_OBJFILES	= $(addprefix obj/,$(LINUX_DM6446_SOURCES:%.c=%.dm6446.o470MV))
MVL_DM6467_OBJFILES	= $(addprefix obj/,$(LINUX_DM6467_SOURCES:%.c=%.dm6467.o470MV))
MVL_DM355_OBJFILES	= $(addprefix obj/,$(LINUX_DM355_SOURCES:%.c=%.dm355.o470MV))
MVL_DM357_OBJFILES	= $(addprefix obj/,$(LINUX_DM357_SOURCES:%.c=%.dm357.o470MV))
MVL_DM365_OBJFILES	= $(addprefix obj/,$(LINUX_DM365_SOURCES:%.c=%.dm365.o470MV))
CS_O3530_OBJFILES	= $(addprefix obj/,$(LINUX_O3530_SOURCES:%.c=%.o3530.o470MV))
BIOS_DM6437_OBJFILES	= $(addprefix obj/,$(BIOS_DM6437_SOURCES:%.c=%.dm6437.o64P))
BIOS_DM6446_OBJFILES	= $(addprefix obj/,$(BIOS_DM6446_SOURCES:%.c=%.dm6446.o64P))
BIOS_DM6467_OBJFILES	= $(addprefix obj/,$(BIOS_DM6467_SOURCES:%.c=%.dm6467.o64P))

MVL_DM6446_CPP_FLAGS	= -DDmai_Device_dm6446
MVL_DM6467_CPP_FLAGS	= -DDmai_Device_dm6467
MVL_DM355_CPP_FLAGS	= -DDmai_Device_dm355
MVL_DM365_CPP_FLAGS	= -DDmai_Device_dm365
MVL_DM357_CPP_FLAGS	= -DDmai_Device_dm6446
CS_O3530_CPP_FLAGS	= -DDmai_Device_omap3530
BIOS_DM6437_CPP_FLAGS	= -DDmai_Device_dm6437 -eodm6437.o64P
BIOS_DM6446_CPP_FLAGS	= -DDmai_Device_dm6446 -eodm6446.o64P
BIOS_DM6467_CPP_FLAGS	= -DDmai_Device_dm6467 -eodm6467.o64P

MVL_COMPILE.c	= $(PRE) $(MVTOOL_PREFIX)gcc $(MVL_CPP_FLAGS) $(GNU_C_FLAGS) -c
MVL_AR.c	= $(PRE) $(MVTOOL_PREFIX)ar rc $(GNU_AR_FLAGS)

CS_COMPILE.c	= $(PRE) $(CSTOOL_PREFIX)gcc $(CS_CPP_FLAGS) $(GNU_C_FLAGS) -c
CS_AR.c		= $(PRE) $(CSTOOL_PREFIX)ar rc $(GNU_AR_FLAGS)

C64P_COMPILE.c	= $(PRE) $(CODEGEN_INSTALL_DIR)/bin/cl6x $(C64P_CPP_FLAGS) $(C64P_C_FLAGS) $(C64P_CPP_FLAGS) -c
C64P_AR.c	= $(PRE) $(CODEGEN_INSTALL_DIR)/bin/ar6x $(C64P_AR_FLAGS) r

XDC_CMD		= $(PRE) CSTOOL_DIR="$(CSTOOL_DIR)" MVTOOL_DIR="$(MVTOOL_DIR)" CODEGEN_INSTALL_DIR="$(CODEGEN_INSTALL_DIR)" XDCPATH="$(XDC_PATH)" $(XDC_INSTALL_DIR)/xdc XDCTARGETS="$(XDCTARGET)"

.PHONY: clean install o3530_al dm357_al dm6437_db dm6446_al dm6467_al dm355_al dm6446_db dm6467_db dm365_al

help:
	@echo
	@echo "You must specify a build target. Available targets are:"
	@echo
	@echo "    dm6446_al   : ARM Linux dm6446"
	@echo "    dm6467_al   : ARM Linux dm6467"
	@echo "    dm355_al    : ARM Linux dm355"
	@echo "    dm357_al    : ARM Linux dm357"
	@echo "    dm365_al    : ARM Linux dm365"
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

all:	o3530_al dm357_al dm6467_al dm355_al dm365_al dm6446_al dm6437_db dm6446_db dm6467_db

dm6446_al:	XDCTARGET="gnu.targets.MVArm9"
dm6446_al:	$(MVL_DM6446_TARGET) package/package.defs.h

dm6467_al:	XDCTARGET="gnu.targets.MVArm9"
dm6467_al:	$(MVL_DM6467_TARGET) package/package.defs.h

dm355_al:	XDCTARGET="gnu.targets.MVArm9"
dm355_al:	$(MVL_DM355_TARGET) package/package.defs.h

dm357_al:	XDCTARGET="gnu.targets.MVArm9"
dm357_al:	$(MVL_DM357_TARGET) package/package.defs.h

dm365_al:	XDCTARGET="gnu.targets.MVArm9"
dm365_al:	$(MVL_DM365_TARGET) package/package.defs.h

o3530_al:	XDCTARGET="gnu.targets.arm.GCArmv5T"
o3530_al:	$(CS_O3530_TARGET) package/package.defs.h

dm6437_db:	XDCTARGET="ti.targets.C64P"
dm6437_db:	$(BIOS_DM6437_TARGET) package/package.defs.h

dm6446_db:	XDCTARGET="ti.targets.C64P"
dm6446_db:	$(BIOS_DM6446_TARGET) package/package.defs.h

dm6467_db:	XDCTARGET="ti.targets.C64P"
dm6467_db:	$(BIOS_DM6467_TARGET) package/package.defs.h

install:
	@echo
	@echo Nothing to be done for 'make install' on $(TARGET)..

package/package.defs.h:	package.xdc package.xs package.bld
	@echo
	@echo Building package interface for $(TARGET)..
	$(XDC_CMD)

$(MVL_DM6446_TARGET):	$(MVL_DM6446_OBJFILES)
	@echo
	@echo Archiving $@ from $^..
	@mkdir -p $(dir $@)
	$(MVL_AR.c) $@ $^

$(MVL_DM6467_TARGET):	$(MVL_DM6467_OBJFILES)
	@echo
	@echo Archiving $@ from $^..
	@mkdir -p $(dir $@)
	$(MVL_AR.c) $@ $^

$(MVL_DM355_TARGET):	$(MVL_DM355_OBJFILES)
	@echo
	@echo Archiving $@ from $^..
	@mkdir -p $(dir $@)
	$(MVL_AR.c) $@ $^

$(MVL_DM357_TARGET):	$(MVL_DM357_OBJFILES)
	@echo
	@echo Archiving $@ from $^..
	@mkdir -p $(dir $@)
	$(MVL_AR.c) $@ $^

$(MVL_DM365_TARGET):	$(MVL_DM365_OBJFILES)
	@echo
	@echo Archiving $@ from $^..
	@mkdir -p $(dir $@)
	$(MVL_AR.c) $@ $^

$(CS_O3530_TARGET):	$(CS_O3530_OBJFILES)
	@echo
	@echo Archiving $@ from $^..
	@mkdir -p $(dir $@)
	$(CS_AR.c) $@ $^

$(BIOS_DM6437_TARGET):	$(BIOS_DM6437_OBJFILES)
	@echo
	@echo Linking $@ from $^..
	$(C64P_AR.c) $@ $^

$(BIOS_DM6446_TARGET):	$(BIOS_DM6446_OBJFILES)
	@echo
	@echo Linking $@ from $^..
	$(C64P_AR.c) $@ $^
	
$(BIOS_DM6467_TARGET):	$(BIOS_DM6467_OBJFILES)
	@echo
	@echo Linking $@ from $^..
	$(C64P_AR.c) $@ $^

$(MVL_DM6446_OBJFILES):		obj/%.dm6446.o470MV: %.c $(LINUX_HEADERS)
	@echo Compiling $@ from $<..
	@mkdir -p $(dir $@)
	$(MVL_COMPILE.c) $(MVL_DM6446_CPP_FLAGS) -o $@ $<

$(MVL_DM6467_OBJFILES):		obj/%.dm6467.o470MV: %.c $(LINUX_HEADERS)
	@echo Compiling $@ from $<..
	@mkdir -p $(dir $@)
	$(MVL_COMPILE.c) $(MVL_DM6467_CPP_FLAGS) -o $@ $<

$(MVL_DM355_OBJFILES):		obj/%.dm355.o470MV: %.c $(LINUX_HEADERS)
	@echo Compiling $@ from $<..
	@mkdir -p $(dir $@)
	$(MVL_COMPILE.c) $(MVL_DM355_CPP_FLAGS) -o $@ $<

$(MVL_DM357_OBJFILES):		obj/%.dm357.o470MV: %.c $(LINUX_HEADERS)
	@echo Compiling $@ from $<..
	@mkdir -p $(dir $@)
	$(MVL_COMPILE.c) $(MVL_DM357_CPP_FLAGS) -o $@ $<

$(MVL_DM365_OBJFILES):		obj/%.dm365.o470MV: %.c $(LINUX_HEADERS)
	@echo Compiling $@ from $<..
	@mkdir -p $(dir $@)
	$(MVL_COMPILE.c) $(MVL_DM365_CPP_FLAGS) -o $@ $<

$(CS_O3530_OBJFILES):		obj/%.o3530.o470MV: %.c $(LINUX_HEADERS)
	@echo Compiling $@ from $<..
	@mkdir -p $(dir $@)
	$(CS_COMPILE.c) $(CS_O3530_CPP_FLAGS) -o $@ $<

$(BIOS_DM6437_OBJFILES):	obj/%.dm6437.o64P: %.c $(BIOS_HEADERS)
	@echo Compiling $@ from $<..
	@mkdir -p $(dir $@)
	$(C64P_COMPILE.c) $(BIOS_DM6437_CPP_FLAGS) $< -fr"$(dir $@)"

$(BIOS_DM6446_OBJFILES):	obj/%.dm6446.o64P: %.c $(BIOS_HEADERS)
	@echo Compiling $@ from $<..
	@mkdir -p $(dir $@)
	$(C64P_COMPILE.c) $(BIOS_DM6446_CPP_FLAGS) $< -fr"$(dir $@)"

$(BIOS_DM6467_OBJFILES):	obj/%.dm6467.o64P: %.c $(BIOS_HEADERS)
	@echo Compiling $@ from $<..
	@mkdir -p $(dir $@)
	$(C64P_COMPILE.c) $(BIOS_DM6467_CPP_FLAGS) $< -fr"$(dir $@)"

clean:
	@echo Removing generated files..
	$(PRE) $(XDC_CMD) clean
	$(PRE) -$(RM) -rf $(MVL_DM365_TARGET) $(MVL_DM6446_TARGET) $(MVL_DM6467_TARGET) $(MVL_DM355_TARGET) $(CS_O3530_TARGET) $(MVL_DM357_TARGET) $(BIOS_DM6437_TARGET) $(BIOS_DM6446_TARGET) $(BIOS_DM6467_TARGET) obj/* *~ *.mak .xdcenv.mak
