// These are functions from the datasheet read it for more information
// These functions were created using I2C fast mode (400 kHz) with an AT24C32 and STM32F103C8T6 (Blue pill).
#ifndef HEADER_AT24CXX_H_
#define HEADER_AT24CXX_H_
#include "stm32f1xx_hal.h" //General purpose library used. library comes with STM32CubeIDE.

#define AT24C_I2C_ADDRESS 0x57 //7-bit I2C address, 0x57 = 0101 0111 | Address is found on datasheet and is given by 1010 A2 A1 A0
#define AT24C_I2C_TIMEOUT 10

void AT24C_Init(I2C_HandleTypeDef *Handle);
//Passes the I2C Handle from main to the handle used in the soruce file.

_Bool AT24C_Poll();
//Polls the AT24C to see if it is ready to communicate.

_Bool AT24C_ByteWrite(uint16_t WordAddress, uint8_t Data);
//Writes a single byte (Data) to Address (WordAddress).

_Bool AT24C_PageWrite(uint16_t WordAddress, uint8_t WriteLength, uint8_t *Data);
//Page write. (WordAddress) is the beginning address while (WrightLength) is how many bytes to write and (*Data) is the pointer to
//the data array in main. Pages work in 32-bytes so read datasheet for more info.

uint8_t AT24C_CurrentAddressRead();
//Reads the current address. Current address = Last address accessed + 1.

uint8_t AT24C_ByteRead(uint16_t WordAddress); //single-byte read.
//Reads a byte from address (WordAddress).

_Bool AT24C_SequentialRead(uint16_t WordAddress, uint8_t ReadLength, uint8_t *Data); //multi-byte read.
//Sequential read. (WordAddress) is the beginning address while (ReadLength) is how many bytes to read and (*Data) is the pointer to
//the data array in main. Shouldn't read any more than 128 bytes at once as after 128 bytes the data read becomes unreliable/corrupt.

#endif /* HEADER_AT24CXX_H_ */
