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

//Global Variables to hold the status of the device
//TODO: Put these in EEPROM?
uint8_t BH_HardwareStatus;
uint8_t BH_ProgramStatus;
uint8_t BH_HIOStatus;

void BH_InitStatus(void)
{
	//Initalize hardware status to 0xFF
	BH_HardwareStatus = 0xFF;
	BH_ProgramStatus = 0xFF;
	
	//LEDs are initalized to off, buttons are pulled high
	BH_HIOStatus = 0x80;
	return;
}

uint8_t BH_GetStatus(uint8_t StatusID)
{
	if(StatusID == BH_STATUS_HW)
	{
		return BH_HardwareStatus;
	}
	else if(StatusID == BH_STATUS_PROG)
	{
		return BH_ProgramStatus;
	}
	else if(StatusID == BH_STATUS_HIO)
	{
		return BH_HIOStatus;
	}
	return 0;
}

/** Bitval must be 1 or 0 */
void BH_SetStatus(uint8_t StatusID, uint8_t BitMask, uint8_t BitVal)
{
	uint8_t *RegToModify;

	if(StatusID == BH_STATUS_HW)
	{
		RegToModify = &BH_HardwareStatus;
	}
	else if(StatusID == BH_STATUS_PROG)
	{
		RegToModify = &BH_ProgramStatus;
	}
	else if(StatusID == BH_STATUS_HIO)
	{
		RegToModify = &BH_HIOStatus;
	}
	else
	{
		return;
	}
	
	if(BitVal == 1)
	{
		*RegToModify |= BitMask;
	}
	else if(BitVal == 0)
	{
		*RegToModify &= ~BitMask;
	}
	
	return;
}


/** @} */