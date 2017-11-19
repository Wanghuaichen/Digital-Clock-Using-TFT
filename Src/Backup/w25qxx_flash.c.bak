#include 	"w25qxx_flash.h"



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
HAL_StatusTypeDef W25_FLASH_READ_ID(W25QXX_Handle_TypeDef* w25qxx_handle,uint8_t* receive_data)
{
	uint8_t send_data[]={ W25_INS_MANUFACTURER_DEVICE_ID,0,0,0,0,0 };
	uint8_t temp_data[6];
	HAL_StatusTypeDef stt = HAL_ERROR;

	if((w25qxx_handle != NULL) && (receive_data != NULL))
	{
		HAL_GPIO_WritePin( w25qxx_handle->cs_port, w25qxx_handle->cs_pin, GPIO_PIN_RESET );

		stt = HAL_SPI_TransmitReceive( w25qxx_handle->hspix, send_data, temp_data,6,1000 );

		HAL_GPIO_WritePin( w25qxx_handle->cs_port, w25qxx_handle->cs_pin, GPIO_PIN_SET );

		receive_data[0]= temp_data[4];
		receive_data[1]= temp_data[5];
	}
	return stt;

}



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
HAL_StatusTypeDef W25_FLASH_READ_SR(W25QXX_Handle_TypeDef* w25qxx_handle,uint8_t* receive_data)
{
	uint8_t send_data[]={ W25_INS_READ_STATUS_REG,0 };
	uint8_t temp_data[2];
	HAL_StatusTypeDef stt = HAL_ERROR;

	if((w25qxx_handle != NULL) && (receive_data != NULL))
	{
		HAL_GPIO_WritePin( w25qxx_handle->cs_port, w25qxx_handle->cs_pin, GPIO_PIN_RESET );

		stt = HAL_SPI_TransmitReceive( w25qxx_handle->hspix, send_data, temp_data,2,1000 );

		HAL_GPIO_WritePin( w25qxx_handle->cs_port, w25qxx_handle->cs_pin, GPIO_PIN_SET );

		*receive_data= temp_data[1];

	}
	return stt;

}




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
static HAL_StatusTypeDef W25_FLASH_WAIT_BUSY(W25QXX_Handle_TypeDef* w25qxx_handle )
{
	uint8_t temp_data;
	HAL_StatusTypeDef stt = HAL_ERROR;	
	do
	{
		stt = W25_FLASH_READ_SR( w25qxx_handle, &temp_data);
	}
	while ( temp_data & W25_STATUS_BUSY );

	return stt;
}





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
static HAL_StatusTypeDef W25_FLASH_WRITE_EN( W25QXX_Handle_TypeDef* w25qxx_handle )
{
	uint8_t send_data = W25_INS_WRITE_ENABLE;
	uint8_t temp_data;
	HAL_StatusTypeDef stt = HAL_ERROR;	

	if(w25qxx_handle != NULL)
	{
		HAL_GPIO_WritePin( w25qxx_handle->cs_port, w25qxx_handle->cs_pin, GPIO_PIN_RESET );

		stt = HAL_SPI_Transmit( w25qxx_handle->hspix, &send_data,1,1000 );

		HAL_GPIO_WritePin( w25qxx_handle->cs_port, w25qxx_handle->cs_pin, GPIO_PIN_SET );

		do
		{
			stt = W25_FLASH_READ_SR( w25qxx_handle, &temp_data);
		}
		while ( !(temp_data & W25_STATUS_WRITE_ENABLE) );
	}

	return stt;
}





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
static HAL_StatusTypeDef W25_FLASH_WRITE_DIS( W25QXX_Handle_TypeDef* w25qxx_handle )
{
	uint8_t send_data = W25_INS_WRITE_DISABLE;
	uint8_t temp_data;
	HAL_StatusTypeDef stt = HAL_ERROR;	

	if(w25qxx_handle != NULL)
	{
		HAL_GPIO_WritePin( w25qxx_handle->cs_port, w25qxx_handle->cs_pin, GPIO_PIN_RESET );

		stt = HAL_SPI_Transmit( w25qxx_handle->hspix, &send_data,1,1000 );

		HAL_GPIO_WritePin( w25qxx_handle->cs_port, w25qxx_handle->cs_pin, GPIO_PIN_SET );

		do
		{
			stt = W25_FLASH_READ_SR( w25qxx_handle, &temp_data);
		}
		while ( temp_data & W25_STATUS_WRITE_ENABLE );
	}

	return stt;
}

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
HAL_StatusTypeDef W25_FLASH_WRITE_SR(W25QXX_Handle_TypeDef* w25qxx_handle,uint8_t* send_data)
{

	uint8_t _send_data[] = { W25_INS_WRITE_STATUS_REG, *send_data };
	HAL_StatusTypeDef stt = HAL_ERROR;	

	W25_FLASH_WRITE_EN(w25qxx_handle);

	HAL_GPIO_WritePin( w25qxx_handle->cs_port, w25qxx_handle->cs_pin, GPIO_PIN_RESET );

	stt = HAL_SPI_Transmit( w25qxx_handle->hspix, _send_data,2,1000 );

	HAL_GPIO_WritePin( w25qxx_handle->cs_port, w25qxx_handle->cs_pin, GPIO_PIN_SET );

	stt = W25_FLASH_WAIT_BUSY( w25qxx_handle );

	return stt;
}




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
HAL_StatusTypeDef W25_FLASH_ERASE_ALL( W25QXX_Handle_TypeDef* w25qxx_handle )
{

	uint8_t send_data = W25_INS_CHIP_ERASE;
	HAL_StatusTypeDef stt = HAL_ERROR;	

	W25_FLASH_WRITE_EN(w25qxx_handle);

	HAL_GPIO_WritePin( w25qxx_handle->cs_port, w25qxx_handle->cs_pin, GPIO_PIN_RESET );

	stt = HAL_SPI_Transmit( w25qxx_handle->hspix, &send_data,1,1000 );

	HAL_GPIO_WritePin( w25qxx_handle->cs_port, w25qxx_handle->cs_pin, GPIO_PIN_SET );

	stt = W25_FLASH_WAIT_BUSY(w25qxx_handle);

	W25_FLASH_WRITE_DIS(w25qxx_handle);
	return stt;
}



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
HAL_StatusTypeDef W25_FLASH_READ_DATA(W25QXX_Handle_TypeDef* w25qxx_handle, uint32_t start_address, uint8_t* receive_data, uint32_t size)
{
	uint8_t send_data[] = { W25_INS_READ_DATA, (uint8_t)(start_address >> 16), (uint8_t)(start_address >> 8), (uint8_t)(start_address & 0xFF) };
	HAL_StatusTypeDef stt = HAL_ERROR;	

	HAL_GPIO_WritePin( w25qxx_handle->cs_port, w25qxx_handle->cs_pin, GPIO_PIN_RESET );

	stt = HAL_SPI_Transmit( w25qxx_handle->hspix, send_data,4,1000 );

	stt = HAL_SPI_Receive( w25qxx_handle->hspix, receive_data, size,1000 );

	HAL_GPIO_WritePin( w25qxx_handle->cs_port, w25qxx_handle->cs_pin, GPIO_PIN_SET );

	return stt;
}





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
HAL_StatusTypeDef W25_FLASH_WRITE_DATA(W25QXX_Handle_TypeDef* w25qxx_handle, uint32_t start_address, uint8_t* send_data, uint32_t size)
{
	uint16_t number_of_page = (uint16_t) (size / W25_SIZE_PAGE);
	uint16_t index =0;
	uint32_t _start_address;
	uint8_t _send_ins[] = { W25_INS_PAGE_PROGRAM, 0x00, 0x00, 0x00 };
	HAL_StatusTypeDef stt = HAL_ERROR;	


	if (number_of_page) /* Page program */
	{
		for( index = 0 ; index < number_of_page ; index ++)
		{
			_start_address = start_address + (index * W25_SIZE_PAGE);
			_send_ins [1] = (uint8_t)(_start_address >> 16);
			_send_ins [2] = (uint8_t)(_start_address >> 8);
			_send_ins [3] = (uint8_t)(_start_address & 0xFF);

			W25_FLASH_WRITE_EN(w25qxx_handle);

			HAL_GPIO_WritePin( w25qxx_handle->cs_port, w25qxx_handle->cs_pin, GPIO_PIN_RESET );

			stt = HAL_SPI_Transmit( w25qxx_handle->hspix, _send_ins,4,1000 );
			stt = HAL_SPI_Transmit( w25qxx_handle->hspix, send_data + (index * W25_SIZE_PAGE) , W25_SIZE_PAGE, 5000 );

			HAL_GPIO_WritePin( w25qxx_handle->cs_port, w25qxx_handle->cs_pin, GPIO_PIN_SET );

			W25_FLASH_WAIT_BUSY(w25qxx_handle);

			W25_FLASH_WRITE_DIS(w25qxx_handle);
		}

	}

	number_of_page = size % W25_SIZE_PAGE;

	if(number_of_page)
	{
		_start_address = start_address + ( index * W25_SIZE_PAGE);

		_send_ins [1] = (uint8_t)(_start_address >> 16);
		_send_ins [2] = (uint8_t)(_start_address >> 8);
		_send_ins [3] = (uint8_t)(_start_address & 0xFF);

		W25_FLASH_WRITE_EN(w25qxx_handle);

		HAL_GPIO_WritePin( w25qxx_handle->cs_port, w25qxx_handle->cs_pin, GPIO_PIN_RESET );

		stt = HAL_SPI_Transmit( w25qxx_handle->hspix, _send_ins,4,1000 );
		stt = HAL_SPI_Transmit( w25qxx_handle->hspix, send_data + ( index * W25_SIZE_PAGE) , number_of_page, 5000 );

		HAL_GPIO_WritePin( w25qxx_handle->cs_port, w25qxx_handle->cs_pin, GPIO_PIN_SET );

		W25_FLASH_WAIT_BUSY(w25qxx_handle);

		W25_FLASH_WRITE_DIS(w25qxx_handle);
	}

	return stt;
}





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
HAL_StatusTypeDef W25_FLASH_PWR_DOWN( W25QXX_Handle_TypeDef* w25qxx_handle )
{
	HAL_StatusTypeDef stt = HAL_ERROR;
	uint8_t send_data = W25_INS_POWER_DOWN;

	HAL_GPIO_WritePin( w25qxx_handle->cs_port, w25qxx_handle->cs_pin, GPIO_PIN_RESET );

	stt = HAL_SPI_Transmit( w25qxx_handle->hspix, &send_data,1,1000 );

	HAL_GPIO_WritePin( w25qxx_handle->cs_port, w25qxx_handle->cs_pin, GPIO_PIN_SET );

	return stt;
}





/********************************************************************************************************
 ********************************************************************************************************
 *	@Description	:	Wake up chip from Power down mode 
 *
 *	@Param			:	+ w25qxx_handle - this is pointer W25QXX_Handle_TypeDef 
 *
 *  @Return			:	HAL status
 *
 *	@Note			:	Need delay 3uS after wake up to enter normal mode
 *
 ********************************************************************************************************
 ********************************************************************************************************/
HAL_StatusTypeDef W25_FLASH_WAKEUP( W25QXX_Handle_TypeDef* w25qxx_handle )
{
	HAL_StatusTypeDef stt = HAL_ERROR;
	uint8_t send_data = W25_INS_RELEASH_POWER_DOWN;

	HAL_GPIO_WritePin( w25qxx_handle->cs_port, w25qxx_handle->cs_pin, GPIO_PIN_RESET );

	stt = HAL_SPI_Transmit( w25qxx_handle->hspix, &send_data,1,1000 );

	HAL_GPIO_WritePin( w25qxx_handle->cs_port, w25qxx_handle->cs_pin, GPIO_PIN_SET );

	return stt;
}







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
HAL_StatusTypeDef W25_FLASH_ERASE_KB( W25QXX_Handle_TypeDef* w25qxx_handle, uint32_t start_address,W25_INS_ERASE_KB  w25_ins_erase_kb )
{
	HAL_StatusTypeDef stt = HAL_ERROR;
	uint8_t _send_ins[] = { w25_ins_erase_kb, (uint8_t)(start_address >> 16), (uint8_t)(start_address >> 8), (uint8_t)(start_address & 0xFF) };


	W25_FLASH_WRITE_EN(w25qxx_handle);

	HAL_GPIO_WritePin( w25qxx_handle->cs_port, w25qxx_handle->cs_pin, GPIO_PIN_RESET );

	stt = HAL_SPI_Transmit( w25qxx_handle->hspix, _send_ins,4,5000 );

	HAL_GPIO_WritePin( w25qxx_handle->cs_port, w25qxx_handle->cs_pin, GPIO_PIN_SET );

	W25_FLASH_WAIT_BUSY(w25qxx_handle);

	W25_FLASH_WRITE_DIS(w25qxx_handle);
	return stt;
}
