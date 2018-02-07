#include "GSM_7100.h"
#include "UARTXinOu.h"
#include "stdlib.h"
#include "string.h"

extern UART_HandleTypeDef huart6;

char volatile error;
extern mqtt_tab mqtt[];
unsigned char AT[] = "AT\r";

extern rx2_EOF_rcv;
char b;
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
	b=Test_AT_send_TEST();
	HAL_Delay(50);
	b=Test_AT_send_TEST();
	b=Configure_TCP() ;
	b=close_socket();
	b=Open_Port();
	
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





bool ATCommand_Response(unsigned char *cmd,uint32_t timeout)
{
	
	uint32_t Tickstart_int = 0U;
	Tickstart_int = HAL_GetTick();
	unsigned char localdata;      // non zero value
	while(gsm_timeout(cmd, Tickstart_int, timeout))
	{
		HAL_Delay(1000);
		if (rx2_EOF_rcv)
		{
			
			if (!strcmp(_AT_, cmd))
			{
				COMReadStr(main_rx_buf, MAIN_RX_BUF_LEN);
				localdata = strcmp("\r\nOK\r\n", main_rx_buf);
			}
			
			if (!strcmp(_CLOSEPORT_, cmd))
			{
				
				HAL_Delay(2000);
				COMReadStr(main_rx_buf, MAIN_RX_BUF_LEN);
				localdata = strcmp("\r\nOK\r\n", main_rx_buf);
			}
			
			if(!strcmp(_TCPCONFIG_, cmd))
			{
				localdata = strcmp("\r\nOK\r\n", main_rx_buf);
			}
			
			if (!strcmp(_OPENPORT_, cmd))
			{
				localdata = strcmp("\r\nOK\r\n\r\n+QIOPEN: 0,0\r\n", main_rx_buf);
			}	
			rx2_EOF_rcv = false;
			flush_buf();
			if (!localdata)
			{
				break;
			}
		}
	}
}
char Test_AT_send_TEST()
{
	uint32_t Timeout_int = 25000;
	uint32_t Tickstart_int = 0U;
	Tickstart_int = HAL_GetTick();
	unsigned char localdata;    // non zero value
	
while(gsm_timeout(_AT_, Tickstart_int, Timeout_int))
	{
	
		COMSendStr(_AT_);
		HAL_Delay(100);
		if (rx2_EOF_rcv)
		{
			error = -2;
			COMReadStr(main_rx_buf, MAIN_RX_BUF_LEN);
			localdata = strcmp("\r\nOK\r\n", main_rx_buf);
			rx2_EOF_rcv = false;
			flush_buf();
			if (!localdata)
			{
				error = 0;
				break;
			}
		}
	}
	return error;
}


bool gsm_timeout(unsigned char *command ,uint32_t tickstart, uint32_t timeout)
{
	
	if ((timeout == 0U) || ((HAL_GetTick() - tickstart) > timeout))
	{
		if(error==0)
		{
			error = -1;
		}
		//Gsm_SendString("time out for command  ");
		//Gsm_SendString(command);
	//	printf("time out for command : %s", command);
		rx2_EOF_rcv = false;
		return false;	
	}
	else
	{
		return true;
	}
}









char close_socket()
{
	uint32_t Timeout_int = 25000;
	uint32_t Tickstart_int = 0U;
	Tickstart_int = HAL_GetTick();
	unsigned char localdata = 1;    // non zero value

while(gsm_timeout(_CLOSEPORT_,Tickstart_int, Timeout_int))
	{
		
		COMSendStr("AT+QICLOSE=0\r");
		HAL_Delay(700);
		if (rx2_EOF_rcv)
		{
			error = -4;
			COMReadStr(main_rx_buf, MAIN_RX_BUF_LEN);
			localdata = strcmp("\r\nOK\r\n", main_rx_buf);
			rx2_EOF_rcv = false;
			flush_buf();
			if (!localdata)
			{
				error = 0;
				break;
			}
		}
	}
	return error;
}



char Configure_TCP()
{
	uint32_t Timeout_int = 2500;
	uint32_t Tickstart_int = 0U;
	Tickstart_int = HAL_GetTick();
	unsigned char localdata;     // non zero value

	//rx2_EOF_rcv = false;
while(gsm_timeout(_TCPCONFIG_,Tickstart_int, Timeout_int))
	{
		COMSendStr("AT+QICSGP=1,1,\"internet.t-mobile.cz\",\"\",\"\",1\r");
		HAL_Delay(300);
		if (rx2_EOF_rcv)
		{	
			error=-3;
			COMReadStr(main_rx_buf, MAIN_RX_BUF_LEN);
			localdata = strcmp("\r\nOK\r\n", main_rx_buf);
			rx2_EOF_rcv = false;
			flush_buf();
			if (!localdata)
			{
				error = 0;
				break;
			}
		}
	}
	
	return error;
	
}



bool connect(unsigned char *clientID)
{
	unsigned char protocol_name[] = "MQTT";
	COMSendStr("AT+QISEND=0\r");
	
	int len = 8 + strlen(protocol_name) + strlen(clientID);
	unsigned char arr1[3] = { 0x10, len, 0x00 }; 
	unsigned char arr2[5] = { 0x04, 0x02, 0x00, 0x3C, 0x00 };
	HAL_Delay(10);	
	COMSendBuf(arr1, 3);
	COMSendByte(strlen(protocol_name));
	HAL_Delay(5);
	COMSendBuf(protocol_name, strlen(protocol_name));
	COMSendBuf(arr2, 5);
	COMSendByte(strlen(clientID));
	HAL_Delay(5);
	COMSendBuf(clientID, strlen(clientID));
	COMSendByte(0x1A);
	HAL_Delay(100);
	
}






char Publishpkt(unsigned char i)

{

		uint32_t Timeout_int = 25000;
		uint32_t Tickstart_int = 0U;
		unsigned char *topic;
		unsigned char *message;
		topic = mqtt[i].topic;
		message = mqtt[i].message;
		unsigned char *mess;
		unsigned char tsize = strlen(topic);
		unsigned char len = 2 + tsize + strlen(message);
	    unsigned char N = tsize + strlen(message);
		unsigned char localdata=1;
		unsigned char arr1[4] = {0x30, len, 0x00, tsize};
		Tickstart_int = HAL_GetTick();
		mess = malloc((N + 1) * sizeof(char));
		for (int i = 0; i < tsize; i++) 
		{
			mess[i] = topic[i];
		}
	for (int i = 0; i < strlen(message); i++) 
		{
			mess[tsize + i] = message[i];
		}
		mess[N] = 0X00;
	
	
		while (gsm_timeout(_PUBLISH_, Tickstart_int, Timeout_int))
		{
			
			COMSendStr("AT+QISEND=0\r");
			HAL_Delay(100);
			flush_buf();
			COMSendBuf(arr1, 4);
			COMSendBuf(mess, N);	
			COMSendByte(0x1A);
			HAL_Delay(120);
			if (rx2_EOF_rcv)
			{
				error = -6;
				COMReadStr(main_rx_buf, MAIN_RX_BUF_LEN);
				localdata = strcmp("\r\nSEND OK\r\n", main_rx_buf);
				rx2_EOF_rcv = false;
			flush_buf();
				if (!localdata)
				{
					error = 0;
					break;
				}
			}
		}
		return error;
}
	



bool Mqtt_subscribe(unsigned char *topic)
{
	COMSendStr("AT+QISEND=0\r");
	HAL_Delay(100);
	unsigned char *mess;
	unsigned char tsize = strlen(topic);
	unsigned char len = 5 + tsize;
	char str[80];
	COMSendByte(0x82);
	HAL_Delay(50);
	COMSendByte(len);
	HAL_Delay(50);
	COMSendByte(0x00);
	HAL_Delay(50);
	COMSendByte(0x0A);
	HAL_Delay(50);
	COMSendByte(0x00);
	HAL_Delay(50);
	COMSendByte(&huart6, tsize);
	HAL_Delay(50);
	for (int i = 0; i < tsize; i++) {
		
		COMSendByte(topic[i]);
		HAL_Delay(50);
	} 
	COMSendByte(0x00);
	HAL_Delay(50);
	COMSendByte(0x1A);
}

char Open_Port()
{
	uint32_t Timeout_int = 25000;
	uint32_t Tickstart_int = 0U;
	Tickstart_int = HAL_GetTick();
	unsigned char localdata = 1;      // non zero value

while(gsm_timeout(_OPENPORT_,Tickstart_int, Timeout_int))
	{
		
		COMSendStr("AT+QIOPEN=1,0,\"TCP\",\"4tech.horcica.cz\",1883,0,1\r");
		HAL_Delay(900);
		if (rx2_EOF_rcv)
		{
			error = -5;
			COMReadStr(main_rx_buf, MAIN_RX_BUF_LEN);
			localdata = strcmp("\r\nOK\r\n\r\n+QIOPEN: 0,0\r\n", main_rx_buf);
			rx2_EOF_rcv = false;
			flush_buf();
			HAL_Delay(100);
			if (!localdata)
			{
				error = 0;
				break;
			}
		}
	}
	return error;
}



/* ***********************************************************************************************





















 *bool Publishpkt_num(unsigned char *topic, char value)
{

	uint32_t Timeout_int = 25000;
	uint32_t Tickstart_int = 0U;
	Tickstart_int = HAL_GetTick();
	unsigned char localdata;
	while (gsm_timeout(_PUBLISH_, Tickstart_int, Timeout_int))
	{
		char v[4];
		
		COMSendStr("AT+QISEND=0\r");
		HAL_Delay(300);
		itoa(value, v, 10);
		unsigned char *mess;
		unsigned char tsize = strlen(topic);
		unsigned char msize = strlen(v);
		unsigned char len = 2 + tsize + msize;
		unsigned char N = tsize + msize;
		flush_buf();
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
		if (rx2_EOF_rcv)
		{
			COMReadStr(main_rx_buf, MAIN_RX_BUF_LEN);
			localdata = strcmp("\r\nSEND OK\r\n", main_rx_buf);
			rx2_EOF_rcv = false;
			flush_buf();
			if (!localdata)
			{
				error = false;
				break;
			}
		}
	}
	return error;
}
	
 *
 *
 **/