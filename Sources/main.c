/* ###################################################################
 **     Filename    : main.c
 **     Project     : Laufkatze
 **     Processor   : MKL25Z128VLK4
 **     Version     : Driver 01.01
 **     Compiler    : GNU C Compiler
 **     Date/Time   : 2018-02-16, 12:25, # CodeGen: 0
 **     Abstract    :
 **         Main module.
 **         This module contains user's application code.
 **     Settings    :
 **     Contents    :
 **         No public methods
 **
 ** ###################################################################*/
/*!
 ** @file main.c
 ** @version 01.01
 ** @brief
 **         Main module.
 **         This module contains user's application code.
 */
/*!
 **  @addtogroup main_module main module documentation
 **  @{
 */
/* MODULE main */

/* Including needed modules to compile this module/procedure */
#include "Cpu.h"
#include "Events.h"
#include "RxBuf.h"
#include "MCUC1.h"
#include "CS1.h"
#include "AS1.h"
#include "LED1.h"
#include "LEDpin1.h"
#include "BitIoLdd1.h"
#include "TU2.h"
#include "SpeedStepper.h"
#include "BitIoLdd2.h"
#include "SpeedDirectionPin.h"
#include "BitIoLdd3.h"
#include "SpeedSteperEnable.h"
#include "BitIoLdd4.h"
#include "WinchStepper.h"
#include "BitIoLdd5.h"
#include "WinchDirectionPin.h"
#include "BitIoLdd6.h"
#include "WinchStepperEnable.h"
#include "BitIoLdd7.h"
#include "Magnet.h"
#include "BitIoLdd8.h"
#include "LoadDetection.h"
#include "PTE.h"
/* Including shared modules, which are used for whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
/* User includes (#include below this line is not maintained by Processor Expert) */
//#include "Project_Headers\RS232.h"
#include "Project_Headers\RecievingCommands.h"
#include "Project_Headers\SendingCommands.h"
#include "Project_Headers\SpeedMotor.h"
#include "Project_Headers\WinchMotor.h"
#include "Project_Headers\RS232.h"
#include "RxBuf.h"

extern bool Flag_Recieved;
extern bool Flag_Send;
extern int counterSpeed;
extern int counterWinch;
extern bool pulsSet;
Command_recieve_t my_recieve_command;
Command_send_t my_send_command;
static long counterFrequenceSpeed = 0;
static long counterStep = 0;
static long offsetSpeed = 0;
static long offsetWinch = 0;
static long tempOffset = 0;
static bool loadDetected = 0;
static bool loadFlag = 0;
static double newDistance = 0;
static double oldDistance = 0;
static uint8_t onlyOneReset = 0;

/*lint -save  -e970 Disable MISRA rule (6.3) checking. */
int main(void)
/*lint -restore Enable MISRA rule (6.3) checking. */
{
	/* Write your local variable definition here */

	/*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
	PE_low_level_init();
	/*** End of Processor Expert internal initialization.                    ***/

	/* Write your code here */

	RS232Init();
	initCommand();
	SpeedDirectionPin_SetVal();
	SpeedMotorInit();
	WinchdMotorInit();

	for (;;) {
		if (pulsSet) {
			if (Flag_Recieved == 1) {
				my_recieve_command = Command_bufferPull();
				Flag_Recieved = 0;
				CheckStatusSpeed(my_recieve_command);
				CheckStatusWinch(my_recieve_command);
				offsetSpeed = CalculateOffsetSpeed(my_recieve_command);
				offsetWinch = CalculateOffsetWinch(my_recieve_command);

				if (my_recieve_command.controlSignal & 0x01 == 1) {
					Magnet_SetVal();
				} else {
					Magnet_ClrVal();
				}
			}

			if (offsetSpeed != 0) {
				setcounterFrequenceSpeed(1);
				ClearOnlyOneResetSpeed();
			} else
				setcounterFrequenceSpeed(0);
			if (offsetWinch != 0) {
				setcounterFrequenceWinch(1);
				ClearOnlyOneResetWinch();
			} else
				setcounterFrequenceWinch(0);

			SetDirectionPinSpeed();
			SetDirectionPinWinch();
			my_send_command.driveDistance = StepSpeed();
			my_send_command.winchSpeed = StepWinch();
			if (LoadDetection_GetVal(NULL)) {
				my_send_command.StatusSignal = 0x00;
				if (loadDetected) {
					loadDetected = FALSE;
					loadFlag = TRUE;
				}
			} else {
				my_send_command.StatusSignal = 0x01;
				if (!loadDetected) {
					loadDetected = TRUE;
					loadFlag = TRUE;
				}
			}
			if (SendFlagSpeed() || SendFlagWinch() || loadFlag) {
				ResetSendFlagSpeed();
				ResetSendFlagWinch();
				CommandSend_bufferPut(my_send_command);
				loadFlag = FALSE;
				Flag_Send = 1;
			}
			pulsSet = 0;

		}
		if (Flag_Send == 1) {
			my_send_command = CommandSend_bufferPull();
			CommandSend(my_send_command);
			Flag_Send = 0;
		}
	}

	/*** Don't write any code pass this line, or it will be deleted during code generation. ***/
	/*** RTOS startup code. Macro PEX_RTOS_START is defined by the RTOS component. DON'T MODIFY THIS CODE!!! ***/
#ifdef PEX_RTOS_START
	PEX_RTOS_START(); /* Startup of the selected RTOS. Macro is defined by the RTOS component. */
#endif
	/*** End of RTOS startup code.  ***/
	/*** Processor Expert end of main routine. DON'T MODIFY THIS CODE!!! ***/
	for (;;) {
	}
	/*** Processor Expert end of main routine. DON'T WRITE CODE BELOW!!! ***/
}
