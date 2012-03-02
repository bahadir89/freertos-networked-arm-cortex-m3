/****************************************************************************************
|  Description: UART driver source file
|    File Name: uart.c
|
|----------------------------------------------------------------------------------------
|                          C O P Y R I G H T
|----------------------------------------------------------------------------------------
|   Copyright (c) 2011  by Feaser    http://www.feaser.com    All rights reserved
|
|----------------------------------------------------------------------------------------
|                            L I C E N S E
|----------------------------------------------------------------------------------------
| This file is part of OpenBLT. OpenBLT is free software: you can redistribute it and/or
| modify it under the terms of the GNU General Public License as published by the Free
| Software Foundation, either version 3 of the License, or (at your option) any later
| version.
|
| OpenBLT is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
| without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
| PURPOSE. See the GNU General Public License for more details.
|
| You should have received a copy of the GNU General Public License along with OpenBLT.
| If not, see <http://www.gnu.org/licenses/>.
|
| A special exception to the GPL is included to allow you to distribute a combined work 
| that includes OpenBLT without being obliged to provide the source code for any 
| proprietary components. The exception text is included at the bottom of the license
| file <license.html>.
| 
****************************************************************************************/

/****************************************************************************************
* Include files
****************************************************************************************/
#include "header.h"                                    /* generic header               */


/****************************************************************************************
** NAME:           UartInit
** PARAMETER:      baudrate communication speed in bits/sec
** RETURN VALUE:   none
** DESCRIPTION:    Initializes the UART interface for the selected communication speed.
**
****************************************************************************************/
void UartInit(unsigned long baudrate)
{
  GPIO_InitTypeDef  GPIO_InitStruct;
  USART_InitTypeDef USART_InitStruct;

  /* enable UART peripheral clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
  /* enable GPIO peripheral clock for transmitter and receiver pins */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
  /* configure USART Tx as alternate function push-pull */
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStruct);
  /* Configure USART Rx as alternate function input floating */
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
  GPIO_Init(GPIOA, &GPIO_InitStruct);
  /* configure UART communcation parameters */
  USART_InitStruct.USART_BaudRate = baudrate;
  USART_InitStruct.USART_WordLength = USART_WordLength_8b;
  USART_InitStruct.USART_StopBits = USART_StopBits_1;
  USART_InitStruct.USART_Parity = USART_Parity_No;
  USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART2, &USART_InitStruct);
  /* enable UART */
  USART_Cmd(USART2, ENABLE);
} /*** end of UartInit ***/


/****************************************************************************************
** NAME:           UartTxChar
** PARAMETER:      ch character to transmit
** RETURN VALUE:   the transmitted character.
** DESCRIPTION:    Transmits a character through the UART interface.
**
****************************************************************************************/
int UartTxChar(int ch)
{
  /* wait for transmit completion of the previous character, if any */
  while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET) { ; }
  /* transmit the character */
  USART_SendData(USART2, (unsigned char)ch);
  /* for stdio compatibility */
  return ch;
} /*** end of UartTxChar ***/


/****************************************************************************************
** NAME:           UartRxChar
** PARAMETER:      blocking 1 to block until a character was received, 0 otherwise.
** RETURN VALUE:   the value of the received character or -1.
** DESCRIPTION:    Receives a character from the UART interface.
**
****************************************************************************************/
int UartRxChar(unsigned char blocking)
{
  if (!blocking)
  {
    /* check flag to see if a byte was received */
    if (USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET)
    {
      return -1;
    }
  }
  else
  {
    /* wait for reception of byte */
    while (USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET) { ; }
  }
  /* retrieve and return the newly received byte */
  return USART_ReceiveData(USART2);
} /*** end of UartRxChar ***/


/*********************************** end of uart.c *************************************/
