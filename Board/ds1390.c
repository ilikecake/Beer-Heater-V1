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
*	\brief		Hardware interface functions for the DS1390 RTC.
*	\author		Pat Satyshur
*	\version	1.0
*	\date		8/7/2012
*	\copyright	Copyright 2012, Pat Satyshur
*	\ingroup 	hardware
*
*	@{
*/

//All headers are included here
#include "main.h"

//Initalize the SD1390. SPI must be initalized elsewhere.
//SPI can be mode 0 or 3
void DS1390Init(void)
{
	DS1390_CS_DDR |= (1<<DS1390_CS_PIN);		//Setup CS pin (change this in the .h file if nessecary)
	DS1390_CS_PORT |= (1<<DS1390_CS_PIN);		//Set CS hig (not selected)
	
	
	DS1390Write(DS1390_CHARGE, 0xA9);			//Setup trickle charger with diode and 250ohm resistor
	
	
	return;
}

//Toggles CS pin for the DS1390. sel = 1 selects the device. Note: devices are selected by a low level signal on CS.
void DS1390Select(uint8_t sel)
{
	if(sel == 1)
	{
		DS1390_CS_PORT &= ~(1<<DS1390_CS_PIN);
	}
	else if (sel == 0)
	{
		DS1390_CS_PORT |= (1<<DS1390_CS_PIN);
	}
}

uint8_t DS1390Read(uint8_t ReadAddress)
{
	int RegData;
	
	if (ReadAddress <= 0x0F && ReadAddress >= 0x00)
	{
		DS1390Select(1);
		SPI_SendByte(ReadAddress);
		RegData = SPI_ReceiveByte();
		DS1390Select(0);
		return RegData;
	}
	return 0xFF;
}

uint8_t DS1390Write(uint8_t WriteAddress, uint8_t DataToWrite)
{
	//int RegData;
	
	if (WriteAddress <= 0x0F && WriteAddress >= 0x00)
	{
		WriteAddress = WriteAddress | DS1390_WRITE;
		
		//Send command, get response
		DS1390Select(1);
		SPI_SendByte(WriteAddress);
		SPI_SendByte(DataToWrite);
		DS1390Select(0);
		
		return 0;
	}
	return -1;
}

//the time will be returned in 24hour mode regardless of the setting of the RTC
uint8_t DS1390GetTime(TimeAndDate *theTime)
{
	int hour, dow, day, month, year;
	int sec1, sec2;
	int min1, min2;
	int i;
	
	for(i=0;i<10;i++)
	{
		//get two consecutive readings from the seconds register
		DS1390Select(1);
		SPI_SendByte(DS1390_TIME_SEC);
		sec1	= SPI_ReceiveByte();
		min1	= SPI_ReceiveByte();
		hour	= SPI_ReceiveByte();
		dow		= SPI_ReceiveByte();
		day		= SPI_ReceiveByte();
		month	= SPI_ReceiveByte();
		year	= SPI_ReceiveByte();
		DS1390Select(0);
		
		SPI_SendByte(0x00);
		
		DS1390Select(1);
		SPI_SendByte(DS1390_TIME_SEC);
		sec2	= SPI_ReceiveByte();
		min2	= SPI_ReceiveByte();
		hour	= SPI_ReceiveByte();
		dow		= SPI_ReceiveByte();
		day		= SPI_ReceiveByte();
		month	= SPI_ReceiveByte();
		year	= SPI_ReceiveByte();
		DS1390Select(0);
		
		if(sec1 == sec2)
		{
			break;
		}
	}

	if(i == 10)
	{
		//Could not get a stable reading from the RTC.
		//Maybe an unstable bus?
		return 0xFF;
	}
	else
	{
		theTime->sec = (sec1&DS1390_SEC_MASK)+((sec1&DS1390_10SEC_MASK)>>4)*10;
		theTime->min = (min1&DS1390_MIN_MASK)+((min1&DS1390_10MIN_MASK)>>4)*10;
		
		if((hour&DS1390_1224_MASK) == DS1390_12HOUR_MODE)
		{
			if((hour&DS1390_AMPM_MASK) == DS1390_AMPM_MASK)		//PM
			{
				theTime->hour = 12+(hour&DS1390_HOUR_MASK)+((hour&DS1390_10HOUR_MASK_12)>>4)*10;
			}
			else
			{
				theTime->hour = (hour&DS1390_HOUR_MASK)+((hour&DS1390_10HOUR_MASK_12)>>4)*10;
			}
		}
		else			//24 Hour Mode
		{
			theTime->hour = (hour&DS1390_HOUR_MASK)+((hour&DS1390_10HOUR_MASK_24)>>4)*10;
		}
		
		theTime->dow = (dow&DS1390_DAY_MASK);
		theTime->day	= (day&DS1390_DATE_MASK)+((day&DS1390_10DATE_MASK)>>4)*10;
		theTime->month = (month&DS1390_MONTH_MASK)+((month&DS1390_10MONTH_MASK)>>4)*10;
		theTime->year = (year&DS1390_YEAR_MASK)+((year&DS1390_10YEAR_MASK)>>4)*10;
		
		return 0;
	}

	//printf("SEC: 	%d\n",sec1);
	//printf("MIN: 	%d\n",min1);
	//printf("HOUR: 	%d\n",hour);
	
	
	
	
	//return 0;
}

//Will always setup the RTC in 24 hour mode.
uint8_t DS1390SetTime(TimeAndDate *theTime)
{
	if(TimeIsValid(theTime))
	{
		DS1390Select(1);
		SPI_SendByte(DS1390_TIME_SEC|DS1390_WRITE);
		SPI_SendByte((((theTime->sec)/10) << 4) + (theTime->sec % 10));
		SPI_SendByte((((theTime->min)/10) << 4) + (theTime->min % 10));
		SPI_SendByte((((theTime->hour)/10) << 4) + (theTime->hour % 10));
		SPI_SendByte(theTime->dow);
		SPI_SendByte((((theTime->day)/10) << 4) + (theTime->day % 10));
		SPI_SendByte((((theTime->month)/10) << 4) + (theTime->month % 10));
		SPI_SendByte((((theTime->year)/10) << 4) + (theTime->year % 10));
		DS1390Select(0);
		return 0;
	}
	return 0xFF;
}

//return 1 if time and date are valid
uint8_t TimeIsValid(TimeAndDate *theTime)
{
	if ((theTime->sec > 59) || (theTime->sec < 0))
	{
		return 0;
	}
	else if ((theTime->min > 59) || (theTime->min < 0))
	{
		return 0;
	}
	else if ((theTime->hour > 23) || (theTime->hour < 0))
	{
		return 0;
	}
	else if ((theTime->dow > 7) || (theTime->dow < 0))
	{
		return 0;
	}
	else if ((theTime->day > 31) || (theTime->day < 0))		//Not checking if the day and month combination is valid, maybe add this later...
	{
		return 0;
	}
	else if ((theTime->month > 12) || (theTime->month < 0))
	{
		return 0;
	}
	else if ((theTime->year > 99) || (theTime->year < 0))
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

/** @} */