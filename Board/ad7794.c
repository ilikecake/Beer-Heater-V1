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

float EEMEM NV_AD7794_INTERNAL_TEMP_CAL;		//Store the internal temperature calibration in non-volatile memory

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

//Functions below this line use floating point math
#if AD7794_USE_FLOAT == 1

/**Calibrates the internal temperature sensor. This function assumes that the internal temperature sensor will be read in unipolar mode with the internal 1.17V reference.
*
*	CurrentTemp The current temperature of the device. This value should be entered as 100 times the temperature in C. EX: 25.48C would be 2548.
*	
*	Note: NV_AD7794_INTERNAL_TEMP_CAL is the zero intercept of the temperature calibration. The slope is given in the datasheet.
*/
uint8_t AD7794InternalTempCal(uint32_t CurrentTemp)
{
	uint8_t i;
	uint8_t SendData[2];
	uint32_t RunningSum = 0;
	
	double cal;
	double slope;
	double intercept;
	double TempInput;
	char OutputString[15];
	
	slope = 0.0001721912F;		//Deg C/count
	TempInput = (double)CurrentTemp/100.0F;
	
	cal = eeprom_read_float(&NV_AD7794_INTERNAL_TEMP_CAL);
	dtostrf(cal, 9, 4, OutputString);
	printf_P(PSTR("Current calibrated zero is: %s\n"), OutputString);
	printf_P(PSTR("Taking 10 readings from internal temperature sensor...\n"));
	
	//Measure internal temperature
	SendData[1] = (AD7794_CRH_UNIPOLAR|AD7794_CRH_GAIN_1);
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
	
	intercept = TempInput - slope*(double)RunningSum;
	dtostrf(intercept, 9, 4, OutputString);
	printf_P(PSTR("New calibration is: %s\n"), OutputString);
	
	//eeprom_update_block((const void *)&TempCalArray, (void *)NV_AD7794_INTERNAL_TEMP_CAL, 3);
	eeprom_update_float(&NV_AD7794_INTERNAL_TEMP_CAL, intercept);
	return 0;
}

/**Returns the internal temperature if the device in degrees C*10000. (ex: 23.4567 would return 234567)
*
*	Note: The returned value should never be larger than 24 bits. It can probably be stored in a 24 bit number in the dataflash to save space.
*	TODO: Add a check to make sure the calibration data exsists
*/
int32_t AD7794GetInternalTemp(void)
{
	uint8_t SendData[2];
	uint32_t ADCData = 0;

	double slope;
	double intercept;
	double InternalTemp;
	//char OutputString[20];
	
	slope = 0.0001721912F;		//Deg C/count
	intercept = eeprom_read_float(&NV_AD7794_INTERNAL_TEMP_CAL);
	
	//Measure internal temperature
	SendData[1] = (AD7794_CRH_UNIPOLAR|AD7794_CRH_GAIN_1);
	SendData[0] = (AD7794_CRL_REF_INT|AD7794_CRL_REF_DETECT|AD7794_CRL_BUFFER_ON|AD7794_CRL_CHANNEL_TEMP);
	AD7794WriteReg(AD7794_CR_REG_CONFIG, SendData);
	SendData[1] = AD7794_MRH_MODE_SINGLE;
	SendData[0] = (AD7794_MRL_CLK_INT_NOOUT | AD7794_MRL_UPDATE_RATE_10_HZ);
	AD7794WriteReg(AD7794_CR_REG_MODE, SendData);
	AD7794WaitReady();
	ADCData = AD7794GetData();
	
	InternalTemp = slope*(double)ADCData + intercept;
	
	//dtostrf(InternalTemp, 9, 4, OutputString);
	//printf_P(PSTR("Internal temperature is: %s C\n"), OutputString);
	
	return ((int32_t)(InternalTemp*10000.0F));
}

#endif

/** @} */