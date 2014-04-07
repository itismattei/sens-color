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
	int bianco;
} colore;

typedef struct _temp{
	float 	Temp;
	int 	tempRaw;
	int 	T_tar;
	int		ok;
} temperatura;

/////
#define			COLOR_ADDR			0x29
#define			DEVICE_ID			0x12
#define			TEMP_ADDR			0x00

void readColourSens(colore *);
void readTempSens(temperatura *tPtr);
void taraturaTemp(temperatura *tempPtr);
void readTemp(temperatura *tempPtr);


#define 	SOGLIAFERITO 			600

#endif /* SENS_H_ */
