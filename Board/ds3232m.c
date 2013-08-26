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
*	\brief		Hardware driver for the DS3232M RTC chip.
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

#include "main.h"

//TODO: Add I2C status checking

//Initalize the DS3232M
uint8_t DS3232M_Init( void )
{
	uint8_t RecieveData;
	uint8_t SendData[2];
	uint8_t stat;

	//Set up control register
	// -Enable interrupt on INT/SQW pin, disable square wave output
	// -Disable pending interrupts
	SendData[0] = DS3232M_REG_CONTROL;
	SendData[1] = 0x04;
	stat = TWIRW(DS3232M_SLA_ADDRESS, SendData, &RecieveData, 2, 0);
	TWI_CHECKSTAT(stat);
	
	DS3232M_DisableAlarm(1);
	DS3232M_DisableAlarm(2);
	
	
	return 0x00;
}

void DS3232M_GetStatus( void )
{
	uint8_t RecieveData[2];
	uint8_t SendData = DS3232M_REG_CONTROL;
	
	TWIRW(DS3232M_SLA_ADDRESS, &SendData, RecieveData, 1, 2);
	printf("Control: 0x%02X\n", RecieveData[0]);
	printf("Status: 0x%02X\n", RecieveData[1]);
	
	return;
}

void DS3232M_SetTime(TimeAndDate *TheTime)
{
	uint8_t RecieveData;
	uint8_t SendData[8];
	
	SendData[0] = DS3232M_REG_SEC;
	SendData[1] = ((TheTime->sec % 10) | ((TheTime->sec / 10) << 4));
	SendData[2] = ((TheTime->min % 10) | ((TheTime->min / 10) << 4));
	SendData[3] = ((((TheTime->hour % 10) | ((TheTime->hour / 10) << 4))) & 0x3F);		//Note: this sets the RTC in 24 hour mode
	SendData[4] = TheTime->dow;
	SendData[5] = ((TheTime->day % 10) | ((TheTime->day / 10) << 4));
	SendData[6] = ((TheTime->month % 10) | ((TheTime->month / 10) << 4));	//NOTE: this probably clears the century bit. maybe look at this later.
	SendData[7] = ((TheTime->year % 10) | ((TheTime->year / 10) << 4));

	if(TWIRW(DS3232M_SLA_ADDRESS, SendData, &RecieveData, 8, 0) != 0)
	{
		printf_P(PSTR("I2C Error\n"));
	}

	return;
}

void DS3232M_GetTime(TimeAndDate *TheTime)
{
	uint8_t RecieveData[7];
	uint8_t SendData;
	//uint8_t i;

	SendData = DS3232M_REG_SEC;


	if(TWIRW(DS3232M_SLA_ADDRESS, &SendData, RecieveData, 1, 7) == 0)
	{
		//Convert registers in BCD into the time struct
		TheTime->sec = ((RecieveData[0] & 0x0F) + ((RecieveData[0] & 0x70) >> 4)*10 );
		TheTime->min = ((RecieveData[1] & 0x0F) + ((RecieveData[1] & 0x70) >> 4)*10 );
		
		//Handle hours in 12 or 24 hour mode. The struct will always take 24 hour format
		if( (RecieveData[2] & 0x40) == 0x04)		//12 hour mode
		{
			if((RecieveData[2] & 0x20) == 0x20)		//PM
			{
				TheTime->hour = 12 + (RecieveData[2] & 0x0F) + ((RecieveData[2] & 0x10) >> 4)*10;
			}
			else
			{
				TheTime->hour = (RecieveData[2] & 0x0F) + ((RecieveData[2] & 0x10) >> 4)*10;
			}
		}
		else			//24 Hour Mode
		{
			TheTime->hour = (RecieveData[2] & 0x0F) + ((RecieveData[2] & 0x30) >> 4)*10;
		}
		
		TheTime->dow = (RecieveData[3] & 0x07);
		TheTime->day = ((RecieveData[4] & 0x0F) + ((RecieveData[4] & 0x30) >> 4)*10);
		TheTime->month = ((RecieveData[5] & 0x0F) + ((RecieveData[5] & 0x10) >> 4)*10);
		//TODO: Handle century bit...
		TheTime->year = ((RecieveData[6] & 0x0F) + ((RecieveData[6] & 0xF0) >> 4)*10);
	}
	else
	{
		printf_P(PSTR("I2C Error\n"));
	}

	return;
}

//AlarmMasks format: <DY/DT(1=day, 0=date)> <AxM4> <AxM3> <AxM2> <A1M1>
//Note: A2M1 does not exsist (no seconds register)
void DS3232M_SetAlarm(uint8_t AlarmNumber, uint8_t AlarmMasks, TimeAndDate *AlarmTime)
{
	uint8_t RecieveData;
	uint8_t SendData[5];
	
	if(AlarmNumber == 1)
	{
		SendData[0] = DS3232M_REG_A1_SEC;
		SendData[1] = (((AlarmMasks & 0x01) << 7) | (AlarmTime->sec % 10) | ((AlarmTime->sec / 10) << 4));
		SendData[2] = (((AlarmMasks & 0x02) << 6) | (AlarmTime->min % 10) | ((AlarmTime->min / 10) << 4));
		SendData[3] = ((((AlarmMasks & 0x04) << 5) | (AlarmTime->hour % 10) | ((AlarmTime->hour / 10) << 4)) & 0xBF);
		
		if((AlarmMasks & 0x10) == 0x10)
		{
			//Alarm on day of week
			SendData[4] = (((AlarmMasks & 0x08) << 4) | ((AlarmMasks & 0x10) << 2) | (AlarmTime->dow));
		}
		else
		{
			//Alarm on date
			SendData[4] = (((AlarmMasks & 0x08) << 4) | ((AlarmMasks & 0x10) << 2) | (AlarmTime->day % 10) | ((AlarmTime->day / 10) << 4));
		}
		TWIRW(DS3232M_SLA_ADDRESS, SendData, &RecieveData, 5, 0);
	}
	else
	{
		SendData[0] = DS3232M_REG_A2_MIN;
		SendData[1] = (((AlarmMasks & 0x02) << 6) | (AlarmTime->min % 10) | ((AlarmTime->min / 10) << 4));
		SendData[2] = ((((AlarmMasks & 0x04) << 5) | (AlarmTime->hour % 10) | ((AlarmTime->hour / 10) << 4)) & 0xBF);
		
		if((AlarmMasks & 0x10) == 0x10)
		{
			//Alarm on day of week
			SendData[3] = (((AlarmMasks & 0x08) << 4) | ((AlarmMasks & 0x10) << 2) | (AlarmTime->dow));
		}
		else
		{
			//Alarm on date
			SendData[3] = (((AlarmMasks & 0x08) << 4) | ((AlarmMasks & 0x10) << 2) | (AlarmTime->day % 10) | ((AlarmTime->day / 10) << 4));
		}
		TWIRW(DS3232M_SLA_ADDRESS, SendData, &RecieveData, 4, 0);
	}
	return;
}

void DS3232M_EnableAlarm(uint8_t AlarmNumber)
{
	uint8_t RecieveData;
	uint8_t SendData[2];
	
	if((AlarmNumber == 1) || (AlarmNumber == 2))
	{
		//Get the current control register
		SendData[0] = DS3232M_REG_CONTROL;
		TWIRW(DS3232M_SLA_ADDRESS, SendData, &RecieveData, 1, 1);
		
		//Enable the requested alarm
		SendData[1] = RecieveData | AlarmNumber;
		TWIRW(DS3232M_SLA_ADDRESS, SendData, &RecieveData, 2, 0);
	}
	return;
}

void DS3232M_DisableAlarm(uint8_t AlarmNumber)
{
	uint8_t RecieveData;
	uint8_t SendData[2];
	
	if((AlarmNumber == 1) || (AlarmNumber == 2))
	{
		//Get the current control register
		SendData[0] = DS3232M_REG_CONTROL;
		TWIRW(DS3232M_SLA_ADDRESS, SendData, &RecieveData, 1, 1);
		
		//Disable the requested alarm
		SendData[1] = (RecieveData & (~AlarmNumber));
		TWIRW(DS3232M_SLA_ADDRESS, SendData, &RecieveData, 2, 0);
		
		//Get the current status register
		SendData[0] = DS3232M_REG_STATUS;
		TWIRW(DS3232M_SLA_ADDRESS, SendData, &RecieveData, 1, 1);
		
		//Clear the interrupt flag for the requested alarm
		SendData[1] = (RecieveData & (~AlarmNumber));
		TWIRW(DS3232M_SLA_ADDRESS, SendData, &RecieveData, 2, 0);
	}
	return;
}

uint8_t DS3232M_GetTemp(int8_t *TempLHS, uint8_t *TempRHS)
{

	uint8_t RecieveData[2];
	uint8_t SendData;
	uint8_t ret;

	SendData = DS3232M_REG_TEMP_HI;
	ret = TWIRW(DS3232M_SLA_ADDRESS, &SendData, RecieveData, 1, 2);

	if(ret == 0)
	{
		*TempLHS = RecieveData[0];				//LHS is in 2's compliment form
		*TempRHS = (RecieveData[1] >> 6)*25;	//The two MSB of this byte are the decimal portion of the temperature in .25s
		return 0;
		//printf("High: 0x%02X\nLow: 0x%02X\n", RecieveData[0], RecieveData[1]);
	}
	else
	{
		printf_P(PSTR("I2C Error (0x%02X)\n"), ret);
		return ret;
	}
}

/** @} */