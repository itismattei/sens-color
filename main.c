
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
extern volatile unsigned int contatore;
volatile bool scansione;

void main(void) {


	volatile unsigned char valore = 0;
	colore col;
	temp tCelsius;
	// Stop watchdog timer
	WDTCTL = WDTPW | WDTHOLD;

	/// set Internal DCO. This value is 2^15*3^2*5^2*3, so it is a mutiple of 2^9*3^2*5^2 = 115200
	setDCO(FDCO);

	/// initialize UART1 per comunicazioni con PC
	initUART1(19200, FDCO);

	initI2C_B1(FDCO, 300000, COLOR_ADDR);
	initMCU();
	/// init port 1
	initPort1();



	initTIMER(FDCO);

	setUCB0_4Wire();
	// Enable interrupt
	__bis_SR_register(GIE);

	printf("==========================\n\r");
	printf("nodo colore inizializzato! \n\r");

	while(1){
		/// controlla se e' ora di raccogliere il dato
		valore = contatore & 1;
		if ((valore == 0) && (scansione == true)){
			scansione = false;
			readColourSens(&col);

			//readTempSens(&tCelsius);
		}
	}
}
