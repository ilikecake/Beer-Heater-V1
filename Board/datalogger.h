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
*	\brief		Header file for the datalogger.
*	\author		Pat Satyshur
*	\version	1.0
*	\date		2/17/2013
*	\copyright	Copyright 2013, Pat Satyshur
*	\ingroup 	Datalogger
*
*	@{
*/

#ifndef _DATALOGGER_H_
#define _DATALOGGER_H_

#include "stdint.h"


#define DATALOGGER_PAGE_SIZE			528		//This should be the same as the dataflash page size.
#define DATALOGGER_DATASET_SIZE			18
#define DATALOGGER_USE_CRC				0


//Initalization options
#define DATALOGGER_INIT_APPEND				0x01		//Search for previously written data and append.
#define DATALOGGER_INIT_OVERWRITE			0x02		//Restart data collection at page 0, address 0.
#define DATALOGGER_INIT_RESTART_IF_FULL		0x04		//If the device is full, restart data collection at page 0, address 0.
#define DATALOGGER_INIT_STOP_IF_FULL		0x08		//If the device is full, do not start collecting data.




#define DATALOGGER_HEADER1_PREFIX	0xA0
#define DATALOGGER_HEADER2_SUFFIX	0x00
//TODO: Add exclude sectors

/*typedef struct 
{
	uint8_t Day;
	uint8_t Hour;
	uint8_t Min;
	uint8_t Sec;
	uint16_t Temp;
	uint16_t Humidity;
	uint16_t Pressure;
	uint16_t RedLightLevel;
	uint16_t GreenLightLevel;
	uint16_t BlueLightLevel;
	uint16_t ClearLightLevel;
} DataSetStruct;*/

/** Look for the last data set written in flash to continue writing.
 *  Find the size of the dataset and save it to a global variable
 *	Also calculate how many datasets we can store in a page
 */
void Datalogger_Init(uint8_t SetupByte);

/** Add a set of data to be saved. This function will automatically write the data to flash when a page gets full.*/
void Datalogger_AddDataSet(uint8_t DataSet[]);

/** Save a partial set of data to flash. Call this if the controller needs to be reset. */
void Datalogger_SaveDataToFlash(void);

/** Locate the last set of data written to flash */
void Datalogger_FindLastDataSet(uint16_t *PageNumber, uint16_t *AddressInPage);

/** Writes a given number of datasets to the screen using prinf */
void Datalogger_ReadBackData(uint16_t NumberOfDataSets);


void Datalogger_Start(void);
void Datalogger_Process(void);
void Datalogger_AddDataSetToAverage(void);

//how to do this?
uint8_t Datalogger_RetrieveDataFromFlash(uint16_t *DataPageNumber, uint16_t DataNumberInPage, uint8_t DataSet[]);






#endif
/** @} */
