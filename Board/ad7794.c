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
*	\brief		Hardware driver for the AD7794/5 A/D converter
*	\author		Pat Satyshur
*	\version	1.0
*	\date		8/30/2012
*	\copyright	Copyright 2012, Pat Satyshur
*	\ingroup 	hardware
*
*	@{
*/

#include "main.h"

uint8_t EEMEM AD7794_INTERNAL_TEMP_CAL[3];		//Store the internal temperature calibration in non-volatile memory

void AD7794Init( void )
{
	uint8_t SendData[3];
	
	AD7794SendReset();
	
	
	//Calibrate channels
	SendData[1] = (AD7794_CRH_BIPOLAR|AD7794_CRH_GAIN_1);
	SendData[0] = (AD7794_CRL_REF_INT|AD7794_CRL_REF_DETECT|AD7794_CRL_BUFFER_ON|AD7794_CRL_CHANNEL_AIN2);
	AD7794WriteReg(AD7794_CR_REG_CONFIG, SendData);
	
	/*
	//Calibrate zero
	SendData[1] = AD7794_MRH_MODE_IZ_CAL & AD7794_CR_REG_MODE_MASK_H;
	SendData[0] = (AD7794_MRL_CLK_INT_NOOUT | AD7794_MRL_UPDATE_RATE_10_HZ) & AD7794_CR_REG_MODE_MASK_L;
	AD7794WriteReg(AD7794_CR_REG_MODE, SendData);
	
	
	
	
	*/
	
	//Put ADC in idle mode
	SendData[1] = AD7794_MRH_MODE_IDLE;
	SendData[0] = (AD7794_MRL_CLK_INT_NOOUT | AD7794_MRL_UPDATE_RATE_10_HZ);
	AD7794WriteReg(AD7794_CR_REG_MODE, SendData);
	
	//Clear data from the data register
	AD7794ReadReg(AD7794_CR_REG_DATA, SendData);
	
	
	return;
}

//sel = 1 to select the chip
void AD7794Select( uint8_t sel )
{
	if(sel == 1)
	{
		PORTD &= ~(1<<4);
	}
	else
	{
		PORTD |= (1<<4);
	}
	return;
}

void AD7794SendReset( void )
{
	AD7794Select(1);
	SPI_SendByte(0xFF);
	SPI_SendByte(0xFF);
	SPI_SendByte(0xFF);
	SPI_SendByte(0xFF);
	SPI_SendByte(0xFF);
	AD7794Select(0);
	return;
}

uint8_t AD7794WaitReady( void )
{
	uint8_t data;
	uint16_t timeout = 0;
	
	for(timeout = 0; timeout < 500; timeout++) {}
	
	timeout = 0;
	//Poll the status register, look for the ready bit to be cleared
	//Timeout after 50000 tries
	do
	{
		AD7794ReadReg(AD7794_CR_REG_STATUS, &data);
		timeout++;
		if(timeout > 50000) return 0xFF;
	}
	while( (data & 0x80) != 0x00 );

	return data;
}

//LSB of returned register is in DataToRead[0]
bool AD7794ReadReg( uint8_t reg, uint8_t *DataToRead )
{
	if( (reg >= 0) && (reg <= 7) )
	{
		AD7794Select(1);
		SPI_SendByte( AD7794_CR_READ | (reg << 3) );
		
		if( (reg == 0) || (reg == 4) || (reg == 5) )
		{
			//8-bit register
			*DataToRead = SPI_ReceiveByte();
		}
		else if( (reg == 1) || (reg == 2) )
		{
			//16-bit register
			DataToRead[1] = SPI_ReceiveByte();
			DataToRead[0] = SPI_ReceiveByte();
		}
		else
		{
			//16-bit register
			DataToRead[2] = SPI_ReceiveByte();
			DataToRead[1] = SPI_ReceiveByte();
			DataToRead[0] = SPI_ReceiveByte();
		}
		AD7794Select(0);
		return true;
	}
	
	//Invalid register
	return false;
}

//MSB of register is sent first, MSB should be in DataToWrite[0]
bool AD7794WriteReg( uint8_t reg, uint8_t *DataToWrite)
{
	if( (reg == 1) || (reg == 2) || (reg == 5) || (reg == 6) || (reg == 7) )
	{
		//Mask the mode register
		if(reg == 1)
		{
			//The masked bits of the mode register must always be zero
			DataToWrite[1] &= AD7794_CR_REG_MODE_MASK_H;
			DataToWrite[0] &= AD7794_CR_REG_MODE_MASK_L;
		}
		
		//Send data
		AD7794Select(1);
		SPI_SendByte( AD7794_CR_WRITE | (reg << 3) );
		if(reg == 5)
		{
			//8-bit write
			SPI_SendByte(DataToWrite[0]);
		}
		else if( (reg == 1) || (reg == 2) )
		{
			//16-bit write
			SPI_SendByte(DataToWrite[1]);
			SPI_SendByte(DataToWrite[0]);
		}
		else
		{
			//24-bit write
			SPI_SendByte(DataToWrite[2]);
			SPI_SendByte(DataToWrite[1]);
			SPI_SendByte(DataToWrite[0]);
		}
		AD7794Select(0);
		return true;
	}

	//Invalid register
	return false;
}


uint32_t AD7794GetData( void )
{
	uint8_t ADC_Data[3];
	uint32_t OutputData = 0;
	
	if(AD7794ReadReg(AD7794_CR_REG_DATA, ADC_Data) == true)
	{
		//printf("0x%02X 0x%02X 0x%02X \n", ADC_Data[2], ADC_Data[1], ADC_Data[0]);
		OutputData = ((uint32_t)ADC_Data[2] << 16) | ((uint32_t)ADC_Data[1] << 8) | ((uint32_t)ADC_Data[0]);
		return OutputData;
		//return (((uint32_t)ADC_Data[2]*65536ul)+(ADC_Data[1]*256)+(ADC_Data[0]));
	}
	return 0;
}

/**Calibrates the internal temperature sensor. This function assumes that the internal temperature sensor will be read in bipolar mode with the internal 1.17V reference.
*
*	CurrentTemp The current temperature of the device. This value should be entered as 100 times the temperature in C. EX: 25.48C would be 2548.
*/
uint8_t AD7794InternalTempCal(uint32_t CurrentTemp)
{
	uint8_t i;
	int32_t TempCorrectionCounts;
	uint8_t SendData[2];
	uint32_t RunningSum = 0;
	uint8_t TempCalArray[3];
	
	eeprom_read_block ((void *)&TempCalArray, (const void *)&AD7794_INTERNAL_TEMP_CAL , 3);
	
	printf_P(PSTR("Current calibration value is 0x%02X%02X%02X\n"), TempCalArray[2], TempCalArray[1], TempCalArray[0]);
	
	//This is the number of ADC counts (theoretically) between the input temperature and the reference temperature of 20C
	TempCorrectionCounts = ((CurrentTemp-AD7794_TEMP_CAL_REF_TEMP)*AD7794_TEMP_CAL_X) / AD7794_TEMP_CAL_Y;
	TempCorrectionCounts = TempCorrectionCounts + ((CurrentTemp-AD7794_TEMP_CAL_REF_TEMP)*AD7794_TEMP_CAL_Z);
	
	printf("Correction: %lu\n", TempCorrectionCounts);
	printf_P(PSTR("Taking 10 readings from internal temperature sensor...\n"));
	
	//Measure internal temperature
	SendData[1] = (AD7794_CRH_BIPOLAR|AD7794_CRH_GAIN_1);
	SendData[0] = (AD7794_CRL_REF_INT|AD7794_CRL_REF_DETECT|AD7794_CRL_BUFFER_ON|AD7794_CRL_CHANNEL_TEMP);
	AD7794WriteReg(AD7794_CR_REG_CONFIG, SendData);
	SendData[1] = AD7794_MRH_MODE_CONTINUOUS;
	SendData[0] = (AD7794_MRL_CLK_INT_NOOUT | AD7794_MRL_UPDATE_RATE_10_HZ);
	AD7794WriteReg(AD7794_CR_REG_MODE, SendData);
	
	for(i=0;i<10;i++)
	{
		AD7794WaitReady();
		RunningSum += AD7794GetData();
		
	}

	RunningSum = RunningSum/i;

	printf_P(PSTR("Internal Temperature: %lu counts\n"), RunningSum);
	
	RunningSum = RunningSum-TempCorrectionCounts;	
	TempCalArray[0] = (uint8_t)(RunningSum&0xFF);
	TempCalArray[1] = (uint8_t)((RunningSum>>8)&0xFF);
	TempCalArray[2] = (uint8_t)((RunningSum>>16)&0xFF);
	
	printf_P(PSTR("New calibration value is 0x%02X%02X%02X\n"), TempCalArray[2], TempCalArray[1], TempCalArray[0]);
	
	eeprom_update_block((const void *)&TempCalArray, (void *)AD7794_INTERNAL_TEMP_CAL, 3);
	
	return 0;
}



/** @} */