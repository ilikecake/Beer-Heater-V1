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
*	\brief		Hardware definitions and functions for the beer heater.
*	\author		Pat Satyshur
*	\version	1.0
*	\date		1/26/2013
*	\copyright	Copyright 2013, Pat Satyshur
*	\ingroup 	hardware
*
*	@{
*/

#include "main.h"

volatile uint16_t	ElapsedMS;

uint8_t ButtonState;
uint8_t OldButtonState;

/**Saftey limits. If these temperatures are exceeded, the relay will be shut off. */
uint8_t EEMEM NV_RED_TEMP_SAFTEY_LIMIT[3];			//The maximum allowable temperature on the red thermistor
uint8_t EEMEM NV_BLACK_TEMP_SAFTEY_LIMIT[3];		//The maximum allowable temperature on the black thermistor
uint8_t EEMEM NV_INTERNAL_TEMP_SAFTEY_LIMIT[3];		//The maximum allowable temperature on the ADC temperature sensor

uint8_t EEMEM NV_SET_TEMPERATURE[3];				//The target fermentation temperature
uint8_t EEMEM NV_TEMP_REGULATING = 0;				//Set to 1 when the temperature regulation is active
uint8_t EEMEM NV_CURRENT_ZERO_CAL[3];				//The zero point for the current calibration


void HardwareInit( void )
{
	ElapsedMS	= 0x0000;
	OldButtonState = 0xFF;
	ButtonState = 0xFF;
	
	/* Disable watchdog if enabled by bootloader/fuses */
	MCUSR &= ~(1 << WDRF);
	wdt_disable();
	
	//Disable JTAG (this command must be sent twice)
	MCUCR = 0x80;
	MCUCR = 0x80;

	/* Disable clock division */
	clock_prescale_set(clock_div_1);

	/* Hardware Initialization */
	//LEDs_Init();
	SPI_Init(SPI_SPEED_FCPU_DIV_2 | SPI_ORDER_MSB_FIRST | SPI_SCK_LEAD_FALLING | SPI_SAMPLE_TRAILING | SPI_MODE_MASTER);
	InitTWI();

	
	//Setup timer 0 for 1ms interrupts
	//CTC Mode
	//Clock is Fcpu/64
	//OCR0A interrupt ~every 1ms
	TCCR0A = 0x02;
	TCCR0B = 0x03;
	TIMSK0 = 0x02;
	OCR0A = 124;

	//Setup INT2 for button interrupts
	EICRA = 0x20;	//Interrupt on falling edge of INT2
	EIFR = 0xFF;	//Clear pending interrupts
	EIMSK = 0x04;	//Enable INT2
	
	//Setup GPIO Pins
	
	//PORT B:
	//	0: Dataflash CS line	(Out, high)
	DDRB |= (1<<0);
	PORTB |= (1<<0);
	
	//PORT C:
	//	6: RTC CS line			(Out, high)
	//DDRC = 0x00;
	//PORTC = 0x00;
	
	//PORT D:
	//	2:  Button interrupt		(Input)
	//	4:	A/D CS line				(Out, high)
	//	6:	Relay control signal	(Out, low)
	DDRD = (1<<4)|(1<<6);
	PORTD = 0x10;
	
	//PORT F:
	//	5:	Buzzer	(Out, low)
	DDRF = (1<<5);
	PORTF = 0x00;

	//Setup timer 3 for buzzer
	//CTC mode
	//Clock is FCPU
	//This clock is not turned on here
	TCCR3A = 0x00;
	//TCCR3B = 0x09;
	OCR3AH = 0x03;
	OCR3AL = 0xE8;
	TIMSK3 = 0x02;
	TCNT3H = 0x00;
	TCNT3L = 0x00;
	
	//Initalize peripherals
	AD7794Init();
	MAX7315Init();
	AT45DB321D_Init();
	DS3232M_Init();
	
	//Enable USB and interrupts
	USB_Init();
	sei();
	
	return;
}

void LED(uint8_t LEDValue, uint8_t LEDState)
{
	if(LEDValue == 1)
	{
		if(LEDState == 1)
		{
			MAX7315ModifyReg(MAX7315_REG_BLINK0, 0x00, 0x80);
		}
		else
		{
			MAX7315ModifyReg(MAX7315_REG_BLINK0, 0x80, 0x80);
		}
	}
	else if(LEDValue == 2)
	{
		if(LEDState == 1)
		{
			MAX7315ModifyReg(MAX7315_REG_BLINK0, 0x00, 0x01);
		}
		else
		{
			MAX7315ModifyReg(MAX7315_REG_BLINK0, 0x01, 0x01);
		}
	}
	else if(LEDValue == 3)
	{
		if(LEDState == 1)
		{
			MAX7315ModifyReg(MAX7315_REG_BLINK0, 0x00, 0x02);
		}
		else
		{
			MAX7315ModifyReg(MAX7315_REG_BLINK0, 0x02, 0x02);
		}
	}
	return;
}

void DelayMS(uint16_t ms)
{
	uint16_t WaitMS = 0;
	
	if(ms == 0) return;
	
	//Delay is too long
	if (ms >= 60000)
	{
		return;
	}
	
	//Look for milisecond overflow
	if((ms + ElapsedMS) > 60000)
	{
		WaitMS = ms + ElapsedMS - 60001;
	}
	else
	{
		WaitMS = ms + ElapsedMS;
	}
	
	while (WaitMS != ElapsedMS)
	{
		asm volatile ("nop");
	}
	return;
}

void Beep(uint16_t BeepTimeMS)
{
	BuzzerOn();
	DelayMS(BeepTimeMS);
	BuzzerOff();
}

void BuzzerOn(void)
{
	//Clear timer
	TCNT3H = 0x00;
	TCNT3L = 0x00;

	//Turn on timer
	TCCR3B = 0x09;
	return;
}


void BuzzerOff(void)
{
	//Turn off timer
	TCCR3B = 0x00;
	PORTF &= ~(1<<5);
	return;
}

//This will check the state of the buttons
uint8_t GetButtonState( void )
{
	uint8_t DataToReceive;
	
	MAX7315ReadReg(MAX7315_REG_INPUTS, &DataToReceive);
	DataToReceive = ((DataToReceive & 0x0C) >> 2);
	return ~DataToReceive;
}

void Relay(uint8_t RelayState)
{
	if(RelayState == 1)
	{
		PORTD |= (1<<6);
	}
	else
	{
		PORTD &= ~(1<<6);
	}
	return;
}



//Get a full set of data from the A/D converter
//This function will be used by the datalogger to get data to save
//TODO: Process the thermistor data into deg C in this function instead of other places
//TODO: Change decimal storage format to two 16-bit numbers, one containing the LHS and sign, the other containing the RHS.
void GetData(uint8_t *TheData)
{
	uint8_t SendData[3];
	uint32_t TempData;
	//uint8_t DataLocation;
	
	//DataLocation = 0;
	
	//Turn on excitation current to red thermistor
	SendData[0] = (AD7794_IO_DIR_IOUT1 | AD7794_IO_10UA);
	AD7794WriteReg(AD7794_CR_REG_IO, SendData);

	//Set up channel 2 (red thermistor)
	//	-Unipolar
	//	-Gain of 2
	//	-Internal 1.17V reference
	//	-Buffered
	SendData[1] = (AD7794_CRH_UNIPOLAR|AD7794_CRH_GAIN_2);
	SendData[0] = (AD7794_CRL_REF_INT|AD7794_CRL_REF_DETECT|AD7794_CRL_BUFFER_ON|AD7794_CRL_CHANNEL_AIN2);
	AD7794WriteReg(AD7794_CR_REG_CONFIG, SendData);
	SendData[1] = AD7794_MRH_MODE_SINGLE;
	SendData[0] = (AD7794_MRL_CLK_INT_NOOUT | AD7794_MRL_UPDATE_RATE_10_HZ);
	AD7794WriteReg(AD7794_CR_REG_MODE, SendData);
	AD7794WaitReady();
	TempData = AD7794GetData();
	TheData[0] = ((TempData>>16) & 0xFF);
	TheData[1] = ((TempData>>8) & 0xFF);
	TheData[2] = (TempData & 0xFF);
	//printf_P(PSTR("Red: %lu counts or %d %d %d\n"), TempData, TheData[0], TheData[1], TheData[2]);
	
	//Turn on excitation current to black thermistor
	SendData[0] = (AD7794_IO_DIR_IOUT2 | AD7794_IO_10UA);
	AD7794WriteReg(AD7794_CR_REG_IO, SendData);
	
	//Set up channel 3 (black thermistor)
	//	-Unipolar
	//	-Gain of 2
	//	-Internal 1.17V reference
	//	-Buffered
	SendData[1] = (AD7794_CRH_UNIPOLAR|AD7794_CRH_GAIN_2);
	SendData[0] = (AD7794_CRL_REF_INT|AD7794_CRL_REF_DETECT|AD7794_CRL_BUFFER_ON|AD7794_CRL_CHANNEL_AIN3);
	AD7794WriteReg(AD7794_CR_REG_CONFIG, SendData);
	SendData[1] = AD7794_MRH_MODE_SINGLE;
	SendData[0] = (AD7794_MRL_CLK_INT_NOOUT | AD7794_MRL_UPDATE_RATE_10_HZ);
	AD7794WriteReg(AD7794_CR_REG_MODE, SendData);
	AD7794WaitReady();
	TempData = AD7794GetData();
	TheData[3] = ((TempData>>16) & 0xFF);
	TheData[4] = ((TempData>>8) & 0xFF);
	TheData[5] = (TempData & 0xFF);
	
	//printf_P(PSTR("Black: %lu counts or %d %d %d\n"), TempData, TheData[3], TheData[4], TheData[5]);

	//Turn off excitation currents
	SendData[0] = (AD7794_IO_DIR_NORMAL | AD7794_IO_OFF);
	AD7794WriteReg(AD7794_CR_REG_IO, SendData);
	
	//Set up channel 6 (heater voltage)
	//	-Unipolar
	//	-Gain of 1
	//	-Internal 1.17V reference
	//	-Buffered
	SendData[1] = (AD7794_CRH_UNIPOLAR|AD7794_CRH_GAIN_1);
	SendData[0] = (AD7794_CRL_REF_INT|AD7794_CRL_REF_DETECT|AD7794_CRL_BUFFER_ON|AD7794_CRL_CHANNEL_AIN6);
	AD7794WriteReg(AD7794_CR_REG_CONFIG, SendData);
	SendData[1] = AD7794_MRH_MODE_SINGLE;
	SendData[0] = (AD7794_MRL_CLK_INT_NOOUT | AD7794_MRL_UPDATE_RATE_10_HZ);
	AD7794WriteReg(AD7794_CR_REG_MODE, SendData);
	AD7794WaitReady();
	TempData = AD7794GetData();
	TheData[6] = ((TempData>>16) & 0xFF);
	TheData[7] = ((TempData>>8) & 0xFF);
	TheData[8] = (TempData & 0xFF);
	
	//printf_P(PSTR("Heater Voltage: %lu counts or %d %d %d\n"), TempData, TheData[6], TheData[7], TheData[8]);
	
	//Get internal temperature
	TempData = AD7794GetInternalTemp();
	TheData[9] = ((TempData>>16) & 0xFF);
	TheData[10] = ((TempData>>8) & 0xFF);
	TheData[11] = (TempData & 0xFF);
	
	//printf_P(PSTR("Internal Temperature: %lu counts or %d %d %d\n"), TempData, TheData[9], TheData[10], TheData[11]);
	
	//Set up channel 1 (heater current)
	//	-Bipolar
	//	-Gain of 1
	//	-Internal 1.17V reference
	//	-Buffered
	SendData[1] = (AD7794_CRH_BIAS_AIN1|AD7794_CRH_BOOST|AD7794_CRH_BIPOLAR|AD7794_CRH_GAIN_1);
	SendData[0] = (AD7794_CRL_REF_INT|AD7794_CRL_REF_DETECT|AD7794_CRL_BUFFER_ON|AD7794_CRL_CHANNEL_AIN1);
	AD7794WriteReg(AD7794_CR_REG_CONFIG, SendData);
	SendData[1] = AD7794_MRH_MODE_SINGLE;
	SendData[0] = (AD7794_MRL_CLK_INT_NOOUT | AD7794_MRL_UPDATE_RATE_10_HZ);
	AD7794WriteReg(AD7794_CR_REG_MODE, SendData);
	AD7794WaitReady();
	TempData = AD7794GetData();
	TheData[12] = ((TempData>>16) & 0xFF);
	TheData[13] = ((TempData>>8) & 0xFF);
	TheData[14] = (TempData & 0xFF);
	
	//printf_P(PSTR("Heater Current: %lu counts or %d %d %d\n"), TempData, TheData[12], TheData[13], TheData[14]);

	return;
}


//Timer interrupt 0 for basic timing stuff
ISR(TIMER0_COMPA_vect)
{
	uint16_t inByte;
	ElapsedMS++;
	
	//Handle USB stuff
	//This happens every ~8 ms
	if( ((ElapsedMS & 0x0007) == 0x0000) )// && (USB_IsInitialized == true) )
	{
		//receiv and process a character from the USB CDC interface
		inByte = CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);
		if((inByte > 0) && (inByte < 255))
		{
			CommandGetInputChar(inByte);	//NOTE: this limits the device to recieve a single character every 8ms (I think). This should not be a problem for user input.
		}
		
		CDC_Device_USBTask(&VirtualSerial_CDC_Interface);
		USB_USBTask();
	}
	
	//Count up to 60000ms (1 min) then reset to zero.
	if(ElapsedMS >= 60000)
	{
		ElapsedMS = 0;
	}
}

//Timer 3 toggles the buzzer pin
ISR(TIMER3_COMPA_vect)
{
	PORTF ^= (1<<5);
}

ISR(INT2_vect)
{
	MAX7315ReadReg(MAX7315_REG_INPUTS, &ButtonState);
	
	if(((ButtonState & (1<<2)) == 0x00) && ((OldButtonState & (1<<2)) == (1<<2)))
	{
		printf_P(PSTR("b1\n"));
	}
	else if(((ButtonState&(1<<3)) == 0x00) && ((OldButtonState & (1<<3)) == (1<<3)))
	{
		printf_P(PSTR("b2\n"));
	}


	//printf_P(PSTR("b\n"));
	OldButtonState = ButtonState;
}

/** @} */
