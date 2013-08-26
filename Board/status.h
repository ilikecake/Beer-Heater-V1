/*   This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/** \file
*	\brief		Status functions header file for the beer heater.
*	\author		Pat Satyshur
*	\version	1.0
*	\date		8/24/2013
*	\copyright	Copyright 2013, Pat Satyshur
*	\ingroup 	hardware
*
*	\defgroup	hardware Hardware Drivers
*
*	@{
*/

#ifndef _STATUS_H_
#define _STATUS_H_

#define STATUS_HW_OK			0x00
#define STATUS_HW_ERROR			0x01

//Status registers
#define BH_STATUS_HW		0x01	//holds status of the hardware
#define BH_STATUS_HIO		0x02	//Holds status of the LEDs and buttons
#define BH_STATUS_PROG		0x03	//Holds status of the timer program

//Bitfield defines in the status registers
#define BH_STATUS_HW_CPU			0x01	//A one in this location indicates an error in the AVR CPU I2C, SPI, etc...
#define BH_STATUS_HW_MAX7315		0x10	//A one in this location indicates an error with the MAX7315 expander
#define BH_STATUS_HW_AD7794			0x20	//A one in this location indicates an error with the AD7794 A/D converter
#define BH_STATUS_HW_AT45DB321D		0x40	//A one in this location indicates an error with the AT45DB321D flash memory
#define BH_STATUS_HW_DS3232M		0x80	//A one in this location indicates an error with the DS3232M RTC

#define BH_STATUS_HIO_B1_PEND		0x01	//Set to 1 if button 1 (red) has been pressed
#define BH_STATUS_HIO_B2_PEND		0x02	//Set to 1 if button 2 (black) has been pressed
#define BH_STATUS_HIO_B1_OLD		0x04	//The old state of button 1 (red)
#define BH_STATUS_HIO_B2_OLD		0x08	//The old state of button 2 (black)
#define BH_STATUS_HIO_LED1			0x10	//The state of LED1
#define BH_STATUS_HIO_LED2			0x20	//The state of LED2
#define BH_STATUS_HIO_LED3			0x40	//The state of LED3

void BH_InitStatus(void);
uint8_t BH_GetStatus(uint8_t StatusID);
void BH_SetStatus(uint8_t StatusID, uint8_t BitMask, uint8_t BitVal);

#endif
/** @} */