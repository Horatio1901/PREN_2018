/*
 * CommandToVehicle.c
 *
 *  Created on: 28.02.2018
 *      Author: mario
 */

#include "Project_Headers\SendingCommands.h"

static Command_send_t Command_buffer[COMMAND_BUFFER_SIZE];
static uint8_t CoBuf_inIdx; /* input index */
static uint8_t CoBuf_outIdx; /* output index */
bool Flag_Send = 0;

void initCommandSend(void) {
	CoBuf_inIdx = 0;
	CoBuf_outIdx = 0;
}

/*
** ===================================================================
**     Method      :  Command_bufferPut (Command_t temp)
**     Description :
**         Puts a new element into the buffer
**     Parameters  :
**         NAME            - DESCRIPTION
**
**
**
** ===================================================================
*/
void CommandSend_bufferPut(Command_send_t temp) {
	Command_buffer[CoBuf_inIdx] = temp;
}

/*
** ===================================================================
**     Method      :  Command_bufferPut (Command_t temp)
**     Description :
**         Removes an element into the buffer
**     Parameters  :
**         NAME            - DESCRIPTION
**
**
**
** ===================================================================
*/
Command_send_t CommandSend_bufferPull(void) {
	Command_send_t temp;
	temp = Command_buffer[CoBuf_outIdx];
	return temp;
}

/*
** ===================================================================
**     Method      :  CommandSend(Command_send_t temp)
**     Description :
**         Send the given Commando to the UART
**     Parameters  :
**         NAME            - DESCRIPTION
**
**
**
** ===================================================================
*/
void CommandSend(Command_send_t temp){
	Send_Status((char)(temp.driveDistance>>8));
	Send_Status((char)temp.driveDistance);
	Send_Status((char)(temp.winchSpeed>>8));
	Send_Status((char)temp.winchSpeed);
	Send_Status((char)temp.StatusSignal);
}






