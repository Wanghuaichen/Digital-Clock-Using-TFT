#ifndef		__W25QXX_FLASH__H
#define		__W25QXX_FLASH__H




/********************************************************************************************************
 ********************************************************************************************************
 *	Description		:	This is library for W25QXX flash memory chip base on HAL lib
 *	Author			:	Thiepnx
 *	Date			:	21/09/2017
 *	Version			:	1.0
 ********************************************************************************************************
 ********************************************************************************************************/


/* Include HAL Library specific for STM32 MCU (change this include if need) */ 
#include "stm32f1xx_hal.h"


/* Define W25QXX_Handle_TypeDef */
typedef struct
{
	SPI_HandleTypeDef*	hspix;
	GPIO_TypeDef*		cs_port;
	uint16_t			cs_pin;

}W25QXX_Handle_TypeDef;			

#define FLASH_TIMEOUT		100

typedef enum
{
	W25Q32 													= 0xEF15,
	W25Q64 													= 0xEF16
}FLASH_ID_TYPEDEF;


/* Define command (op code) for W25QXX Flash (Do not change this define)*/
typedef enum
{
	W25_INS_WRITE_ENABLE 										=	0x06,
	W25_INS_WRITE_DISABLE 										= 	0x04,
	W25_INS_READ_STATUS_REG										=	0x05,
	W25_INS_WRITE_STATUS_REG									=	0x01,
	W25_INS_READ_DATA											=	0x03,
	W25_INS_FAST_READ_DATA										=	0x0B,
	W25_INS_FAST_READ_DUAL_OUTPUT								=	0x3B,
	W25_INS_FAST_READ_QUAD_OUTPUT								=	0x6B,
	W25_INS_PAGE_PROGRAM										=	0x02,
	W25_INS_CHIP_ERASE											=	0xC7,
	W25_INS_POWER_DOWN											=	0xB9,
	W25_INS_RELEASH_POWER_DOWN									=	0xAB,
	W25_INS_MANUFACTURER_DEVICE_ID								=	0x90,
	W25_INS_JEDEC_DEVICE_ID										=	0x9F
}FLASH_INSTRUCTION_TYPEDEF;


typedef enum
{
	W25_INS_SECTOR_4K_ERASE										=	0x20,
	W25_INS_SECTOR_32K_ERASE									= 	0x52,
	W25_INS_SECTOR_64K_ERASE									=	0xD8
}W25_INS_ERASE_KB;

/* Define Status of Flash */ 
typedef enum
{
	W25_STATUS_BUSY												=	0x01,
	W25_STATUS_WRITE_ENABLE										=	0x02
}FLASH_STATUS_TYPEDEF;


typedef enum
{
	W25_SIZE_PAGE												= 	256,
	W25_SIZE_SECTOR												= 	4096
}FLASH_SIZE_TYPEDEF;



typedef enum 
{
	w25qxx_State_TimeOut	= -2,
	w25qxx_State_Err		= -1,
	w25qxx_State_OK			= 0,
	w25qxx_State_Busy		= 1,
}FLASH_Status_t;



/********************************************************************************************************
 ********************************************************************************************************
 *	@Description	:	Read Manufacture Device ID from W25QXX chip. Using this function to test 
 *						connection between mcu and flash chip.
 *
 *	@Param			:	+ w25qxx_handle - this is pointer W25QXX_Handle_TypeDef 
 *						+ receive_data - this is array to store Manufacture ID
 *
 *  @Return			:	HAL status
 *
 *	@Note			:	Manufacture ID include 3 bytes, first byte = 0xEF, second byte = 0x15 if Device is
 *						W25Q32FV, second byte = 0x16 if Device is W25Q64FV
 *
 ********************************************************************************************************
 ********************************************************************************************************/
HAL_StatusTypeDef W25_FLASH_READ_ID(W25QXX_Handle_TypeDef* w25qxx_handle,uint8_t* receive_data);





/********************************************************************************************************
 ********************************************************************************************************
 *	@Description	:	Read-Status-Register 
 *
 *	@Param			:	+ w25qxx_handle - this is pointer W25QXX_Handle_TypeDef 
 *						+ receive_data - this is variable to store status register value
 *
 *  @Return			:	HAL status
 *
 *	@Note			:	None
 *
 ********************************************************************************************************
 ********************************************************************************************************/
HAL_StatusTypeDef W25_FLASH_READ_SR(W25QXX_Handle_TypeDef* w25qxx_handle,uint8_t* receive_data);





/********************************************************************************************************
 ********************************************************************************************************
 *	@Description	:	Wait - if chip is busy
 *
 *	@Param			:	+ w25qxx_handle - this is pointer W25QXX_Handle_TypeDef 
 *
 *  @Return			:	HAL status
 *
 *	@Note			:	None
 *
 ********************************************************************************************************
 ********************************************************************************************************/
//static HAL_StatusTypeDef W25_FLASH_WAIT_BUSY(W25QXX_Handle_TypeDef* w25qxx_handle );





/********************************************************************************************************
 ********************************************************************************************************
 *	@Description	:	Write-Enable 
 *
 *	@Param			:	+ w25qxx_handle - this is pointer W25QXX_Handle_TypeDef 
 *
 *  @Return			:	HAL status
 *
 *	@Note			:	None
 *
 ********************************************************************************************************
 ********************************************************************************************************/
//static HAL_StatusTypeDef W25_FLASH_WRITE_EN( W25QXX_Handle_TypeDef* w25qxx_handle );




/********************************************************************************************************
 ********************************************************************************************************
 *	@Description	:	Write-Disable 
 *
 *	@Param			:	+ w25qxx_handle - this is pointer W25QXX_Handle_TypeDef 
 *
 *  @Return			:	HAL status
 *
 *	@Note			:	None
 *
 ********************************************************************************************************
 ********************************************************************************************************/
//static HAL_StatusTypeDef W25_FLASH_WRITE_DIS( W25QXX_Handle_TypeDef* w25qxx_handle );




/********************************************************************************************************
 ********************************************************************************************************
 *	@Description	:	Write-Status-Register 
 *
 *	@Param			:	+ w25qxx_handle - this is pointer W25QXX_Handle_TypeDef 
 *						+ send_data - this is variable to write to status register 
 *
 *  @Return			:	HAL status
 *
 *	@Note			:	None
 *
 ********************************************************************************************************
 ********************************************************************************************************/
HAL_StatusTypeDef W25_FLASH_WRITE_SR(W25QXX_Handle_TypeDef* w25qxx_handle,uint8_t* send_data);




/********************************************************************************************************
 ********************************************************************************************************
 *	@Description	:	Erase all of flash 
 *
 *	@Param			:	+ w25qxx_handle - this is pointer W25QXX_Handle_TypeDef 
 *
 *  @Return			:	HAL status
 *
 *	@Note			:	None
 *
 ********************************************************************************************************
 ********************************************************************************************************/
HAL_StatusTypeDef W25_FLASH_ERASE_ALL( W25QXX_Handle_TypeDef* w25qxx_handle );




/********************************************************************************************************
 ********************************************************************************************************
 *	@Description	:	Read-Data from address of the flash 
 *
 *	@Param			:	+ w25qxx_handle - this is pointer W25QXX_Handle_TypeDef 
 *						+ start_address - begin address which data read out
 *						+ receive_data 	- this is variable to store data value
 *						+ size      	- this is number of data want to read	
 *
 *  @Return			:	HAL status
 *
 *	@Note			:	None
 *
 ********************************************************************************************************
 ********************************************************************************************************/
HAL_StatusTypeDef W25_FLASH_READ_DATA(W25QXX_Handle_TypeDef* w25qxx_handle, uint32_t start_address, uint8_t* receive_data, uint32_t size);





/********************************************************************************************************
 ********************************************************************************************************
 *	@Description	:	Write-Data to address of the flash 
 *
 *	@Param			:	+ w25qxx_handle - this is pointer W25QXX_Handle_TypeDef 
 *						+ start_address - begin address to write
 *						+ send_data 	- this is variable to store data to send
 *						+ size      	- this is number of data want to write	
 *
 *  @Return			:	HAL status
 *
 *	@Note			:	None
 *
 ********************************************************************************************************
 ********************************************************************************************************/
HAL_StatusTypeDef W25_FLASH_WRITE_DATA(W25QXX_Handle_TypeDef* w25qxx_handle, uint32_t start_address, uint8_t* send_data, uint32_t size);





/********************************************************************************************************
 ********************************************************************************************************
 *	@Description	:	Power down chip to save energy (20uA)
 *
 *	@Param			:	+ w25qxx_handle - this is pointer W25QXX_Handle_TypeDef 
 *
 *  @Return			:	HAL status
 *
 *	@Note			:	None
 *
 ********************************************************************************************************
 ********************************************************************************************************/
HAL_StatusTypeDef W25_FLASH_PWR_DOWN( W25QXX_Handle_TypeDef* w25qxx_handle );




/********************************************************************************************************
 ********************************************************************************************************
 *	@Description	:	Wake up chip from Power down mode 
 *
 *	@Param			:	+ w25qxx_handle - this is pointer W25QXX_Handle_TypeDef 
 *
 *  @Return			:	HAL status
 *
 *	@Note			:	None
 *
 ********************************************************************************************************
 ********************************************************************************************************/
HAL_StatusTypeDef W25_FLASH_WAKEUP( W25QXX_Handle_TypeDef* w25qxx_handle );




/********************************************************************************************************
 ********************************************************************************************************
 *	@Description	:	The Sector Erase instruction sets all memory within a specified sector (4K-bytes),
 *						a specified block (32K-bytes) or (64K-bytes)to the erased state of all depend on
 *						w25_ins_erase_kb parameter
 *
 *	@Param			:	+ w25qxx_handle - this is pointer W25QXX_Handle_TypeDef
 *
 *						+ start_address - begin address to erase
 *
 *						+ w25_ins_erase_kb 	- specifie the volume erase.
 *  @Return			:	HAL status
 *
 *	@Note			:	None
 *
 ********************************************************************************************************
 ********************************************************************************************************/
HAL_StatusTypeDef W25_FLASH_ERASE_KB( W25QXX_Handle_TypeDef* w25qxx_handle, uint32_t start_address,W25_INS_ERASE_KB  w25_ins_erase_kb );
#endif


