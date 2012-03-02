/****************************************************************************************
|  Description: bootloader timer driver source file
|    File Name: timer.c
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


/****************************************************************************************
* Type definitions
****************************************************************************************/
typedef struct
{
  volatile blt_int32u CTRL;                     /* SysTick Control and Status Register */
  volatile blt_int32u LOAD;                     /* SysTick Reload Value Register       */
  volatile blt_int32u VAL;                      /* SysTick Current Value Register      */
} tSysTickRegs;


/****************************************************************************************
* Macro definitions
****************************************************************************************/
#define SYSTICK_BIT_CLKSOURCE    ((blt_int32u)0x00000004)
#define SYSTICK_BIT_ENABLE       ((blt_int32u)0x00000001)
#define SYSTICK_BIT_COUNTERFLAG  ((blt_int32u)0x00010000)


/****************************************************************************************
* Local data declarations
****************************************************************************************/
static blt_int16u millisecond_counter;


/****************************************************************************************
* Register definitions
****************************************************************************************/
#define SYSTICK          ((tSysTickRegs *) (blt_int32u)0xE000E010)


/****************************************************************************************
** NAME:           TimerInit
** PARAMETER:      none
** RETURN VALUE:   none
** DESCRIPTION:    Initializes the polling based millisecond timer driver. 
**
****************************************************************************************/
void TimerInit(void)
{
  /* reset the timer configuration */
  TimerReset();
  /* configure the systick frequency as a 1 ms event generator */
  SYSTICK->LOAD = BOOT_CPU_SYSTEM_SPEED_KHZ - 1;
  /* reset the current counter value */
  SYSTICK->VAL = 0;
  /* select core clock as source and enable the timer */
  SYSTICK->CTRL = SYSTICK_BIT_CLKSOURCE | SYSTICK_BIT_ENABLE;
  /* reset the millisecond counter value */
  TimerSet(0);
} /*** end of TimerInit ***/


/****************************************************************************************
** NAME:           TimerReset
** PARAMETER:      none
** RETURN VALUE:   none
** DESCRIPTION:    Reset the timer by placing the timer back into it's default reset
**                 configuration.
**
****************************************************************************************/
void TimerReset(void)
{
  /* set the systick's status and control register back into the default reset value */
  SYSTICK->CTRL = 0;
} /* end of TimerReset */


/****************************************************************************************
** NAME:           TimerUpdate
** PARAMETER:      none
** RETURN VALUE:   none
** DESCRIPTION:    Updates the millisecond timer.
**
****************************************************************************************/
void TimerUpdate(void)
{
  /* check if the milliseond event occurred */
  if ((SYSTICK->CTRL & SYSTICK_BIT_COUNTERFLAG) != 0)
  {
    /* increment the millisecond counter */
    millisecond_counter++;
  }
} /*** end of TimerUpdate ***/


/****************************************************************************************
** NAME:           TimerSet
** PARAMETER:      timer_value initialize value of the millisecond timer.
** RETURN VALUE:   none
** DESCRIPTION:    Sets the initial counter value of the millisecond timer.
**
****************************************************************************************/
void TimerSet(blt_int32u timer_value)
{
  /* set the millisecond counter value */
  millisecond_counter = timer_value;
} /*** end of TimerSet ***/


/****************************************************************************************
** NAME:           TimerGet
** PARAMETER:      none
** RETURN VALUE:   current value of the millisecond timer
** DESCRIPTION:    Obtains the counter value of the millisecond timer.
**
****************************************************************************************/
blt_int32u TimerGet(void)
{
  /* updating timer here allows this function to be called in a loop with timeout
   * detection.
   */
  TimerUpdate();
  /* read and return the amount of milliseconds that passed since initialization */
  return millisecond_counter;
} /*** end of TimerGet ***/


/*********************************** end of timer.c ************************************/
