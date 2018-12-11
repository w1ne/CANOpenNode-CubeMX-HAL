/*!*****************************************************************************
 * \file        task.c
 * \author      Andrii Shylenko
 *
 * \brief
 * Simple 1ms task implementation.
 ******************************************************************************/

/*-----------------------------------------------------------------------------
 * INCLUDE SECTION
 *----------------------------------------------------------------------------*/
#include "task.h"

#include "can.h"

#include "CanOpen.h"

/*EEPROM driver is not the part of the demonstration code*/
//#define CAN_USE_EEPROM

/*-----------------------------------------------------------------------------
 * LOCAL (static) DEFINITIONS
 *----------------------------------------------------------------------------*/

static CO_NMT_reset_cmd_t reset;
#ifdef CAN_USE_EEPROM
static CO_EE_t                     CO_EEO;         /* Eeprom object */
#endif


/*-----------------------------------------------------------------------------
 * GLOBAL DEFINITIONS
 *----------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------
 * LOCAL FUNCTION PROTOTYPES
 *----------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------
 * LOCAL FUNCTIONS
 *----------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------
 * GLOBAL FUNCTIONS - see descriptions in header file
 *----------------------------------------------------------------------------*/
void task_coldStart(void)
{
   __HAL_DBGMCU_FREEZE_TIM6();


/*------------------------CAN Open stack--------------------------------*/
   CO_ReturnError_t err;
   /* CAN module address, NodeID, Bitrate */
   /* We do not use CAN registers directly, so address here is a pointer to the CAN_HandleTypeDef object. */
   err = CO_init((uint32_t)&hcan1, 2, 250);

   if(err != CO_ERROR_NO)
   {
  	 //TODO behavior in a case of the stack error. Currently not defined.
  	 _Error_Handler(0, 0);
   }

   /* start CAN */
   CO_CANsetNormalMode(CO->CANmodule[0]);

   reset = CO_RESET_NOT;
}


void task_oneMs(void)
{

    /* CANopen process */
    reset = CO_process(CO, 1, NULL);

    /* Process EEPROM */
#ifdef CAN_USE_EEPROM
          CO_EE_process(&CO_EEO);
#endif

   if(CO->CANmodule[0]->CANnormal)
   {
        bool_t syncWas;

        /* Process Sync and read inputs */
        syncWas = CO_process_SYNC_RPDO(CO, 1000);

        /* Further I/O or nonblocking application code may go here. */

        /* Write outputs */
        CO_process_TPDO(CO, syncWas, 1000);

        CO_CANpolling_Tx(CO->CANmodule[0]);

        /* verify timer overflow */
        if(0) {
            CO_errorReport(CO->em, CO_EM_ISR_TIMER_OVERFLOW, CO_EMC_SOFTWARE_INTERNAL, 0U);
        }
    }
}

