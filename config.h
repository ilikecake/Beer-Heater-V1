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
*	\brief		Project specific config options for the standalone timer module
*	\author		Pat Satyshur
*	\version	1.1
*	\date		1/20/2013
*	\copyright	Copyright 2013, Pat Satyshur
*	\ingroup 	beer_heater_v1_main
*
*	@{
*/

//Global config options
//Put options here to configure the common modules.

#ifndef _CONFIG_H_
#define _CONFIG_H_

//Config for the TWI module
#define	TWI_USER_CONFIG							//Define this in your user code to allow use of the TWI module
#undef	TWI_USE_ISR								//Define this to enable interrupt driven TWI interface (this code does not work).
#define	_TWI_DEBUG								//Define this to enable the output of debug messages.
#undef	TWI_USE_INTERNAL_PULLUPS				//Define this to use the internal pull-up resistors of the device.
#define	TWI_SCL_FREQ_HZ				100000		//Set the SCL frequency

#endif

/** @} */
