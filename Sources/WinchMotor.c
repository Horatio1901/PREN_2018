/*
 * WinchMotor.c
 *
 *  Created on: 30.03.2018
 *      Author: mario
 */

#include <stdio.h>
#include "Project_Headers\RecievingCommands.h"
#include "Project_Headers\SendingCommands.h"
#include "Project_Headers\WinchMotor.h"


static long counterFrequenceWinch = 0;
static long offset = 0;
static float tempOffset;
static bool onlyOneResetWinch = 0;
static bool resetActive = 0;
static bool sendFlag = 0;
static enum DirectionWinch direction;
static enum ModuloValueWinch moduloValue;
static double newDistance = 0;
static double oldDistance = 0;
static long counterStep = 0;
static int counterModulo = 0;

void WinchdMotorInit() {
	direction = STOPPED;
	newDistance = 0;
	oldDistance = 0;
	counterStep = 0;
}

long CalculateOffsetWinch(Command_recieve_t my_recieved_command) {
	//my_recieved_command = Command_bufferPull();
	if (my_recieved_command.winchSpeed != 0) {
		WinchStepperEnable_ClrVal();
		tempOffset = (0.507 / (0.0002 * my_recieved_command.winchSpeed)); //Offset for 200 Steps = 0.161778; for 400 Steps = 0.081139
		offset = tempOffset;
		tempOffset -= (long) tempOffset;	// Offset new for 200 Steps = 0.507
		if (tempOffset > 0.0 && tempOffset <= 0.2)
			moduloValue = FIFTH1;
		else if (tempOffset > 0.2 && tempOffset <= 0.4)
			moduloValue = FIFTH2;
		else if (tempOffset > 0.4 && tempOffset <= 0.6)
			moduloValue = FIFTH3;
		else if (tempOffset > 0.6 && tempOffset <= 0.8)
			moduloValue = FIFTH4;
		else if (tempOffset > 0.8 && tempOffset <= 1.0)
			moduloValue = FIFTH5;
		if (offset < 0) {
			offset = abs(offset);
			direction = BACKWARD;

		} else
			direction = FORWARD;

	} else {
		offset = 0;
		direction = STOPPED;
	}
	return offset;
}

enum DirectionWinch GetDirectionWinch() {
	return direction;
}

void SetDirectionPinWinch() {
	if (direction == FORWARD) {
		WinchDirectionPin_SetVal();
	} else
		WinchDirectionPin_ClrVal();
}
void SetTickToWinch() {
	WinchStepper_NegVal();
}
void setcounterFrequenceWinch(int add) {
	if (add != 0)
		counterFrequenceWinch += add;
	else
		counterFrequenceWinch = 0;
}

void SetOnlyOneResetWinch() {
	onlyOneResetWinch = 1;
}

void ClearOnlyOneResetWinch() {
	onlyOneResetWinch = 0;
}

void CheckResetWinch(Command_recieve_t my_recieved_command) {
	if ((my_recieved_command.controlSignal & 0x04) != 0) {
		newDistance = 0;
		oldDistance = 0;
		counterStep = 0;
		SetOnlyOneResetWinch();
		resetActive = 1;
	}
}
int16_t StepWinch() {
	if (counterFrequenceWinch >= offset && direction != STOPPED) {
		SetTickToWinch();
		counterModulo += 10;
		LED1_Neg();
		if (GetDirectionWinch() == FORWARD) {
			counterStep++;
		} else
			counterStep--;
		newDistance = counterStep * 0.507 / 2;
		if (((newDistance - oldDistance) > 10)
				&& (GetDirectionWinch() == FORWARD)) {
			oldDistance = newDistance;
			sendFlag = 1;

		} else if (((oldDistance - newDistance) > 10)
				&& (GetDirectionWinch() == BACKWARD)) {
			oldDistance = newDistance;
			sendFlag = 1;
		}
		counterFrequenceWinch = 0;
	}
	if (resetActive) {
		resetActive = 0;
		sendFlag = 1;
		return 0;
	}
	switch (moduloValue) {
	case FIFTH1:
		if (counterModulo == 100) {
			setcounterFrequenceSpeed(-1);
			counterModulo = 0;
		}
		break;
	case FIFTH2:
		if (counterModulo == 30) {
			setcounterFrequenceSpeed(-1);
			counterModulo = 0;
		}
		break;
	case FIFTH3:
		if (counterModulo == 20) {
			setcounterFrequenceSpeed(-1);
			counterModulo = 0;
		}
		break;
	case FIFTH4:
		if (counterModulo % 8 != 0) {
			setcounterFrequenceSpeed(-1);
			counterModulo = 0;
		}
		break;
	case FIFTH5:
		if (counterModulo % 9 != 0) {
			setcounterFrequenceSpeed(-1);
			counterModulo = 0;
		}
		break;
	}
	return (int16_t) newDistance;
}
bool SendFlagWinch() {
	return sendFlag;
}

void ResetSendFlagWinch() {
	sendFlag = 0;
}
