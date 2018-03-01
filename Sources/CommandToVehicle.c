/*
 * CommandToVehicle.c
 *
 *  Created on: 28.02.2018
 *      Author: mario
 */

#include "CommandToVehicle.h"

static Command_t Command_buffer[COMMAND_BUFFER_SIZE];
static uint8_t CoBuf_inIdx; /* input index */
static uint8_t CoBuf_outIdx; /* output index */
static uint8_t CoBuf_inSize; /* size data in buffer */

void initCommand(void) {
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

void Command_bufferPut(Command_t temp) {
	if (CoBuf_inSize < COMMAND_BUFFER_SIZE) {
		Command_buffer[CoBuf_inIdx] = temp;
		CoBuf_inSize++;
		CoBuf_inIdx++;
	}
	if (CoBuf_inSize == COMMAND_BUFFER_SIZE) {
		CoBuf_inIdx = 0;
	}
}

Command_t Command_bufferPull(void) {
	Command_t temp;
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

uint8_t getBufferSoze(void) {
	return CoBuf_inSize;
}

