/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
 ******************************************************************************
 * This notice applies to any and all portions of this file
 * that are not between comment pairs USER CODE BEGIN and
 * USER CODE END. Other portions of this file, whether
 * inserted by the user or by software development tools
 * are owned by their respective copyright owners.
 *
 * Copyright (c) 2017 STMicroelectronics International N.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted, provided that the following conditions are met:
 *
 * 1. Redistribution of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of STMicroelectronics nor the names of other
 *    contributors to this software may be used to endorse or promote products
 *    derived from this software without specific written permission.
 * 4. This software, including modifications and/or derivative works of this
 *    software, must execute solely and exclusively on microcontroller or
 *    microprocessor devices manufactured by or for STMicroelectronics.
 * 5. Redistribution and use of this software other than as permitted under
 *    this license is void and will automatically terminate your rights under
 *    this license.
 *
 * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
 * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT
 * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

/* USER CODE BEGIN Includes */     

#include 	"stdio.h"
#include	"stdlib.h"

/* TFT LCD 240x320 using 9341 driver chip */
#include	"tft9341.h"

/* Flash W25Q64 for 64kbit (8Mbytes) image data */
#include	"w25qxx_flash.h"

/* Temperature and Humidity Sensor HDC1080 */
#include	"hdc1080.h"

#include	"spi.h"

#include	"i2c.h"

#include 	"time.h"

#include	"Application.h"
/* USER CODE END Includes */

/* Variables -----------------------------------------------------------------*/
osThreadId mainTaskHandle;
osThreadId slideTaskHandle;
osThreadId ActionTaskHandle;
osThreadId TFTgateKeeperTaskHandle;
osTimerId countdownTimerHandle;
osMutexId mutexControlI2CHandle;

/* USER CODE BEGIN Variables */
QueueHandle_t queueControlTFT;

extern uint16_t POINT_COLOR,BACK_COLOR ;
extern EClock_HandleTypeDef eClock_HandleTypeDef;

uint8_t time_value_temp[7] = { 0 };
uint8_t string_buffer[20];
volatile static uint8_t suspendTaskFlag = 0, time_out = 10;
static uint8_t pressModeCount = 0;

/* USER CODE END Variables */

/* Function prototypes -------------------------------------------------------*/
void MainTaskFunction(void const * argument);
void SlideTaskFunction(void const * argument);
void ActionTaskFunction(void const * argument);
void TFTgateKeeperTaskFunction(void const * argument);
void countdownTimerCallBack(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* USER CODE BEGIN FunctionPrototypes */

/* Read image from flash then display it on TFTLCD
 * image_id from 0 to 53 ( 54 Images )
 */
void Display_Image(W25QXX_Handle_TypeDef * flash_data, uint8_t image_id);

void Display_Template(W25QXX_Handle_TypeDef * flash_data, uint8_t img_id);

void Play_Speaker(GPIO_TypeDef* Speaker_port, uint16_t Speaker_pin, uint8_t number_of_play);
/* USER CODE END FunctionPrototypes */

/* Hook prototypes */

/* Init FreeRTOS */

void MX_FREERTOS_Init(void) {
	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Create the mutex(es) */
	/* definition and creation of mutexControlI2C */
	osMutexDef(mutexControlI2C);
	mutexControlI2CHandle = osMutexCreate(osMutex(mutexControlI2C));

	/* USER CODE BEGIN RTOS_MUTEX */
	/* add mutexes, ... */
	/* USER CODE END RTOS_MUTEX */

	/* USER CODE BEGIN RTOS_SEMAPHORES */
	/* add semaphores, ... */

	/* USER CODE END RTOS_SEMAPHORES */

	/* Create the timer(s) */
	/* definition and creation of countdownTimer */
	osTimerDef(countdownTimer, countdownTimerCallBack);
	countdownTimerHandle = osTimerCreate(osTimer(countdownTimer), osTimerPeriodic, NULL);

	/* USER CODE BEGIN RTOS_TIMERS */
	/* start timers, add new ones, ... */
	/* USER CODE END RTOS_TIMERS */

	/* Create the thread(s) */
	/* definition and creation of mainTask */
	osThreadDef(mainTask, MainTaskFunction, osPriorityNormal, 0, 256);
	mainTaskHandle = osThreadCreate(osThread(mainTask), NULL);

	/* definition and creation of slideTask */
	osThreadDef(slideTask, SlideTaskFunction, osPriorityBelowNormal, 0, 256);
	slideTaskHandle = osThreadCreate(osThread(slideTask), NULL);

	/* definition and creation of ActionTask */
	osThreadDef(ActionTask, ActionTaskFunction, osPriorityAboveNormal, 0, 256);
	ActionTaskHandle = osThreadCreate(osThread(ActionTask), NULL);

	/* definition and creation of TFTgateKeeperTask */
	osThreadDef(TFTgateKeeperTask, TFTgateKeeperTaskFunction, osPriorityRealtime, 0, 256);
	TFTgateKeeperTaskHandle = osThreadCreate(osThread(TFTgateKeeperTask), NULL);

	/* USER CODE BEGIN RTOS_THREADS */
	/* add threads, ... */
	/* USER CODE END RTOS_THREADS */

	/* USER CODE BEGIN RTOS_QUEUES */
	/* add queues, ... */
	queueControlTFT	=	xQueueCreate(10,sizeof(TFTGateKeeperHandleTypedef));
	/* USER CODE END RTOS_QUEUES */
}

/* MainTaskFunction function */
void MainTaskFunction(void const * argument)
{

	/* USER CODE BEGIN MainTaskFunction */
	UNUSED(argument);
	uint8_t i;
	static	uint8_t dot_flag = 1;
	TFTGateKeeperHandleTypedef sSend = {TFT_FONT_TEMPLATE, (uint8_t*)"",TIME_DISPLAY_COLOR,0,0,0,0,TFT_STRING_MODE_BACKGROUND };

	srand(time(0));

	//Display template
	xQueueSend(queueControlTFT, &sSend, portMAX_DELAY);

	Play_Speaker(eClock_HandleTypeDef.eClockHardware.Speaker_Port, eClock_HandleTypeDef.eClockHardware.Speaker_Pin, 3);

	/* Infinite loop */
	while(1)
	{

		if(osMutexWait(mutexControlI2CHandle, osWaitForever) == osOK)
		{
			//Read time from RTC
			HAL_I2C_Mem_Read(&hi2c1, DS3231_ADD<<1, 0x00, I2C_MEMADD_SIZE_8BIT, time_value_temp, 7, 1000);
			osMutexRelease(mutexControlI2CHandle);
		}

		sSend.tftFontSize = TFT_FONT_26x48;
		sSend.rowIndex1 = HOUR_LOCAL_ROW;
		sSend.columnIndex1 = HOUR_LOCAL_COLUMN;

		//Check Time value and display time
		for(i = 0 ; i < 7 ; ++i)
		{
			if(*(eClock_HandleTypeDef.etime_value + i) != *(time_value_temp+i))
			{
				*(eClock_HandleTypeDef.etime_value + i) = *(time_value_temp+i);

				switch(i)
				{
				case 1: //Minus

					//random change image every PERIOD_CHANGING_IMAGE minutes.
					if((*(eClock_HandleTypeDef.etime_value + 1) % PERIOD_CHANGING_IMAGE == 0) && (eClock_HandleTypeDef.eClockSystemState == EClockSystemState_Running))
					{
						//Display template
						sSend.tftFontSize = TFT_FONT_TEMPLATE;
						xQueueSend(queueControlTFT, &sSend, portMAX_DELAY);

						//Display hour
						sprintf( (char*)string_buffer,"%02x" , *(eClock_HandleTypeDef.etime_value + 2));
						sSend.tftFontSize = TFT_FONT_26x48;
						sSend.data = string_buffer;
						xQueueSend(queueControlTFT, &sSend, portMAX_DELAY);


						//Display date/month
						sprintf((char*)string_buffer," %02x/%02x ",*(eClock_HandleTypeDef.etime_value + 4),*(eClock_HandleTypeDef.etime_value + 5));
						sSend.tftFontSize = TFT_FONT_14x24;
						sSend.rowIndex1 = MONTH_LOCAL_ROW;
						sSend.columnIndex1 = 17;
						sSend.data = string_buffer;
						xQueueSend(queueControlTFT, &sSend, portMAX_DELAY);
					}

					sprintf( (char*)string_buffer,"%02x"  , *(eClock_HandleTypeDef.etime_value + 1));
					sSend.tftFontSize = TFT_FONT_26x48;
					sSend.data = string_buffer;
					sSend.rowIndex1 = MINUS_LOCAL_ROW;
					sSend.columnIndex1 = MINUS_LOCAL_COLUMN;
					xQueueSend(queueControlTFT, &sSend, portMAX_DELAY);
					break;

				case 2: //Hour

					sprintf( (char*)string_buffer,"%02x" , *(eClock_HandleTypeDef.etime_value + 2));
					sSend.data = string_buffer;
					sSend.rowIndex1 = HOUR_LOCAL_ROW;
					sSend.columnIndex1 = HOUR_LOCAL_COLUMN;
					xQueueSend(queueControlTFT, &sSend, portMAX_DELAY);

					//If changing hour, play speaker when ( 6h AM <= hour <= 22h PM )
					if((*(eClock_HandleTypeDef.etime_value + 2) >= 0x06) && (*(eClock_HandleTypeDef.etime_value + 2) <= 0x22) && (eClock_HandleTypeDef.eClockSystemState == EClockSystemState_Running))
					{
						Play_Speaker(eClock_HandleTypeDef.eClockHardware.Speaker_Port, eClock_HandleTypeDef.eClockHardware.Speaker_Pin, 3);
					}

					if(eClock_HandleTypeDef.eClockSystemState == EClockSystemState_Start)
					{
						eClock_HandleTypeDef.eClockSystemState = EClockSystemState_Running;
					}
					break;
				}
			}
		}


		if(dot_flag) //Blink dot (:)
		{
			dot_flag = 0;
			sSend.color = TIME_DISPLAY_COLOR;
		}
		else
		{
			dot_flag = 1;
			sSend.color = BACKGROUND_COLOR;
		}

		sSend.data = (uint8_t*)":";
		sSend.rowIndex1 = DOT_LOCAL_ROW;
		sSend.columnIndex1 = DOT_LOCAL_COLUMN;
		xQueueSend(queueControlTFT, &sSend, portMAX_DELAY);
		sSend.color = TIME_DISPLAY_COLOR;

		osDelay(eClock_HandleTypeDef.eperiod_read_time);
	}
	/* USER CODE END MainTaskFunction */
}

/* SlideTaskFunction function */
void SlideTaskFunction(void const * argument)
{
	/* USER CODE BEGIN SlideTaskFunction */
	UNUSED(argument);
	static uint8_t slide_flag = 0;
	TFTGateKeeperHandleTypedef sSend = {TFT_FONT_14x24, (uint8_t*)"",TIME_DISPLAY_COLOR,MONTH_LOCAL_ROW,17,0,0,TFT_STRING_MODE_BACKGROUND };

	/* Infinite loop */
	while(1)
	{

		if(osMutexWait(mutexControlI2CHandle, osWaitForever) == osOK)
		{
			hdc1080_start_measurement(&hi2c1,&eClock_HandleTypeDef.etemperature,&eClock_HandleTypeDef.ehumidity);
			osMutexRelease(mutexControlI2CHandle);
		}


		switch(slide_flag)
		{
		case 0://Display temperature
			sprintf((char*)string_buffer,"%5.2f C",eClock_HandleTypeDef.etemperature);
			sSend.data = string_buffer;
			xQueueSend(queueControlTFT, &sSend, portMAX_DELAY);

			//Display Code for degree Celsius
			sSend.tftFontSize = TFT_FONT_SPECIAL;
			xQueueSend(queueControlTFT, &sSend, portMAX_DELAY);
			sSend.tftFontSize = TFT_FONT_14x24;
			break;

		case 1://Display Humidity
			sprintf((char*)string_buffer,"  %02d%%  ",eClock_HandleTypeDef.ehumidity);
			sSend.data = string_buffer;
			xQueueSend(queueControlTFT, &sSend, portMAX_DELAY);
			break;

		case 2://Display Date/Month
			sprintf((char*)string_buffer," %02x/%02x ",*(eClock_HandleTypeDef.etime_value + 4),*(eClock_HandleTypeDef.etime_value + 5));
			sSend.data = string_buffer;
			xQueueSend(queueControlTFT, &sSend, portMAX_DELAY);
			break;
		}

		if(++slide_flag == 3)
		{
			slide_flag = 0;
		}

		osDelay(eClock_HandleTypeDef.eperiod_read_temhumi);
	}

	/* USER CODE END SlideTaskFunction */
}

/* ActionTaskFunction function */
void ActionTaskFunction(void const * argument)
{
	/* USER CODE BEGIN ActionTaskFunction */
	static uint8_t  image_id_slide = 255;
	TFTGateKeeperHandleTypedef sSend = {TFT_FONT_26x48, (uint8_t*)"",TIME_DISPLAY_COLOR,HOUR_LOCAL_ROW_ST,HOUR_LOCAL_COLUMN_ST,0,0,TFT_STRING_MODE_BACKGROUND };
	osEvent eClockEvent;

	/* Infinite loop */
	for(;;)
	{
		eClockEvent = osSignalWait(BUTTON_MODE_SIGNAL|BUTTON_UP_SIGNAL|BUTTON_DOWN_SIGNAL,osWaitForever);

		time_out = 11;

		if(eClock_HandleTypeDef.eClockMode == EClockMode_Settime)
		{
			osTimerStart(countdownTimerHandle, 1000);
		}

		if(suspendTaskFlag)
		{
			//Suspend others task
			osThreadSuspend(mainTaskHandle);
			osThreadSuspend(slideTaskHandle);

			suspendTaskFlag = 0;

			if(eClock_HandleTypeDef.eClockMode == EClockMode_Settime)
			{
				for (uint8_t index = 0; index < 7; index++)
				{
					*(eClock_HandleTypeDef.etime_value + index) = Convert_Bcd2Dec(*(eClock_HandleTypeDef.etime_value + index));
				}
			}
		}

		switch (eClockEvent.value.signals)
		{
		case BUTTON_MODE_SIGNAL:
			if(eClock_HandleTypeDef.eClockMode == EClockMode_Settime)
			{
				if(!pressModeCount)//First press mode_button go to settime mode
				{
					//Clear Screen
					sSend.tftFontSize = TFT_FONT_CLEAR;
					xQueueSend(queueControlTFT,&sSend,portMAX_DELAY);

					//Display hour
					sprintf( (char*)string_buffer,"%02d" , *(eClock_HandleTypeDef.etime_value + 2));
					sSend.tftFontSize = TFT_FONT_26x48;
					sSend.data = string_buffer;
					sSend.rowIndex1 = HOUR_LOCAL_ROW_ST;
					sSend.columnIndex1 = HOUR_LOCAL_COLUMN_ST;
					xQueueSend(queueControlTFT,&sSend,portMAX_DELAY);

					sSend.columnIndex1 = 67;
					sSend.data = (uint8_t*)":";
					xQueueSend(queueControlTFT,&sSend,portMAX_DELAY);

					//Display minus
					sprintf( (char*)string_buffer,"%02d" , *(eClock_HandleTypeDef.etime_value + 1));
					sSend.data = string_buffer;
					sSend.rowIndex1 = MINUS_LOCAL_ROW_ST;
					sSend.columnIndex1 = MINUS_LOCAL_COLUMN_ST;
					xQueueSend(queueControlTFT,&sSend,portMAX_DELAY);

					sSend.columnIndex1 = 145;
					sSend.data = (uint8_t*)":";
					xQueueSend(queueControlTFT,&sSend,portMAX_DELAY);

					//Display second
					sprintf( (char*)string_buffer,"%02d" , *(eClock_HandleTypeDef.etime_value + 0));
					sSend.rowIndex1 = SECOND_LOCAL_ROW_ST;
					sSend.columnIndex1 = SECOND_LOCAL_COLUMN_ST;
					sSend.data = string_buffer;
					xQueueSend(queueControlTFT,&sSend,portMAX_DELAY);

					//Display date
					sprintf( (char*)string_buffer,"%02d" , *(eClock_HandleTypeDef.etime_value + 4));
					sSend.rowIndex1 = DATE_LOCAL_ROW_ST;
					sSend.columnIndex1 = DATE_LOCAL_COLUMN_ST;
					xQueueSend(queueControlTFT,&sSend,portMAX_DELAY);


					sSend.columnIndex1 = 67;
					sSend.data = (uint8_t*)"/";
					xQueueSend(queueControlTFT,&sSend,portMAX_DELAY);

					//Display month
					sprintf( (char*)string_buffer,"%02d" , *(eClock_HandleTypeDef.etime_value + 5));
					sSend.rowIndex1 = MONTH_LOCAL_ROW_ST;
					sSend.columnIndex1 = MONTH_LOCAL_COLUMN_ST;
					sSend.data = string_buffer;
					xQueueSend(queueControlTFT,&sSend,portMAX_DELAY);

					sSend.columnIndex1 = 145;
					sSend.data = (uint8_t*)"/";
					xQueueSend(queueControlTFT,&sSend,portMAX_DELAY);

					//Display year
					sprintf( (char*)string_buffer,"%02d" , *(eClock_HandleTypeDef.etime_value + 6));
					sSend.rowIndex1 = YEAR_LOCAL_ROW_ST;
					sSend.columnIndex1 = YEAR_LOCAL_COLUMN_ST;
					sSend.data = string_buffer;
					xQueueSend(queueControlTFT,&sSend,portMAX_DELAY);

					//Display day
					sSend.rowIndex1 = DAY_LOCAL_ROW_ST;
					sSend.columnIndex1 = DAY_LOCAL_COLUMN_ST;
					if( *(eClock_HandleTypeDef.etime_value + 3)!=1)
					{
						sprintf( (char*)string_buffer,"T%01d" , *(eClock_HandleTypeDef.etime_value + 3));
						sSend.data = string_buffer;
					}
					else
					{
						sSend.data = (uint8_t*)"CN";
					}
					xQueueSend(queueControlTFT,&sSend,portMAX_DELAY);

					//Focus hour
					sSend.rowIndex1 = HOUR_LOCAL_ROW_ST-1;
					sSend.columnIndex1 = HOUR_LOCAL_COLUMN_ST-1;
					sSend.rowIndex2 = HOUR_LOCAL_ROW_ST+49;
					sSend.columnIndex2 = HOUR_LOCAL_COLUMN_ST+51;
					sSend.color = FOCUS_COLOR;
					sSend.tftFontSize = TFT_FONT_DRAW_RECTANGLE;
					xQueueSend(queueControlTFT,&sSend,portMAX_DELAY);

					//Display Time out value (if time out = 0 go back to normal mode)
					sprintf((char*)string_buffer, "Time Out = %02d", time_out );
					sSend.rowIndex1 = 295;
					sSend.columnIndex1 = 0;
					sSend.color = TIME_DISPLAY_COLOR;
					sSend.tftFontSize = TFT_FONT_14x24;
					sSend.data = string_buffer;
					xQueueSend(queueControlTFT,&sSend,portMAX_DELAY);

				}
				else
				{
					switch (pressModeCount)
					{
					case 1:
						//Jump from focus hour to focus minus
						sSend.tftFontSize = TFT_FONT_DRAW_RECTANGLE;

						sSend.rowIndex1 = HOUR_LOCAL_ROW_ST-1;
						sSend.columnIndex1 = HOUR_LOCAL_COLUMN_ST-1;
						sSend.rowIndex2 = HOUR_LOCAL_ROW_ST+49;
						sSend.columnIndex2 = HOUR_LOCAL_COLUMN_ST+51;
						sSend.color = BACK_COLOR;
						xQueueSend(queueControlTFT,&sSend,portMAX_DELAY);

						sSend.rowIndex1 = MINUS_LOCAL_ROW_ST-1;
						sSend.columnIndex1 = MINUS_LOCAL_COLUMN_ST-1;
						sSend.columnIndex2 = MINUS_LOCAL_COLUMN_ST+51;
						sSend.color = FOCUS_COLOR;
						xQueueSend(queueControlTFT,&sSend,portMAX_DELAY);
						break;
					case 2:
						//Jump from focus minus to focus second
						sSend.tftFontSize = TFT_FONT_DRAW_RECTANGLE;

						sSend.rowIndex1 = MINUS_LOCAL_ROW_ST-1;
						sSend.columnIndex1 = MINUS_LOCAL_COLUMN_ST-1;
						sSend.rowIndex2 = HOUR_LOCAL_ROW_ST+49;
						sSend.columnIndex2 = MINUS_LOCAL_COLUMN_ST+51;
						sSend.color = BACK_COLOR;
						xQueueSend(queueControlTFT,&sSend,portMAX_DELAY);

						sSend.rowIndex1 = SECOND_LOCAL_ROW_ST-1;
						sSend.columnIndex1 = SECOND_LOCAL_COLUMN_ST-1;
						sSend.columnIndex2 = SECOND_LOCAL_COLUMN_ST+51;
						sSend.color = FOCUS_COLOR;
						xQueueSend(queueControlTFT,&sSend,portMAX_DELAY);
						break;
					case 3:
						//Jump from focus second to focus day
						sSend.tftFontSize = TFT_FONT_DRAW_RECTANGLE;
						sSend.rowIndex1 = SECOND_LOCAL_ROW_ST-1;
						sSend.columnIndex1 = SECOND_LOCAL_COLUMN_ST-1;
						sSend.rowIndex2 = HOUR_LOCAL_ROW_ST+49;
						sSend.columnIndex2 = SECOND_LOCAL_COLUMN_ST+51;
						sSend.color = BACK_COLOR;
						xQueueSend(queueControlTFT,&sSend,portMAX_DELAY);

						sSend.rowIndex1 = DAY_LOCAL_ROW_ST-1;
						sSend.columnIndex1 = DAY_LOCAL_COLUMN_ST-1;
						sSend.rowIndex2 = DAY_LOCAL_ROW_ST+49;
						sSend.columnIndex2 = DAY_LOCAL_COLUMN_ST+51;
						sSend.color = FOCUS_COLOR;
						xQueueSend(queueControlTFT,&sSend,portMAX_DELAY);
						break;
					case 4:
						//Jump from focus day to focus date
						sSend.tftFontSize = TFT_FONT_DRAW_RECTANGLE;
						sSend.rowIndex1 = DAY_LOCAL_ROW_ST-1;
						sSend.columnIndex1 = DAY_LOCAL_COLUMN_ST-1;
						sSend.rowIndex2 = DAY_LOCAL_ROW_ST+49;
						sSend.columnIndex2 = DAY_LOCAL_COLUMN_ST+51;
						sSend.color = BACK_COLOR;
						xQueueSend(queueControlTFT,&sSend,portMAX_DELAY);

						sSend.rowIndex1 = DATE_LOCAL_ROW_ST-1;
						sSend.columnIndex1 = DATE_LOCAL_COLUMN_ST-1;
						sSend.rowIndex2 = DATE_LOCAL_ROW_ST+49;
						sSend.columnIndex2 = DATE_LOCAL_COLUMN_ST+51;
						sSend.color = FOCUS_COLOR;
						xQueueSend(queueControlTFT,&sSend,portMAX_DELAY);
						break;
					case 5:
						//Jump from focus date to focus month
						sSend.tftFontSize = TFT_FONT_DRAW_RECTANGLE;
						sSend.rowIndex1 = DATE_LOCAL_ROW_ST-1;
						sSend.columnIndex1 = DATE_LOCAL_COLUMN_ST-1;
						sSend.rowIndex2 = DATE_LOCAL_ROW_ST+49;
						sSend.columnIndex2 = DATE_LOCAL_COLUMN_ST+51;
						sSend.color = BACK_COLOR;
						xQueueSend(queueControlTFT,&sSend,portMAX_DELAY);

						sSend.rowIndex1 = MONTH_LOCAL_ROW_ST-1;
						sSend.columnIndex1 = MONTH_LOCAL_COLUMN_ST-1;
						sSend.columnIndex2 = MONTH_LOCAL_COLUMN_ST+51;
						sSend.color = FOCUS_COLOR;
						xQueueSend(queueControlTFT,&sSend,portMAX_DELAY);
						break;
					case 6:
						//Jump from focus month to focus year
						sSend.tftFontSize = TFT_FONT_DRAW_RECTANGLE;
						sSend.rowIndex1 = MONTH_LOCAL_ROW_ST-1;
						sSend.columnIndex1 = MONTH_LOCAL_COLUMN_ST-1;
						sSend.rowIndex2 = DATE_LOCAL_ROW_ST+49;
						sSend.columnIndex2 = MONTH_LOCAL_COLUMN_ST+51;
						sSend.color = BACK_COLOR;
						xQueueSend(queueControlTFT,&sSend,portMAX_DELAY);

						sSend.rowIndex1 = YEAR_LOCAL_ROW_ST-1;
						sSend.columnIndex1 = YEAR_LOCAL_COLUMN_ST-1;
						sSend.columnIndex2 = YEAR_LOCAL_COLUMN_ST+51;
						sSend.color = FOCUS_COLOR;
						xQueueSend(queueControlTFT,&sSend,portMAX_DELAY);
						break;
					}
				}
				pressModeCount ++;
				if(pressModeCount == 8)
				{
					pressModeCount = 0;

					for (uint8_t index = 0; index < 7; index++)
					{
						*(eClock_HandleTypeDef.etime_value + index ) = Convert_Dec2Bcd(*(eClock_HandleTypeDef.etime_value + index));
					}

					if(osMutexWait(mutexControlI2CHandle, osWaitForever) == osOK)
					{
						//Write new time value to Ds3231
						HAL_I2C_Mem_Write(&hi2c1, DS3231_ADD<<1, 0x00, I2C_MEMADD_SIZE_8BIT, (uint8_t*)eClock_HandleTypeDef.etime_value, 7, 2000);
						osMutexRelease(mutexControlI2CHandle);
					}
					eClock_HandleTypeDef.eClockMode = EClockMode_Normal;

					//Display Template
					sSend.tftFontSize = TFT_FONT_TEMPLATE;
					xQueueSend(queueControlTFT,&sSend,portMAX_DELAY);

					//Display hour
					sprintf( (char*)string_buffer,"%02x" , *(eClock_HandleTypeDef.etime_value + 2));
					sSend.tftFontSize = TFT_FONT_26x48;
					sSend.data = string_buffer;
					sSend.rowIndex1 = HOUR_LOCAL_ROW;
					sSend.columnIndex1 = HOUR_LOCAL_COLUMN;
					sSend.color = TIME_DISPLAY_COLOR;
					xQueueSend(queueControlTFT,&sSend,portMAX_DELAY);

					//Display minus
					sprintf( (char*)string_buffer,"%02x"  , *(eClock_HandleTypeDef.etime_value + 1));
					sSend.rowIndex1 = MINUS_LOCAL_ROW;
					sSend.columnIndex1 = MINUS_LOCAL_COLUMN;
					xQueueSend(queueControlTFT,&sSend,portMAX_DELAY);

					//Display date/month
					sprintf((char*)string_buffer," %02x/%02x ",*(eClock_HandleTypeDef.etime_value + 4),*(eClock_HandleTypeDef.etime_value + 5));
					sSend.tftFontSize = TFT_FONT_14x24;
					sSend.rowIndex1 = MONTH_LOCAL_ROW;
					sSend.columnIndex1 = 17;
					xQueueSend(queueControlTFT,&sSend,portMAX_DELAY);

					Play_Speaker(eClock_HandleTypeDef.eClockHardware.Speaker_Port, eClock_HandleTypeDef.eClockHardware.Speaker_Pin, 2);

					osThreadResume(mainTaskHandle);
					osThreadResume(slideTaskHandle);
				}
			}
			else if(eClock_HandleTypeDef.eClockMode == EClockMode_Slide)
			{
				eClock_HandleTypeDef.eClockMode = EClockMode_Normal;

				//Display Template
				sSend.tftFontSize = TFT_FONT_TEMPLATE;
				xQueueSend(queueControlTFT,&sSend,portMAX_DELAY);

				//Display hour
				sprintf( (char*)string_buffer,"%02x" , *(eClock_HandleTypeDef.etime_value + 2));
				sSend.tftFontSize = TFT_FONT_26x48;
				sSend.data = string_buffer;
				sSend.rowIndex1 = HOUR_LOCAL_ROW;
				sSend.columnIndex1 = HOUR_LOCAL_COLUMN;
				sSend.color = TIME_DISPLAY_COLOR;
				xQueueSend(queueControlTFT,&sSend,portMAX_DELAY);


				//Display minute
				sprintf( (char*)string_buffer,"%02x"  , *(eClock_HandleTypeDef.etime_value + 1));
				sSend.rowIndex1 = MINUS_LOCAL_ROW;
				sSend.columnIndex1 = MINUS_LOCAL_COLUMN;
				xQueueSend(queueControlTFT,&sSend,portMAX_DELAY);

				//Display date/month
				sprintf((char*)string_buffer," %02x/%02x ",*(eClock_HandleTypeDef.etime_value + 4),*(eClock_HandleTypeDef.etime_value + 5));
				sSend.tftFontSize = TFT_FONT_14x24;
				sSend.rowIndex1 = MONTH_LOCAL_ROW;
				sSend.columnIndex1 = 17;
				xQueueSend(queueControlTFT,&sSend,portMAX_DELAY);

				osThreadResume(mainTaskHandle);
				osThreadResume(slideTaskHandle);
			}
			break;
		case BUTTON_UP_SIGNAL:
			switch ((uint8_t)eClock_HandleTypeDef.eClockMode)
			{
			case EClockMode_Settime:

				switch (pressModeCount)
				{
				case 1://Change hour
					(*(eClock_HandleTypeDef.etime_value+2)) ++;

					if(*(eClock_HandleTypeDef.etime_value+2) > 23 )
					{
						*(eClock_HandleTypeDef.etime_value+2) = 0;
					}
					sprintf( (char*)string_buffer,"%02d" ,*(eClock_HandleTypeDef.etime_value + 2));
					sSend.tftFontSize = TFT_FONT_26x48;
					sSend.data = string_buffer;
					sSend.color = TIME_DISPLAY_COLOR;
					sSend.rowIndex1 = HOUR_LOCAL_ROW_ST;
					sSend.columnIndex1 = HOUR_LOCAL_COLUMN_ST;
					sSend.color = TIME_DISPLAY_COLOR;
					xQueueSend(queueControlTFT,&sSend,portMAX_DELAY);
					break;
				case 2: //Change minute
					(*(eClock_HandleTypeDef.etime_value+1)) ++;
					if(*(eClock_HandleTypeDef.etime_value+1) > 59 )
					{
						*(eClock_HandleTypeDef.etime_value+1) = 0;
					}
					sprintf( (char*)string_buffer,"%02d" , *(eClock_HandleTypeDef.etime_value + 1));
					sSend.tftFontSize = TFT_FONT_26x48;
					sSend.color = TIME_DISPLAY_COLOR;
					sSend.rowIndex1 = MINUS_LOCAL_ROW_ST;
					sSend.columnIndex1 = MINUS_LOCAL_COLUMN_ST;
					sSend.data = string_buffer;
					xQueueSend(queueControlTFT,&sSend,portMAX_DELAY);
					break;
				case 3: //Change Second
					(*(eClock_HandleTypeDef.etime_value+0)) ++;
					if(*(eClock_HandleTypeDef.etime_value+0) > 59 )
					{
						*(eClock_HandleTypeDef.etime_value+0) = 0;
					}
					sprintf( (char*)string_buffer,"%02d" , *(eClock_HandleTypeDef.etime_value + 0));
					sSend.tftFontSize = TFT_FONT_26x48;
					sSend.color = TIME_DISPLAY_COLOR;
					sSend.rowIndex1 = SECOND_LOCAL_ROW_ST;
					sSend.columnIndex1 = SECOND_LOCAL_COLUMN_ST;
					xQueueSend(queueControlTFT,&sSend,portMAX_DELAY);
					break;
				case 4: //Change day
					(*(eClock_HandleTypeDef.etime_value+3)) ++;
					if(*(eClock_HandleTypeDef.etime_value+3) > 7 )
					{
						*(eClock_HandleTypeDef.etime_value+3) = 1;
					}

					sSend.rowIndex1 = DAY_LOCAL_ROW_ST;
					sSend.columnIndex1 = DAY_LOCAL_COLUMN_ST;

					if( *(eClock_HandleTypeDef.etime_value + 3)!=1)
					{
						sprintf( (char*)string_buffer,"T%01d" , *(eClock_HandleTypeDef.etime_value + 3));
						sSend.data = string_buffer;
					}
					else
					{
						sSend.data = (uint8_t*) "CN";
					}
					sSend.tftFontSize = TFT_FONT_26x48;
					sSend.color = TIME_DISPLAY_COLOR;
					xQueueSend(queueControlTFT,&sSend,portMAX_DELAY);
					break;
				case 5: //Change date
					(*(eClock_HandleTypeDef.etime_value+4)) ++;
					if(*(eClock_HandleTypeDef.etime_value+4) > 31 )
					{
						*(eClock_HandleTypeDef.etime_value+4) = 1;
					}
					sprintf( (char*)string_buffer,"%02d" , *(eClock_HandleTypeDef.etime_value + 4));
					sSend.tftFontSize = TFT_FONT_26x48;
					sSend.color = TIME_DISPLAY_COLOR;
					sSend.data = string_buffer;
					sSend.rowIndex1 = DATE_LOCAL_ROW_ST;
					sSend.columnIndex1 = DATE_LOCAL_COLUMN_ST;
					xQueueSend(queueControlTFT,&sSend,portMAX_DELAY);
					break;
				case 6: //Change month
					(*(eClock_HandleTypeDef.etime_value+5)) ++;
					if(*(eClock_HandleTypeDef.etime_value+5) > 12 )
					{
						*(eClock_HandleTypeDef.etime_value+5) = 1;
					}
					//Display month
					sprintf( (char*)string_buffer,"%02d" , *(eClock_HandleTypeDef.etime_value + 5));
					sSend.rowIndex1 = MONTH_LOCAL_ROW_ST;
					sSend.columnIndex1 = MONTH_LOCAL_COLUMN_ST;
					sSend.tftFontSize = TFT_FONT_26x48;
					sSend.color = TIME_DISPLAY_COLOR;
					xQueueSend(queueControlTFT,&sSend,portMAX_DELAY);
					break;
				case 7://Change year
					(*(eClock_HandleTypeDef.etime_value+6)) ++;
					if(*(eClock_HandleTypeDef.etime_value+6) > 99 )
					{
						*(eClock_HandleTypeDef.etime_value+6) = 0;
					}
					sprintf( (char*)string_buffer,"%02d" , *(eClock_HandleTypeDef.etime_value + 6));
					sSend.rowIndex1 = YEAR_LOCAL_ROW_ST;
					sSend.columnIndex1 = YEAR_LOCAL_COLUMN_ST;
					sSend.tftFontSize = TFT_FONT_26x48;
					sSend.color = TIME_DISPLAY_COLOR;
					xQueueSend(queueControlTFT,&sSend,portMAX_DELAY);
					break;
				}
				break;
				case EClockMode_Slide:

					if(++image_id_slide >53)
					{
						image_id_slide = 0;
					}

					//Display Image
					sSend.tftFontSize = TFT_FONT_IMAGE;
					sSend.rowIndex1 = image_id_slide;
					xQueueSend(queueControlTFT,&sSend,portMAX_DELAY);

					//Display Image Id
					sprintf( (char*)string_buffer,"%02d" , image_id_slide);
					sSend.tftFontSize = TFT_FONT_14x24;
					sSend.data = string_buffer;
					sSend.rowIndex1 = 296;
					sSend.columnIndex1 = 0;
					sSend.color = TIME_DISPLAY_COLOR;
					xQueueSend(queueControlTFT,&sSend,portMAX_DELAY);
					break;
			}
			break;
			case BUTTON_DOWN_SIGNAL:
				switch ((uint8_t)eClock_HandleTypeDef.eClockMode)
				{
				case EClockMode_Settime:

					switch (pressModeCount)
					{
					case 1://Change hour
						(*(eClock_HandleTypeDef.etime_value+2)) --;

						if(*(eClock_HandleTypeDef.etime_value+2) == 255 )
						{
							*(eClock_HandleTypeDef.etime_value+2) = 23;
						}
						sprintf( (char*)string_buffer,"%02d" , *(eClock_HandleTypeDef.etime_value + 2));
						sSend.tftFontSize = TFT_FONT_26x48;
						sSend.data = string_buffer;
						sSend.rowIndex1 = HOUR_LOCAL_ROW_ST;
						sSend.columnIndex1 = HOUR_LOCAL_COLUMN_ST;
						sSend.color = TIME_DISPLAY_COLOR;
						xQueueSend(queueControlTFT,&sSend,portMAX_DELAY);
						break;
					case 2: //Change minute
						(*(eClock_HandleTypeDef.etime_value+1)) --;
						if(*(eClock_HandleTypeDef.etime_value+1) == 255 )
						{
							*(eClock_HandleTypeDef.etime_value+1) = 59;
						}
						sprintf( (char*)string_buffer,"%02d" , *(eClock_HandleTypeDef.etime_value + 1));
						sSend.tftFontSize = TFT_FONT_26x48;
						sSend.color = TIME_DISPLAY_COLOR;
						sSend.rowIndex1 = MINUS_LOCAL_ROW_ST;
						sSend.columnIndex1 = MINUS_LOCAL_COLUMN_ST;
						xQueueSend(queueControlTFT,&sSend,portMAX_DELAY);
						break;
					case 3: //Change Second
						(*(eClock_HandleTypeDef.etime_value+0)) --;
						if(*(eClock_HandleTypeDef.etime_value+0) == 255 )
						{
							*(eClock_HandleTypeDef.etime_value+0) = 59;
						}
						sprintf( (char*)string_buffer,"%02d" , *(eClock_HandleTypeDef.etime_value + 0));
						sSend.tftFontSize = TFT_FONT_26x48;
						sSend.color = TIME_DISPLAY_COLOR;
						sSend.rowIndex1 = SECOND_LOCAL_ROW_ST;
						sSend.columnIndex1 = SECOND_LOCAL_COLUMN_ST;
						xQueueSend(queueControlTFT,&sSend,portMAX_DELAY);
						break;
					case 4: //Change day
						(*(eClock_HandleTypeDef.etime_value+3)) --;
						if(*(eClock_HandleTypeDef.etime_value+3) < 1 )
						{
							*(eClock_HandleTypeDef.etime_value+3) = 7;
						}
						sSend.rowIndex1 = DAY_LOCAL_ROW_ST;
						sSend.columnIndex1 = DAY_LOCAL_COLUMN_ST;

						if( *(eClock_HandleTypeDef.etime_value + 3)!=1)
						{
							sprintf( (char*)string_buffer,"T%01d" , *(eClock_HandleTypeDef.etime_value + 3));
							sSend.data = string_buffer;
						}
						else
						{
							sSend.data = (uint8_t *)"CN";
						}
						sSend.tftFontSize = TFT_FONT_26x48;
						sSend.color = TIME_DISPLAY_COLOR;
						xQueueSend(queueControlTFT,&sSend,portMAX_DELAY);
						break;
					case 5: //Change date
						(*(eClock_HandleTypeDef.etime_value+4)) --;
						if(*(eClock_HandleTypeDef.etime_value+4) < 1 )
						{
							*(eClock_HandleTypeDef.etime_value+4) = 31;
						}
						sprintf( (char*)string_buffer,"%02d" , *(eClock_HandleTypeDef.etime_value + 4));

						sSend.data = string_buffer;
						sSend.rowIndex1 = DATE_LOCAL_ROW_ST;
						sSend.columnIndex1 = DATE_LOCAL_COLUMN_ST;
						sSend.tftFontSize = TFT_FONT_26x48;
						sSend.color = TIME_DISPLAY_COLOR;
						xQueueSend(queueControlTFT,&sSend,portMAX_DELAY);
						break;
					case 6: //Change month
						(*(eClock_HandleTypeDef.etime_value+5)) --;
						if(*(eClock_HandleTypeDef.etime_value+5) < 1 )
						{
							*(eClock_HandleTypeDef.etime_value+5) = 12;
						}
						//Display month
						sprintf( (char*)string_buffer,"%02d" , *(eClock_HandleTypeDef.etime_value + 5));
						sSend.rowIndex1 = MONTH_LOCAL_ROW_ST;
						sSend.columnIndex1 = MONTH_LOCAL_COLUMN_ST;
						sSend.tftFontSize = TFT_FONT_26x48;
						sSend.color = TIME_DISPLAY_COLOR;
						xQueueSend(queueControlTFT,&sSend,portMAX_DELAY);
						break;
					case 7://Change year
						(*(eClock_HandleTypeDef.etime_value+6)) -- ;
						if(*(eClock_HandleTypeDef.etime_value+6) == 255 )
						{
							*(eClock_HandleTypeDef.etime_value+6) = 99;
						}
						sprintf( (char*)string_buffer,"%02d" , *(eClock_HandleTypeDef.etime_value + 6));
						sSend.rowIndex1 = YEAR_LOCAL_ROW_ST;
						sSend.columnIndex1 = YEAR_LOCAL_COLUMN_ST;
						sSend.tftFontSize = TFT_FONT_26x48;
						sSend.color = TIME_DISPLAY_COLOR;
						xQueueSend(queueControlTFT,&sSend,portMAX_DELAY);
						break;
					}
					break;
					case EClockMode_Slide:
						if(--image_id_slide == 255)
						{
							image_id_slide = 53;
						}

						//Display Image
						sSend.tftFontSize = TFT_FONT_IMAGE;
						sSend.rowIndex1 = image_id_slide;
						xQueueSend(queueControlTFT,&sSend,portMAX_DELAY);

						//Display Image Id
						sprintf( (char*)string_buffer,"%02d" , image_id_slide);
						sSend.tftFontSize = TFT_FONT_14x24;
						sSend.rowIndex1 = 296;
						sSend.columnIndex1 = 0;
						sSend.data = string_buffer;
						sSend.color = TIME_DISPLAY_COLOR;
						xQueueSend(queueControlTFT,&sSend,portMAX_DELAY);

						break;
				}
				break;
		}

	}
	/* USER CODE END ActionTaskFunction */
}

/* TFTgateKeeperTaskFunction function */
void TFTgateKeeperTaskFunction(void const * argument)
{
	/* USER CODE BEGIN TFTgateKeeperTaskFunction */
	TFTGateKeeperHandleTypedef prvTFTHandle;
	/* Infinite loop */
	for(;;)
	{
		if(xQueueReceive(queueControlTFT, &prvTFTHandle, portMAX_DELAY) == pdTRUE)
		{
			if(prvTFTHandle.color != POINT_COLOR)
			{
				POINT_COLOR = prvTFTHandle.color;
			}

			switch (prvTFTHandle.tftFontSize)
			{
			case TFT_FONT_14x24:
				tft_puts14x24(prvTFTHandle.rowIndex1 , prvTFTHandle.columnIndex1 , (int8_t*)prvTFTHandle.data , TFT_STRING_MODE_BACKGROUND);
				break;
			case TFT_FONT_26x48:
				tft_puts26x48(prvTFTHandle.rowIndex1 , prvTFTHandle.columnIndex1 , (int8_t*)prvTFTHandle.data , TFT_STRING_MODE_BACKGROUND);
				break;
			case TFT_FONT_SPECIAL:
				tft_putchar14x24(81,MONTH_LOCAL_ROW,128,TFT_STRING_MODE_BACKGROUND); // Display Code for degree Celsius
				break;
			case TFT_FONT_TEMPLATE:
				Display_Template(eClock_HandleTypeDef.eflash, rand() % 54);
				break;
			case TFT_FONT_CLEAR:
				tft_clear(BACK_COLOR);
				break;
			case TFT_FONT_DRAW_RECTANGLE:
				tft_draw_rectangle(prvTFTHandle.rowIndex1 , prvTFTHandle.columnIndex1, prvTFTHandle.rowIndex2 , prvTFTHandle.columnIndex2, prvTFTHandle.color);
				break;
			case TFT_FONT_IMAGE:
				Display_Image(eClock_HandleTypeDef.eflash, prvTFTHandle.rowIndex1);
				break;

			}
		}
	}
	/* USER CODE END TFTgateKeeperTaskFunction */
}

/* countdownTimerCallBack function */
void countdownTimerCallBack(void const * argument)
{
	/* USER CODE BEGIN countdownTimerCallBack */
	TFTGateKeeperHandleTypedef sSend = {TFT_FONT_14x24, string_buffer,TIME_DISPLAY_COLOR,295,140,0,0,TFT_STRING_MODE_BACKGROUND };

	time_out --;
	sprintf((char*)string_buffer, "%02d", time_out );
	xQueueSend(queueControlTFT,&sSend,0);

	if(time_out == 0)
	{
		osTimerStop(countdownTimerHandle);
		eClock_HandleTypeDef.eClockMode = EClockMode_Slide;
		pressModeCount = 0;
		for (uint8_t index = 0; index < 7; index++)
		{
			*(eClock_HandleTypeDef.etime_value + index) = Convert_Dec2Bcd(*(eClock_HandleTypeDef.etime_value + index));
		}
		osSignalSet(ActionTaskHandle,BUTTON_MODE_SIGNAL);
	}
	/* USER CODE END countdownTimerCallBack */
}

/* USER CODE BEGIN Application */

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	switch(GPIO_Pin)
	{
	case BT_MODE_PIN:
		//Wait button is released
		while(HAL_GPIO_ReadPin(eClock_HandleTypeDef.eClockHardware.BT_Mode_Port,eClock_HandleTypeDef.eClockHardware.BT_Mode_Pin) == GPIO_PIN_SET);
		if(eClock_HandleTypeDef.eClockMode == EClockMode_Normal)
		{
			eClock_HandleTypeDef.eClockMode = EClockMode_Settime;
			suspendTaskFlag = 1;
		}
		osSignalSet(ActionTaskHandle,BUTTON_MODE_SIGNAL);
		break;

	case BT_UP_PIN:
		while(HAL_GPIO_ReadPin(eClock_HandleTypeDef.eClockHardware.BT_Up_Port,eClock_HandleTypeDef.eClockHardware.BT_Up_Pin) == GPIO_PIN_SET);
		if(eClock_HandleTypeDef.eClockMode == EClockMode_Normal)
		{
			eClock_HandleTypeDef.eClockMode = EClockMode_Slide;
			suspendTaskFlag = 1;
		}
		osSignalSet(ActionTaskHandle,BUTTON_UP_SIGNAL);
		break;

	case BT_DOWN_PIN:
		while(HAL_GPIO_ReadPin(eClock_HandleTypeDef.eClockHardware.BT_Down_Port,eClock_HandleTypeDef.eClockHardware.BT_Down_Pin) == GPIO_PIN_SET);
		osSignalSet(ActionTaskHandle,BUTTON_DOWN_SIGNAL);
		break;
	}
}

/* Read image from flash then display it on TFTLCD
 * image_id from 0 to 53 ( 54 Images )
 */
void Display_Image(W25QXX_Handle_TypeDef* flash_data, uint8_t image_id)
{
	if(image_id > 53) image_id = 53;
	uint8_t index;
	uint16_t _index;
	uint8_t * read_arr = (uint8_t*) malloc(1920);
	uint16_t temp;
	uint32_t start_add = image_id * 153600;

	if(read_arr != NULL)
	{
		W25_FLASH_WAKEUP(flash_data); //Wake up flash

		tft_address_set(0,0,tft_W-1,tft_H-1);

		for(index = 0 ; index < 80 ; index++) //80*1920 = 153600
		{
			W25_FLASH_READ_DATA( flash_data, start_add + (index * 1920), read_arr, 1920);
			for(_index = 0 ; _index < 1920 ; _index += 2)
			{
				temp = (read_arr[_index + 1]<<8) | read_arr[_index];
				tft_write_data(temp);
			}
		}

		W25_FLASH_PWR_DOWN(flash_data); // Turn off power of flash to reduce power consumtion
		free(read_arr);
	}
}

void Display_Template(W25QXX_Handle_TypeDef * flash_data, uint8_t img_id)
{

	Display_Image(flash_data,img_id);

	tft_fill(HOUR_LOCAL_ROW-2,HOUR_LOCAL_COLUMN-2, HOUR_LOCAL_ROW-1,239, COLOR_BLACK);
	tft_fill(HOUR_LOCAL_ROW-2,HOUR_LOCAL_COLUMN-2, 319,HOUR_LOCAL_COLUMN-1, COLOR_BLACK);

	tft_fill(MONTH_LOCAL_ROW-2,15, MONTH_LOCAL_ROW-1,110, COLOR_BLACK);
	tft_fill(MONTH_LOCAL_ROW-2,15, 319,16, COLOR_BLACK);


}

void Play_Speaker(GPIO_TypeDef* Speaker_port, uint16_t Speaker_pin, uint8_t number_of_play)
{
	for(uint8_t index = 0; index < number_of_play; index++)
	{
		HAL_GPIO_WritePin(Speaker_port,Speaker_pin,GPIO_PIN_SET);
		osDelay(100);
		HAL_GPIO_WritePin(Speaker_port,Speaker_pin,GPIO_PIN_RESET);
		osDelay(100);
	}
}

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
