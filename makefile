#
#             LUFA Library
#     Copyright (C) Dean Camera, 2012.
#
#  dean [at] fourwalledcubicle [dot] com
#           www.lufa-lib.org
#
# --------------------------------------
#         LUFA Project Makefile.
# --------------------------------------

MCU          = atmega32u4
ARCH         = AVR8
BOARD        = USER
F_CPU        = 8000000
F_USB        = $(F_CPU)
OPTIMIZATION = s
TARGET       = main
SRC          = $(TARGET).c Descriptors.c Board/Hardware.c Board/commands.c Board/ds1390.c Board/ad7794.c Board/max7315.c version.c $(COMMON_PATH)/command.c $(COMMON_PATH)/twi.c $(COMMON_PATH)/dfu_jump.c $(COMMON_PATH)/mem_usage.c $(LUFA_SRC_USB) $(LUFA_SRC_USBCLASS)
LUFA_PATH    = common/LUFA-120730
COMMON_PATH	 = common
CC_FLAGS     = -DUSE_LUFA_CONFIG_HEADER -IConfig/ -IBoard -I$(COMMON_PATH)
LD_FLAGS     =

# Default target
all:

##Added code to make a build string
##Based on the example by user ksvitale at AVRFreaks.net
version.c:
	echo '#include <avr/pgmspace.h>' > $@
##	echo '#include "version.h"' >> $@
	echo -n 'const char fwCompileDate[] PROGMEM = "Build: ' >> $@
	date +'%Y%m%d.%H%M\r\n";' >> $@;

# Always compile version.c to get correct compilation date and time
.PHONY:   version.c

##end of build string code

# Include LUFA build script makefiles
include $(LUFA_PATH)/Build/lufa_core.mk
include $(LUFA_PATH)/Build/lufa_sources.mk
include $(LUFA_PATH)/Build/lufa_build.mk
include $(LUFA_PATH)/Build/lufa_cppcheck.mk
include $(LUFA_PATH)/Build/lufa_doxygen.mk
include $(LUFA_PATH)/Build/lufa_dfu.mk
include $(LUFA_PATH)/Build/lufa_hid.mk
include $(LUFA_PATH)/Build/lufa_avrdude.mk
include $(LUFA_PATH)/Build/lufa_atprogram.mk

