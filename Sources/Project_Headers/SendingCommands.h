/*
 * SendingCommands.h
 *
 *  Created on: 09.03.2018
 *      Author: mario
 */

#ifndef SOURCES_PROJECT_HEADERS_SENDINGCOMMANDS_H_
#define SOURCES_PROJECT_HEADERS_SENDINGCOMMANDS_H_

#include "PE_Types.h"
#include "Project_Headers\RS232.h"

#ifndef COMMAND_BUFFER_SIZE
  #define COMMAND_BUFFER_SIZE         64   /* number of elements in the buffer */
#endif

typedef struct {
	int16_t driveSpeed;	/* Speed of the drive motor in mm/s */
	int16_t winchSpeed; /* Speed of the winch motor in mm/s */
	uint8_t StatusSignal;	/*control signal Bit[0] 0/1 Cube not detected/is detectet, Bit[1-7] res*/
} Command_send_t;

void initCommandSend(void);
void CommandSend_bufferPut(Command_send_t temp);
Command_send_t CommandSend_bufferPull(void);



#endif /* SOURCES_PROJECT_HEADERS_SENDINGCOMMANDS_H_ */
