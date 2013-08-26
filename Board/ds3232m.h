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
*	\brief		Hardware driver header for the DS3232M RTC chip.
*	\author		Pat Satyshur
*	\version	1.0
*	\date		1/14/2013
*	\copyright	Copyright 2013, Pat Satyshur
*	\ingroup 	hardware
*
*	\defgroup	hardware Hardware Drivers
*
*	@{
*/

#ifndef _DS3232M_H_
#define _DS3232M_H_

#include "stdint.h"
#include "common_types.h"

#define DS3232M_SLA_ADDRESS		0x68

//Register map
#define DS3232M_REG_SEC			0x00	
#define DS3232M_REG_MIN			0x01
#define DS3232M_REG_HOUR		0x02
#define DS3232M_REG_DAY			0x03
#define DS3232M_REG_MONTH		0x05
#define DS3232M_REG_YEAR		0x06

#define DS3232M_REG_A1_SEC		0x07
#define DS3232M_REG_A1_MIN		0x08
#define DS3232M_REG_A1_HOUR		0x09
#define DS3232M_REG_A1_DATE		0x0A

#define DS3232M_REG_A2_MIN		0x0B
#define DS3232M_REG_A2_HOUR		0x0C
#define DS3232M_REG_A2_DATE		0x0D

#define DS3232M_REG_CONTROL		0x0E
#define DS3232M_REG_STATUS		0x0F
#define DS3232M_REG_AGING		0x10
#define DS3232M_REG_TEMP_HI		0x11
#define DS3232M_REG_TEMP_LO		0x12
#define DS3232M_REG_TEST		0x03

uint8_t DS3232M_Init( void );
void DS3232M_GetStatus( void );
void DS3232M_SetTime(TimeAndDate *TheTime);
void DS3232M_GetTime(TimeAndDate *TheTime);
void DS3232M_SetAlarm(uint8_t AlarmNumber, uint8_t AlarmMasks, TimeAndDate *AlarmTime);
void DS3232M_EnableAlarm(uint8_t AlarmNumber);
void DS3232M_DisableAlarm(uint8_t AlarmNumber);
uint8_t DS3232M_GetTemp(int8_t *TempLHS, uint8_t *TempRHS);

#endif

/** @} */