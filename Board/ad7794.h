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
*	\brief		Header file for the AD7794/5 A/D converter.
*	\author		Pat Satyshur
*	\version	1.0
*	\date		8/30/2012
*	\copyright	Copyright 2012, Pat Satyshur
*	\ingroup 	hardware
*
*	@{
*/



#ifndef _AD7794_H_
#define _AD7794_H_

//Setup info

//Set to 1 to enable floating point. The proper libraries and includes need to be set.
#define AD7794_USE_FLOAT			1




//Control register setup
#define AD7794_CR_IGNORE			0x80
#define AD7794_CR_READ				0x40
#define AD7794_CR_WRITE				0x00
#define AD7794_CR_CREAD				0x04

//Registers (for external use)
#define AD7794_CR_REG_COMM			0x00	//Only for write
#define AD7794_CR_REG_STATUS		0x00	//Only for read
#define AD7794_CR_REG_MODE			0x01
#define AD7794_CR_REG_MODE_MASK_H	0xF2
#define AD7794_CR_REG_MODE_MASK_L	0xDF
#define AD7794_CR_REG_CONFIG		0x02
#define AD7794_CR_REG_DATA			0x03
#define AD7794_CR_REG_ID			0x04
#define AD7794_CR_REG_IO			0x05
#define AD7794_CR_REG_OFFSET		0x06
#define AD7794_CR_REG_FS			0x07

//Mode register high byte
#define AD7794_MRH_MODE_CONTINUOUS	0x00
#define AD7794_MRH_MODE_SINGLE		0x20
#define AD7794_MRH_MODE_IDLE		0x40
#define AD7794_MRH_MODE_POWERDOWN	0x60
#define AD7794_MRH_MODE_IZ_CAL		0x80
#define AD7794_MRH_MODE_IFS_CAL		0xA0
#define AD7794_MRH_MODE_SZ_CAL		0xC0
#define AD7794_MRH_MODE_SFS_CAL		0xE0
#define AD7794_MRH_PSW				0x10
#define AD7794_MRH_AMP_CM			0x02

//Mode register low byte
#define AD7794_MRL_CLK_INT_NOOUT		0x00
#define AD7794_MRL_CLK_INT_OUT			0x40
#define AD7794_MRL_CLK_EXT				0x80
#define AD7794_MRL_CLK_EXT_DIV2			0xC0
#define AD7794_MRL_CHOP_DIS				0x10

//update rates defined with chop enabled, for chop disabled, the rates are doubled
#define AD7794_MRL_UPDATE_RATE_0_HZ				0x00
#define AD7794_MRL_UPDATE_RATE_470_HZ			0x01
#define AD7794_MRL_UPDATE_RATE_242_HZ			0x02
#define AD7794_MRL_UPDATE_RATE_123_HZ			0x03
#define AD7794_MRL_UPDATE_RATE_62_HZ			0x04
#define AD7794_MRL_UPDATE_RATE_50_HZ			0x05
#define AD7794_MRL_UPDATE_RATE_39_HZ			0x06
#define AD7794_MRL_UPDATE_RATE_33_2_HZ			0x07
#define AD7794_MRL_UPDATE_RATE_19_6_HZ			0x08
#define AD7794_MRL_UPDATE_RATE_16_7_HZ_50DB		0x09
#define AD7794_MRL_UPDATE_RATE_16_7_HZ_60DB		0x0A
#define AD7794_MRL_UPDATE_RATE_12_5_HZ			0x0B
#define AD7794_MRL_UPDATE_RATE_10_HZ			0x0C
#define AD7794_MRL_UPDATE_RATE_8_33_HZ			0x0D
#define AD7794_MRL_UPDATE_RATE_6_25_HZ			0x0E
#define AD7794_MRL_UPDATE_RATE_4_17_HZ			0x0F	

//Config register high byte
#define AD7794_CRH_BIAS_OFF			0x00
#define AD7794_CRH_BIAS_AIN1		0x40
#define AD7794_CRH_BIAS_AIN2		0x80
#define AD7794_CRH_BIAS_AIN3		0xC0
#define AD7794_CRH_BURNOUT			0x20
#define AD7794_CRH_UNIPOLAR			0x10
#define AD7794_CRH_BIPOLAR			0x00
#define AD7794_CRH_BOOST			0x80
#define AD7794_CRH_GAIN_1			0x00
#define AD7794_CRH_GAIN_2			0x01
#define AD7794_CRH_GAIN_4			0x02
#define AD7794_CRH_GAIN_8			0x03
#define AD7794_CRH_GAIN_16			0x04
#define AD7794_CRH_GAIN_32			0x05
#define AD7794_CRH_GAIN_64			0x06
#define AD7794_CRH_GAIN_128			0x07

//Config register low byte
#define AD7794_CRL_REF_IN1			0x00
#define AD7794_CRL_REF_IN2			0x40
#define AD7794_CRL_REF_INT			0x80
#define AD7794_CRL_REF_DETECT		0x20
#define AD7794_CRL_BUFFER_ON		0x10
#define AD7794_CRL_CHANNEL_AIN1		0x00
#define AD7794_CRL_CHANNEL_AIN2		0x01
#define AD7794_CRL_CHANNEL_AIN3		0x02
#define AD7794_CRL_CHANNEL_AIN4		0x03
#define AD7794_CRL_CHANNEL_AIN5		0x04
#define AD7794_CRL_CHANNEL_AIN6		0x05
#define AD7794_CRL_CHANNEL_TEMP		0x06
#define AD7794_CRL_CHANNEL_AVDD		0x07
#define AD7794_CRL_CHANNEL_GND		0x08

//IO Register
#define AD7794_IO_IOEN				0x40
#define AD7794_IO_DIR_NORMAL		0x00
#define AD7794_IO_DIR_REVERSE		0x04
#define AD7794_IO_DIR_IOUT1			0x08
#define AD7794_IO_DIR_IOUT2			0x0C
#define AD7794_IO_OFF				0x00
#define AD7794_IO_10UA				0x01
#define AD7794_IO_210UA				0x02
#define AD7794_IO_1MA				0x03

//Constants for calibrating the intenal temperature sensor
//The reference value for temperature is 20C
//These values are calculated as the values needed to convert the difference between the current temperature and the reference temperature in 100 times degrees C to the number of counts.
//The equation is counts = ((Tcurrent - Tref)*100) * ((X/Y)+Z)
//The quantity ((X/Y)+Z) represents the slope of the internal temperature sensor in V/(100*deg C) when the part is in bipolar mode.
#define AD7794_TEMP_CAL_REF_TEMP	2000
#define AD7794_TEMP_CAL_X			1755
#define AD7794_TEMP_CAL_Y			23400
#define AD7794_TEMP_CAL_Z			58

//Low Level Functions
void AD7794Init( void );
void AD7794Select(uint8_t sel);
void AD7794SendReset( void );
uint8_t AD7794WaitReady( void );

bool AD7794ReadReg(uint8_t reg, uint8_t *DataToRead);
bool AD7794WriteReg(uint8_t reg, uint8_t *DataToWrite);

uint32_t AD7794GetData( void );


//These functions use floating point math
//TODO: Add basic functions that do not use floating point?
#if AD7794_USE_FLOAT == 1
uint8_t AD7794InternalTempCal(uint32_t CurrentTemp);
int32_t AD7794GetInternalTemp(void);
#endif

















#endif
/** @} */