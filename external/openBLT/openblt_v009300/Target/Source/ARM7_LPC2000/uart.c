/****************************************************************************************
|  Description: bootloader UART communication interface source file
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
#include "boot.h"                                /* bootloader generic header          */


#if (BOOT_COM_UART_ENABLE > 0)
/****************************************************************************************
* Macro definitions
****************************************************************************************/
#define UART_DLAB      (0x80)                    /* divisor latch access bit           */
#define UART_MODE_8N1  (0x03)                    /* 8 data and 1 stop bit, no parity   */
#define UART_FIFO_RX1  (0x07)                    /* FIFO reset and RX FIFO 1 deep      */
#define UART_RDR       (0x01)                    /* receiver data ready                */
#define UART_THRE      (0x20)                    /* transmitter holding register empty */


/****************************************************************************************
* Register definitions
****************************************************************************************/
#define U0RBR          (*((volatile blt_int8u *) 0xE000C000))
#define U0THR          (*((volatile blt_int8u *) 0xE000C000))
#define U0IER          (*((volatile blt_int8u *) 0xE000C004))
#define U0IIR          (*((volatile blt_int8u *) 0xE000C008))
#define U0FCR          (*((volatile blt_int8u *) 0xE000C008))
#define U0LCR          (*((volatile blt_int8u *) 0xE000C00C))
#define U0LSR          (*((volatile blt_int8u *) 0xE000C014))
#define U0SCR          (*((volatile blt_int8u *) 0xE000C01C))
#define U0DLL          (*((volatile blt_int8u *) 0xE000C000))
#define U0DLM          (*((volatile blt_int8u *) 0xE000C004))


/****************************************************************************************
* Function prototypes
****************************************************************************************/
static blt_bool UartReceiveByte(blt_int8u *data);
static blt_bool UartTransmitByte(blt_int8u data);


/****************************************************************************************
** NAME:           UartInit
** PARAMETER:      none
** RETURN VALUE:   none
** DESCRIPTION:    Initializes the UART communication interface
**
****************************************************************************************/
void UartInit(void)
{
  blt_int32u baud_reg_value;                           /* baudrate register value      */
  
  /* the current implementation supports UART0. throw an assertion error in case 
   * a different UART channel is configured.  
   */
  ASSERT_CT(BOOT_COM_UART_CHANNEL_INDEX == 0); 
  /* disable UART related interrupt generation. this driver works in polling mode */
  U0IER = 0;
  /* clear interrupt id register */
  U0IIR = 0;
  /* clear line status register */
  U0LSR = 0;
  /* set divisor latch DLAB = 1 so buadrate can be configured */
  U0LCR = UART_DLAB;
  /* Baudrate calculation: 
   *   y = BOOT_CPU_SYSTEM_SPEED_KHZ * 1000 / 16 / BOOT_COM_UART_BAUDRATE and add 
   *   smartness to automatically round the value up/down using the following trick:
   *     y = x/n can round with y = (x + (n + 1)/2 ) / n
   */
  /* check that baudrate register value is not 0 */
  ASSERT_CT((((BOOT_CPU_SYSTEM_SPEED_KHZ*1000/16)+((BOOT_COM_UART_BAUDRATE+1)/2))/  \
               BOOT_COM_UART_BAUDRATE) > 0);
  /* check that baudrate register value is not greater than max 16-bit unsigned value */
  ASSERT_CT((((BOOT_CPU_SYSTEM_SPEED_KHZ*1000/16)+((BOOT_COM_UART_BAUDRATE+1)/2))/  \
               BOOT_COM_UART_BAUDRATE) <= 65535);
  baud_reg_value = (((BOOT_CPU_SYSTEM_SPEED_KHZ*1000/16)+ \
                    ((BOOT_COM_UART_BAUDRATE+1)/2))/BOOT_COM_UART_BAUDRATE);
  /* write the calculated baudrate selector value to the registers */
  U0DLL = (blt_int8u)baud_reg_value;
  U0DLM = (blt_int8u)(baud_reg_value >> 8);
  /* configure 8 data bits, no parity and 1 stop bit and set DLAB = 0 */
  U0LCR = UART_MODE_8N1;
  /* enable and reset transmit and receive FIFO. necessary for UART operation */
  U0FCR = UART_FIFO_RX1;
} /*** end of UartInit ***/


/****************************************************************************************
** NAME:           UartTransmitPacket
** PARAMETER:      data pointer to byte array with data that it to be transmitted.
**                 len  number of bytes that are to be transmitted.
** RETURN VALUE:   none
** DESCRIPTION:    Transmits a packet formatted for the communication interface.
**
****************************************************************************************/
void UartTransmitPacket(blt_int8u *data, blt_int8u len)
{
  blt_int16u data_index;

  /* verify validity of the len-paramenter */
  ASSERT_RT(len <= BOOT_COM_TX_MAX_DATA);  

  /* first transmit the length of the packet */  
  ASSERT_RT(UartTransmitByte(len) == BLT_TRUE);  
  
  /* transmit all the packet bytes one-by-one */
  for (data_index = 0; data_index < len; data_index++)
  {
    /* keep the watchdog happy */
    CopService();
    /* write byte */
    ASSERT_RT(UartTransmitByte(data[data_index]) == BLT_TRUE);  
  }
} /*** end of UartTransmitPacket ***/


/****************************************************************************************
** NAME:           UartReceivePacket
** PARAMETER:      data pointer to byte array where the data is to be stored.
** RETURN VALUE:   BLT_TRUE if a packet was received, BLT_FALSE otherwise.
** DESCRIPTION:    Receives a communication interface packet if one is present.
**
****************************************************************************************/
blt_bool UartReceivePacket(blt_int8u *data)
{
  static blt_int8u xcpCtoReqPacket[XCP_CTO_PACKET_LEN+1];  /* one extra for length */
  static blt_int8u xcpCtoRxLength;
  static blt_bool  xcpCtoRxInProgress = BLT_FALSE;

  /* start of cto packet received? */
  if (xcpCtoRxInProgress == BLT_FALSE)
  {
    /* store the message length when received */
    if (UartReceiveByte(&xcpCtoReqPacket[0]) == BLT_TRUE)
    {
      /* indicate that a cto packet is being received */
      xcpCtoRxInProgress = BLT_TRUE;

      /* reset packet data count */
      xcpCtoRxLength = 0;
    }
  }
  else
  {
    /* store the next packet byte */
    if (UartReceiveByte(&xcpCtoReqPacket[xcpCtoRxLength+1]) == BLT_TRUE)
    {
      /* increment the packet data count */
      xcpCtoRxLength++;

      /* check to see if the entire packet was received */
      if (xcpCtoRxLength == xcpCtoReqPacket[0])
      {
        /* copy the packet data */
        CpuMemCopy((blt_int32u)data, (blt_int32u)&xcpCtoReqPacket[1], xcpCtoRxLength);        
        /* done with cto packet reception */
        xcpCtoRxInProgress = BLT_FALSE;

        /* packet reception complete */
        return BLT_TRUE;
      }
    }
  }
  
  /* packet reception not yet complete */
  return BLT_FALSE;
} /*** end of UartReceivePacket ***/


/****************************************************************************************
** NAME:           UartReceiveByte
** PARAMETER:      data pointer to byte where the data is to be stored.
** RETURN VALUE:   BLT_TRUE if a byte was received, BLT_FALSE otherwise.
** DESCRIPTION:    Receives a communication interface byte if one is present.
**
****************************************************************************************/
static blt_bool UartReceiveByte(blt_int8u *data)
{
  /* check if a new byte was received by means of the RDR-bit */
  if((U0LSR & UART_RDR) != 0)
  {
    /* store the received byte */
    data[0] = U0RBR;
    /* inform caller of the newly received byte */
    return BLT_TRUE;
  }
  /* inform caller that no new data was received */
  return BLT_FALSE;
} /*** end of UartReceiveByte ***/


/****************************************************************************************
** NAME:           UartTransmitByte
** PARAMETER:      data value of byte that is to be transmitted.
** RETURN VALUE:   BLT_TRUE if the byte was transmitted, BLT_FALSE otherwise.
** DESCRIPTION:    Transmits a communication interface byte.
**
****************************************************************************************/
static blt_bool UartTransmitByte(blt_int8u data)
{
  /* check if tx holding register can accept new data */
  if ((U0LSR & UART_THRE) == 0)
  {
    /* UART not ready. should not happen */
    return BLT_FALSE;
  }
  /* write byte to transmit holding register */
  U0THR = data;
  /* wait for tx holding register to be empty */
  while((U0LSR & UART_THRE) == 0) 
  { 
    /* keep the watchdog happy */
    CopService();
  }
  /* byte transmitted */
  return BLT_TRUE;
} /*** end of UartTransmitByte ***/
#endif /* BOOT_COM_UART_ENABLE > 0 */


/*********************************** end of uart.c *************************************/
