/**
  ******************************************************************************
  * @file    stm32f4xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  *
  * COPYRIGHT(c) 2017 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "stm32f4xx.h"
#include "stm32f4xx_it.h"
#include "stdbool.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern DMA_HandleTypeDef hdma_usart2_rx;
extern DMA_HandleTypeDef hdma_usart2_tx;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart6;

#define	CR		13

#define	LF		10 

// USART1 Receiver buffer & variables
#define RX2_BUFFER_SIZE 100
char rx2_buffer[RX2_BUFFER_SIZE];
char rx2_EOF_chr = CR;
uint16_t rx2_wr_index, rx2_rd_index;
volatile uint16_t rx2_counter;
volatile bool rx2_EOF_rcv=false, rx2_buffer_overflow; 
volatile char EOF_count;
// USART1 Transmitter buffer & variables
#define TX2_BUFFER_SIZE 100
char tx2_buffer[TX2_BUFFER_SIZE];
volatile uint16_t tx2_wr_index, tx2_rd_index, tx2_counter;
/******************************************************************************/
/*            Cortex-M4 Processor Interruption and Exception Handlers         */ 
/******************************************************************************/

/**
* @brief This function handles System service call via SWI instruction.
*/
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVCall_IRQn 0 */

  /* USER CODE END SVCall_IRQn 0 */
  /* USER CODE BEGIN SVCall_IRQn 1 */

  /* USER CODE END SVCall_IRQn 1 */
}

/**
* @brief This function handles Pendable request for system service.
*/
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
* @brief This function handles System tick timer.
*/
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  HAL_SYSTICK_IRQHandler();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32F4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f4xx.s).                    */
/******************************************************************************/

/**
* @brief This function handles DMA1 stream5 global interrupt.
*/
void DMA1_Stream5_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Stream5_IRQn 0 */

  /* USER CODE END DMA1_Stream5_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_usart2_rx);
  /* USER CODE BEGIN DMA1_Stream5_IRQn 1 */

  /* USER CODE END DMA1_Stream5_IRQn 1 */
}

/**
* @brief This function handles DMA1 stream6 global interrupt.
*/
void DMA1_Stream6_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Stream6_IRQn 0 */

  /* USER CODE END DMA1_Stream6_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_usart2_tx);
  /* USER CODE BEGIN DMA1_Stream6_IRQn 1 */

  /* USER CODE END DMA1_Stream6_IRQn 1 */
}

/**
* @brief This function handles USART2 global interrupt.
*/
// Special characters





void USART2_IRQHandler(void)
{
		// HAL_UART_IRQHandler(&huart1);
	char c;
	uint16_t SRflags = USART2->SR;

	// TODO: UART error handling

	if(__HAL_UART_GET_FLAG(&huart2, UART_FLAG_RXNE))
	{
		                  // read interrupt
   c = USART2->DR & 0x00FF;
		rx2_buffer[rx2_wr_index] = c;
		if (c == rx2_EOF_chr)
		{
			rx2_EOF_rcv = true;
			EOF_count++;
		}
			
		if (++rx2_wr_index == RX2_BUFFER_SIZE)
			rx2_wr_index = 0;
		if (++rx2_counter == RX2_BUFFER_SIZE)
		{
			rx2_counter = 0;
			rx2_buffer_overflow = true;
		}
	}

	if (__HAL_UART_GET_IT_SOURCE(&huart2, UART_IT_TXE) && __HAL_UART_GET_FLAG(&huart2, UART_FLAG_TXE))
		// IT source must be tested not to execute the branch during a reception
		{   
			char c;
			c = tx2_buffer[tx2_rd_index];
			USART2->DR = c;
			if (++tx2_rd_index == TX2_BUFFER_SIZE)
				tx2_rd_index = 0;
			if (--tx2_counter == 0)
				__HAL_UART_DISABLE_IT(&huart2, UART_IT_TXE); 	// stop further transmission
		}
}

/**
* @brief This function handles USART6 global interrupt.
*/
void USART6_IRQHandler(void)
{
  /* USER CODE BEGIN USART6_IRQn 0 */

  /* USER CODE END USART6_IRQn 0 */
  HAL_UART_IRQHandler(&huart6);
  /* USER CODE BEGIN USART6_IRQn 1 */

  /* USER CODE END USART6_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
int32_t COMReadStr(char *buf, uint32_t buflen)
{
	uint32_t nchr = 0;
	char c, C_COUNT;
	if (rx2_counter != 0)
	{
		__HAL_UART_DISABLE_IT(&huart2, UART_IT_RXNE);
		for (; buflen > 1; --buflen)		// last char in buffer for '\0'
			{
				c = *buf++ = rx2_buffer[rx2_rd_index];
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
uint32_t COMSendStr(char *s)
{
	uint32_t nchr = 0;
	unsigned char c;
	while (tx2_counter == TX2_BUFFER_SIZE) ;
	HAL_Delay(150);
	__HAL_UART_DISABLE_IT(&huart2, UART_IT_TXE);
	while (*s != '\0')
	{	
		c = *s++;
		HAL_Delay(50);
		tx2_buffer[tx2_wr_index] = c;
		if (++tx2_wr_index == TX2_BUFFER_SIZE)
			tx2_wr_index = 0;
		++tx2_counter;
		++nchr;
	}
	__HAL_UART_ENABLE_IT(&huart2, UART_IT_TXE);
	return nchr;
}

uint32_t COMSendBuf(char *buf, uint32_t size)
{
	uint32_t i;
	while (tx2_counter == TX2_BUFFER_SIZE) ;
	__HAL_UART_DISABLE_IT(&huart2, UART_IT_TXE);
	for (i = 0; i < size; i++)
	{
		tx2_buffer[tx2_wr_index] = buf[i];
		if (++tx2_wr_index == TX2_BUFFER_SIZE)
			tx2_wr_index = 0;
		++tx2_counter;
	}
	__HAL_UART_ENABLE_IT(&huart2, UART_IT_TXE);
	return size;
}

//-----------------------------------------------------------------------------
uint32_t COMSendByte(uint8_t data)
{
	if (tx2_counter == TX2_BUFFER_SIZE)
		return 0;	// when buffer is full returns 0
	__HAL_UART_DISABLE_IT(&huart2, UART_IT_TXE);
	tx2_buffer[tx2_wr_index] = (char)data;
	if (++tx2_wr_index == TX2_BUFFER_SIZE)
		tx2_wr_index = 0;
	++tx2_counter;
	__HAL_UART_ENABLE_IT(&huart2, UART_IT_TXE);
	return 1;	// always only 1 byte
}
