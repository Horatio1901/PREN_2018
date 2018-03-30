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

Command_recieve_t my_recieved_command;
Command_send_t my_send_command;
static long counterFrequenceWinch = 0;
static long offset = 0;
static float tempOffset;
static bool onlyOneResetWinch = 0;
static enum DirectionWinch direction;
static enum ModuloValueWinch moduloValue;
static double newDistance = 0;
static double oldDistance = 0;
static long counterStep = 0;

long CalculateOffsetWinch() {
	my_recieved_command = Command_bufferPull();
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

void CheckResetWinch() {
	if (((my_recieved_command.controlSignal & 0x04) != 0) && !onlyOneResetWinch) {
		newDistance = 0;
		oldDistance = 0;
		counterStep = 0;
		SetOnlyOneResetWinch();
		my_send_command.winchSpeed = 0;
		CommandSend_bufferPut(my_send_command);
	}
}
void StepAndSendWinch() {
	if (counterFrequenceWinch >= offset && counterFrequenceWinch != 0) {
		SetTickToWinch();
		LED1_Neg();
		if (GetDirectionWinch() == FORWARD) {
			counterStep++;
		} else
			counterStep--;
		newDistance = counterStep * 0.507 / 2;
		if (((newDistance - oldDistance) > 10)
				&& (GetDirectionWinch() == FORWARD)) {
			my_send_command.winchSpeed = (int16_t) newDistance;
			CommandSend_bufferPut(my_send_command);
			oldDistance = newDistance;

		} else if (((oldDistance - newDistance) > 10)
				&& (GetDirectionWinch() == BACKWARD)) {
			my_send_command.winchSpeed = (int16_t) newDistance;
			CommandSend_bufferPut(my_send_command);
			oldDistance = newDistance;
		}
		counterFrequenceWinch = 0;
	}
}
