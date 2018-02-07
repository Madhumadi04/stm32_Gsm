#pragma once
#include "main.h"
#include "stm32f4xx_hal.h"
#include "stdbool.h"
#define		_GSM_USART									huart6
#define		_GSM_TX_SIZE								256
#define		Domain_name									"\"4tech.horcica.cz\","
#define		Portno										"1883,"
#define	 MAIN_RX_BUF_LEN									55
#define	 main_rx_buf									&Mainrxbuf
#define _AT_ "AT\r"
#define _CLOSEPORT_ "AT+QICLOSE=0\r"
#define _TCPCONFIG_ "AT+QICSGP=1,1,\"internet.t-mobile.cz\",\"\",\"\",1\r"
#define _OPENPORT_ "AT+QIOPEN=1,0,\"TCP\",\"playground.g4t.io\",1884,0,1\r"
#define _PUBLISH_ "Publish error"

char Mainrxbuf[MAIN_RX_BUF_LEN];
typedef struct
{

	uint8_t 								TxBuffer[_GSM_TX_SIZE];
	
}Gsm_t;

bool init_quectel();
bool Gsm_SendRaw(uint8_t *, uint16_t);
bool	Gsm_SendString(char *);
bool flush_buf();
bool Test_AT_send();
char Test_AT_send_TEST();
char close_socket();
char Configure_TCP();
char Open_Port();
bool conn_pkt();
bool pub_pkt();
bool connect(unsigned char *clientID);
char Publishpkt(unsigned char i);
bool Mqtt_subscribe(unsigned char *topic);
bool gsm_timeout(unsigned char *cmd, uint32_t tickstart, uint32_t timeout);
bool ATCommand_Response(unsigned char *cmd, uint32_t timeout);