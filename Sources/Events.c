/* ###################################################################
 **     Filename    : Events.c
 **     Project     : Laufkatze
 **     Processor   : MKL25Z128VLK4
 **     Component   : Events
 **     Version     : Driver 01.00
 **     Compiler    : GNU C Compiler
 **     Date/Time   : 2018-02-16, 12:25, # CodeGen: 0
 **     Abstract    :
 **         This is user's event module.
 **         Put your event handler code here.
 **     Contents    :
 **         Cpu_OnNMIINT - void Cpu_OnNMIINT(void);
 **
 ** ###################################################################*/
/*!
 ** @file Events.c
 ** @version 01.00
 ** @brief
 **         This is user's event module.
 **         Put your event handler code here.
 */
/*!
 **  @addtogroup Events_module Events module documentation
 **  @{
 */
/* MODULE Events */

#include "Cpu.h"
#include "Project_Headers\Events.h"

#ifdef __cplusplus
extern "C" {
#endif 

/* User includes (#include below this line is not maintained by Processor Expert) */
#include "Project_Headers\RS232.h"
#include "Project_Headers\RecievingCommands.h"
#include "RxBuf.h"

static short dataByteCounter = 0;
static Command_recieve_t temp_Command;
static RxBuf_ElementType *temp_ElementLow;
static RxBuf_ElementType *temp_ElementHigh;
static uint8_t temp1;
static uint8_t temp2;
/*
 ** ===================================================================
 **     Event       :  Cpu_OnNMIINT (module Events)
 **
 **     Component   :  Cpu [MKL25Z128LK4]
 */
/*!
 **     @brief
 **         This event is called when the Non maskable interrupt had
 **         occurred. This event is automatically enabled when the [NMI
 **         interrupt] property is set to 'Enabled'.
 */
/* ===================================================================*/
void Cpu_OnNMIINT(void) {
	/* Write your code here ... */
}

/*
 ** ===================================================================
 **     Event       :  AS1_OnBlockReceived (module Events)
 **
 **     Component   :  AS1 [Serial_LDD]
 */
/*!
 **     @brief
 **         This event is called when the requested number of data is
 **         moved to the input buffer.
 **     @param
 **         UserDataPtr     - Pointer to the user or
 **                           RTOS specific data. This pointer is passed
 **                           as the parameter of Init method.
 */
/* ===================================================================*/
void AS1_OnBlockReceived(LDD_TUserData *UserDataPtr) {
	RxBuf_ElementType chr[100];
	UART_Desc *ptr = (UART_Desc*) UserDataPtr;

	if (dataByteCounter < 5) {
		dataByteCounter++;
		(void) ptr->rxPutFct(ptr->rxChar); /* put received character into buffer */
		if (dataByteCounter == 5) {
			RxBuf_Get(&temp_ElementHigh);
			temp1 = temp_ElementHigh;
			RxBuf_Get(&temp_ElementLow);
			temp2 = temp_ElementLow;
			temp_Command.driveSpeed = (temp1 << 8
					| temp2); // Write the first 2 bytes from the Buffer to the driveSpeed
			RxBuf_Get(&temp_ElementHigh);
			temp1 = temp_ElementHigh;
			RxBuf_Get(&temp_ElementLow);
			temp2 = temp_ElementLow;
			temp_Command.winchSpeed = (temp1 << 8
					| temp2); // Write the second 2 bytes from the Buffer to the winchSpeed
			RxBuf_Get(&temp_ElementLow);
			temp1 = temp_ElementLow;
			temp_Command.controlSignal = temp1; // Write Byte 5 to ControlSignal
			Command_bufferPut(temp_Command);
			dataByteCounter = 0;
		}
	}
	(void) AS1_ReceiveBlock(ptr->handle, (LDD_TData *) &ptr->rxChar,
			sizeof(ptr->rxChar));
}

/*((highByte << 8 ) | lowByte);
 ** ===================================================================
 **     Event       :  AS1_OnBlockSent (module Events)
 **
 **     Component   :  AS1 [Serial_LDD]
 */
/*!
 **     @brief
 **         This event is called after the last character from the
 **         output buffer is moved to the transmitter.
 **     @param
 **         UserDataPtr     - Pointer to the user or
 **                           RTOS specific data. This pointer is passed
 **                           as the parameter of Init method.
 */
/* ===================================================================*/
void AS1_OnBlockSent(LDD_TUserData *UserDataPtr) {
	UART_Desc *ptr = (UART_Desc*) UserDataPtr;

	ptr->isSent = TRUE; /* set flag so sender knows we have finished */
}

/* END Events */

#ifdef __cplusplus
} /* extern "C" */
#endif 

/*!
 ** @}
 */
/*
 ** ###################################################################
 **
 **     This file was created by Processor Expert 10.5 [05.21]
 **     for the Freescale Kinetis series of microcontrollers.
 **
 ** ###################################################################
 */
