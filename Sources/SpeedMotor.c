/*
 * SpeedMotor.c
 *
 *  Created on: 04.03.2018
 *      Author: mario
 */

/*¨
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

int16_t speed;
int16_t frequence;

void claculate_Frequence(int16_t speed) {
	frequence = speed*12.36;
}



