/*
 * CommandToVehicle.h
 *
 *  Created on: 28.02.2018
 *      Author: mario
 */

#ifndef SOURCES_COMMANDTOVEHICLE_H_
#define SOURCES_COMMANDTOVEHICLE_H_

#include "PE_Types.h"

#ifndef COMMAND_BUFFER_SIZE
  #define COMMAND_BUFFER_SIZE         64   /* number of elements in the buffer */
#endif

typedef struct {
	int16_t driveSpeed;	/* Speed of the drive motor in mm/s */
	int16_t winchSpeed; /* Speed of the winch motor in mm/s */
	uint8_t controlSignal;	/*control signal Bit[0] 0/1 Electromanet off/on, Bit[1] 0/1 drive motor no Reset/Reset, Bit[2] 0/1 winch motor no Reset/Reset*/
} Command_t;

void initCommand(void);
void Command_bufferPut(Command_t temp);
Command_t Command_bufferPull(void);


#endif /* SOURCES_COMMANDTOVEHICLE_H_ */
