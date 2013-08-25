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
*	\brief		Thermistor functions.
*	\author		Pat Satyshur
*	\version	1.0
*	\date		6/22/2013
*	\copyright	Copyright 2013, Pat Satyshur
*	\ingroup 	hardware
*
*	@{
*/

#include "main.h"
#include "math.h"
#include <avr/pgmspace.h>
#include <stdlib.h>

//Convert the thermistor measurment in counts into a temperature in degrees C.
//Using the Steinhart-Hart curve fit
void ThermistorCountsToTemp (uint32_t Counts, char *outputval)
{
	double A;
	double B;
	double C;
	
	double Temp;
	double R;
	
	//uint8_t i;
	//char outputval[10];

	//Steinhart-Hart Coefficients for the thermistor
	//TODO: put these in progmem?
	//TODO: allow for multiple coefficients?
	A = 0.001126107;
	B = 0.000235532;
	C = 8.94216394359268E-008;
	
	//Note: The number here is calculated as (Vref/(2^bits*Gain*Current(amps)))
	//Ex: 1.17/(2^24*2*0.00002) = 0.0017434359
	//TODO: can this be done with compiler directives?
	R = (double)Counts*0.0017434359F;
	
	Temp = A + B*log(R) + C*pow(log(R),3);
	
	//Convert Kelvin to Celcius
	Temp = (1/Temp) - 272.15F;
	dtostrf(Temp, 9, 4, outputval);
	
	return;
}

int32_t ThermistorCountsToTempNum (uint32_t Counts)
{
	double A;
	double B;
	double C;
	
	double Temp;
	double R;
	
	//uint8_t i;
	//char outputval[10];

	//Steinhart-Hart Coefficients for the thermistor
	//TODO: put these in progmem?
	//TODO: allow for multiple coefficients?
	A = 0.001126107;
	B = 0.000235532;
	C = 8.94216394359268E-008;
	
	//Note: The number here is calculated as (Vref/(2^bits*Gain*Current(amps)))
	//Ex: 1.17/(2^24*2*0.00002) = 0.0017434359
	//TODO: can this be done with compiler directives?
	R = (double)Counts*0.0017434359F;
	
	Temp = A + B*log(R) + C*pow(log(R),3);
	
	//Convert Kelvin to Celcius
	Temp = (1/Temp) - 272.15F;
	//dtostrf(Temp, 9, 4, outputval);
	
	return ((int32_t)(Temp*10000.0F));
}




/** @} */