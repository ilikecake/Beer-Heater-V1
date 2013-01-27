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
*	\brief		Hardware interface function headers for the DS1390 RTC.
*	\author		Pat Satyshur
*	\version	1.0
*	\date		8/7/2012
*	\copyright	Copyright 2012, Pat Satyshur
*	\ingroup 	hardware
*
*	@{
*/

#ifndef _DS1390_H_
#define _DS1390_H_

//Definition of DS1390 CS pin.
#define DS1390_CS_DDR	DDRC
#define DS1390_CS_PORT	PORTC
#define DS1390_CS_PIN	6

//Memory Addresses
#define DS1390_TIME_HOS		0x00
#define DS1390_TIME_SEC		0x01
#define DS1390_TIME_MIN		0x02
#define DS1390_TIME_HOUR	0x03
#define DS1390_TIME_DAY		0x04
#define DS1390_TIME_DATE	0x05
#define DS1390_TIME_MONC	0x06
#define DS1390_TIME_YEAR	0x07
#define DS1390_ALARM_HOS	0x08
#define DS1390_ALARM_SEC	0x09
#define DS1390_ALARM_MIN	0x0A
#define DS1390_ALARM_HOUR	0x0B
#define DS1390_ALARM_DAY	0x0C
#define DS1390_CONTROL		0x0D
#define DS1390_STATUS		0x0E
#define DS1390_CHARGE		0x0F

#define DS1390_READ			0x00
#define DS1390_WRITE		0x80

//Address Masks
#define DS1390_HOS_MASK			0x0F
#define DS1390_TOS_MASK			0xF0
#define DS1390_SEC_MASK			0x0F
#define DS1390_10SEC_MASK		0x70
#define DS1390_MIN_MASK			0x0F
#define DS1390_10MIN_MASK		0x70
#define DS1390_HOUR_MASK		0x0F
#define DS1390_10HOUR_MASK_12	0x10
#define DS1390_10HOUR_MASK_24	0x30
#define DS1390_AMPM_MASK		0x20
#define DS1390_1224_MASK		0x40
#define DS1390_12HOUR_MODE		0x40
#define DS1390_24HOUR_MODE		0x00
#define DS1390_DAY_MASK			0x07
#define DS1390_DATE_MASK		0x0F
#define DS1390_10DATE_MASK		0x30
#define DS1390_MONTH_MASK		0x0F
#define DS1390_10MONTH_MASK		0x10
#define DS1390_CENTURY_MASK		0x80
#define DS1390_YEAR_MASK		0x0F
#define DS1390_10YEAR_MASK		0xF0



typedef struct TimeAndDate 
{
	int year;
	int month;
	int day;
	int dow;
	int hour;
	int min;
	int sec;
} TimeAndDate;

//Function Prototypes
void DS1390Init(void);
void DS1390Select(uint8_t sel);
uint8_t DS1390Read(uint8_t ReadAddress);
uint8_t DS1390Write(uint8_t WriteAddress, uint8_t DataToWrite);
uint8_t DS1390GetTime(TimeAndDate *theTime);
uint8_t DS1390SetTime(TimeAndDate *theTime);

uint8_t TimeIsValid(TimeAndDate *theTime);

#endif

/** @} */