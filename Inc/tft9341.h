#ifndef TFT_9341_H
#define TFT_9341_H

/******************** TFT-320-QVT-9341 tft LIBRARY ***********************
 ********
 ********  Description	: This is library for TFT-9341 using STM32F407 uC
 ********  Interface		: 16bits Parallel
 ********  Author			: Xuan Thiep
 ********  Version			: 1.0
 ********  Date				: 31/5/2016
 ********  Including		: Define private macro, definece connection, declaring function prototype
 **************************************************************************/

/* Include Library */
#include "stm32f1xx_hal.h"



/* Define for connection */

#define 	RS_PORT		GPIOB
#define		RS_PIN		GPIO_PIN_10

#define		WR_PORT		GPIOB
#define 	WR_PIN		GPIO_PIN_5

#define		RD_PORT		GPIOB
#define		RD_PIN		GPIO_PIN_4

#define		CS_PORT		GPIOA
#define		CS_PIN		GPIO_PIN_11

#define		REST_PORT	GPIOA
#define		REST_PIN	GPIO_PIN_12

#define		D0_PORT		GPIOA
#define		D0_PIN		GPIO_PIN_0

#define		D1_PORT		GPIOA
#define		D1_PIN		GPIO_PIN_1

#define		D2_PORT		GPIOA
#define		D2_PIN		GPIO_PIN_2

#define		D3_PORT		GPIOA
#define		D3_PIN		GPIO_PIN_3

#define		D4_PORT		GPIOA
#define		D4_PIN		GPIO_PIN_4

#define		D5_PORT		GPIOA
#define		D5_PIN		GPIO_PIN_5

#define		D6_PORT		GPIOA
#define		D6_PIN		GPIO_PIN_6

#define		D7_PORT		GPIOA
#define		D7_PIN		GPIO_PIN_7

#define		D8_PORT		GPIOB
#define		D8_PIN		GPIO_PIN_0

#define		D9_PORT		GPIOB
#define		D9_PIN		GPIO_PIN_1

#define		D10_PORT	GPIOB
#define		D10_PIN		GPIO_PIN_2

#define		D11_PORT	GPIOB
#define		D11_PIN		GPIO_PIN_11

#define		D12_PORT	GPIOB
#define		D12_PIN		GPIO_PIN_12

#define		D13_PORT	GPIOA
#define		D13_PIN		GPIO_PIN_8

#define		D14_PORT	GPIOA
#define		D14_PIN		GPIO_PIN_9

#define		D15_PORT	GPIOA
#define		D15_PIN		GPIO_PIN_10



/* Declare Private Macro */

#define		pin_low(port,pin)			HAL_GPIO_WritePin(port,pin,GPIO_PIN_RESET)
#define		pin_high(port,pin)			HAL_GPIO_WritePin(port,pin,GPIO_PIN_SET)
#define		max(a,b)					(a>b)?a:b
#define		min(a,b)					(a<b)?a:b
#define		abs(a)						(a>=0)?a:(-a)

/* Declare TFT Dimension */
#define tft_W 240
#define tft_H 320

#define COLOR_WHITE         	 		0xFFFF
#define COLOR_BLACK         	 		0x0000
#define COLOR_BLUE         	 			0x001F
#define COLOR_BRED             			0XF81F
#define COLOR_GRED 			 			0XFFE0
#define COLOR_GBLUE			 			0X07FF
#define COLOR_RED           	 		0xF800
#define COLOR_VIOLET	       	 		0xF81F
#define COLOR_GREEN         	 		0x07E0
#define COLOR_CYAN          	 		0x7FFF
#define COLOR_YELLOW        	 		0xFFE0
#define COLOR_BROWN 			 		0XBC40
#define COLOR_BRRED 			 		0XFC07
#define COLOR_GRAY  			 		0X8430
#define COLOR_DARKBLUE      	 		0X01CF
#define COLOR_LIGHTBLUE      	 		0X7D7C
#define COLOR_GRAYBLUE       	 		0X5458
#define COLOR_LIGHTGREEN     	 		0X841F
#define COLOR_LGRAY 			 		0XC618
#define COLOR_LGRAYBLUE        			0XA651
#define COLOR_LBBLUE           			0X2B12

/* Define TFT String Mode */
#define TFT_STRING_MODE_NO_BACKGROUND		0x01
#define TFT_STRING_MODE_BACKGROUND			0x00

/* Declare Function Prototype */


/******************************** User Function ***********************************/
void tft_init(void);
void tft_clear(uint16_t color);
void tft_draw_line(uint16_t row1, uint16_t column1, uint16_t row2, uint16_t column2,uint16_t color);
void tft_draw_rectangle(uint16_t row1, uint16_t column1, uint16_t row2, uint16_t column2,uint16_t color);
void tft_draw_circle(uint16_t row,uint16_t column,uint16_t radian,uint16_t color);
void tft_fill(uint16_t row1,uint16_t column1,uint16_t row2,uint16_t column2,uint16_t color);
void tft_puts_image(const unsigned char* image_arr);
void tft_puts8x16(uint16_t row,uint16_t column,int8_t *string,uint8_t TFT_STRING_MODE);
void tft_puts14x24(uint16_t row,uint16_t column,int8_t *string,uint8_t TFT_STRING_MODE);
void tft_puts18x32(uint16_t row,uint16_t column,int8_t *string,uint8_t TFT_STRING_MODE);
void tft_puts26x48(uint16_t row,uint16_t column,int8_t *string,uint8_t TFT_STRING_MODE);
void tft_puts37x48(uint16_t row,uint16_t column,int8_t *string,uint8_t TFT_STRING_MODE);


/******************************** Private Function *********************************/
// Private Function do not use in user program

void tft_write_bus(uint8_t high_byte,uint8_t low_byte);
void tft_write_data(uint16_t data);
void tft_write_reg(uint16_t data);
void tft_write_reg_data(uint16_t reg,uint16_t data);
void tft_address_set(uint16_t x1,uint16_t y1, uint16_t x2, uint16_t y2);
void tft_draw_point(uint16_t x,uint16_t y,uint16_t color);
void tft_draw_point_big(uint16_t x,uint16_t y,uint16_t color);
void tft_write_data8(uint8_t high_byte,uint8_t low_byte);
void tft_putchar(uint16_t x,uint16_t y,uint8_t num,uint8_t mode);
void tft_putchar14x24(uint16_t x,uint16_t y,uint8_t data,uint8_t mode);
void tft_putchar18x32(uint16_t x,uint16_t y,uint8_t data,uint8_t mode);
void tft_putchar26x48(uint16_t x,uint16_t y,uint8_t data,uint8_t mode);
void tft_putchar37x48(uint16_t x,uint16_t y,uint8_t data,uint8_t mode);
#endif

