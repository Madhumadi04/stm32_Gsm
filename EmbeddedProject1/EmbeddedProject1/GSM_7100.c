#include "GSM_7100.h"
extern UART_HandleTypeDef huart6;
extern unsigned char incr, ite, f, temp;
extern char Rx_indx, Rx_data[2], Rx_Buffer[100];
extern uint8_t CR_check;
extern uint8_t LF_check;
extern Transfer_cplt;
extern localdata;
//unsigned char conn_frame[] = { 16, 18, 0, 4, 77, 81, 84, 84, 4, 2, 0, 60, 0, 6, 65, 66, 67, 68, 69, 69, 26 };
#define		SIMPIN			"1134\r"
unsigned char pub_frame[] = {0x30,0x0A,0x00,0x03,0x41,0x41,0x41,0x68,0x65,0x66,0x6C,0x6F,0x1A};
Gsm_t	Gsm;

bool init_quectel()
{
	
	HAL_Delay(1000);
	Gsm_senddata(&huart6, 26);
	HAL_Delay(500);
	Gsm_SendString("AT\r");
	
	HAL_Delay(500);
	while (!Test_AT_send()) ;
	
	//Gsm_SendString("AT\r");
	HAL_Delay(500);
	Gsm_SendString("AT+QICLOSE=0\r");
	HAL_Delay(500);
	while (!unlock_sim()) ;
	HAL_Delay(1000);
	
	//
	//Gsm_SendString("AT+CPIN=1134\r");
	//Gsm_SendString(SIMPIN);
	//
	
	while(!Configure_TCP());
	HAL_Delay(1000);
	Open_Port();
	//Gsm_SendString("AT+QIOPEN=1,0,\"TCP\",\"4tech.horcica.cz\",1884,0,1\r");	
	HAL_Delay(1000);
}

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


bool Test_AT_send()
{
	unsigned char localdata;  // non zero value
	
	Gsm_SendString("AT\r");
	HAL_Delay(500);
	if (Transfer_cplt == 1)
	{
		localdata = strcmp("OK", Rx_Buffer);
		localdata = !localdata;
		Transfer_cplt = 0;
		if (localdata)
		{
			return true; 
		}
		else 
			return false;
	}
	else return false;
}


bool close_socket()
{
	
	unsigned char localdata;   // non zero value
	
	Gsm_SendString("AT+QICLOSE=0\r");
	//while(1);
	HAL_Delay(100);
	if (Transfer_cplt == 1)
	{
		localdata = strcmp("OK", Rx_Buffer);
		localdata = !localdata;
		Transfer_cplt = 0;
		if (localdata)
		{
			return true; 
		}
		else 
			return false;
	}
	else return false;
}
bool unlock_sim()
{
	unsigned char localdata1, localdata2;    // non zero value
	Gsm_SendString("AT+CPIN=1134\r");
	HAL_Delay(100);
	if (Transfer_cplt == 1)
	{
		localdata1 = strcmp("+CME ERROR: 3", Rx_Buffer);
		localdata2= strcmp("OK", Rx_Buffer);
		localdata1 = !localdata1;
		localdata2 = !localdata2;
		Transfer_cplt = 0;
		if ((localdata1)||(localdata2))
		{
			return true;  // returs 0 when case matches OK to exit infinite loop in main
		}
		else 
			return false;
	}
	
}

bool Configure_TCP()
{
	unsigned char localdata;     // non zero value
	Gsm_SendString("AT+QICSGP=1,1,\"internet.t-mobile.cz\",\"\",\"\",1\r");
	HAL_Delay(100);
	if (Transfer_cplt == 1)
	{
		localdata = strcmp("OK", Rx_Buffer);
		localdata = !localdata;
		Transfer_cplt = 0;
		if (localdata)
		{
			//Open_Port();
			
			HAL_Delay(100);
			return true;  // returs 0 when case matches OK to exit infinite loop in main
		}
		else 
			return false;
	}
	
}



bool connect(unsigned char *clientID)
{
	unsigned char protocol_name[] = "MQTT";
	Gsm_SendString("AT+QISEND=0\r");
	int len = 8 + strlen(protocol_name) + strlen(clientID);
	Rx_indx = 0;
	ite = 0;
	HAL_Delay(100);	
	Gsm_senddata(&huart6, 0x10);
	HAL_Delay(100);
	Gsm_senddata(&huart6, len);
	HAL_Delay(100);
	Gsm_senddata(&huart6, 0x00);
	HAL_Delay(100);
	Gsm_senddata(&huart6, strlen(protocol_name));
	HAL_Delay(100);
	for (int i = 0; i < strlen(protocol_name); i++) 
	{
		
		Gsm_senddata(&huart6, protocol_name[i]);
		HAL_Delay(100);
	} 
	Gsm_senddata(&huart6, 0x04);
	HAL_Delay(100);
	Gsm_senddata(&huart6, 0x02);
	HAL_Delay(100);
	Gsm_senddata(&huart6, 0x00);
	HAL_Delay(100);
	Gsm_senddata(&huart6, 0x3C);
	HAL_Delay(100);
	Gsm_senddata(&huart6, 0x00);
	HAL_Delay(100);
	Gsm_senddata(&huart6, strlen(clientID));
	HAL_Delay(100);
	for (int i = 0; i < strlen(clientID); i++) 
	{
		
		Gsm_senddata(&huart6, clientID[i]);
		HAL_Delay(100);
	} 
	Gsm_senddata(&huart6, 0x1A);
	
}
bool pub_pkt()
{
	
	Gsm_SendString("AT+QISEND=0\r");
	HAL_Delay(100);
	Rx_indx = 0;
	ite = 0;
	char temp = sizeof(pub_frame);
	for (int i = 0; i < temp; i++)
	{
		Gsm_senddata(&huart6, pub_frame[i]); 
		HAL_Delay(100);
	}
	
}


bool Publishpkt(unsigned char *topic, unsigned char *message)
{
	Gsm_SendString("AT+QISEND=0\r");
	HAL_Delay(100);
	Rx_indx = 0;
	ite = 0;
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
	Gsm_senddata(&huart6, 0x30);
	HAL_Delay(100);
	Gsm_senddata(&huart6, len);
	HAL_Delay(100);
	Gsm_senddata(&huart6, 0x00);
	HAL_Delay(100);
	Gsm_senddata(&huart6, tsize);
	HAL_Delay(100);
	for (int i = 0; i < N; i++) {
		
		Gsm_senddata(&huart6, mess[i]);
		HAL_Delay(100);
	} 
	Gsm_senddata(&huart6, 0x1A);
}


bool Mqtt_subscribe(unsigned char *topic)
{
	Gsm_SendString("AT+QISEND=0\r");
	HAL_Delay(100);
	Rx_indx = 0;
	ite = 0;
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
	Gsm_SendString("AT+QIOPEN=1,0,\"TCP\",");
	HAL_Delay(50);
	Gsm_SendString(Domain_name);
	HAL_Delay(50);
	Gsm_SendString(Portno);
	HAL_Delay(50);
	Gsm_SendString("0,1\r");
	HAL_Delay(50);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)  
{
	/**/
	uint8_t i;
	
	
	if (huart->Instance == USART6)  //current UART
		{
			incr++;
			if (Rx_indx == 0) {for (i = 0; i < 100; i++) Rx_Buffer[i] = 0; } 
			if (ite == 1)		//last CR LF check
			{
				
				if ((Rx_data[0] != 13)&&(Rx_data[0] != 10))//if received data different from ascii 13 (enter)
				{
						Rx_Buffer[Rx_indx++] = Rx_data[0];
				}
		
				if ((Rx_data[0] == 10)&&(f == 1)) 
				{
					temp = incr;
					temp = temp - 1;
					if (CR_check == temp)
						LF_check = 1;
				}
				if (Rx_data[0] == 13)
				{
					CR_check = incr;
					f = 1;
				}
				
				if (LF_check == 1)
				{
					
					Transfer_cplt = 1;
					LF_check = 0;
					CR_check = 5;
					ite = 0;
					f = 0;
					Rx_indx = 0;
				}
				
				
			}

			if (ite == 0)    // First CR LF check
			{
				
				if (Rx_data[0] == 13) //if received data different from ascii 13 (enter)
					{
						CR_check = incr;
					}
			
			
				if ((Rx_data[0] == 10)&&(CR_check == (incr) - 1))
				{
					LF_check = 1;
				}
			
				if (LF_check == 1)
				{
					Rx_indx = 0;
					incr = 0;
					LF_check = 0;
					ite = 1;
				
				}
			}
		
		

			HAL_UART_Receive_IT(&huart6, Rx_data, 1);       //activate UART receive interrupt every time
		}

	
}



/*
 *bool conn_pkt()
{
	Gsm_SendString("AT+QISEND=0\r");
	Rx_indx = 0;
	ite = 0;
	HAL_Delay(100);
	for (int i = 0; i < (sizeof(conn_frame)); i++)
	{
		Gsm_senddata(&huart6, conn_frame[i]); 
		HAL_Delay(100);
	}
}
*/