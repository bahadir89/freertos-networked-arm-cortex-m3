/****************************************************************************************
|  Description: bootloader configuration header file
|    File Name: config.h
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
#ifndef CONFIG_H
#define CONFIG_H

/****************************************************************************************
*   C P U   D R I V E R   C O N F I G U R A T I O N
****************************************************************************************/
/* To properly initialize the baudrate clocks of the communication interface, typically
 * the speed of the crystal oscillator and/or the speed at which the system runs is 
 * needed. Set these through configurables BOOT_CPU_XTAL_SPEED_KHZ and
 * BOOT_CPU_SYSTEM_SPEED_KHZ, respectively. To enable data exchange with the host that is
 * not dependent on the targets architecture, the byte ordering needs to be known. 
 * Setting BOOT_CPU_BYTE_ORDER_MOTOROLA to 1 selects little endian mode and 0 selects 
 * big endian mode.
 */ 
#define BOOT_CPU_XTAL_SPEED_KHZ         (8000)
#define BOOT_CPU_SYSTEM_SPEED_KHZ       (72000)
#define BOOT_CPU_BYTE_ORDER_MOTOROLA    (0)
#define BOOT_CPU_CONNECT_STATE_PREFIX   __no_init


/****************************************************************************************
*   C O M M U N I C A T I O N   I N T E R F A C E   C O N F I G U R A T I O N
****************************************************************************************/
/* The CAN communication interface is selected by setting the BOOT_COM_CAN_ENABLE 
 * configurable to 1. Configurable BOOT_COM_CAN_BAUDRATE selects the communication speed
 * in bits/second. Two CAN messages are reserved for communication with the host. The 
 * message identifier for sending data from the target to the host is configured with
 * BOOT_COM_CAN_TXMSG_ID. The one for receiving data from the host is configured with
 * BOOT_COM_CAN_RXMSG_ID. The maximum amount of data bytes in a message for data 
 * transmission and reception is set through BOOT_COM_CAN_TX_MAX_DATA and 
 * BOOT_COM_CAN_RX_MAX_DATA, respectively. It is common for a microcontroller to have more
 * than 1 CAN controller on board. The zero-based BOOT_COM_CAN_CHANNEL_INDEX selects the
 * CAN controller channel.
 * 
 */
#define BOOT_COM_CAN_ENABLE             (0)
#define BOOT_COM_CAN_BAUDRATE           (500000)
#define BOOT_COM_CAN_TX_MSG_ID          (0x7E1)
#define BOOT_COM_CAN_TX_MAX_DATA        (8)
#define BOOT_COM_CAN_RX_MSG_ID          (0x667)
#define BOOT_COM_CAN_RX_MAX_DATA        (8)
#define BOOT_COM_CAN_CHANNEL_INDEX      (0)

/* The UART communication interface is selected by setting the BOOT_COM_UART_ENABLE 
 * configurable to 1. Configurable BOOT_COM_UART_BAUDRATE selects the communication speed
 * in bits/second. The maximum amount of data bytes in a message for data transmission 
 * and reception is set through BOOT_COM_UART_TX_MAX_DATA and BOOT_COM_UART_RX_MAX_DATA, 
 * respectively. It is common for a microcontroller to have more than 1 UART interface
 * on board. The zero-based BOOT_COM_UART_CHANNEL_INDEX selects the UART interface.
 * 
 */
#define BOOT_COM_UART_ENABLE            (1)
#define BOOT_COM_UART_BAUDRATE          (57600)
#define BOOT_COM_UART_TX_MAX_DATA       (64)
#define BOOT_COM_UART_RX_MAX_DATA       (64)
#define BOOT_COM_UART_CHANNEL_INDEX     (1)


/****************************************************************************************
*   B A C K D O O R   E N T R Y   C O N F I G U R A T I O N
****************************************************************************************/
/* It is possible to implement an application specific method to force the bootloader to
 * stay active after a reset. Such a backdoor entry into the bootloader is desired in
 * situations where the user program does not run properly and therefore cannot 
 * reactivate the bootloader. By enabling these hook functions, the application can
 * implement the backdoor, which overrides the default backdoor entry that is programmed
 * into the bootloader. When desired for security purposes, these hook functions can
 * also be implemented in a way that disables the backdoor entry altogether.
 */
#define BOOT_BACKDOOR_HOOKS_ENABLE      (0)


/****************************************************************************************
*   N O N - V O L A T I L E   M E M O R Y   D R I V E R   C O N F I G U R A T I O N
****************************************************************************************/
/* The NVM driver typically supports erase and program operations of the internal memory
 * present on the microcontroller. Through these hook functions the NVM driver can be
 * extended to support additional memory types such as external flash memory and serial
 * eeproms. The size of the internal memory in kilobytes is specified with configurable
 * BOOT_NVM_SIZE_KB.
 */
#define BOOT_NVM_HOOKS_ENABLE           (0)
#define BOOT_NVM_SIZE_KB                (128)


/****************************************************************************************
*   W A T C H D O G   D R I V E R   C O N F I G U R A T I O N
****************************************************************************************/
/* The COP driver cannot be configured internally in the bootloader, because its use
 * and configuration is application specific. The bootloader does need to service the
 * watchdog in case it is used. When the application requires the use of a watchdog,
 * set BOOT_COP_HOOKS_ENABLE to be able to initialize and service the watchdog through
 * hook functions.
 */
#define BOOT_COP_HOOKS_ENABLE           (0)


#endif /* CONFIG_H */
/*********************************** end of config.h ***********************************/
