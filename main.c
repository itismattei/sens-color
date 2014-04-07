
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
volatile bool scansione, letturaCampioni;

void main(void) {


	volatile unsigned char valore = 0;
	char stato;
	unsigned char buffer[4];
	colore col;
	temp tCelsius;
	// Stop watchdog timer
	WDTCTL = WDTPW | WDTHOLD;

	/// set Internal DCO. This value is 2^15*3^2*5^2*3, so it is a mutiple of 2^9*3^2*5^2 = 115200
	setDCO(FDCO);

	/// initialize UART1 per comunicazioni con PC
	initUART1(19200, FDCO);

	initI2C_B1(FDCO, 100000, TEMP_ADDR);
	initMCU();
	/// init port 1
	initPort1();
	P1REN =  BIT1;			/// 0 0 0 0 0 0 1 0  pull up
	P1OUT |= BIT1;
	/// wait until P1.1 isn't pressed
	while(P1IN & BIT1);


	initTIMER(FDCO);

	setUCB0_4Wire();

	initP20int();

	// Enable interrupt
	__bis_SR_register(GIE);


	printf("==========================\n\r");
	printf("nodo colore inizializzato! \n\r");

	//valore = readI2CByteFromAddress(DEVICE_ID, &stato);



	while(1){
		/// controlla se e' ora di raccogliere il dato
		valore = contatore & 1;
		if (letturaCampioni == true){
			letturaCampioni = false;
		///	scansione = false;
		///	readColourSens(&col);

			//readTempSens(&tCelsius);
			col.luminanza = contaImpulsi;
			contaImpulsi = 0;
		}
		/// adesso deve leggere il sensore di temperatura
		valore = readI2C_N_Byte( 0x07, 3, buffer);

	}
}
