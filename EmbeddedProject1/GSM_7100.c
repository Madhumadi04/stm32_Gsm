#include "GSM_7100.h"
extern UART_HandleTypeDef huart6;
Gsm_t	Gsm;
bool Gsm_SendRaw(uint8_t *data, uint16_t len)
{
	if (len <= _GSM_TX_SIZE)
	{
		memcpy(Gsm.TxBuffer, data, len);
		if (HAL_UART_Transmit(&_GSM_USART, Gsm.TxBuffer, len, 100) == HAL_OK)
	
			return true;
		else
			return false;
	}
	else
		return false;
}


bool	Gsm_SendString(char *data)
{
	return Gsm_SendRaw((uint8_t*)data, strlen(data));
}