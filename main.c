
/*			GESTIONE SENSORE DI COLORE e TERMOMETRO INFRARED		*/


#include <msp430.h> 
#include <stdio.h>
#include "init.h"
#include "sens.h"

/*
 * main.c
 */
extern volatile unsigned int contatore;

void main(void) {

	colore col;
	temp tCelsius;
	// Stop watchdog timer
	WDTCTL = WDTPW | WDTHOLD;

	/// set Internal DCO. This value is 2^15*3^2*5^2*3, so it is a mutiple of 2^9*3^2*5^2 = 115200
	setDCO(FDCO);

	/// initialize UART1 per comunicazioni con PC
	initUART1(19200, FDCO);

	initI2C_B1(FDCO, 400000, COLOR_ADDR);
	initMCU();
	/// init port 1
	initPort1();
	P1REN = 2;			/// 0 0 0 0 0 0 1 0  pull up
	P1OUT = 2;

	P4DIR = 0x80;
	P4OUT = 0x80;


	initTIMER(FDCO);

	setUCB0_4Wire();
	printf("==========================\n\r");
	printf("nodo colore inizializzato! \n\r");
	// Enable interrupt
	__bis_SR_register(GIE);

	/// wait until P1.1 isn't pressed
	while(P1IN & BIT1);

	while(1){
		/// controlla se e' ora di raccogliere il dato
		if (contatore & 1){
			readColourSens(&col);
			readTempSens(&tCelsius);
		}
	}
}
