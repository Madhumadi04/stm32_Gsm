#pragma once
//#include "main.h"
#include "stm32f4xx_hal.h"
#include "stdbool.h"
#define		_GSM_USART									huart6
#define		_GSM_TX_SIZE								256
#define		Domain_name									"\"4tech.horcica.cz\","
#define		Portno										"1883,"

typedef struct
{

	uint8_t 								TxBuffer[_GSM_TX_SIZE];
	
}Gsm_t;

bool init_quectel();
bool Gsm_SendRaw(uint8_t *, uint16_t);
bool	Gsm_SendString(char *);
bool Test_AT_send();
bool close_socket();
bool unlock_sim();
bool Configure_TCP();
bool Open_Port();
bool conn_pkt();
bool pub_pkt();
bool connect(unsigned char *clientID);
bool Publishpkt(unsigned char *topic, unsigned char *message);
bool Mqtt_subscribe(unsigned char *topic);