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

DMAI_INSTALL_DIR = .

include $(DMAI_INSTALL_DIR)/Rules.make


.PHONY: clean install o3530_al dm357_al dm355_al dm365_al dm6446_al dm6467_al dm6437_db dm6446_db dm6467_db

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

all:	o3530_al dm6467_al dm355_al dm6446_al dm357_al dm365_al dm6437_db dm6446_db dm6467_db

dm357_al:
	@cd $(DMAI_INSTALL_DIR)/packages/ti/sdo/dmai ; $(MAKE) dm357_al
	@cd $(DMAI_INSTALL_DIR)/packages/ti/sdo/dmai/apps ; $(MAKE) dm357_al

o3530_al:
	@cd $(DMAI_INSTALL_DIR)/packages/ti/sdo/dmai ; $(MAKE) o3530_al
	@cd $(DMAI_INSTALL_DIR)/packages/ti/sdo/dmai/apps ; $(MAKE) o3530_al

dm355_al:
	@cd $(DMAI_INSTALL_DIR)/packages/ti/sdo/dmai ; $(MAKE) dm355_al
	@cd $(DMAI_INSTALL_DIR)/packages/ti/sdo/dmai/apps ; $(MAKE) dm355_al

dm365_al:
	@cd $(DMAI_INSTALL_DIR)/packages/ti/sdo/dmai ; $(MAKE) dm365_al
	@cd $(DMAI_INSTALL_DIR)/packages/ti/sdo/dmai/apps ; $(MAKE) dm365_al

dm6446_al:
	@cd $(DMAI_INSTALL_DIR)/packages/ti/sdo/dmai ; $(MAKE) dm6446_al
	@cd $(DMAI_INSTALL_DIR)/packages/ti/sdo/dmai/apps ; $(MAKE) dm6446_al

dm6467_al:
	@cd $(DMAI_INSTALL_DIR)/packages/ti/sdo/dmai ; $(MAKE) dm6467_al
	@cd $(DMAI_INSTALL_DIR)/packages/ti/sdo/dmai/apps ; $(MAKE) dm6467_al

dm6437_db:
	@cd $(DMAI_INSTALL_DIR)/packages/ti/sdo/dmai ; $(MAKE) dm6437_db
	@cd $(DMAI_INSTALL_DIR)/packages/ti/sdo/dmai/apps ; $(MAKE) dm6437_db

dm6446_db:
	@cd $(DMAI_INSTALL_DIR)/packages/ti/sdo/dmai ; $(MAKE) dm6446_db
	@cd $(DMAI_INSTALL_DIR)/packages/ti/sdo/dmai/apps ; $(MAKE) dm6446_db

dm6467_db:
	@cd $(DMAI_INSTALL_DIR)/packages/ti/sdo/dmai ; $(MAKE) dm6467_db
	@cd $(DMAI_INSTALL_DIR)/packages/ti/sdo/dmai/apps ; $(MAKE) dm6467_db

install:
	$(MAKE) -C $(DMAI_INSTALL_DIR)/packages/ti/sdo/dmai install
	$(MAKE) -C $(DMAI_INSTALL_DIR)/packages/ti/sdo/dmai/apps install

clean:
	$(MAKE) -C $(DMAI_INSTALL_DIR)/packages/ti/sdo/dmai clean
	$(MAKE) -C $(DMAI_INSTALL_DIR)/packages/ti/sdo/dmai/apps clean
