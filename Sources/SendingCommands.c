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
static uint8_t CoBuf_inSize; /* size data in buffer */
bool Flag_Send = 0;

void initCommandSend(void) {
	CoBuf_inIdx = 0;
	CoBuf_outIdx = 0;
	CoBuf_inSize = 0;
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
	Flag_Send = 1;
	if (CoBuf_inSize < COMMAND_BUFFER_SIZE) {
		Command_buffer[CoBuf_inIdx] = temp;
		CoBuf_inSize++;
		CoBuf_inIdx++;
		if(CoBuf_inIdx == COMMAND_BUFFER_SIZE){
			CoBuf_inIdx =0;
		}
	}
	if (CoBuf_inSize == COMMAND_BUFFER_SIZE) {
		CoBuf_inIdx = 0;
	}
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
	if (CoBuf_inSize > 0) {
		temp = Command_buffer[CoBuf_outIdx];
		CoBuf_inSize--;
		CoBuf_outIdx++;
		if (CoBuf_outIdx == COMMAND_BUFFER_SIZE) {
			CoBuf_outIdx = 0;
		}
	} else {
		return;
	}
	return temp;
}

void CommandSend(Command_send_t temp){
	Send_Status((char)(temp.driveDistance>>8));
	Send_Status((char)temp.driveDistance);
	Send_Status((char)(temp.winchSpeed>>8));
	Send_Status((char)temp.winchSpeed);
	Send_Status((char)temp.StatusSignal);
}






