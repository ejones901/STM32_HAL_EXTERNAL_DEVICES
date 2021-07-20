#ifndef HEADER_STM32_HAL_KEYPAD_H_
#define HEADER_STM32_HAL_KEYPAD_H_

#include "stm32f1xx_hal.h"

//Configurables macros
#define DEBOUNCE_TIME 20 //20 ms
#define KEYPAD_ROWS 4
#define KEYPAD_COLUMNS 4
#define NULL_CHAR 0 //This sets the NULL character, used to discern between a key pressed and nothing pressed.

//Configurable variables (Located in stm32_hal_keypad.c)
//KEYPAD MAPPING
extern const char keypadMap[KEYPAD_ROWS * KEYPAD_COLUMNS];

//ROW PINS MAPPING
extern uint16_t rowPins[KEYPAD_ROWS];

//COLUMN PINS MAPPING
extern uint16_t columnPins[KEYPAD_COLUMNS];

#define ROW_PORT GPIOB
#define COLUMN_PORT GPIOB

//INITIALISES I/O FOR KEYPAD
_Bool keypad_Init();

//Reads the keypad and returns the key pressed.
char keypad_ReadKey();

#endif /* HEADER_STM32_HAL_KEYPAD_H_ */
