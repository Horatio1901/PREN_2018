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

Command_recieve_t my_recieved_command;
Command_send_t my_send_command;
static long counterFrequenceSpeed = 0;
static long offset = 0;
static float tempOffset;
static bool onlyOneResetSpeed = 0;
enum Direction direction;
enum ModuloValue moduloValue;
static double newDistance = 0;
static double oldDistance = 0;
static long counterStep = 0;

long CalculateOffsetSpeed() {
	my_recieved_command = Command_bufferPull();
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
	}
	return offset;
}

enum Direction GetDirectionSpeed() {
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

void CheckResetSpeed() {
	if (((my_recieved_command.controlSignal & 0x02) != 0) && !onlyOneResetSpeed) {
		newDistance = 0;
		oldDistance = 0;
		counterStep = 0;
		SetOnlyOneResetSpeed();
		my_send_command.driveDistance = 0;
		CommandSend_bufferPut(my_send_command);
	}
}
void StepAndSendSpeed() {
	if (counterFrequenceSpeed >= offset && counterFrequenceSpeed != 0) {
		SetTickToSpeed();
		LED1_Neg();
		if (GetDirectionSpeed() == FORWARD) {
			counterStep++;
		} else
			counterStep--;
		newDistance = counterStep * 0.507 / 2;
		if (((newDistance - oldDistance) > 10)
				&& (GetDirectionSpeed() == FORWARD)) {
			my_send_command.driveDistance = (int16_t) newDistance;
			CommandSend_bufferPut(my_send_command);
			oldDistance = newDistance;

		} else if (((oldDistance - newDistance) > 10)
				&& (GetDirectionSpeed() == BACKWARD)) {
			my_send_command.driveDistance = (int16_t) newDistance;
			CommandSend_bufferPut(my_send_command);
			oldDistance = newDistance;
		}
		counterFrequenceSpeed = 0;
	}
}

