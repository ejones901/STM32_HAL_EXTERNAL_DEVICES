#ifndef HEADER_DS3231_DRIVER_H_
#define HEADER_DS3231_DRIVER_H_

#include "stm32f1xx_hal.h"

//DS3231 REGISTER ADDRESSES
#define DS3231_SECONDS_REGISTER_ADDRESS 0x00
#define DS3231_MINUTES_REGISTER_ADDRESS 0x01
#define DS3231_HOURS_REGISTER_ADDRESS 0x02
#define DS3231_DAY_REGISTER_ADDRESS 0x03
#define DS3231_DATE_REGISTER_ADDRESS 0x04
#define DS3231_MONTH_REGISTER_ADDRESS 0x05
#define DS3231_YEAR_REGISTER_ADDRESS 0x06
#define DS3231_ALARM1_SECONDS_REGISTER_ADDRESS 0x07
#define DS3231_ALARM1_MINUTES_REGISTER_ADDRESS 0x08
#define DS3231_ALARM1_HOURS_REGISTER_ADDRESS 0x09
#define DS3231_ALARM1_DAY_REGISTER_ADDRESS 0x0A
#define DS3231_ALARM2_MINUTES_REGISTER_ADDRESS 0x0B
#define DS3231_ALARM2_HOURS_REGISTER_ADDRESS 0x0C
#define DS3231_ALARM2_DAY_REGISTER_ADDRESS 0x0D
#define DS3231_CONTROL_REGISTER_ADDRESS 0x0E
#define DS3231_STATUS_REGISTER_ADDRESS 0x0F
#define DS3231_AGING_OFFSET_REGISTER_ADDRESS 0x10
#define DS3231_TEMP_MSB_REGISTER_ADDRESS 0x11
#define DS3231_TEMP_LSB_REGISTER_ADDRESS 0x12

#define DS3231_I2C_ADDRESS 0x68 //7-bit i2c address
#define DS3231_I2C_TIMEOUT 10 //i2c timeout

typedef enum
{
	ALARM1,
	ALARM2
}RTC_alarm;

typedef enum
{
	ALARM_MODE_ONCE_PER_SECOND, //Only for alarm 1.
	ALARM_MODE_SECOND, //Only for alarm 1.
	ALARM_MODE_MINUTE_SECOND, //Only for alarm 1.
	ALARM_MODE_HOUR_MINUTE_SECOND, //Only for alarm 1.

	ALARM_MODE_ALL, //For Alarm 1 and 2.

	ALARM_MODE_ONCE_PER_MINUTE, //Only for alarm 2.
	ALARM_MODE_MINUTE, //Only for alarm 2.
	ALARM_MODE_HOUR_MINUTE //Only for alarm 2.
}RTC_alarmMode;

typedef struct
{
	uint8_t RTC_seconds;
	uint8_t RTC_minutes;
	uint8_t RTC_hours;
	uint8_t RTC_day;
	uint8_t RTC_date;
	uint8_t RTC_month;
	uint8_t RTC_year;
}RTC_time;

//Alarm configuration structure.
typedef struct
{
	RTC_alarm Alarm;
	uint8_t Alarm_second;
	uint8_t Alarm_minute;
	uint8_t Alarm_hour;
	uint8_t Alarm_date;
}RTC_AlarmConfig;

//This function passes the i2c handle from main
void DS3231_Init(I2C_HandleTypeDef *Handle);

//This function reads the time.
_Bool DS3231_ReadTime(RTC_time *RTC_time_ptr);

//This function sets the time.
_Bool DS3231_SetTime(RTC_time *RTC_time_ptr);

//This functions reads the internal temperature sensor.
_Bool DS3231_ReadTemp(float *temperature);

//This function configures the alarm.
_Bool DS3231_ConfigureAlarm(RTC_alarmMode alarmMode, RTC_AlarmConfig *AlarmConfig);

//This function clears the alarm flag. SQW is high by default and when an alarm is triggered it will be pulled low
//until the alarm's flag has been cleared. Parameter (Alarm) is which alarm wants to be cleared.
_Bool DS3231_ClearAlarmFlag(RTC_alarm Alarm);

//This function enables the alarm.
_Bool DS3231_EnableAlarm(RTC_alarm Alarm);

//This function disables the alarm.
_Bool DS3231_DisableAlarm(RTC_alarm Alarm);

#endif /* HEADER_DS3231_DRIVER_H_ */
