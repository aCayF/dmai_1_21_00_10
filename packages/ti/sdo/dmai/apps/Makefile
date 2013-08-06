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

SUBDIRS = $(sort $(dir $(wildcard */)))

CLEANSUBDIRS = $(addsuffix .clean, $(SUBDIRS))
INSTALLSUBDIRS = $(addsuffix .install, $(SUBDIRS))

O3530ALSUBDIRS = $(addsuffix .o3530_al, $(SUBDIRS))
DM355ALSUBDIRS = $(addsuffix .dm355_al, $(SUBDIRS))
DM357ALSUBDIRS = $(addsuffix .dm357_al, $(SUBDIRS))
DM365ALSUBDIRS = $(addsuffix .dm365_al, $(SUBDIRS))
DM6446ALSUBDIRS = $(addsuffix .dm6446_al, $(SUBDIRS))
DM6467ALSUBDIRS = $(addsuffix .dm6467_al, $(SUBDIRS))
DM6437DBSUBDIRS = $(addsuffix .dm6437_db, $(SUBDIRS))
DM6446DBSUBDIRS = $(addsuffix .dm6446_db, $(SUBDIRS))
DM6467DBSUBDIRS = $(addsuffix .dm6467_db, $(SUBDIRS))

.PHONY: install clean o3530_al dm355_al dm357_al dm365_al dm6446_al dm6467_al dm6437_db dm6446_db dm6467_db $(SUBDIRS) $(INSTALLSUBDIRS) $(CLEANSUBDIRS) $(O3530ALSUBDIRS) $(DM355ALSUBDIRS) $(DM6446ALSUBDIRS) $(DM6467ALSUBDIRS) $(DM357ALSUBDIRS) $(DM355ALSUBDIRS) $(DM6437DBSUBDIRS) $(DM6446DBSUBDIRS) $(DM6467SUBDIRS)

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

all:	o3530_al dm357_al dm355_al dm365_al dm6446_al dm6467_al dm6437_db dm6446_db dm6467_db

o3530_al:	$(O3530ALSUBDIRS)

$(O3530ALSUBDIRS):
	@echo
	@echo Executing make o3530_al in subdirectory $(basename $@)...
	@cd $(basename $@) ; $(MAKE) o3530_al

dm357_al:	$(DM357ALSUBDIRS)

$(DM357ALSUBDIRS):
	@echo
	@echo Executing make dm357_al in subdirectory $(basename $@)...
	@cd $(basename $@) ; $(MAKE) dm357_al

dm355_al:	$(DM355ALSUBDIRS)

$(DM355ALSUBDIRS):
	@echo
	@echo Executing make dm355_al in subdirectory $(basename $@)...
	@cd $(basename $@) ; $(MAKE) dm355_al

dm365_al:	$(DM365ALSUBDIRS)

$(DM365ALSUBDIRS):
	@echo
	@echo Executing make dm365_al in subdirectory $(basename $@)...
	@cd $(basename $@) ; $(MAKE) dm365_al

dm6446_al:	$(DM6446ALSUBDIRS)

$(DM6446ALSUBDIRS):
	@echo
	@echo Executing make dm6446_al in subdirectory $(basename $@)...
	@cd $(basename $@) ; $(MAKE) dm6446_al

dm6467_al:	$(DM6467ALSUBDIRS)

$(DM6467ALSUBDIRS):
	@echo $(DIRS)
	@echo
	@echo Executing make dm6467_al in subdirectory $(basename $@)...
	@cd $(basename $@) ; $(MAKE) dm6467_al

dm6437_db:	$(DM6437DBSUBDIRS)

$(DM6437DBSUBDIRS):
	@echo $(DIRS)
	@echo
	@echo Executing make dm6437_db in subdirectory $(basename $@)...
	@cd $(basename $@) ; $(MAKE) dm6437_db
	
dm6446_db:	$(DM6446DBSUBDIRS)

$(DM6446DBSUBDIRS):
	@echo $(DIRS)
	@echo
	@echo Executing make dm6446_db in subdirectory $(basename $@)...
	@cd $(basename $@) ; $(MAKE) dm6446_db
	
dm6467_db:	$(DM6467DBSUBDIRS)

$(DM6467DBSUBDIRS):
	@echo $(DIRS)
	@echo
	@echo Executing make dm6467_db in subdirectory $(basename $@)...
	@cd $(basename $@) ; $(MAKE) dm6467_db

$(SUBDIRS):
	@echo
	@echo Making all in subdirectory $@...
	@$(MAKE) -C $@

install: $(INSTALLSUBDIRS)

$(INSTALLSUBDIRS):
	@echo
	@echo Executing make install in subdirectory $(basename $@)...
	@cd $(basename $@) ; $(MAKE) install

clean:	$(CLEANSUBDIRS)

$(CLEANSUBDIRS):
	@cd $(basename $@) ; $(MAKE) clean
