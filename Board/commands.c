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
*	\brief		Command interpreter application specific functions
*	\author		Pat Satyshur
*	\version	1.1
*	\date		1/13/2013
*	\copyright	Copyright 2013, Pat Satyshur
*	\ingroup 	beer_heater_main
*
*	@{
*/

#include "main.h"
//#include "commands.h"


//The number of commands
const uint8_t NumCommands = 11;

//Handler function declerations

//LED control function
static int _F1_Handler (void);
const char _F1_NAME[] PROGMEM 			= "led";
const char _F1_DESCRIPTION[] PROGMEM 	= "Turn LED on or off";
const char _F1_HELPTEXT[] PROGMEM 		= "led <number>";

//Jump to DFU bootloader
static int _F2_Handler (void);
const char _F2_NAME[] PROGMEM 			= "dfu";
const char _F2_DESCRIPTION[] PROGMEM 	= "Jump to bootloader";
const char _F2_HELPTEXT[] PROGMEM 		= "'dfu' has no parameters";

//Get time from RTC
static int _F3_Handler (void);
const char _F3_NAME[] PROGMEM 			= "gettime";
const char _F3_DESCRIPTION[] PROGMEM 	= "Show the current date and time";
const char _F3_HELPTEXT[] PROGMEM 		= "'gettime' has no parameters";

//Set time on RTC
static int _F4_Handler (void);
const char _F4_NAME[] PROGMEM 			= "settime";
const char _F4_DESCRIPTION[] PROGMEM 	= "Set the time";
const char _F4_HELPTEXT[] PROGMEM 		= "settime <month> <day> <year> <hr> <min> <sec> <dow>";

//Read a register from the ADC
static int _F5_Handler (void);
const char _F5_NAME[] PROGMEM 			= "adread";
const char _F5_DESCRIPTION[] PROGMEM 	= "Read a register from the ADC";
const char _F5_HELPTEXT[] PROGMEM 		= "adread <register>";

//Write a register to the ADC
static int _F6_Handler (void);
const char _F6_NAME[] PROGMEM 			= "adwrite";
const char _F6_DESCRIPTION[] PROGMEM 	= "write to a register on the ADC";
const char _F6_HELPTEXT[] PROGMEM 		= "adwrite <register> <data>";

//Test the buzzer
static int _F8_Handler (void);
const char _F8_NAME[] PROGMEM 			= "beep";
const char _F8_DESCRIPTION[] PROGMEM 	= "Test the buzzer";
const char _F8_HELPTEXT[] PROGMEM 		= "beep <time>";

//Turn the relay on or off
static int _F9_Handler (void);
const char _F9_NAME[] PROGMEM 			= "relay";
const char _F9_DESCRIPTION[] PROGMEM 	= "Control the relay";
const char _F9_HELPTEXT[] PROGMEM 		= "relay <state>";

//Manual calibration of the ADC
static int _F10_Handler (void);
const char _F10_NAME[] PROGMEM 			= "cal";
const char _F10_DESCRIPTION[] PROGMEM 	= "Calibrate the ADC";
const char _F10_HELPTEXT[] PROGMEM 		= "'cal' has no parameters";

//Get temperatures from the ADC
static int _F11_Handler (void);
const char _F11_NAME[] PROGMEM 			= "temp";
const char _F11_DESCRIPTION[] PROGMEM 	= "Get temperatures from the ADC";
const char _F11_HELPTEXT[] PROGMEM 		= "'temp' has no parameters";

//Scan the TWI bus for devices
static int _F12_Handler (void);
const char _F12_NAME[] PROGMEM 			= "twiscan";
const char _F12_DESCRIPTION[] PROGMEM 	= "Scan for TWI devices";
const char _F12_HELPTEXT[] PROGMEM 		= "'twiscan' has no parameters";

//Command list
const CommandListItem AppCommandList[] PROGMEM =
{
	{ _F1_NAME,		1,  2,	_F1_Handler,	_F1_DESCRIPTION,	_F1_HELPTEXT	},		//led
	{ _F2_NAME, 	0,  0,	_F2_Handler,	_F2_DESCRIPTION,	_F2_HELPTEXT	},		//dfu
	{ _F3_NAME, 	0,  0,	_F3_Handler,	_F3_DESCRIPTION,	_F3_HELPTEXT	},		//gettime
	{ _F4_NAME, 	7,  7,	_F4_Handler,	_F4_DESCRIPTION,	_F4_HELPTEXT	},		//settime
	{ _F5_NAME, 	1,  1,	_F5_Handler,	_F5_DESCRIPTION,	_F5_HELPTEXT	},		//adread
	{ _F6_NAME, 	2,  2,	_F6_Handler,	_F6_DESCRIPTION,	_F6_HELPTEXT	},		//adwrite	
	{ _F8_NAME,		1,  1,	_F8_Handler,	_F8_DESCRIPTION,	_F8_HELPTEXT	},		//beep
	{ _F9_NAME,		1,  1,	_F9_Handler,	_F9_DESCRIPTION,	_F9_HELPTEXT	},		//relay
	{ _F10_NAME,	0,  0,	_F10_Handler,	_F10_DESCRIPTION,	_F10_HELPTEXT	},		//cal
	{ _F11_NAME,	0,  0,	_F11_Handler,	_F11_DESCRIPTION,	_F11_HELPTEXT	},		//temp
	{ _F12_NAME,	0,  0,	_F12_Handler,	_F12_DESCRIPTION,	_F12_HELPTEXT	},		//twiscan
};

//Command functions

//LED control function
static int _F1_Handler (void)
{
	uint8_t tempData;
	
	if( NumberOfArguments() == 1 )
	{
		if(argAsInt(1) == 1)
		{
			MAX7315WriteReg(MAX7315_REG_BLINK0, 0x00);
		}
		else
		{
			MAX7315WriteReg(MAX7315_REG_BLINK0, 0xFF);
		}
	}
	else if(argAsInt(1) == 4)	//Read a register from the expander.
	{
		MAX7315ReadReg(argAsInt(2), &tempData);
		printf_P(PSTR("reg: 0x%02X\n"), tempData);
	}
	
	else
	{
		LED( (uint8_t)argAsInt(1), (uint8_t)argAsInt(2) );
	}
	return 0;
}

//Jump to DFU bootloader
static int _F2_Handler (void)
{
	printf_P(PSTR("Jumping to bootloader. A manual reset will be required\nPress 'y' to continue..."));
	
	if(GetKeyPress() == 121)
	{
		printf_P(PSTR("Jump\n"));
		DelayMS(100);
		Jump_To_Bootloader();
	}
	
	printf_P(PSTR("Canceled\n"));
	return 0;
}

//Get time from RTC
static int _F3_Handler (void)
{

	TimeAndDate CurrentTime;
	if(DS1390GetTime(&CurrentTime) != 0)
	{
		printf_P(PSTR("Error\n"));
		return 0;
	}
	
	printf_P(PSTR("%02d/%02d/20%02d %02d:%02d:%02d\n"), CurrentTime.month, CurrentTime.day, CurrentTime.year, CurrentTime.hour, CurrentTime.min, CurrentTime.sec);
	return 0;
}

//Set time on RTC
static int _F4_Handler (void)
{
	TimeAndDate CurrentTime;
	
	CurrentTime.month	= argAsInt(1);
	CurrentTime.day		= argAsInt(2);
	CurrentTime.year	= argAsInt(3);
	CurrentTime.hour	= argAsInt(4);
	CurrentTime.min		= argAsInt(5);
	CurrentTime.sec		= argAsInt(6);
	CurrentTime.dow		= argAsInt(7);
	
	if(DS1390SetTime(&CurrentTime) != 0)
	{
		printf_P(PSTR("Error\n"));
		return 0;
	}
	
	printf_P(PSTR("Done\n"));
	return 0;
}

//Read a register from the ADC
static int _F5_Handler (void)
{
	uint8_t data[3];
	uint8_t regToRead = argAsInt(1);
	
	data[0] = 0xFA;
	data[1] = 0xFA;
	data[2] = 0xFA;

	if(AD7794ReadReg(regToRead, data) == true)
	{
		printf_P(PSTR("Data[0]: 0x%02X\n"), data[0]);
		printf_P(PSTR("Data[1]: 0x%02X\n"), data[1]);
		printf_P(PSTR("Data[2]: 0x%02X\n"), data[2]);
	}
	else
	{
		printf_P(PSTR("Error\n"));
	}

	return 0;
}

//Write a register to the ADC
static int _F6_Handler (void)
{
	uint8_t RegToWrite = argAsInt(1);
	uint32_t DataToWrite = argAsInt(2);

	uint8_t data[3];
	
	if(RegToWrite == 5)
	{
		data[0] =  (DataToWrite & 0xFF);
	}
	else if( (RegToWrite == 1) || (RegToWrite == 2) )
	{
		data[0] = ((DataToWrite >> 8) & 0xFF);
		data[1] = (DataToWrite & 0xFF);
	}
	else if( (RegToWrite == 6) || (RegToWrite == 7) )
	{
		data[0] = ((DataToWrite >> 16) & 0xFF);
		data[1] = ((DataToWrite >> 8) & 0xFF);
		data[2] = (DataToWrite& 0xFF);
	}
	else
	{
		return 0;
	}
	
	printf("Data[0]: 0x%02X\n", data[0]);
	printf("Data[1]: 0x%02X\n", data[1]);
	printf("Data[2]: 0x%02X\n", data[2]);
	
	AD7794WriteReg(RegToWrite, data);

	return 0;
}

//Test the buzzer
static int _F8_Handler (void)
{
	uint16_t TimeToBeep = argAsInt(1);

	Beep(TimeToBeep);

	return 0;
}

//Turn the relay on or off
static int _F9_Handler (void)
{
	uint8_t RelayState = argAsInt(1);
	Relay(RelayState);
	return 0;
}

//Manual calibration of the ADC
static int _F10_Handler (void)
{
	uint8_t SendData[3];
	
	//Calibrate channel 1
	SendData[1] = (AD7794_CRH_BIPOLAR|AD7794_CRH_GAIN_1);
	SendData[0] = (AD7794_CRL_REF_INT|AD7794_CRL_REF_DETECT|AD7794_CRL_BUFFER_ON|AD7794_CRL_CHANNEL_AIN1);
	AD7794WriteReg(AD7794_CR_REG_CONFIG, SendData);
	
	//Calibrate zero
	printf_P(PSTR("Calibrating channel 1 zero...."));
	SendData[1] = AD7794_MRH_MODE_IZ_CAL & AD7794_CR_REG_MODE_MASK_H;
	SendData[0] = (AD7794_MRL_CLK_INT_NOOUT | AD7794_MRL_UPDATE_RATE_10_HZ) & AD7794_CR_REG_MODE_MASK_L;
	AD7794WriteReg(AD7794_CR_REG_MODE, SendData);

	AD7794WaitReady();
	printf_P(PSTR("Done!\n"));
	
	//Calibrate full scale
	printf_P(PSTR("Calibrating channel 1 FS...."));
	SendData[1] = AD7794_MRH_MODE_IFS_CAL & AD7794_CR_REG_MODE_MASK_H;
	SendData[0] = (AD7794_MRL_CLK_INT_NOOUT | AD7794_MRL_UPDATE_RATE_10_HZ) & AD7794_CR_REG_MODE_MASK_L;
	AD7794WriteReg(AD7794_CR_REG_MODE, SendData);

	AD7794WaitReady();
	printf_P(PSTR("Done!\n"));
	
	//Calibrate channel 2
	SendData[1] = (AD7794_CRH_BIPOLAR|AD7794_CRH_GAIN_1);
	SendData[0] = (AD7794_CRL_REF_INT|AD7794_CRL_REF_DETECT|AD7794_CRL_BUFFER_ON|AD7794_CRL_CHANNEL_AIN2);
	AD7794WriteReg(AD7794_CR_REG_CONFIG, SendData);
	
	//Calibrate zero
	printf_P(PSTR("Calibrating channel 2 zero...."));
	SendData[1] = AD7794_MRH_MODE_IZ_CAL & AD7794_CR_REG_MODE_MASK_H;
	SendData[0] = (AD7794_MRL_CLK_INT_NOOUT | AD7794_MRL_UPDATE_RATE_10_HZ) & AD7794_CR_REG_MODE_MASK_L;
	AD7794WriteReg(AD7794_CR_REG_MODE, SendData);

	AD7794WaitReady();
	printf_P(PSTR("Done!\n"));
	
	//Calibrate full scale
	printf_P(PSTR("Calibrating channel 2 FS...."));
	SendData[1] = AD7794_MRH_MODE_IFS_CAL & AD7794_CR_REG_MODE_MASK_H;
	SendData[0] = (AD7794_MRL_CLK_INT_NOOUT | AD7794_MRL_UPDATE_RATE_10_HZ) & AD7794_CR_REG_MODE_MASK_L;
	AD7794WriteReg(AD7794_CR_REG_MODE, SendData);

	AD7794WaitReady();
	printf_P(PSTR("Done!\n"));
	
	//Calibrate channel 3
	SendData[1] = (AD7794_CRH_BIPOLAR|AD7794_CRH_GAIN_1);
	SendData[0] = (AD7794_CRL_REF_INT|AD7794_CRL_REF_DETECT|AD7794_CRL_BUFFER_ON|AD7794_CRL_CHANNEL_AIN3);
	AD7794WriteReg(AD7794_CR_REG_CONFIG, SendData);
	
	//Calibrate zero
	printf_P(PSTR("Calibrating channel 3 zero...."));
	SendData[1] = AD7794_MRH_MODE_IZ_CAL & AD7794_CR_REG_MODE_MASK_H;
	SendData[0] = (AD7794_MRL_CLK_INT_NOOUT | AD7794_MRL_UPDATE_RATE_10_HZ) & AD7794_CR_REG_MODE_MASK_L;
	AD7794WriteReg(AD7794_CR_REG_MODE, SendData);

	AD7794WaitReady();
	printf_P(PSTR("Done!\n"));
	
	//Calibrate full scale
	printf_P(PSTR("Calibrating channel 3 FS...."));
	SendData[1] = AD7794_MRH_MODE_IFS_CAL & AD7794_CR_REG_MODE_MASK_H;
	SendData[0] = (AD7794_MRL_CLK_INT_NOOUT | AD7794_MRL_UPDATE_RATE_10_HZ) & AD7794_CR_REG_MODE_MASK_L;
	AD7794WriteReg(AD7794_CR_REG_MODE, SendData);

	AD7794WaitReady();
	printf_P(PSTR("Done!\n"));
	
	//Calibrate channel 6
	SendData[1] = (AD7794_CRH_BIPOLAR|AD7794_CRH_GAIN_1);
	SendData[0] = (AD7794_CRL_REF_INT|AD7794_CRL_REF_DETECT|AD7794_CRL_BUFFER_ON|AD7794_CRL_CHANNEL_AIN6);
	AD7794WriteReg(AD7794_CR_REG_CONFIG, SendData);
	
	//Calibrate zero
	printf_P(PSTR("Calibrating channel 6 zero...."));
	SendData[1] = AD7794_MRH_MODE_IZ_CAL & AD7794_CR_REG_MODE_MASK_H;
	SendData[0] = (AD7794_MRL_CLK_INT_NOOUT | AD7794_MRL_UPDATE_RATE_10_HZ) & AD7794_CR_REG_MODE_MASK_L;
	AD7794WriteReg(AD7794_CR_REG_MODE, SendData);

	AD7794WaitReady();
	printf_P(PSTR("Done!\n"));
	
	//Calibrate full scale
	printf_P(PSTR("Calibrating channel 6 FS...."));
	SendData[1] = AD7794_MRH_MODE_IFS_CAL & AD7794_CR_REG_MODE_MASK_H;
	SendData[0] = (AD7794_MRL_CLK_INT_NOOUT | AD7794_MRL_UPDATE_RATE_10_HZ) & AD7794_CR_REG_MODE_MASK_L;
	AD7794WriteReg(AD7794_CR_REG_MODE, SendData);

	AD7794WaitReady();
	printf_P(PSTR("Done!\n"));

	return 0;
}

//Get temperatures from the ADC
static int _F11_Handler (void)
{
	uint8_t SendData[3];

	//Turn on excitation currents
	SendData[0] = (AD7794_IO_DIR_NORMAL | AD7794_IO_10UA);
	AD7794WriteReg(AD7794_CR_REG_IO, SendData);

	printf_P(PSTR("Taking measurements...\n"));
	
	//Set up channel 2
	SendData[1] = (AD7794_CRH_BIPOLAR|AD7794_CRH_GAIN_1);
	SendData[0] = (AD7794_CRL_REF_INT|AD7794_CRL_REF_DETECT|AD7794_CRL_BUFFER_ON|AD7794_CRL_CHANNEL_AIN2);
	AD7794WriteReg(AD7794_CR_REG_CONFIG, SendData);
	SendData[1] = AD7794_MRH_MODE_SINGLE;
	SendData[0] = (AD7794_MRL_CLK_INT_NOOUT | AD7794_MRL_UPDATE_RATE_10_HZ);
	AD7794WriteReg(AD7794_CR_REG_MODE, SendData);
	AD7794WaitReady();
	printf_P(PSTR("Red: %lu counts\n"), AD7794GetData() );
	
	//Set up channel 3
	SendData[1] = (AD7794_CRH_BIPOLAR|AD7794_CRH_GAIN_1);
	SendData[0] = (AD7794_CRL_REF_INT|AD7794_CRL_REF_DETECT|AD7794_CRL_BUFFER_ON|AD7794_CRL_CHANNEL_AIN3);
	AD7794WriteReg(AD7794_CR_REG_CONFIG, SendData);
	SendData[1] = AD7794_MRH_MODE_SINGLE;
	SendData[0] = (AD7794_MRL_CLK_INT_NOOUT | AD7794_MRL_UPDATE_RATE_10_HZ);
	AD7794WriteReg(AD7794_CR_REG_MODE, SendData);
	AD7794WaitReady();
	printf_P(PSTR("Black: %lu counts\n"), AD7794GetData() );

	//Turn off excitation currents
	SendData[0] = (AD7794_IO_DIR_NORMAL | AD7794_IO_OFF);
	AD7794WriteReg(AD7794_CR_REG_IO, SendData);
	
	//Measure input voltage
	SendData[1] = (AD7794_CRH_BIPOLAR|AD7794_CRH_GAIN_1);
	SendData[0] = (AD7794_CRL_REF_INT|AD7794_CRL_REF_DETECT|AD7794_CRL_BUFFER_ON|AD7794_CRL_CHANNEL_AIN6);
	AD7794WriteReg(AD7794_CR_REG_CONFIG, SendData);
	SendData[1] = AD7794_MRH_MODE_SINGLE;
	SendData[0] = (AD7794_MRL_CLK_INT_NOOUT | AD7794_MRL_UPDATE_RATE_10_HZ);
	AD7794WriteReg(AD7794_CR_REG_MODE, SendData);
	AD7794WaitReady();
	printf_P(PSTR("Heater Voltage: %lu counts\n"), AD7794GetData() );
	
	//Measure internal temperature
	SendData[1] = (AD7794_CRH_BIPOLAR|AD7794_CRH_GAIN_1);
	SendData[0] = (AD7794_CRL_REF_INT|AD7794_CRL_REF_DETECT|AD7794_CRL_BUFFER_ON|AD7794_CRL_CHANNEL_TEMP);
	AD7794WriteReg(AD7794_CR_REG_CONFIG, SendData);
	SendData[1] = AD7794_MRH_MODE_SINGLE;
	SendData[0] = (AD7794_MRL_CLK_INT_NOOUT | AD7794_MRL_UPDATE_RATE_10_HZ);
	AD7794WriteReg(AD7794_CR_REG_MODE, SendData);
	AD7794WaitReady();
	printf_P(PSTR("Internal Temperature: %lu counts\n"), AD7794GetData() );
	
	//Measure heater current
	SendData[1] = (AD7794_CRH_BIPOLAR|AD7794_CRH_GAIN_1);
	SendData[0] = (AD7794_CRL_REF_INT|AD7794_CRL_REF_DETECT|AD7794_CRL_BUFFER_ON|AD7794_CRL_CHANNEL_AIN1);
	AD7794WriteReg(AD7794_CR_REG_CONFIG, SendData);
	SendData[1] = AD7794_MRH_MODE_SINGLE;
	SendData[0] = (AD7794_MRL_CLK_INT_NOOUT | AD7794_MRL_UPDATE_RATE_10_HZ);
	AD7794WriteReg(AD7794_CR_REG_MODE, SendData);
	AD7794WaitReady();
	printf_P(PSTR("Heater Current: %lu counts\n"), AD7794GetData() );
	
	
	AD7794InternalTempCal(2525);
	
	return 0;
}

//Scan the TWI bus for devices
static int _F12_Handler (void)
{
	SPI_Disable();
	InitTWI();

	TWIScan();
	
	DeinitTWI();
	SPI_Init(SPI_SPEED_FCPU_DIV_2 | SPI_ORDER_MSB_FIRST | SPI_SCK_LEAD_FALLING | SPI_SAMPLE_TRAILING | SPI_MODE_MASTER);
	
	return  0;
}

//I think this is handled elsewhere...
/*ISR(USART_RX_vect)
{
	uint8_t c;
	c = UDR0;				//Get char from UART recieve buffer
	CommandGetInput(c);
	//UDR0 = c;				//Send char out on UART transmit buffer
}*/

/** @} */
