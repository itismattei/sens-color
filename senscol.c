/*
 * senscol.c
 *
 *  Created on: 30/mar/2014
 *      Author: robotis
 */


#include "sens.h"
#include "init.h"
#include <math.h>


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
void readTempSens(temp *tPtr){

	unsigned char buff[8];
	if (readI2C_N_Byte( 0x03, 2, buff) == 0){
		/// ha caricato i valori e quindi può procedere
		tPtr->temperatura = (float)(buff[1] << 8) + buff[0];
	}
}
