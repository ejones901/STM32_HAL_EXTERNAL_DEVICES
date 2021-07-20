#include "stm32_hal_keypad.h"

//Configurable variables

//KEYPAD MAPPING
const char keypadMap[KEYPAD_ROWS * KEYPAD_COLUMNS] = {'1', '2', '3', 'A',
													'4', '5', '6', 'B',
													'7', '8', '9', 'C',
													'*', '0', '#', 'D'};

//ROW PINS MAPPING
uint16_t rowPins[KEYPAD_ROWS] = {GPIO_PIN_5, GPIO_PIN_9, GPIO_PIN_10, GPIO_PIN_11};

//COLUMN PINS MAPPING
uint16_t columnPins[KEYPAD_COLUMNS] = {GPIO_PIN_12, GPIO_PIN_13, GPIO_PIN_14, GPIO_PIN_15};

static void keypad_Debounce()
{
	HAL_Delay(DEBOUNCE_TIME); //wait 20 ms
}

static void Pin_Write(uint16_t Pin, _Bool Status) //function that writes pin.
{
	HAL_GPIO_WritePin(ROW_PORT, Pin, Status);
}

static _Bool Pin_Read(uint16_t Pin) //function that reads pin.
{
	return HAL_GPIO_ReadPin(COLUMN_PORT, Pin);
}

static _Bool keypad_SetRow(uint8_t row) //this function selects row (row)
{
	//Reset pins to 0.
	for(uint8_t i = 0; i < KEYPAD_ROWS; i++)
	{
		Pin_Write(rowPins[i], 0);
	}

	//Enable pin for row (row).
	Pin_Write(rowPins[row], 1);

	return 1;
}

static void Pins_Init() //this function initalises the pins
{
	__HAL_RCC_GPIOB_CLK_ENABLE(); //Enable port clock must be correct port.

	GPIO_InitTypeDef GPIOinit = { 0 };

	//Initialise outputs (rows)
	for(uint8_t i = 0; i < KEYPAD_ROWS; i++)
	{
		GPIOinit.Pin |= rowPins[i];
	}

	GPIOinit.Mode = GPIO_MODE_OUTPUT_PP;
	GPIOinit.Pull = GPIO_NOPULL;
	GPIOinit.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(ROW_PORT, &GPIOinit);

	//initialise inputs (columns)
	GPIOinit.Pin = 0;

	for(uint8_t i = 0; i < KEYPAD_ROWS; i++)
	{
		GPIOinit.Pin |= columnPins[i];
	}

	GPIOinit.Mode = GPIO_MODE_INPUT;
	GPIOinit.Pull = GPIO_PULLDOWN; //pull down required
	GPIOinit.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(COLUMN_PORT, &GPIOinit);
}

_Bool keypad_Init()
{
	Pins_Init();

	return 1;
}

char keypad_ReadKey()
{
	keypad_Debounce(); //key release debounce

	for(uint8_t i = 0; i < KEYPAD_ROWS; i++)
	{
		keypad_SetRow(i); //set current row to row i

		for(uint8_t j = 0; j < KEYPAD_COLUMNS; j++) //read columns of row i
		{
			if(Pin_Read(columnPins[j])) return keypadMap[4*i + j];
		}
	}

	keypad_Debounce(); //key press debounce

	return NULL_CHAR;
}
