
/*			GESTIONE SENSORE DI COLORE e TERMOMETRO INFRARED		*/


#include <msp430.h> 
#include <stdio.h>
#include <stdbool.h>
#include "init.h"
#include "sens.h"
#include "spi.h"

/*
 * main.c
 */
extern volatile unsigned int contatore, contaImpulsi;
volatile bool scansione, letturaCampioni, leggiSensCol;
volatile tile *tilePTR;
volatile survivor *survPTR;

void main(void) {

	volatile unsigned char valore = 0;
	char stato;
	unsigned char buffer[4];
	colore col;
	tile mattonella;
	temperatura T;
	survivor S;
	int Temp;
	bool presenzaFerito = false;
	tilePTR = &mattonella;
	survPTR = &S;
	// Stop watchdog timer
	WDTCTL = WDTPW | WDTHOLD;

	/// set Internal DCO. This value is 2^15*3^2*5^2*3, so it is a mutiple of 2^9*3^2*5^2 = 115200
	setDCO(FDCO);

	/// initialize UART1 per comunicazioni con PC
	//initUART1(19200, FDCO);

	initI2C_B1(FDCO, 100000, TEMP_ADDR);
	//initMCU();
	/// init port 1
	//initPort1();
	//P1REN =  BIT1;			/// 0 0 0 0 0 0 1 0  pull up
	//P1OUT |= BIT1;
	/// wait until P1.1 isn't pressed
	//while(P1IN & BIT1);


	initTIMER(FDCO);

	//setUCB0_4Wire();

	//initP20int();
	//initSensCol();

	// Enable interrupt
	__bis_SR_register(GIE);

	/// taratura del sensore di colore: genera una finestra da 50 ms e legge il valore del "bianco"

	printf("==========================\n\r");
	printf("nodo colore inizializzato! \n\r");

	//valore = readI2CByteFromAddress(DEVICE_ID, &stato);
	//taraturaSensCol(&col);
	readTemp(&T);
	taraturaTemp(&T);


	while(1){
		/// il dato di luminosità si raccoglie ogni 100 ms

		/// controlla se e' ora di raccogliere il dato di colore

		if (letturaCampioni == true){
			letturaCampioni = false;
		///	scansione = false;
		///	readColourSens(&col);

			//readTempSens(&tCelsius);
			col.luminanza = contaImpulsi;
			contaImpulsi = 0;
			if (col.luminanza - col.bianco < SOGLIA_DARK_TILE){
				mattonella.isDark = IS_DARK_TILE;
			}
			if (col.luminanza - col.bianco > SOGLIA_DARK_TILE + 50){
				mattonella.isDark = NO_DARK_TILE;
			}

		}
		/// adesso deve leggere il sensore di temperatura, ogni 200 ms
		if ((contatore & 3) == 0){
			readTemp(&T);
			if (T.tempRaw - T.T_tar > SOGLIAFERITO)
				S.isSurvivor = IS_SURVIOR;
		}

	}
}
