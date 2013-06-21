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
*	\brief		MAX7315 I2C GPIO expander driver.
*	\author		Pat Satyshur
*	\version	1.0
*	\date		1/24/2013
*	\copyright	Copyright 2013, Pat Satyshur
*	\ingroup 	hardware
*
*	@{
*/

#include "main.h"

void MAX7315Init( void )
{
	MAX7315WriteReg(MAX7315_REG_PORT_CONFIG, 0x7C);		//Ports 0, 1, and 7 are outputs
	MAX7315WriteReg(MAX7315_REG_CONFIG, 0x08);			//Blink disabled, global intensity control disabled, INT is enabled
	MAX7315WriteReg(MAX7315_REG_BLINK0, 0xFF);
	
	MAX7315ReadReg(MAX7315_REG_INPUTS, NULL);			//This is needed to enable the interrupt
	return;
}

//Returns 0 if everything worked
uint8_t MAX7315ReadReg(uint8_t RegToRead, uint8_t *RegData)
{
	uint8_t stat;
	if(MAX7315IsReg(RegToRead) == 1)
	{
		//Switch to TWI
		//InitTWI();
		
		stat = TWIRW(MAX7315_SLA_7B, &RegToRead, RegData, 1, 1);
		
		//Switch to SPI
		//DeinitTWI();
		
		if(stat > 0)
		{
			return stat;
		}
		return 0;
	}
	return 0xFF;
}

uint8_t MAX7315WriteReg(uint8_t RegToWrite, uint8_t RegData)
{
	uint8_t WriteReg[2];
	uint8_t stat;
	if(MAX7315IsReg(RegToWrite) == 1)
	{
		WriteReg[0] = RegToWrite;
		WriteReg[1] = RegData;
		
		//Switch to TWI
		//SPI_Disable();
		//InitTWI();
		
		stat = TWIRW(MAX7315_SLA_7B, WriteReg, &RegData, 2, 0);
		
		//Switch to SPI
		//DeinitTWI();
		//SPI_Init(SPI_SPEED_FCPU_DIV_2 | SPI_ORDER_MSB_FIRST | SPI_SCK_LEAD_FALLING | SPI_SAMPLE_TRAILING | SPI_MODE_MASTER);
		
		if(stat > 0)
		{
			return stat;
		}
		return 0;
	}
	return 0xFF;
}

uint8_t MAX7315ModifyReg(uint8_t RegToWrite, uint8_t BitData, uint8_t BitMask)
{
	uint8_t WriteReg[2];
	uint8_t ReadReg;
	uint8_t stat;
	
	//Make sure the register is valid
	if(MAX7315IsReg(RegToWrite) != 1)
	{
		return 0xFF;
	}

	//Switch to TWI
	//SPI_Disable();
	//InitTWI();

	//Read the register
	WriteReg[0] = RegToWrite;
	stat = TWIRW(MAX7315_SLA_7B, WriteReg, &ReadReg, 1, 1);
	if(stat > 0)
	{
		return stat;
	}
	
	//Modify the register data and write back to the device
	WriteReg[1] = (ReadReg & (~BitMask)) | BitData;
	stat = TWIRW(MAX7315_SLA_7B, WriteReg, &ReadReg, 2, 0);
	
	//Switch to SPI
	//DeinitTWI();
	//SPI_Init(SPI_SPEED_FCPU_DIV_2 | SPI_ORDER_MSB_FIRST | SPI_SCK_LEAD_FALLING | SPI_SAMPLE_TRAILING | SPI_MODE_MASTER);
	
	if(stat > 0)
	{
		return stat;
	}
	
	return 0;
}

uint8_t MAX7315IsReg(uint8_t reg)
{
	if((reg == 0x00) || (reg == 0x01) || (reg == 0x03) || (reg == 0x09) || (reg == 0x0E) || (reg == 0x0F) || (reg == 0x10) || (reg == 0x11) || (reg == 0x12) || (reg == 0x13))
	{
		return 1;
	}
	return 0;
}

/** @} */