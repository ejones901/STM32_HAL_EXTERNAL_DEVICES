#include "ds3231_Driver.h"

static I2C_HandleTypeDef *DS3231_I2C_Handle; //i2c handle

void DS3231_Init(I2C_HandleTypeDef *Handle)
{
	DS3231_I2C_Handle = Handle;
}

static uint8_t BCD_to_Decimal(uint8_t data) //converts from bcd to decimal
{
	return (10 * ((data >> 4) & 0x0F) + (data & 0x0F));
}

static uint8_t Decimal_to_BCD(uint8_t data) //converts from decimal to bcd
{
	return ((data/10 << 4) + (data % 10));
}

static _Bool DS3231_I2C_Write(uint8_t *DataBuffer, uint8_t DataLength)
{
	if(HAL_I2C_Master_Transmit(DS3231_I2C_Handle, (DS3231_I2C_ADDRESS << 1), DataBuffer, DataLength, DS3231_I2C_TIMEOUT) == HAL_OK) return 1;
	return 0;
}

static _Bool DS3231_I2C_Read(uint8_t *DataBuffer, uint8_t DataLength)
{
	if(HAL_I2C_Master_Receive(DS3231_I2C_Handle, (DS3231_I2C_ADDRESS << 1) | 0x01, DataBuffer, DataLength, DS3231_I2C_TIMEOUT) == HAL_OK) return 1;
	return 0;
}

_Bool DS3231_ReadTime(RTC_time *RTC_time_ptr)
{
	uint8_t RTC_timeRegisterAddress = DS3231_SECONDS_REGISTER_ADDRESS;
	uint8_t TimeData[7];

	if(!DS3231_I2C_Write(&RTC_timeRegisterAddress, 1)) return 0; //Write pointer to select the register that wants to be read.

	if(!DS3231_I2C_Read(TimeData, 7)) return 0; //Read all the time registers the hardware address pointer will increment with each read.

	//Bit mask has been applied.
	RTC_time_ptr->RTC_seconds = BCD_to_Decimal((TimeData[0] & 0x7F));
	RTC_time_ptr->RTC_minutes = BCD_to_Decimal((TimeData[1] & 0x7F));
	RTC_time_ptr->RTC_hours = BCD_to_Decimal((TimeData[2] & 0x3F));
	RTC_time_ptr->RTC_day = BCD_to_Decimal((TimeData[3] & 0x07));
	RTC_time_ptr->RTC_date = BCD_to_Decimal((TimeData[4] & 0x3F));
	RTC_time_ptr->RTC_month = BCD_to_Decimal((TimeData[5] & 0x1F));
	RTC_time_ptr->RTC_year = BCD_to_Decimal((TimeData[6]));

	return 1;
}

_Bool DS3231_SetTime(RTC_time *RTC_time_ptr)
{
	uint8_t TimeData[8] = {DS3231_SECONDS_REGISTER_ADDRESS,
			Decimal_to_BCD(RTC_time_ptr->RTC_seconds),
			Decimal_to_BCD(RTC_time_ptr->RTC_minutes),
			Decimal_to_BCD(RTC_time_ptr->RTC_hours),
			Decimal_to_BCD(RTC_time_ptr->RTC_day),
			Decimal_to_BCD(RTC_time_ptr->RTC_date),
			Decimal_to_BCD(RTC_time_ptr->RTC_month),
			Decimal_to_BCD(RTC_time_ptr->RTC_year)
	};

	if(!DS3231_I2C_Write(TimeData, 8)) return 0; //Write pointer to select the register that wants to be read.

	return 1;
}

_Bool DS3231_ReadTemp(float *temperature)
{
	uint8_t TemperatureAddress = DS3231_TEMP_MSB_REGISTER_ADDRESS;
	uint8_t TemperatureBuffer[2];

	if(!DS3231_I2C_Write(&TemperatureAddress, 1)) return 0; //Write pointer to select the register that wants to be read.

	if(!DS3231_I2C_Read(TemperatureBuffer, 2)) return 0; //Read temperature value

	if((TemperatureBuffer[0] >> 7) == 0) //if value is positive (checks sign bit)
	{
		*temperature = (float)TemperatureBuffer[0] + (float)((TemperatureBuffer[1] >> 6)/4.0);
	}
	else
	{
		*temperature = (-1)*((float)(~(TemperatureBuffer[0])) + (float)(~(TemperatureBuffer[1] >> 6)/4.0)) - 0.25;
	}

	return 1;
}

_Bool DS3231_ConfigureAlarm(RTC_alarmMode alarmMode, RTC_AlarmConfig *AlarmConfig)
{
	if(AlarmConfig->Alarm == ALARM1)
	{
		uint8_t alarmDate = Decimal_to_BCD(AlarmConfig->Alarm_date);
		uint8_t alarmHour = Decimal_to_BCD(AlarmConfig->Alarm_hour);
		uint8_t alarmMinute = Decimal_to_BCD(AlarmConfig->Alarm_minute);
		uint8_t alarmSecond = Decimal_to_BCD(AlarmConfig->Alarm_second);

		switch (alarmMode)
		{
			case (ALARM_MODE_ONCE_PER_SECOND):
					alarmDate |= 0x80;
					alarmHour |= 0x80;
					alarmMinute |= 0x80;
					alarmSecond |= 0x80;
					break;
			case (ALARM_MODE_SECOND):
					alarmDate |= 0x80;
					alarmHour |= 0x80;
					alarmMinute |= 0x80;
					break;
			case (ALARM_MODE_MINUTE_SECOND):
					alarmDate |= 0x80;
					alarmHour |= 0x80;
					break;
			case (ALARM_MODE_HOUR_MINUTE_SECOND):
					alarmDate |= 0x80;
					break;
			case (ALARM_MODE_ALL):
					break;
			default:
					return 0;
		}

		uint8_t alarm1Address = DS3231_ALARM1_SECONDS_REGISTER_ADDRESS;
		uint8_t alarm1Data[5] = {alarm1Address, alarmSecond, alarmMinute, alarmHour, alarmDate};

		if(!DS3231_I2C_Write(alarm1Data, 5)) return 0; //beginning of alarm1 register
	}
	else if(AlarmConfig->Alarm == ALARM2)
	{
		uint8_t alarmDate = Decimal_to_BCD(AlarmConfig->Alarm_date);
		uint8_t alarmHour = Decimal_to_BCD(AlarmConfig->Alarm_hour);
		uint8_t alarmMinute = Decimal_to_BCD(AlarmConfig->Alarm_minute);

		switch (alarmMode)
		{
			case (ALARM_MODE_ONCE_PER_MINUTE):
					alarmDate |= 0x80;
					alarmHour |= 0x80;
					alarmMinute |= 0x80;
					break;
			case (ALARM_MODE_MINUTE):
					alarmDate |= 0x80;
					alarmHour |= 0x80;
					break;
			case (ALARM_MODE_HOUR_MINUTE):
					alarmDate |= 0x80;
					break;
			case (ALARM_MODE_ALL):
					break;
			default:
					return 0;
		}

		uint8_t alarm2Address = DS3231_ALARM2_MINUTES_REGISTER_ADDRESS;
		uint8_t alarm2Data[5] = {alarm2Address, alarmMinute, alarmHour, alarmDate};

		if(!DS3231_I2C_Write(alarm2Data, 4)) return 0; //beginning of alarm2 register
	}
	else return 0;

	return 1;
}

_Bool DS3231_ClearAlarmFlag(RTC_alarm Alarm)
{
	if(Alarm == ALARM1)
	{
		uint8_t StatusAddress = DS3231_STATUS_REGISTER_ADDRESS;
		uint8_t StatusConfig = 0;

		if(!DS3231_I2C_Write(&StatusAddress, 1)) return 0; //Set register pointer to status register
		if(!DS3231_I2C_Read(&StatusConfig, 1)) return 0; //Read status register

		uint8_t AlarmFlagClear[2] = {StatusAddress, (StatusConfig & ~(0x01))}; //clear ALARM1 enable bit.

		if(!DS3231_I2C_Write(AlarmFlagClear, 2)) return 0;
	}
	else if(Alarm == ALARM2)
	{
		uint8_t StatusAddress = DS3231_STATUS_REGISTER_ADDRESS;
		uint8_t StatusConfig = 0;

		if(!DS3231_I2C_Write(&StatusAddress, 1)) return 0; //Set register pointer to status register
		if(!DS3231_I2C_Read(&StatusConfig, 1)) return 0; //Read status register

		uint8_t AlarmFlagClear[2] = {StatusAddress, (StatusConfig & ~(0x02))}; //clear ALARM2 enable bit.

		if(!DS3231_I2C_Write(AlarmFlagClear, 2)) return 0;
	}
	else return 0;

	return 1;
}

_Bool DS3231_EnableAlarm(RTC_alarm Alarm)
{
	if(Alarm == ALARM1)
	{
		uint8_t ControlAddress = DS3231_CONTROL_REGISTER_ADDRESS;
		uint8_t ControlConfig = 0;

		if(!DS3231_I2C_Write(&ControlAddress, 1)) return 0;
		if(!DS3231_I2C_Read(&ControlConfig, 1)) return 0;

		uint8_t AlarmEnable[2] = {ControlAddress, (ControlConfig | 0x01)};

		if(!DS3231_I2C_Write(AlarmEnable, 2)) return 0; //beginning of alarm1 register
	}
	else if(Alarm == ALARM2)
	{
		uint8_t ControlAddress = DS3231_CONTROL_REGISTER_ADDRESS;
		uint8_t ControlConfig = 0;

		if(!DS3231_I2C_Write(&ControlAddress, 1)) return 0;
		if(!DS3231_I2C_Read(&ControlConfig, 1)) return 0;

		uint8_t AlarmEnable[2] = {ControlAddress, (ControlConfig | 0x02)};

		if(!DS3231_I2C_Write(AlarmEnable, 2)) return 0; //beginning of alarm2 register
	}
	else return 0;

	return 1;
}

_Bool DS3231_DisableAlarm(RTC_alarm Alarm)
{
	if(Alarm == ALARM1)
	{
		uint8_t ControlAddress = DS3231_CONTROL_REGISTER_ADDRESS;
		uint8_t ControlConfig = 0;

		if(!DS3231_I2C_Write(&ControlAddress, 1)) return 0;
		if(!DS3231_I2C_Read(&ControlConfig, 1)) return 0;

		uint8_t AlarmEnable[2] = {ControlAddress, (ControlConfig & ~(0x01))};

		if(!DS3231_I2C_Write(AlarmEnable, 2)) return 0; //beginning of alarm1 register
	}
	else if(Alarm == ALARM2)
	{
		uint8_t ControlAddress = DS3231_CONTROL_REGISTER_ADDRESS;
		uint8_t ControlConfig = 0;

		if(!DS3231_I2C_Write(&ControlAddress, 1)) return 0;
		if(!DS3231_I2C_Read(&ControlConfig, 1)) return 0;

		uint8_t AlarmEnable[2] = {ControlAddress, (ControlConfig & ~(0x02))};

		if(!DS3231_I2C_Write(AlarmEnable, 2)) return 0; //beginning of alarm2 register
	}
	else return 0;

	return 1;
}
