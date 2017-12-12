#pragma once
#include "main.h"
#include "stm32f4xx_hal.h"
#include "stdbool.h"
#define		_GSM_USART									huart6
#define		_GSM_TX_SIZE								256


typedef struct
{

	uint8_t 								TxBuffer[_GSM_TX_SIZE];
	
}Gsm_t;

bool Gsm_SendRaw(uint8_t *, uint16_t);
bool	Gsm_SendString(char *);