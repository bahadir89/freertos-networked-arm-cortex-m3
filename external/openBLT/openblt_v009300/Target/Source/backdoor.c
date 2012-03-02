/****************************************************************************************
|  Description: bootloader backdoor entry source file
|    File Name: backdoor.c
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
* Macro definitions
****************************************************************************************/
#if (BOOT_BACKDOOR_HOOKS_ENABLE == 0)
  /* set the time in milliseconds that the backdoor is open */
  #define BACKDOOR_ENTRY_TIMEOUT_MS  (50)
#endif

/****************************************************************************************
* Hook functions
****************************************************************************************/
#if (BOOT_BACKDOOR_HOOKS_ENABLE > 0)
void BackDoorInitHook(void);
blt_bool BackDoorEntryHook(void);
#endif


/****************************************************************************************
* Local data declarations
****************************************************************************************/
#if (BOOT_BACKDOOR_HOOKS_ENABLE == 0)
static blt_bool backdoorOpen;
#endif


/****************************************************************************************
** NAME:           BackDoorInit
** PARAMETER:      none
** RETURN VALUE:   none
** DESCRIPTION:    Initializes the backdoor entry option.
**
****************************************************************************************/
void BackDoorInit(void)
{
#if (BOOT_BACKDOOR_HOOKS_ENABLE > 0)
  /* initialize application's backdoor functionality */
  BackDoorInitHook();
  
  /* attempt to start the user program when no backdoor entry is requested or
   * a pending COM module connection request.
   */
  if ( (BackDoorEntryHook() == BLT_FALSE) && (ComIsConnectEntryState() == BLT_FALSE) )
  {
    /* this function does not return if a valid user program is present */
    CpuStartUserProgram();
  }
  
#else
  /* open the backdoor after a reset */
  backdoorOpen = BLT_TRUE;
  /* initialize default backdoor entry */
  TimerInit();
#endif
  /* perform the first check that open/closes the backdoor */
  BackDoorCheck();
} /*** end of BackDoorInit ***/


/****************************************************************************************
** NAME:           BackDoorCheck
** PARAMETER:      none
** RETURN VALUE:   none
** DESCRIPTION:    The default backdoor entry feature keeps the bootloader active for a
**                 predetermined time after reset, allowing the host application to
**                 establish a connection and start a programming sequence. This function
**                 controls the opening/closing of the backdoor.
**
****************************************************************************************/
void BackDoorCheck(void)
{
#if (BOOT_BACKDOOR_HOOKS_ENABLE == 0)
  /* check if a connection with the host was already established. in this case the
   * backdoor stays open anyway, so no need to check if it needs to be closed
   */
  if (ComIsConnected() == BLT_TRUE)
  {
    return;
  }
  
  /* when the backdoor is still open, check if it's time to close it */
  if (backdoorOpen == BLT_TRUE)
  {
    /* check if the backdoor entry time window elapsed */
    if (TimerGet() >= BACKDOOR_ENTRY_TIMEOUT_MS)
    {
      /* close the backdoor */
      backdoorOpen = BLT_FALSE;
      /* reset the timer */
      TimerReset();
      /* this function does not return if a valid user program is present */
      CpuStartUserProgram();
    }
  }
#endif
} /*** end of BackDoorCheck ***/


/*********************************** end of backdoor.c *********************************/
