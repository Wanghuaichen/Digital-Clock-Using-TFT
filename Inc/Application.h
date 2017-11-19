/*
 * Application.h
 *
 *  Created on: Oct 27, 2017
 *  Finished on: Nov 15, 2017
 *      Author: Xuan Thiep
 */

#ifndef APPLICATION_H_
#define APPLICATION_H_

#include "stm32f1xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"
#include "w25qxx_flash.h"

typedef enum
{
	EClockMode_Normal 					= 0,
	EClockMode_Settime 					= 1,
	EClockMode_Slide					= 2
}EClockMode;

typedef enum
{
	EClockSystemState_Start 			= 0,
	EClockSystemState_Running 			= 1,
}EClockSystemState;

typedef struct
{
	/* Speaker */
	GPIO_TypeDef* 						Speaker_Port;
	uint16_t 	 						Speaker_Pin;

	/* Button Mode */
	GPIO_TypeDef* 						BT_Mode_Port;
	uint16_t 	  						BT_Mode_Pin;

	/* Button UP */
	GPIO_TypeDef* 						BT_Up_Port;
	uint16_t 	  						BT_Up_Pin;

	/* Button Down */
	GPIO_TypeDef* 						BT_Down_Port;
	uint16_t 	  						BT_Down_Pin;
}EclockHardware;


typedef struct
{
	EclockHardware						eClockHardware;
	EClockMode 							eClockMode;
	EClockSystemState 					eClockSystemState;
	W25QXX_Handle_TypeDef* 				eflash;
	uint32_t							eperiod_read_temhumi;
	uint16_t							eperiod_read_time;
	uint8_t 							etime_value[7];
	float 								etemperature;
	uint8_t				 				ehumidity;
}EClock_HandleTypeDef;


typedef enum
{
	TFT_FONT_14x24 = 0	,
	TFT_FONT_26x48		,
	TFT_FONT_TEMPLATE	,
	TFT_FONT_CLEAR		,
	TFT_FONT_DRAW_RECTANGLE,
	TFT_FONT_IMAGE		,
	TFT_FONT_SPECIAL
}TFTFontSize;


typedef struct
{
	TFTFontSize		tftFontSize;
	uint8_t*		data;
	uint16_t 		color;
	uint16_t		rowIndex1;
	uint16_t		columnIndex1;
	uint16_t		rowIndex2;
	uint16_t		columnIndex2;
	uint8_t			tftStringMode;
}TFTGateKeeperHandleTypedef;

/*---------------------- Begin Define for hardware connection ------------------------ */
#define		SPEAKER_PORT				GPIOB
#define		SPEAKER_PIN					GPIO_PIN_9


#define		BT_MODE_PORT				GPIOC
#define		BT_MODE_PIN					GPIO_PIN_13


#define		BT_UP_PORT					GPIOC
#define		BT_UP_PIN					GPIO_PIN_14


#define		BT_DOWN_PORT				GPIOC
#define		BT_DOWN_PIN					GPIO_PIN_15

/*---------------------- End Define for hardware connection ------------------------- */

/* DS3231 Real Time Clock 7bits I2C Address */
#define 	DS3231_ADD					0x68

/* Define Signal Flag to synchronous between tasks */
#define 	BUTTON_MODE_SIGNAL			0x01
#define 	BUTTON_UP_SIGNAL			0x02
#define 	BUTTON_DOWN_SIGNAL			0x04

/*----------------- Begin Define Display Time local address (x,y) on TFT LCD ----------*/
#define		MINUS_LOCAL_ROW				272
#define 	MINUS_LOCAL_COLUMN			188

#define 	HOUR_LOCAL_ROW				272
#define 	HOUR_LOCAL_COLUMN			111

#define 	DOT_LOCAL_ROW				272
#define 	DOT_LOCAL_COLUMN			162

#define		MONTH_LOCAL_ROW				296
#define		MONTH_LOCAL_COLUMN			82

#define		DATE_LOCAL_ROW				296
#define		DATE_LOCAL_COLUMN			41

#define		SLASH_LOCAL_ROW				296
#define		SLASH_LOCAL_COLUMN			68
/*------------------ End Define Display Time local address (x,y) on TFT LCD -----------*/

/*----------------- Begin Define Display Time local address (x,y) on TFT LCD When System In Mode Settime ----------*/
#define 	SECOND_LOCAL_ROW_ST			54
#define 	SECOND_LOCAL_COLUMN_ST		172

#define		MINUS_LOCAL_ROW_ST			54
#define 	MINUS_LOCAL_COLUMN_ST		94

#define 	HOUR_LOCAL_ROW_ST			54
#define 	HOUR_LOCAL_COLUMN_ST		16

#define 	DAY_LOCAL_ROW_ST			156
#define 	DAY_LOCAL_COLUMN_ST			96

#define 	YEAR_LOCAL_ROW_ST			204
#define 	YEAR_LOCAL_COLUMN_ST		172

#define		MONTH_LOCAL_ROW_ST			204
#define 	MONTH_LOCAL_COLUMN_ST		94

#define 	DATE_LOCAL_ROW_ST			204
#define 	DATE_LOCAL_COLUMN_ST		16


/*------------------ End Define Display Time local address (x,y) on TFT LCD When System In Mode Settime -----------*/
/* Define reading time period from DS3231 (in miliseconds) */
#define		PERIOD_READ_TIME			250

/* Define reading temperature and humidity period from hdc1080 (in miliseconds) */
#define 	PERIOD_READ_TEMP_HUMI		10000

/* Define changing image period (in minutes) */
#define		PERIOD_CHANGING_IMAGE		10

/* Define Time Display Color */
#define 	TIME_DISPLAY_COLOR 			COLOR_RED

/* Define background color */
#define		BACKGROUND_COLOR			COLOR_WHITE

/* Define Focus corlor in settime Mode */
#define		FOCUS_COLOR					COLOR_BLUE

/*-------------------------- Begin Others Useful Macro Define -------------------------*/

/* Convert a number from BCD value to Decimal value */
#define		Convert_Bcd2Dec(convert_number)		((convert_number >> 4) * 10 + (convert_number & 0x0f))

/* Convert a number from Decimal value to BCD value */
#define		Convert_Dec2Bcd(convert_number)		(((convert_number / 10) << 4) | (convert_number % 10))

/*--------------------------- End Others Useful Macro Define ---------------------------*/


#endif /* APPLICATION_H_ */
