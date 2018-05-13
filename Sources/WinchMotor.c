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
static float round;
static bool onlyOneResetWinch = 0;
static bool resetActive = 0;
static bool sendFlag = 0;
static enum DirectionWinch direction;
static enum ModuloValueWinch moduloValue;
static enum StatusWinch statusWinch;
static double newDistance = 0;
static double oldDistance = 0;
static long counterStep = 0;
static int counterModulo = 0;

void WinchdMotorInit() {
	direction = STOPPEDWINCH;
	newDistance = 0;
	oldDistance = 0;
	counterStep = 0;
	statusWinch = RESETWINCH;
}

void CheckStatusWinch(Command_recieve_t my_recieved_command) {
	if (my_recieved_command.winchSpeed == 0) {
		statusWinch = STOPANDWAITWINCH;
		if ((my_recieved_command.controlSignal & 0x04) != 0) {
			newDistance = 0;
			oldDistance = 0;
			counterStep = 0;
			SetOnlyOneResetWinch();
			resetActive = 1;
			statusWinch = RESETWINCH;
		}
	} else {
		statusWinch = DRIVEWINCH;
	}
}

long CalculateOffsetWinch(Command_recieve_t my_recieved_command) {
	if (statusWinch == DRIVEWINCH) {
		WinchStepperEnable_ClrVal();
		if (my_recieved_command.winchSpeed > 0) {
			direction = FORWARDWINCH;
			tempOffset = (0.15707 / (0.0002 * my_recieved_command.winchSpeed)); //Offset for 200 Steps = 0.161778; for 400 Steps = 0.081139
			offset = (int) tempOffset;
			round = (float) (tempOffset - offset);
		} else if (my_recieved_command.winchSpeed < 0) {
			direction = BACKWARDWINCH;
			tempOffset = (0.15707 / (0.0002 * my_recieved_command.winchSpeed)); //Offset for 200 Steps = 0.161778; for 400 Steps = 0.081139
			offset = (int) abs(tempOffset);
			round = (float) abs(tempOffset - offset);
		}
		if (round > 0.0 && round <= 0.2)
			moduloValue = FIFTH1WINCH;
		else if (round > 0.2 && round <= 0.4)
			moduloValue = FIFTH2WINCH;
		else if (round > 0.4 && round <= 0.6)
			moduloValue = FIFTH3WINCH;
		else if (round > 0.6 && round <= 0.8)
			moduloValue = FIFTH4WINCH;
		else if (round > 0.8 && round <= 1.0)
			moduloValue = FIFTH5WINCH;
	} else if (statusWinch == RESETWINCH || statusWinch == STOPANDWAITWINCH) {
		return 0;
	}
	return offset;
}

enum DirectionWinch GetDirectionWinch() {
	return direction;
}

void SetDirectionPinWinch() {
	if (direction == FORWARDWINCH) {
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

int16_t StepWinch() {
	if (statusWinch == DRIVEWINCH) {
		if (counterFrequenceWinch >= offset && direction != STOPPEDWINCH) {
			SetTickToWinch();
			counterModulo += 10;
			LED1_Neg();
			if (GetDirectionWinch() == FORWARDWINCH) {
				counterStep++;
			} else
				counterStep--;
			newDistance = counterStep * 0.15707 / 2;	//0.0397
			if (((newDistance - oldDistance) > 10)
					&& (GetDirectionWinch() == FORWARDWINCH)) {
				oldDistance = newDistance;
				sendFlag = 1;

			} else if (((oldDistance - newDistance) > 10)
					&& (GetDirectionWinch() == BACKWARDWINCH)) {
				oldDistance = newDistance;
				sendFlag = 1;
			}
			counterFrequenceWinch = 0;
		}
		switch (moduloValue) {
		case FIFTH1WINCH:
			if (counterModulo == 100) {
				setcounterFrequenceSpeed(-1);
				counterModulo = 0;
			}
			break;
		case FIFTH2WINCH:
			if (counterModulo == 30) {
				setcounterFrequenceSpeed(-1);
				counterModulo = 0;
			}
			break;
		case FIFTH3WINCH:
			if (counterModulo == 20) {
				setcounterFrequenceSpeed(-1);
				counterModulo = 0;
			}
			break;
		case FIFTH4WINCH:
			if (counterModulo % 8 != 0) {
				setcounterFrequenceSpeed(-1);
				counterModulo = 0;
			}
			break;
		case FIFTH5WINCH:
			if (counterModulo % 9 != 0) {
				setcounterFrequenceSpeed(-1);
				counterModulo = 0;
			}
			break;
		}
		return (int16_t) (newDistance);
	} else if (statusWinch == RESETWINCH) {
		if (resetActive) {
			resetActive = 0;
			sendFlag = 1;
		}
		return 0;
	} else if (statusWinch == STOPANDWAITWINCH) {
		return newDistance;
	}
}
bool SendFlagWinch() {
	return sendFlag;
}

void ResetSendFlagWinch() {
	sendFlag = 0;
}
