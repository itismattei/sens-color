/*
 * sens.h
 *
 *  Created on: 30/mar/2014
 *      Author: robotis
 */

#ifndef SENS_H_
#define SENS_H_

/// struttura dati del sensore di colore

typedef struct _colore{
	int rosso;
	int verde;
	int blu;
	int luminanza;
} colore;

typedef struct _temp{
	float temperatura;
} temp;

/////
#define			COLOR_ADDR			0x29
#define			DEVICE_ID			0x12
#define			TEMP_ADDR			0x33

void readColourSens(colore *);
void readTempSens(temp *tPtr);

#endif /* SENS_H_ */
