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
*	\brief		AT45DB321D dataflash driver.
*	\author		Pat Satyshur
*	\version	1.0
*	\date		2/17/2013
*	\copyright	Copyright 2013, Pat Satyshur
*	\ingroup 	hardware
*
*	@{
*/

#include "main.h"

/** Returns 0 if the device initalizes correctly, 0xFF if not */
uint8_t AT45DB321D_Init(void)
{
	uint8_t DataByteRead[3];
	
	AT45DB321D_Deselect();
	
	//Check the device ID.
	AT45DB321D_ReadID(DataByteRead);
	
	//The first two bytes must match for the device to be recognized
	if((DataByteRead[0] == 0x1F) && (DataByteRead[1] == 0x27))
	{
		return 0x00;
	}
	
	return 0xFF;
}

void AT45DB321D_Select(void)
{
	AT45DB321D_CS_PORT &= ~(1<<AT45DB321D_CS_PIN);
	return;
}

void AT45DB321D_Deselect(void)
{
	AT45DB321D_CS_PORT |= (1<<AT45DB321D_CS_PIN);
	return;
}

uint8_t AT45DB321D_ReadStatus(void)
{
	uint8_t StatusByte;
	
	AT45DB321D_Select();
	AT45DB321D_SPISEND(AT45DB321D_CMD_READ_STATUS);
	StatusByte = AT45DB321D_SPISEND(0x00);
	AT45DB321D_Deselect();

	return StatusByte;	
}




//
void AT45DB321D_BufferRead(uint8_t Buffer, uint16_t BufferStartAddress, uint8_t DataReadBuffer[], uint16_t BytesToRead)
{
	uint16_t i;
	//uint8_t DataByte;

	//No funny stuff...
	//TODO: add check for length and start address
	if( (Buffer != 1) && (Buffer != 2) )
	{
		return;
	}
	
	AT45DB321D_Select();
	if(Buffer == 1)
	{
		AT45DB321D_SPISEND(AT45DB321D_CMD_BUFFER1_READ_HS);
	}
	else
	{
		AT45DB321D_SPISEND(AT45DB321D_CMD_BUFFER2_READ_HS);
	}
	
	//Send address to read
	//The address is 3 bytes, but only the 10 LSBs matter
	AT45DB321D_SPISEND(0x00);
	AT45DB321D_SPISEND((BufferStartAddress & 0x0300)>>8);
	AT45DB321D_SPISEND(BufferStartAddress & 0xFF);
	
	//An extra byte needs to be clocked in to initalize the read
	AT45DB321D_SPISEND(0x00);
	
	for(i = 0; i<BytesToRead; i++)
	{
		*DataReadBuffer = AT45DB321D_SPISEND(0x00);
		//printf_P(PSTR("[%u]: 0x%02X\n"), i, *DataReadBuffer);
		DataReadBuffer++;
	}
	AT45DB321D_Deselect();

	return;
}

void AT45DB321D_BufferWrite(uint8_t Buffer, uint16_t BufferStartAddress, uint8_t DataWriteBuffer[], uint16_t BytesToWrite)
{
	uint16_t i;

	//No funny stuff...
	//TODO: add check for length and start address
	if( (Buffer != 1) && (Buffer != 2) )
	{
		return;
	}
	
	AT45DB321D_Select();
	if(Buffer == 1)
	{
		AT45DB321D_SPISEND(AT45DB321D_CMD_BUFFER1_WRITE);
	}
	else
	{
		AT45DB321D_SPISEND(AT45DB321D_CMD_BUFFER2_WRITE);
	}

	//Send address to read
	//The address is 3 bytes, but only the 10 LSBs matter (9 LSBs for 512 mode)
	AT45DB321D_SPISEND(0x00);
	AT45DB321D_SPISEND((BufferStartAddress & 0x0300)>>8);
	AT45DB321D_SPISEND(BufferStartAddress & 0xFF);
	
	for(i = 0; i<BytesToWrite; i++)
	{
		//printf_P(PSTR("write: 0x%02X\n"), *DataWriteBuffer);
		AT45DB321D_SPISEND(*DataWriteBuffer);
		DataWriteBuffer++;
	}
	AT45DB321D_Deselect();

	return;
}

void AT45DB321D_CopyPageToBuffer(uint8_t Buffer, uint16_t PageAddress)
{
	//No funny stuff...
	//TODO: add check for PageAddress
	if( (Buffer != 1) && (Buffer != 2) )
	{
		return;
	}

	AT45DB321D_Select();
	if(Buffer == 1)
	{
		AT45DB321D_SPISEND(AT45DB321D_CMD_TRANSFER_PAGE_TO_BUFFER1);
	}
	else
	{
		AT45DB321D_SPISEND(AT45DB321D_CMD_TRANSFER_PAGE_TO_BUFFER2);
	}
	
	//Send page address, this is different for 512 and 528 mode
	AT45DB321D_SendPageAddress(PageAddress);
	
	AT45DB321D_Deselect();
	
	//Add code to check for complete?
	
	return;
}

void AT45DB321D_CopyBufferToPage(uint8_t Buffer, uint16_t PageAddress)
{
	//No funny stuff...
	//TODO: add check for PageAddress
	if( (Buffer != 1) && (Buffer != 2) )
	{
		return;
	}

	AT45DB321D_Select();
	if(Buffer == 1)
	{
		AT45DB321D_SPISEND(AT45DB321D_CMD_BUFFER1_TO_PAGE_ERASE);
	}
	else
	{
		AT45DB321D_SPISEND(AT45DB321D_CMD_BUFFER2_TO_PAGE_ERASE);
	}

	//Send page address, this is different for 512 and 528 mode
	AT45DB321D_SendPageAddress(PageAddress);
	
	AT45DB321D_Deselect();
	
	//Add code to check for complete?
	
	return;
}

void AT45DB321D_ErasePage(uint16_t PageAddress)
{
	AT45DB321D_Select();
	AT45DB321D_SPISEND(AT45DB321D_CMD_PAGE_ERASE);
	AT45DB321D_SendPageAddress(PageAddress);
	AT45DB321D_Deselect();
	return;
}

uint8_t AT45DB321D_WaitForReady(void)
 {
	uint8_t StatusByte = 0x00;
	while((StatusByte & AT45DB321D_STATUS_READY_MASK) != AT45DB321D_STATUS_READY_MASK)
	{
		StatusByte = AT45DB321D_ReadStatus();
	}
	return StatusByte;
 }

void AT45DB321D_SendPageAddress(uint16_t PageAddress)
{
	//Send page address, this is different for 512 and 528 mode
	#if AT45DB321D_PAGE_SIZE_BYTES == 512
	AT45DB321D_SPISEND( (uint8_t)(PageAddress>>7) );
	AT45DB321D_SPISEND( (uint8_t)(PageAddress<<1) );
	AT45DB321D_SPISEND(0x00);
	#else
	AT45DB321D_SPISEND( (uint8_t)(PageAddress>>6) );
	AT45DB321D_SPISEND( (uint8_t)(PageAddress<<2) );
	AT45DB321D_SPISEND(0x00);
	#endif
	return;
}

//Untested
void AT45DB321D_Powerdown(void)
{
	AT45DB321D_Select();
	AT45DB321D_SPISEND(AT45DB321D_CMD_POWERDOWN);
	AT45DB321D_Deselect();
	return;
}

void AT45DB321D_Powerup(void)
{
	AT45DB321D_Select();
	AT45DB321D_SPISEND(AT45DB321D_CMD_POWERUP);
	AT45DB321D_Deselect();
	return;
}

void AT45DB321D_ChipErase(void)
{
	AT45DB321D_Select();
	AT45DB321D_SPISEND(AT45DB321D_CMD_CHIP_ERASE1);
	AT45DB321D_SPISEND(AT45DB321D_CMD_CHIP_ERASE2);
	AT45DB321D_SPISEND(AT45DB321D_CMD_CHIP_ERASE3);
	AT45DB321D_SPISEND(AT45DB321D_CMD_CHIP_ERASE4);
	AT45DB321D_Deselect();
	return;
}

void AT45DB321D_Protect(void)
{
	AT45DB321D_Select();
	AT45DB321D_SPISEND(0x3D);
	AT45DB321D_SPISEND(0x2A);
	AT45DB321D_SPISEND(0x7F);
	AT45DB321D_SPISEND(0xA9);
	AT45DB321D_Deselect();
	return;
}

void AT45DB321D_Unprotect(void)
{
	AT45DB321D_Select();
	AT45DB321D_SPISEND(0x3D);
	AT45DB321D_SPISEND(0x2A);
	AT45DB321D_SPISEND(0x7F);
	AT45DB321D_SPISEND(0x9A);
	AT45DB321D_Deselect();
	return;
}

void AT45DB321D_ReadID(uint8_t *DataByteRead)
{
	AT45DB321D_Select();
	AT45DB321D_SPISEND(AT45DB321D_CMD_READ_DEVICE_ID);
	DataByteRead[0] = AT45DB321D_SPISEND(0x00);
	DataByteRead[1] = AT45DB321D_SPISEND(0x00);
	DataByteRead[2] = AT45DB321D_SPISEND(0x00);
	AT45DB321D_Deselect();
	return;
}






//This command should only need to be sent once. It can not be undone.
//The device needs to be power cycled after this command is sent.
//The change can be verified by reading the status register.
//Make sure to set the 'AT45DB321D_PAGE_SIZE_BYTES' variable in at45db321d.h correctly.
#if AT45DB321D_PAGE_SIZE_BYTES != 512
void AT45DB321D_SwitchTo512(void)
{
	AT45DB321D_Select();
	AT45DB321D_SPISEND(0x3D);
	AT45DB321D_SPISEND(0x2A);
	AT45DB321D_SPISEND(0x80);
	AT45DB321D_SPISEND(0xA6);
	AT45DB321D_Deselect();
	AT45DB321D_WaitForReady();
	printf_P(PSTR("Device page size set to 512. Please power cycle the device\n"));
	return;
}
#endif

/** @} */