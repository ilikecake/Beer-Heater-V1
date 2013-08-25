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
*	\brief		Header file for the MAX7315 I2C GPIO expander driver.
*	\author		Pat Satyshur
*	\version	1.0
*	\date		1/24/2013
*	\copyright	Copyright 2013, Pat Satyshur
*	\ingroup 	hardware
*
*	@{
*/



#ifndef _MAX7315_H_
#define _MAX7315_H_

//The 7-bit i2c slave address (note: this can be different based on the device connections)
#define MAX7315_SLA_7B				0x20

//Registers
#define MAX7315_REG_INPUTS			0x00
#define MAX7315_REG_BLINK0			0x01
#define MAX7315_REG_BLINK1			0x09
#define MAX7315_REG_PORT_CONFIG		0x03
#define MAX7315_REG_MASTER			0x0E
#define MAX7315_REG_CONFIG			0x0F
#define MAX7315_REG_INT10			0x10
#define MAX7315_REG_INT32			0x11
#define MAX7315_REG_INT54			0x12
#define MAX7315_REG_INT76			0x13


//
uint8_t MAX7315Init(void);
uint8_t MAX7315ReadReg(uint8_t RegToRead, uint8_t *RegData);
uint8_t MAX7315WriteReg(uint8_t RegToWrite, uint8_t RegData);
uint8_t MAX7315ModifyReg(uint8_t RegToWrite, uint8_t BitData, uint8_t BitMask);
uint8_t MAX7315IsReg(uint8_t reg);

#endif
/** @} */