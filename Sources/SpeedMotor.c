/*
 * SpeedMotor.c
 *
 *  Created on: 04.03.2018
 *      Author: mario
 */

/*
 * 30.9 Umdrehungen = 1000mm
 * 1 umdrehung = 32.36246mm
 * 1 Umdrehung = 400 Steps
 * 1 Step = 1Hz = 0.0809061488
 * 400Hz = 32.36246mm/s
 * 12.36Hz = 1mm/s
 *
 * v(soll) = input*12.36 >> Zahl noch korrekt runden.
 *
 *
 *
 *
 */
#include <stdio.h>
#include "Project_Headers\RecievingCommands.h"
#include "Project_Headers\SendingCommands.h"
#include "Project_Headers\SpeedMotor.h"

static long counterFrequenceSpeed = 0;
static long offset = 0;
static float tempOffset;
static bool onlyOneResetSpeed = 0;
static bool resetActive = 0;
static bool sendFlag = 0;
static enum DirectionSpeed direction;
static enum ModuloValueSpeed moduloValue;
static double newDistance = 0;
static double oldDistance = 0;
static long counterStep = 0;
static int counterModulo = 0;

void SpeedMotorInit() {
	direction = STOPPED;
	newDistance = 0;
	oldDistance = 0;
	counterStep = 0;
}

long CalculateOffsetSpeed(Command_recieve_t my_recieved_command) {
	if (my_recieved_command.driveSpeed != 0) {
		SpeedSteperEnable_ClrVal();
		tempOffset = (0.507 / (0.0002 * my_recieved_command.driveSpeed)); //Offset for 200 Steps = 0.161778; for 400 Steps = 0.081139
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

enum DirectionSpeed GetDirectionSpeed() {
	return direction;
}

void SetDirectionPinSpeed() {
	if (direction == FORWARD) {
		SpeedDirectionPin_SetVal();
	} else
		SpeedDirectionPin_ClrVal();
}
void SetTickToSpeed() {
	SpeedStepper_NegVal();
}
void setcounterFrequenceSpeed(int add) {
	if (add != 0)
		counterFrequenceSpeed += add;
	else
		counterFrequenceSpeed = 0;
}

void SetOnlyOneResetSpeed() {
	onlyOneResetSpeed = 1;
}

void ClearOnlyOneResetSpeed() {
	onlyOneResetSpeed = 0;
}

void CheckResetSpeed(Command_recieve_t my_recieved_command) {
	if ((my_recieved_command.controlSignal & 0x02) != 0) {
		newDistance = 0;
		oldDistance = 0;
		counterStep = 0;
		SetOnlyOneResetSpeed();
		resetActive = 1;
	}
}
int16_t StepSpeed() {

	if (counterFrequenceSpeed >= offset && direction != STOPPED) {
		counterModulo += 10;
		SetTickToSpeed();
		LED1_Neg();
		if (GetDirectionSpeed() == FORWARD) {
			counterStep++;
		} else
			counterStep--;
		newDistance = counterStep * 0.537 / 2;
		if (((newDistance - oldDistance) > 10)
				&& (GetDirectionSpeed() == FORWARD)) {
			oldDistance = newDistance;
			sendFlag = 1;

		} else if (((oldDistance - newDistance) > 10)
				&& (GetDirectionSpeed() == BACKWARD)) {
			oldDistance = newDistance;
			sendFlag = 1;
		}
		counterFrequenceSpeed = 0;
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

bool SendFlagSpeed() {
	return sendFlag;
}

void ResetSendFlagSpeed() {
	sendFlag = 0;
}

