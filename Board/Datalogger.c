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
*	\brief		Datalogger driver.
*	\author		Pat Satyshur
*	\version	1.0
*	\date		2/17/2013
*	\copyright	Copyright 2013, Pat Satyshur
*	\ingroup 	Datalogger
*
*	@{
*/

//Impliment a datalogger program. This program is meant to be used to record data sets relativly slowly.

#include "main.h"

#define DATALOGGER_DATA_SAVE_RATE		30	//these must be less than 60 for now...
#define DATALOGGER_MIN_TO_SKIP			5

uint8_t DataToSave[DATALOGGER_DATASET_SIZE];		//The running average of the data
uint8_t DataLoggerStarted = 0;
		//The current data set to average

//uint8_t DataSaveRate = 30;		//How many minutes between data saving intervals
//uint8_t MinToSkip = 5;			//How many minutes between data sample intervals
uint8_t DataSetNumber;
uint8_t MinToWaitFor;		//Counter for the current minute to wait for
uint8_t NextTimeToSaveData;

//Variables used to write data
uint16_t DataSetAddress;	//Points to the address in the page where the next data set goes
uint16_t DataPageAddress;	//Points to the current page to which we are writing data
uint8_t BufferInUse;		//Points to the current buffer to which we are saving data

//I think these are useless
uint8_t DataSetSizeBytes;	
//uint8_t DataSetsPerPage;

uint8_t DataloggerInitalized = 0;

void Datalogger_Init(uint8_t SetupByte)
{
	uint16_t StartingPage;
	uint16_t StartingLocationInPage;
	
	DataSetSizeBytes = DATALOGGER_DATASET_SIZE + 2;
	#if DATALOGGER_USE_CRC == 1
	DataSetSizeBytes++;
	#endif

	printf_P(PSTR("Data set size: %u\n"), DataSetSizeBytes);
	printf_P(PSTR("Sets per page: %u\n"), DATALOGGER_PAGE_SIZE/DataSetSizeBytes);

	//printf_P(PSTR("h1: 0x%02X\n"), ((DataSetSizeBytes >> 4) | DATALOGGER_HEADER1_PREFIX) );
	//printf_P(PSTR("h2: 0X%02X\n"), ((uint8_t)(DataSetSizeBytes << 4) | DATALOGGER_HEADER2_SUFFIX));

	//This should eventually search for preexisting data sets, but for now, initalize to zero
	if((SetupByte & DATALOGGER_INIT_APPEND) == DATALOGGER_INIT_APPEND)
	{
		Datalogger_FindLastDataSet(&StartingPage, &StartingLocationInPage);
		if((StartingPage > 0x1FFF) && (StartingLocationInPage > 0x1FFF))
		{
			if((SetupByte & DATALOGGER_INIT_RESTART_IF_FULL) == DATALOGGER_INIT_RESTART_IF_FULL)
			{
				DataSetAddress = 0;
				DataPageAddress = 0;
			}
			else
			{
				DataloggerInitalized = 0;
				return;
			}
		}
		else
		{
			DataSetAddress = StartingLocationInPage;
			DataPageAddress = StartingPage;
		}
	}
	else
	{
		DataSetAddress = 0;
		DataPageAddress = 0;
	}
	
	BufferInUse = 1;
	
	printf_P(PSTR("Starting data collection in page 0x%04X at address 0x%04X\n"), DataPageAddress, DataSetAddress);
	
	
	DataloggerInitalized = 1;
	

	return;
}


void Datalogger_AddDataSet(uint8_t DataSet[])
{
	uint8_t DataSetHeader[2];

	if(DataloggerInitalized != 1)
	{
		return;
	}
	printf_P(PSTR("Writing to buffer %u\n"), BufferInUse);
	
	DataSetHeader[0] = ((DataSetSizeBytes >> 4) | DATALOGGER_HEADER1_PREFIX);
	DataSetHeader[1] = ((uint8_t)(DataSetSizeBytes << 4) | DATALOGGER_HEADER2_SUFFIX);
	
	//Write data set header
	AT45DB321D_BufferWrite(BufferInUse, DataSetAddress, DataSetHeader, 2);
	DataSetAddress += 2;
	
	//Write data
	AT45DB321D_BufferWrite(BufferInUse, DataSetAddress, DataSet, DATALOGGER_DATASET_SIZE);
	DataSetAddress += DATALOGGER_DATASET_SIZE;
	
	//Write CRC
	#if DATALOGGER_USE_CRC == 1
	AT45DB321D_BufferWrite(BufferInUse, DataSetAddress, DataSetCRC, 1);
	DataSetAddress += 1;
	#endif
	
	//If the page is full...
	//fix this to handle CRC later
	#if DATALOGGER_USE_CRC == 1
	if((DataSetAddress + (DATALOGGER_DATASET_SIZE+3)) > DATALOGGER_PAGE_SIZE)
	#else
	if((DataSetAddress + (DATALOGGER_DATASET_SIZE+2)) > DATALOGGER_PAGE_SIZE)
	#endif
	{
		printf_P(PSTR("Writing to page %u\n"), DataPageAddress);
	
		//Save the data buffer to flash
		AT45DB321D_CopyBufferToPage(BufferInUse, DataPageAddress);
		
		//Switch to the other buffer (is this useful?)
		if(BufferInUse == 1)
		{
			BufferInUse = 2;
		}
		else
		{
			BufferInUse = 1;
		}
		
		//Increment page address
		DataPageAddress++;
		if(DataPageAddress > 0x1FFF)
		{
			DataPageAddress = 0;
		}
		
		//Reset address in page to zero
		DataSetAddress = 0;
	}
	
	printf_P(PSTR("BufferInUse: %u\n"), BufferInUse);
	printf_P(PSTR("DataSetAddress: %u\n"), DataSetAddress);
	printf_P(PSTR("DataPageAddress: %u\n"), DataPageAddress);
	
	return;
}

void Datalogger_SaveDataToFlash(void)
{
	if(DataloggerInitalized != 1)
	{
		return;
	}

	AT45DB321D_CopyBufferToPage(BufferInUse, DataPageAddress);
	AT45DB321D_WaitForReady();
	return;
}

//The page should always start with a dataset header.
//The pages should always start at 0 and go up
void Datalogger_FindLastDataSet(uint16_t *PageNumber, uint16_t *AddressInPage)
{
	uint16_t PageToLook = 0;
	uint16_t AddressToLook = 0;
	uint8_t TempBuffer = 0;
	
	uint8_t TempVal[2];
	uint8_t TempDataSetSize = 0;
	
	//Select the buffer that is not in use
	if(BufferInUse == 1)
	{
		TempBuffer = 2;
	}
	else
	{
		TempBuffer = 1;
	}
	
	//printf_P(PSTR("Looking for data in page 0x%04X at address 0x%04X using buffer %u\n"), PageToLook, AddressToLook, TempBuffer);
	
	while(PageToLook <= 0x1FFF)
	{
		//printf_P(PSTR("Looking for data in page 0x%04X at address 0x%04X using buffer %u\n"), PageToLook, AddressToLook, TempBuffer);
		
		//Retrieve the memory page
		AT45DB321D_CopyPageToBuffer(TempBuffer, PageToLook);
		AT45DB321D_WaitForReady();
		
		//Look for the data start 
		while(AddressToLook <= DATALOGGER_PAGE_SIZE)
		{
			AT45DB321D_BufferRead(TempBuffer, AddressToLook, TempVal, 2);
			TempDataSetSize = ((TempVal[0] & 0x0F) << 4) | ((TempVal[1] & 0xF0) >> 4);
			//printf_P(PSTR("0x%02X 0x%02X"), TempVal[0], TempVal[1]);
			
			if( ((TempVal[0] & 0xF0) == DATALOGGER_HEADER1_PREFIX) && ((TempVal[1] & 0x0F) == DATALOGGER_HEADER2_SUFFIX) && (TempDataSetSize > 0) )
			{
				//printf_P(PSTR("Header found at 0x%04X of size %u\n"), AddressToLook, TempDataSetSize);
				AddressToLook += TempDataSetSize;
			}
			else
			{
				break;
			}
		}
		
		//Check if the page is full
		if((AddressToLook + TempDataSetSize) > DATALOGGER_PAGE_SIZE)
		{
			PageToLook++;
			AddressToLook = 0;
		}
		else
		{
			break;
		}
	}
	
	if(PageToLook == 0x1FFF)
	{
		//printf_P(PSTR("The device is full\n"));
		
		*PageNumber = 0xFFFF;
		*AddressInPage = 0xFFFF;
		
		return;
	}
	if( (PageToLook == 0x00) && (AddressToLook == 0x00) )
	{
		//printf_P(PSTR("The device is empty\n"));
		
		*PageNumber = 0x0000;
		*AddressInPage = 0x0000;
		
		return;
	}
	
	//printf_P(PSTR("Final data header is in page 0x%04X. New data should start at location 0x%04X\n"), PageToLook, AddressToLook);
	
	//printf_P(PSTR("Final data header is in page 0x%04X at address 0x%04X and is of size %u.\n"), PageToLook, AddressToLook-TempDataSetSize, TempDataSetSize);
	//printf_P(PSTR("The next dataset should start at address 0x%04X\n"), AddressToLook);
	
	*PageNumber = PageToLook;
	*AddressInPage = AddressToLook;
	
	return;
}

void Datalogger_ReadBackData(uint16_t NumberOfDataSets)
{
	uint16_t PageToLook = 0;
	uint16_t AddressToLook = 0;
	uint8_t TempBuffer = 0;
	uint8_t i;
	
	uint8_t TempVal[2];
	uint8_t TempDataSetSize = 0;
	
	//Select the buffer that is not in use
	if(BufferInUse == 1)
	{
		TempBuffer = 2;
	}
	else
	{
		TempBuffer = 1;
	}
	
	//printf_P(PSTR("Looking for data in page 0x%04X at address 0x%04X using buffer %u\n"), PageToLook, AddressToLook, TempBuffer);
	
	while(PageToLook <= 0x1FFF)
	{
		//printf_P(PSTR("Looking for data in page 0x%04X at address 0x%04X using buffer %u\n"), PageToLook, AddressToLook, TempBuffer);
		
		//Retrieve the memory page
		AT45DB321D_CopyPageToBuffer(TempBuffer, PageToLook);
		AT45DB321D_WaitForReady();
		
		//Look for the data start 
		while(AddressToLook <= DATALOGGER_PAGE_SIZE)
		{
			AT45DB321D_BufferRead(TempBuffer, AddressToLook, TempVal, 2);
			TempDataSetSize = ((TempVal[0] & 0x0F) << 4) | ((TempVal[1] & 0xF0) >> 4);
			//printf_P(PSTR("0x%02X 0x%02X"), TempVal[0], TempVal[1]);
			
			if( ((TempVal[0] & 0xF0) == DATALOGGER_HEADER1_PREFIX) && ((TempVal[1] & 0x0F) == DATALOGGER_HEADER2_SUFFIX) && (TempDataSetSize > 0) )
			{
				NumberOfDataSets--;
				for(i=2; i<TempDataSetSize; i++)
				{
					AT45DB321D_BufferRead(TempBuffer, AddressToLook+i, TempVal, 1);
					AT45DB321D_WaitForReady();
					printf_P(PSTR("0x%02X, "), TempVal[0]);
				}
				printf_P(PSTR("\b\b \b\n"));
				if(NumberOfDataSets == 0)
				{
					return;
				}
				AddressToLook += TempDataSetSize;
			}
			else
			{
				break;
			}
		}
		
		//Check if the page is full
		if((AddressToLook + TempDataSetSize) > DATALOGGER_PAGE_SIZE)
		{
			PageToLook++;
			AddressToLook = 0;
		}
		else
		{
			break;
		}
	}



	return;
}



//Call this to start the data logging.
void Datalogger_Start(void)
{
	if(DataloggerInitalized == 1)
	{
		TimeAndDate CurrentTime;
		//GetTime(&CurrentTime);

		//Take an inital set of data
		//GetDataSet(DataToSave);
		
		//Clear the time data. This data is put in when the data is written to flash.
		DataToSave[0] = 0;
		DataToSave[1] = 0;
		DataToSave[2] = 0;
		DataToSave[3] = 0;
		
		//Find the next time to get data
		MinToWaitFor = ((CurrentTime.min/DATALOGGER_MIN_TO_SKIP)+1)*DATALOGGER_MIN_TO_SKIP;
		if(MinToWaitFor > 59)
		{
			MinToWaitFor = 0;
		}
		
		//Find the next time to save data
		NextTimeToSaveData = ((CurrentTime.min/DATALOGGER_DATA_SAVE_RATE)+1)*DATALOGGER_DATA_SAVE_RATE;
		if(NextTimeToSaveData > 59)
		{
			NextTimeToSaveData = 0;
		}
		
		DataSetNumber = 1;
		
		printf_P(PSTR("Time is %u min, next data set at %u min, Next time to save data is %u\n"), CurrentTime.min, MinToWaitFor, NextTimeToSaveData);
	}
	
	
	
	DataLoggerStarted = 1;
	return;
}

void Datalogger_Process(void)
{
	TimeAndDate CurrentTime;
	uint8_t i;

	if(DataLoggerStarted == 1)
	{
		GetTime(&CurrentTime);
		
		//check to see if it is time to get another set of data to average.
		if(CurrentTime.min == MinToWaitFor)
		{
			Datalogger_AddDataSetToAverage();
			MinToWaitFor += DATALOGGER_MIN_TO_SKIP;
			if(MinToWaitFor > 59)
			{
				MinToWaitFor -= 60;
			}
			printf_P(PSTR("Next time to take data at %u min\n"), MinToWaitFor);
		}
		
		if(CurrentTime.min == NextTimeToSaveData)
		{
			//Add time data
			DataToSave[0] = CurrentTime.month;
			DataToSave[1] = CurrentTime.day;
			DataToSave[2] = CurrentTime.hour;
			DataToSave[3] = CurrentTime.min;
		
			//Save the data
			Datalogger_AddDataSet(DataToSave);
			
			printf_P(PSTR("Saving at min %u: "), NextTimeToSaveData);
			for(i=0;i<18;i++)
			{
				printf_P(PSTR("0x%02X, "), DataToSave[i]);
			}
			printf_P(PSTR("\n"));
			
			
			
			NextTimeToSaveData += DATALOGGER_DATA_SAVE_RATE;
			if(NextTimeToSaveData > 59)
			{
				NextTimeToSaveData -= 60;
			}
			printf_P(PSTR("Next time to save data at %u min\n"), NextTimeToSaveData);
			
			//Clear the old data
			DataSetNumber = 0;
			for(i=0;i<18;i++)
			{
				DataToSave[i] = 0;
			}
		}
	}
	
	return;
}


void Datalogger_AddDataSetToAverage(void)
{
	uint8_t TempDataSet[DATALOGGER_DATASET_SIZE];
	uint8_t i;
	uint32_t TempAverage;
	
	uint16_t TempData;
	int32_t delta;
	
	//Get the new data set
	//GetDataSet(TempDataSet);
	
	//printf_P(PSTR("datasetnumber=%u\n"), DataSetNumber);
	
	//for testing...
	for(i=0;i<18;i++)
	{
		DataToSave[i] = i;
		TempDataSet[i] = (42-2*i);
	}
	DataSetNumber = 2;
	
	
	
	
	printf_P(PSTR("OLD%u, "), DataSetNumber);
	for(i=0;i<18;i++)
	{
		printf_P(PSTR("0x%02X, "), DataToSave[i]);
	}
	printf_P(PSTR("\b\b \n"));
	
	printf_P(PSTR("NEW%u, "), DataSetNumber);
	for(i=0;i<18;i++)
	{
		printf_P(PSTR("0x%02X, "), TempDataSet[i]);
	}
	printf_P(PSTR("\b\b \n"));
	
	
	
	
	for (i=4; i<18; i+=2)
	{
		//printf_P(PSTR("i=%u\n"), i);
		//printf_P(PSTR("OLD0: 0x%02X\nOLD1: 0x%02X\n"), DataToSave[i], DataToSave[i+1]);
		//printf_P(PSTR("NEW0: 0x%02X\nNEW1: 0x%02X\n"), TempDataSet[i], TempDataSet[i+1]);
		TempData = ((DataToSave[i] << 8) | (DataToSave[i+1]));
		
		delta = (int32_t)((TempDataSet[i] << 8) | (TempDataSet[i+1]));
		delta = delta - (int32_t)(TempData);
		
		//rounding (half away from zero)
		if(delta >= 0)
		{
			delta = (((delta << 1l)/(DataSetNumber+1l)) + 1l) >> 1l;
			//delta = ((delta*10)+10)/(10*(DataSetNumber+1));
		}
		else
		{
			delta = (((delta << 1l)/(DataSetNumber+1l)) - 1l) >> 1l;
			//delta = ((delta*10)-10)/(10*(DataSetNumber+1));
		}
		
		TempData = (uint16_t)((int32_t)TempData + delta);
		
		DataToSave[i] = (uint8_t)((TempData & 0xFF00) >> 8);
		DataToSave[i+1] = (uint8_t)(TempData & 0xFF);
		
		//printf_P(PSTR("AV0: 0x%02X\nAV1: 0x%02X\n"), DataToSave[i], DataToSave[i+1]);
	}
	
	/*printf_P(PSTR("CUR%u: "), DataSetNumber);
	for(i=0;i<18;i++)
	{
		printf_P(PSTR("0x%02X, "), TempDataSet[i]);
	}
	printf_P(PSTR("\n"));*/
	
	printf_P(PSTR("AV%u, "), DataSetNumber);
	for(i=0;i<18;i++)
	{
		printf_P(PSTR("0x%02X, "), DataToSave[i]);
	}
	printf_P(PSTR("\b\b \n"));
	
	DataSetNumber++;
	return;
}


/** @} */