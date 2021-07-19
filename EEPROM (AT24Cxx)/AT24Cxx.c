#include "AT24Cxx.h"

static I2C_HandleTypeDef *AT24C_I2C_Handle; //i2c handle

void AT24C_Init(I2C_HandleTypeDef *Handle)
{
	AT24C_I2C_Handle = Handle;
}

static _Bool AT24C_I2C_Write(uint8_t *DataBuffer, uint8_t DataLength)
{
	if(HAL_I2C_Master_Transmit(AT24C_I2C_Handle, (AT24C_I2C_ADDRESS << 1), DataBuffer, DataLength, AT24C_I2C_TIMEOUT) == HAL_OK) return 1;
	return 0;
}

static _Bool AT24C_I2C_Read(uint8_t *DataBuffer, uint8_t DataLength)
{
	if(HAL_I2C_Master_Receive(AT24C_I2C_Handle, (AT24C_I2C_ADDRESS << 1) | 0x01, DataBuffer, DataLength, AT24C_I2C_TIMEOUT) == HAL_OK) return 1;
	return 0;
}

_Bool AT24C_Poll()
{
	if(HAL_I2C_IsDeviceReady(AT24C_I2C_Handle, (AT24C_I2C_ADDRESS << 1), 1, AT24C_I2C_TIMEOUT) == HAL_OK) return 1;
	return 0;
}

_Bool AT24C_ByteWrite(uint16_t WordAddress, uint8_t Data)
{
	uint8_t DataBuffer[3]; //3 bytes required for single byte write

	DataBuffer[0] = (WordAddress >> 8); //MSByte of memory address
	DataBuffer[1] = WordAddress; //LSByte of memory address
	DataBuffer[2] = Data; //Data byte

	if(!AT24C_I2C_Write(DataBuffer, 3)) return 0;

	while(!AT24C_Poll()); //Wait until EEPROM has finished writing.

	return 1;
}

_Bool AT24C_PageWrite(uint16_t WordAddress, uint8_t WriteLength, uint8_t *Data)
{
	uint8_t DataBuffer[2 + WriteLength]; //2 bytes for memory address + (WriteLength) bytes for how many bytes to write

	DataBuffer[0] = (WordAddress >> 8); //MSByte of memory address
	DataBuffer[1] = WordAddress; //LSByte of memory address

	for(uint8_t i = 0; i < WriteLength; i++) //Fill data buffer with all the data needed to be sent
	{
		DataBuffer[2+i] = Data[i];
	}

	if(!AT24C_I2C_Write(DataBuffer, (2 + WriteLength))) return 0;

	while(!AT24C_Poll()); //Wait until EEPROM has finished writing.

	return 1;
}

uint8_t AT24C_CurrentAddressRead()
{
	uint8_t DataBuffer;

	AT24C_I2C_Read(&DataBuffer, 1);

	return DataBuffer;
}

uint8_t AT24C_ByteRead(uint16_t WordAddress)
{
	uint8_t DataBuffer[2]; //2 bytes for memory address to select

	DataBuffer[0] = (WordAddress >> 8); //MSByte of memory address
	DataBuffer[1] = WordAddress; //LSByte of memory address

	AT24C_I2C_Write(DataBuffer, 2); //Selects address
	AT24C_I2C_Read(DataBuffer, 1); //Reads address

	return DataBuffer[0];
}

_Bool AT24C_SequentialRead(uint16_t WordAddress, uint8_t ReadLength, uint8_t *Data)
{

	uint8_t DataBuffer[2]; //2 bytes for first memory address to select

	DataBuffer[0] = (WordAddress >> 8); //MSByte of memory address
	DataBuffer[1] = WordAddress; //LSByte of memory address

	if(!AT24C_I2C_Write(DataBuffer, 2)) return 0; //Selects address
	if(!AT24C_I2C_Read(Data, ReadLength)) return 0; //reads address and more

	return 1;
}
