/*
 * Application.h
 *
 *  Created on: 23.02.2018
 *      Author: mario
 */

#ifndef APPLICATION_H_
#define APPLICATION_H_

#include "PE_Types.h"
#include "PE_LDD.h"

#ifndef STATUS_BUFFER_SIZE
  #define STATUS_BUFFER_SIZE         64   /* number of elements in the buffer */
#endif

typedef struct {
  LDD_TDeviceData *handle; /* LDD device handle */
  volatile uint8_t isSent; /* this will be set to 1 once the block has been sent */
  uint8_t rxChar; /* single character buffer for receiving chars */
  uint8_t (*rxPutFct)(uint8_t); /* callback to put received character into buffer */
} UART_Desc;

typedef struct {
	int16_t drivePosition;	/* Position of the drive motor in mm/s */
	int16_t winchPosition; /* Position of the winch motor in mm/s */
	uint8_t controlStatus;	/*Status signal Bit[0] 0/1 Electromanet off/on, Bit[1] 0/1 drive motor no Reset/Reset, Bit[2] 0/1 winch motor no Reset/Reset*/
} Status_t;


void Send_Satus(char test);
void RS232Init(void);

#endif /* APPLICATION_H_ */
