/*
             LUFA Library
     Copyright (C) Dean Camera, 2012.

  dean [at] fourwalledcubicle [dot] com
           www.lufa-lib.org
*/

/*
  Copyright 2012  Dean Camera (dean [at] fourwalledcubicle [dot] com)

  Permission to use, copy, modify, distribute, and sell this
  software and its documentation for any purpose is hereby granted
  without fee, provided that the above copyright notice appear in
  all copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaim all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/

/** \file
 *
 *  Header file for VirtualSerial.c.
 */

#ifndef _BEER_HEATER_V1_H_
#define _BEER_HEATER_V1_H_

	/* Includes: */
		#include <avr/io.h>
		#include <avr/wdt.h>
		#include <avr/power.h>
		#include <avr/interrupt.h>
		#include <avr/eeprom.h>
		#include <string.h>
		#include <stdio.h>
		#include <math.h>
		#include <stdlib.h>

		#include "Descriptors.h"

		#include <LUFA-120730/Drivers/Board/LEDs.h>
		#include <LUFA-120730/Drivers/USB/USB.h>
		#include <LUFA-120730/Drivers/Peripheral/SPI.h>
		
		#include "twi.h"
		#include "common_types.h"
		
		#include "Board/Hardware.h"
		#include "commands.h"
		#include "dfu_jump.h"
		#include "ad7794.h"
		#include "max7315.h"
		#include "at45db321d.h"
		#include "datalogger.h"
		#include "ds3232m.h"
		#include "thermistor.h"
		
	/* Macros: */
		/** LED mask for the library LED driver, to indicate that the USB interface is not ready. */
		#define LEDMASK_USB_NOTREADY      LEDS_LED1

		/** LED mask for the library LED driver, to indicate that the USB interface is enumerating. */
		#define LEDMASK_USB_ENUMERATING  (LEDS_LED2 | LEDS_LED3)

		/** LED mask for the library LED driver, to indicate that the USB interface is ready. */
		#define LEDMASK_USB_READY        (LEDS_LED2 | LEDS_LED4)

		/** LED mask for the library LED driver, to indicate that an error has occurred in the USB interface. */
		#define LEDMASK_USB_ERROR        (LEDS_LED1 | LEDS_LED3)

		extern USB_ClassInfo_CDC_Device_t VirtualSerial_CDC_Interface;


	/* Function Prototypes: */
		void SetupHardware(void);
		void CheckJoystickMovement(void);

		void EVENT_USB_Device_Connect(void);
		void EVENT_USB_Device_Disconnect(void);
		void EVENT_USB_Device_ConfigurationChanged(void);
		void EVENT_USB_Device_ControlRequest(void);

#endif

