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
const uint8_t NumCommands = 13;

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

//Set up the calibration for the internal temperature sensor
static int _F7_Handler (void);
const char _F7_NAME[] PROGMEM 			= "tempcal";
const char _F7_DESCRIPTION[] PROGMEM 	= "Calibrate the internal temperature sensor";
const char _F7_HELPTEXT[] PROGMEM 		= "'tempcal' has no parameters";

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

//Dataflash functions
static int _F13_Handler (void);
const char _F13_NAME[] PROGMEM 			= "mem";
const char _F13_DESCRIPTION[] PROGMEM 	= "dataflash functions";
const char _F13_HELPTEXT[] PROGMEM 		= "mem <1> <2> <3>";

//Command list
const CommandListItem AppCommandList[] PROGMEM =
{
	{ _F1_NAME,		1,  2,	_F1_Handler,	_F1_DESCRIPTION,	_F1_HELPTEXT	},		//led
	{ _F2_NAME, 	0,  0,	_F2_Handler,	_F2_DESCRIPTION,	_F2_HELPTEXT	},		//dfu
	{ _F3_NAME, 	0,  0,	_F3_Handler,	_F3_DESCRIPTION,	_F3_HELPTEXT	},		//gettime
	{ _F4_NAME, 	7,  7,	_F4_Handler,	_F4_DESCRIPTION,	_F4_HELPTEXT	},		//settime
	{ _F5_NAME, 	1,  1,	_F5_Handler,	_F5_DESCRIPTION,	_F5_HELPTEXT	},		//adread
	{ _F6_NAME, 	2,  2,	_F6_Handler,	_F6_DESCRIPTION,	_F6_HELPTEXT	},		//adwrite	
	{ _F7_NAME, 	0,  0,	_F7_Handler,	_F7_DESCRIPTION,	_F7_HELPTEXT	},		//tempcal
	{ _F8_NAME,		1,  1,	_F8_Handler,	_F8_DESCRIPTION,	_F8_HELPTEXT	},		//beep
	{ _F9_NAME,		1,  1,	_F9_Handler,	_F9_DESCRIPTION,	_F9_HELPTEXT	},		//relay
	{ _F10_NAME,	0,  0,	_F10_Handler,	_F10_DESCRIPTION,	_F10_HELPTEXT	},		//cal
	{ _F11_NAME,	0,  0,	_F11_Handler,	_F11_DESCRIPTION,	_F11_HELPTEXT	},		//temp
	{ _F12_NAME,	0,  0,	_F12_Handler,	_F12_DESCRIPTION,	_F12_HELPTEXT	},		//twiscan
	{ _F13_NAME,	1,  3,	_F13_Handler,	_F13_DESCRIPTION,	_F13_HELPTEXT	},		//twiscan
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
			LED(1,1);
			LED(2,1);
			LED(3,1);
			//MAX7315WriteReg(MAX7315_REG_BLINK0, 0x00);
		}
		else
		{
			LED(1,0);
			LED(2,0);
			LED(3,0);
			//MAX7315WriteReg(MAX7315_REG_BLINK0, 0xFF);
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
	
	if(WaitForAnyKey() == 'y')
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
	DS3232M_GetTime(&CurrentTime);
	/*if(DS1390GetTime(&CurrentTime) != 0)
	{
		printf_P(PSTR("Error\n"));
		return 0;
	}*/
	
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
	
	DS3232M_SetTime(&CurrentTime);
	/*if(DS1390SetTime(&CurrentTime) != 0)
	{
		printf_P(PSTR("Error\n"));
		return 0;
	}*/
	
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

//tempcal
static int _F7_Handler (void)
{
	int32_t RedTemp;
	int32_t BlackTemp;
	char selection;
	uint8_t Dataset[16];
	uint32_t TempData;
	
	printf_P(PSTR("Taking measurements...\n"));
	GetData(Dataset);
	
	//Process the temperature data
	TempData = (((uint32_t)Dataset[0]<<16) | ((uint32_t)Dataset[1]<<8) | (uint32_t)Dataset[2]);
	RedTemp = ThermistorCountsToTempNum(TempData);
	
	//ThermistorCountsToTemp(TempData, RedOutput);
	
	TempData = (((uint32_t)Dataset[3]<<16) | ((uint32_t)Dataset[4]<<8) | (uint32_t)Dataset[5]);
	BlackTemp = ThermistorCountsToTempNum(TempData);
	//ThermistorCountsToTemp(TempData, BlackOutput);
	
	printf_P(PSTR("Select temperature source\n"));
	printf_P(PSTR("[1] Red:   %d.%lu C\n"), (int16_t)(RedTemp/10000), (uint32_t)(RedTemp-((RedTemp/10000)*10000)) );
	printf_P(PSTR("[2] Black: %d.%lu C\n"), (int16_t)(BlackTemp/10000), (uint32_t)(BlackTemp-((BlackTemp/10000)*10000)) );
	//printf_P(PSTR("[1] Red:   %s C\n"), RedOutput);
	//printf_P(PSTR("[2] Black: %s C\n"), BlackOutput);
	printf_P(PSTR("[3] Manual input\n"));
	
	selection = WaitForAnyKey()-48;
	
	if(selection == 1)
	{
		printf_P(PSTR("Calibrating using red\n"));
		AD7794InternalTempCal(RedTemp);
	}
	else if(selection == 2)
	{
		printf_P(PSTR("Calibrating using black\n"));
		AD7794InternalTempCal(BlackTemp);
	}
	else if(selection == 3)
	{
		printf_P(PSTR("Enter the temperature in degrees C*10000\n"));
		GetNewCommand();
		
		TempData = argAsInt(0);
		
		if((TempData < 500000) && (TempData > 0))
		{
			AD7794InternalTempCal(TempData);
		}
	}
	else
	{
		printf_P(PSTR("Invalid input\n"));
	}
	
	//printf("I heard %d\n", selection);
	
	
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
//TODO: Take some of the often used strings and set them as globals to save code space.
static int _F10_Handler (void)
{
	uint8_t SendData[3];
	uint8_t selection;
	//uint32_t TempData;
	
	printf_P(PSTR("Performing A/D internal calibraion\n"));
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
	
	printf_P(PSTR("Get current sensor zero value? [y/n]\n"));
	selection = WaitForAnyKey();
	
	if((selection == 'Y') || (selection == 'y'))
	{
		printf_P(PSTR("Taking zero reading from current sensor...."));
		
		CalibrateHeaterCurrent();
		
		
		
		
		printf_P(PSTR("Done!\n"));
	}
	
	
	
	return 0;
}

//Get temperatures from the ADC
static int _F11_Handler (void)
{
	uint8_t Dataset[16];
	uint32_t TempData;
	int32_t signedTempData;
	
	printf_P(PSTR("Taking measurements...\n"));
	
	GetData(Dataset);
	
	TempData = (((uint32_t)Dataset[0]<<16) | ((uint32_t)Dataset[1]<<8) | (uint32_t)Dataset[2]);
	TempData = ThermistorCountsToTempNum(TempData);
	printf_P(PSTR("Red: %d.%04lu C\n"), (int16_t)(TempData/10000), (uint32_t)(TempData-((TempData/10000)*10000)) );
	
	TempData = (((uint32_t)Dataset[3]<<16) | ((uint32_t)Dataset[4]<<8) | (uint32_t)Dataset[5]);
	TempData = ThermistorCountsToTempNum(TempData);
	printf_P(PSTR("Black: %d.%04lu C\n"), (int16_t)(TempData/10000), (uint32_t)(TempData-((TempData/10000)*10000)) );
	
	TempData = (((uint32_t)Dataset[6]<<16) | ((uint32_t)Dataset[7]<<8) | (uint32_t)Dataset[8]);
	TempData = ConvertHeaterVoltage(TempData);
	printf_P(PSTR("Heater Voltage: %d.%04lu V\n"), (int16_t)(TempData/10000), (uint32_t)(TempData-((TempData/10000)*10000)) );
	
	TempData = (((uint32_t)Dataset[9]<<16) | ((uint32_t)Dataset[10]<<8) | (uint32_t)Dataset[11]);
	printf_P(PSTR("Internal Temperature: %d.%04lu C\n"), (int16_t)(TempData/10000), (uint32_t)(TempData-((TempData/10000)*10000)) );
	
	TempData = (((uint32_t)Dataset[12]<<16) | ((uint32_t)Dataset[13]<<8) | (uint32_t)Dataset[14]);
	signedTempData = ConvertHeaterCurrent(TempData);
	//printf_P(PSTR("Heater Current: %ld\n"), signedTempData);
	//printf_P(PSTR("int1: %ld\n"), (signedTempData-((signedTempData/10000)*10000)));
	//printf_P(PSTR("int1: %lu\n"), labs((signedTempData-((signedTempData/10000)*10000))));
	printf_P(PSTR("Heater Current: %d.%04lu A\n"), (int16_t)(signedTempData/10000), labs(signedTempData-((signedTempData/10000)*10000)) );
	
	return 0;
}

//Scan the TWI bus for devices
static int _F12_Handler (void)
{
	InitTWI();
	TWIScan();
	DeinitTWI();
	return  0;
}

//Dataflash functions
static int _F13_Handler (void)
{
	uint8_t arg1 = argAsInt(1);
	uint8_t arg2;
	uint8_t arg3;
	uint8_t ReadBytes[3];
	
	switch (arg1)
	{
		case 1:
			arg2 = argAsInt(2);
			if(arg2 == 1)
			{
				printf_P(PSTR("Dataflash selected\n"));
				AT45DB321D_Select();
			}
			else
			{
				printf_P(PSTR("Dataflash deselected\n"));
				AT45DB321D_Deselect();
			}
			break;
			
		case 2:
			arg3 = AT45DB321D_ReadStatus();
			printf_P(PSTR("Stat: 0x%02X\n"), AT45DB321D_ReadStatus());
			break;
			
		case 3:
			printf_P(PSTR("HW Status: 0x%02X\n"), BH_GetStatus(BH_STATUS_HW));
			printf_P(PSTR("HIO Status: 0x%02X\n"), BH_GetStatus(BH_STATUS_HIO));
			printf_P(PSTR("Prog Status: 0x%02X\n"), BH_GetStatus(BH_STATUS_PROG));
			break;
			
		case 4:
			AT45DB321D_ReadID(ReadBytes);
			printf_P(PSTR("ID1: 0x%02X\n"), ReadBytes[0]);
			printf_P(PSTR("ID2: 0x%02X\n"), ReadBytes[1]);
			printf_P(PSTR("ID3: 0x%02X\n"), ReadBytes[2]);
			break;
			
		case 5:
			DS3232M_GetStatus();
			break;
	
	}
	
	return  0;
}

/** @} */
