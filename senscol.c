/*
 * senscol.c
 *
 *  Created on: 30/mar/2014
 *      Author: robotis
 */

#include <msp430.h>
#include "sens.h"
#include "init.h"
#include <math.h>
#include <msp430.h>
#include <stdbool.h>


///
/// legge il sensore di colore
void readColourSens(colore *colPtr){
	unsigned char buff[8];
	/*if (readI2C_N_Byte( 0x01, 8, buff) == 0){
		/// ha caricato i valori e quindi può procedere
		colPtr->rosso = (int)(buff[1] << 8) + buff[0];
		colPtr->verde = (int)(buff[3] << 8) + buff[2];
		colPtr->blu = (int)(buff[5] << 8) + buff[4];
		colPtr->luminanza = (int)(buff[7] << 8) + buff[6];
	}*/
}

///
/// normalizza i colori sencondo norma 2
void normalizzaColori(colore *colPtr){

	float v[3], n = 0.0;
	int i;

	v[0] = (float) colPtr->rosso;
	v[1] = (float) colPtr->verde;
	v[2] = (float) colPtr->blu;

	for ( i = 0; i < 3; i++)
		n += v[i] * v[i];
	n = sqrt(n);

	/// normalizzazione dei colori
	colPtr->rosso /= n;
	colPtr->verde /= n;
	colPtr->blu   /= n;
}


///
/// legge il sensore di temperatura
void readTempSens(temperatura *tPtr){

	unsigned char buff[8];
	if (readI2C_N_Byte( 0x03, 2, buff) == 0){
		/// ha caricato i valori e quindi può procedere
		tPtr->tempRaw = (float)(buff[1] << 8) + buff[0];
	}
}


void initSensCol(){
	/// uso del pin P2.0 per ricevere i segnali dell'encoder ad interruzione
	    P2IES &= ~BIT0;                           // P2.0 Lo/Hi edge
	    P2IFG &= ~BIT0;                           // P2.0 IFG cleared
	    P2IE  |=  BIT0;                           // P2.0 interrupt enabled
}


extern volatile unsigned int contatore, contaImpulsi;
extern volatile bool scansione, letturaCampioni;

///
/// taratura del sensore
void taraturaSensCol(colore *colPtr){

	contatore = 1;
	while (contatore == 1);
	/// entrato qui vuol dire che sto leggendo il sensore di colore
	while (letturaCampioni == false);
	/// ha termitato la finestra ed il dato e' pronto.
	colPtr->bianco = contaImpulsi;
}


///
/// taratura in un punto noto della temepratura
void taraturaTemp(temperatura *tempPtr){
	/// taratura del sensore

	readTemp(tempPtr);
	if (tempPtr->ok == 0){
		/// ok
		tempPtr->T_tar = tempPtr->tempRaw;
	}

}

///
/// lettura della temperatura
void readTemp(temperatura *tempPtr){

	unsigned char buffer[4];
	char valore;
	/// per leggere la temperatura deve chiamare il sensore su I2C
	valore = readI2C_N_Byte( 0x07, 3, buffer);
	if (valore == 0){
		tempPtr->tempRaw = (buffer[1] << 8) + buffer[0];
		tempPtr->ok = 0;
	}
	else
		tempPtr->ok = 1;
}

