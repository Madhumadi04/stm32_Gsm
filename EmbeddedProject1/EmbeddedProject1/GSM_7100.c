#include "GSM_7100.h"
#include "UARTXinOu.h"

extern UART_HandleTypeDef huart6;


bool localdata;
extern rx2_EOF_rcv;
//extern Mainrxbuf;
//unsigned char conn_frame[] = { 16, 18, 0, 4, 77, 81, 84, 84, 4, 2, 0, 60, 0, 6, 65, 66, 67, 68, 69, 69, 26 };
#define		SIMPIN			"1134\r"
//unsigned char pub_frame[] = {0x30,0x0A,0x00,0x03,0x41,0x41,0x41,0x68,0x65,0x66,0x6C,0x6F,0x1A};
Gsm_t	Gsm;

bool init_quectel()
{
	
	COMSendStr("AT+IPR=115200\r");
	HAL_Delay(50);
	flush_buf();
	COMSendStr("AT&W\r");
	HAL_Delay(50);
	flush_buf();
	COMSendByte(0x1A);
	HAL_Delay(50);
	COMSendStr("ATE0\r");
	HAL_Delay(50);
	flush_buf();
	Test_AT_send_TEST();
	HAL_Delay(50);
	Test_AT_send_TEST();

	Configure_TCP() ;
	close_socket();
	
	Open_Port();
	
}

bool Gsm_SendRaw(uint8_t *data, uint16_t len)
{
	if (len <= _GSM_TX_SIZE)
	{
		memcpy(Gsm.TxBuffer, data, len);
		if (HAL_UART_Transmit(&huart6, Gsm.TxBuffer, len, 100) == HAL_OK)
	
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

bool flush_buf()
{
	if (rx2_EOF_rcv)
	{
		COMReadStr(main_rx_buf, MAIN_RX_BUF_LEN);
		
	}
	memset(main_rx_buf, 0, MAIN_RX_BUF_LEN);
}

bool Test_AT_send_TEST()
{
	uint32_t Timeout_int = 25000;
	uint32_t Tickstart_int = 0U;
	Tickstart_int = HAL_GetTick();
	unsigned char localdata;    // non zero value
	
while(gsm_timeout(Tickstart_int, Timeout_int))
	{
		
		COMSendStr("AT\r");
		HAL_Delay(100);
		if (rx2_EOF_rcv)
		{
			COMReadStr(main_rx_buf, MAIN_RX_BUF_LEN);
			localdata = strcmp("\r\nOK\r\n", main_rx_buf);
			rx2_EOF_rcv = false;
			flush_buf();
			if (!localdata)
			{
				break;
			}
		}
	}
}


bool gsm_timeout(uint32_t tickstart, uint32_t timeout)
{
	if ((timeout == 0U) || ((HAL_GetTick() - tickstart) > timeout))
	{
		Gsm_SendString("time out");		
		rx2_EOF_rcv = false;
		return false;
	}
	else
	{
		return true;
	}
}
bool Test_AT_send()
{
	unsigned char localdata;  // non zero value

	COMSendStr("AT\r");
	HAL_Delay(100);
	if (rx2_EOF_rcv)
	{
		COMReadStr(main_rx_buf, MAIN_RX_BUF_LEN);
		localdata = strcmp("\r\nOK\r\n", main_rx_buf);
		Gsm_SendString(main_rx_buf);
		localdata = !localdata;
		rx2_EOF_rcv =false;
		flush_buf();
		if (localdata)
		{
			
			HAL_Delay(100);
			return true; 
			
		}
		else 
		{
			
		}
				
	}
	//flush_buf();
	return false;
}






bool close_socket()
{
	uint32_t Timeout_int = 25000;
	uint32_t Tickstart_int = 0U;
	Tickstart_int = HAL_GetTick();
	unsigned char localdata = 1;    // non zero value

while(gsm_timeout(Tickstart_int, Timeout_int))
	{
		
		COMSendStr("AT+QICLOSE=0\r");
		HAL_Delay(1200);
		if (rx2_EOF_rcv)
		{
			COMReadStr(main_rx_buf, MAIN_RX_BUF_LEN);
			localdata = strcmp("\r\nOK\r\n", main_rx_buf);
			rx2_EOF_rcv = false;
			flush_buf();
			if (!localdata)
			{
				break;
			}
		}
	}
	
}



bool Configure_TCP()
{
	uint32_t Timeout_int = 25000;
	uint32_t Tickstart_int = 0U;
	Tickstart_int = HAL_GetTick();
	unsigned char localdata;     // non zero value
	
while(gsm_timeout(Tickstart_int, Timeout_int))
	{
		
		COMSendStr("AT+QICSGP=1,1,\"internet.t-mobile.cz\",\"\",\"\",1\r");
		HAL_Delay(300);
		if (rx2_EOF_rcv)
		{
			COMReadStr(main_rx_buf, MAIN_RX_BUF_LEN);
			localdata = strcmp("\r\nOK\r\n", main_rx_buf);
			rx2_EOF_rcv = false;
			flush_buf();
			if (!localdata)
			{
				break;
			}
		}
	}
	
	
	
}



bool connect(unsigned char *clientID)
{
	unsigned char protocol_name[] = "MQTT";
	COMSendStr("AT+QISEND=0\r");
	int len = 8 + strlen(protocol_name) + strlen(clientID);
	HAL_Delay(10);	
	COMSendByte(0x10);
	HAL_Delay(5);
	COMSendByte(len);
	HAL_Delay(5);
	COMSendByte(0x00);
	HAL_Delay(5);
	COMSendByte(strlen(protocol_name));
	HAL_Delay(5);
	for (int i = 0; i < strlen(protocol_name); i++) 
	{
		
		COMSendByte(protocol_name[i]);
		HAL_Delay(5);
	} 
	COMSendByte(0x04);
	HAL_Delay(5);
	COMSendByte(0x02);
	HAL_Delay(5);
	COMSendByte(0x00);
	HAL_Delay(5);
	COMSendByte(0x3C);
	HAL_Delay(5);
	COMSendByte(0x00);
	HAL_Delay(5);
	COMSendByte(strlen(clientID));
	HAL_Delay(5);
	for (int i = 0; i < strlen(clientID); i++) 
	{
		
		COMSendByte(clientID[i]);
		HAL_Delay(5);
	} 
	COMSendByte(0x1A);
	
	HAL_Delay(100);
	
}

bool Publishpkt_num(unsigned char *topic, char value)
{
	
	COMSendStr("AT+QISEND=0\r");
	HAL_Delay(300);
	
	char v[4];
	
	itoa(value,v,10);
	unsigned char *mess;
	unsigned char tsize = strlen(topic);
	unsigned char msize = strlen(v);
	unsigned char len = 2 + tsize + msize;
	unsigned char N = tsize + msize;

	mess = malloc((N + 1) * sizeof(char));
	for (int i = 0; i < tsize; i++) {
		mess[i] = topic[i];
	}
	for (int i = 0; i < msize; i++) {
		mess[tsize + i] = v[i];
	}
	mess[N] = 0X00;
	COMSendByte(0x30);
	HAL_Delay(100);
	COMSendByte(len);
	HAL_Delay(100);
	COMSendByte(0x00);
	HAL_Delay(100);
	COMSendByte(tsize);
	HAL_Delay(100);
	for (int i = 0; i < N; i++) {
		
		COMSendByte(mess[i]);
		HAL_Delay(100);
	} 
	COMSendByte(0x1A);
	HAL_Delay(100);
	
}

bool Publishpkt(unsigned char *topic, unsigned char *message)
{
	COMSendStr("AT+QISEND=0\r");
	HAL_Delay(100);
	
	unsigned char *mess;
	unsigned char tsize = strlen(topic);
	unsigned char msize = strlen(message);
	unsigned char len = 2 + tsize + msize;
	unsigned char N = tsize + msize;

	mess=malloc((N+1) * sizeof(char));
	for (int i = 0; i < tsize; i++) {
		mess[i] = topic[i];
	}
	for (int i = 0; i < msize; i++) {
		mess[tsize+i] = message[i];
	}
	mess[N] = 0X00;
	COMSendByte(0x30);
	HAL_Delay(10);
	COMSendByte(len);
	HAL_Delay(10);
	COMSendByte(0x00);
	HAL_Delay(10);
	COMSendByte(tsize);
	HAL_Delay(10);
	for (int i = 0; i < N; i++) {
		
		COMSendByte(mess[i]);
		HAL_Delay(10);
	} 
	COMSendByte(0x1A);
	HAL_Delay(120);
	

}


bool Mqtt_subscribe(unsigned char *topic)
{
	Gsm_SendString("AT+QISEND=0\r");
	HAL_Delay(100);
	unsigned char *mess;
	unsigned char tsize = strlen(topic);
	unsigned char len = 5 + tsize;
	char str[80];
	Gsm_senddata(&huart6, 0x82);
	HAL_Delay(50);
	Gsm_senddata(&huart6, len);
	HAL_Delay(50);
	Gsm_senddata(&huart6, 0x00);
	HAL_Delay(50);
	Gsm_senddata(&huart6, 0x0A);
	HAL_Delay(50);
	Gsm_senddata(&huart6, 0x00);
	HAL_Delay(50);
	Gsm_senddata(&huart6, tsize);
	HAL_Delay(50);
	for (int i = 0; i < tsize; i++) {
		
		Gsm_senddata(&huart6, topic[i]);
		HAL_Delay(50);
	} 
	Gsm_senddata(&huart6, 0x00);
	HAL_Delay(50);
	Gsm_senddata(&huart6, 0x1A);
}
bool Open_Port()
{
	uint32_t Timeout_int = 25000;
	uint32_t Tickstart_int = 0U;
	Tickstart_int = HAL_GetTick();
	unsigned char localdata = 1;      // non zero value

while(gsm_timeout(Tickstart_int, Timeout_int))
	{
		
		COMSendStr("AT+QIOPEN=1,0,\"TCP\",\"4tech.horcica.cz\",1883,0,1\r");
		HAL_Delay(1200);
		if (rx2_EOF_rcv)
		{
			COMReadStr(main_rx_buf, MAIN_RX_BUF_LEN);
			localdata = strcmp("\r\nOK\r\n\r\n+QIOPEN: 0,0\r\n", main_rx_buf);
			rx2_EOF_rcv = false;
			flush_buf();
			if (!localdata)
			{
				break;
			}
		}
	}

}