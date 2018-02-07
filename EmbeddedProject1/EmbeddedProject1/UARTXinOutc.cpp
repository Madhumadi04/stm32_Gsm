// USART2
// Receiver buffer & variables
#define RX2_BUFFER_SIZE 50
char rx2_buffer[RX2_BUFFER_SIZE];
char rx2_EOF_chr = LF;
uint16_t rx2_wr_index, rx2_rd_index;
volatile uint16_t rx2_counter;
volatile bool rx2_EOF_rcv, rx2_buffer_overflow; 
// Transmitter buffer & variables
#define TX2_BUFFER_SIZE 1000


char tx2_buffer[TX2_BUFFER_SIZE];
volatile uint16_t tx2_wr_index, tx2_rd_index, tx2_counter;

//-----------------------------------------------------------------------------
uint32_t UART2ReadStr(char *buf, uint32_t buflen)
{
	uint32_t nchr = 0;
	if (rx2_counter != 0)
	{
		__HAL_UART_DISABLE_IT(&huart2, UART_IT_RXNE);
		for (; buflen > 1; --buflen)		// last char in buffer for '\0'
			{
				*buf++ = rx2_buffer[rx2_rd_index];
				++nchr;
				if (++rx2_rd_index == RX2_BUFFER_SIZE)
					rx2_rd_index = 0;
				if (--rx2_counter == 0)
					break;
			}
		__HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE);
	}
	*buf = '\0';
	return nchr;
}

//-----------------------------------------------------------------------------
uint32_t UART2SendStr(char *s)
{
	uint32_t nchr = 0;
	while (tx2_counter == TX2_BUFFER_SIZE) ;
	__HAL_UART_DISABLE_IT(&huart2, UART_IT_TXE);
	while (*s != '\0')
	{
		tx2_buffer[tx2_wr_index] = *s++;
		if (++tx2_wr_index == TX2_BUFFER_SIZE)
			tx2_wr_index = 0;
		++tx2_counter;
		++nchr;
	}
	__HAL_UART_ENABLE_IT(&huart2, UART_IT_TXE);
	return nchr;
}

