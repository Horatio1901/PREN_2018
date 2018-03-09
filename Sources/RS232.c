/*
 * Application.c
 *
 *  Created on: 23.02.2018
 *      Author: mario
 */

#include "RS232.h"
#include "RxBuf.h"
#include "AS1.h"
#include <stdio.h>

static UART_Desc deviceData;
static Status_t Status_buffer[STATUS_BUFFER_SIZE];
static uint8_t CoBuf_inIdx; /* input index */
static uint8_t CoBuf_outIdx; /* output index */
static uint8_t CoBuf_inSize; /* size data in buffer */

static void SendChar(char ch, UART_Desc *desc) {
	desc->isSent = FALSE; /* this will be set to 1 once the block has been sent */
	while (AS1_SendBlock(desc->handle, (LDD_TData*) &ch, 1) != ERR_OK) {
	} /* Send char */
	while (!desc->isSent) {
	} /* wait until we get the green flag from the TX interrupt */
}

static void SendString(const unsigned char *str, UART_Desc *desc) {
	while (*str != '\0') {
		SendChar(*str++, desc);
	}
}

/*
 ** ===================================================================
 **     Method      :  Status_bufferPut (Status_t temp)
 **     Description :
 **         Puts a new element into the buffer
 **     Parameters  :
 **         NAME            - DESCRIPTION
 **
 **
 **
 ** ===================================================================
 */

void Status_bufferPut(Status_t temp) {
	if (CoBuf_inSize < STATUS_BUFFER_SIZE) {
		Status_buffer[CoBuf_inIdx] = temp;
		CoBuf_inSize++;
		CoBuf_inIdx++;
	}
	if (CoBuf_inSize == STATUS_BUFFER_SIZE) {
		CoBuf_inIdx = 0;
	}
}

/*
 ** ===================================================================
 **     Method      :  Status_bufferPut (Status_t temp)
 **     Description :
 **         Removes an element into the buffer
 **     Parameters  :
 **         NAME            - DESCRIPTION
 **
 **
 **
 ** ===================================================================
 */

Status_t Status_bufferPull(void) {
	Status_t temp;
	if (CoBuf_inSize > 0) {
		temp = Status_buffer[CoBuf_outIdx];
		CoBuf_inSize--;
		CoBuf_outIdx++;
		if (CoBuf_outIdx == STATUS_BUFFER_SIZE) {
			CoBuf_outIdx = 0;
		}
	} else {
		return;
	}
	return temp;
}

void RS232Init(void) {
	CoBuf_inIdx = 0;
	CoBuf_outIdx = 0;
	CoBuf_inSize = 0;
	/* initialize struct fields */
	deviceData.handle = AS1_Init(&deviceData);
	deviceData.isSent = FALSE;
	deviceData.rxChar = '\0';
	deviceData.rxPutFct = RxBuf_Put;
	/* set up to receive RX into input buffer */
	RxBuf_Init(); /* initialize RX buffer */
	/* Set up ReceiveBlock() with a single byte buffer. We will be called in OnBlockReceived() event. */
	while (AS1_ReceiveBlock(deviceData.handle, (LDD_TData *) &deviceData.rxChar,
			sizeof(deviceData.rxChar)) != ERR_OK) {
	} /* initial kick off for receiving data */
}

char Convert_uint16ToChar(uint16_t speed) {

}

void Send_Status(char test) {
	int i;
	SendChar(test, &deviceData);
	//for(;;) {
	//if (RxBuf_NofElements()!=0) {
	//SendString((unsigned char*)"echo: ", &deviceData);
	// while (RxBuf_NofElements()!=0) {
	// unsigned char ch;

	//(void)RxBuf_Get(&ch);
	//SendChar(ch, &deviceData);
	//}
	//SendString((unsigned char*)"\r\n", &deviceData);
	//}
	// }
}

