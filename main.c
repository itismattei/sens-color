
/*			GESTIONE SENSORE DI COLORE e TERMOMETRO INFRARED		*/


#include <msp430.h> 
#include <stdio.h>
#include "init.h"

/*
 * main.c
 */
void main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
    // Stop watchdog timer
        WDTCTL = WDTPW | WDTHOLD;

        /// set Internal DCO. This value is 2^15*3^2*5^2*3, so it is a mutiple of 2^9*3^2*5^2 = 115200
        setDCO(FDCO);

        /// initialize UART1 per comunicazioni con PC
        initUART1(19200, FDCO);


        initMCU();
        /// init port 1
        initPort1();
        P1REN = 2;			/// 0 0 0 0 0 0 1 0  pull up
        P1OUT = 2;

        P4DIR = 0x80;
        P4OUT = 0x80;


        initTIMER(FDCO);

        setUCB0_4Wire();
        printf("=========================\n\r");
        printf("CPU inizializzata! \n\r");
        // Enable interrupt
        __bis_SR_register(GIE);

    	/// wait until P1.1 isn't pressed
    	while(P1IN & BIT1);

        while(1){

        	/// Lettura comando arrivato ed invio al modulo motori
        	//receiveCmd();
        	/// misura della distanza percorsa e dal muro
        	//calcolaDistanze();
        	/// richiesta giroscopio
        	//richiediGiroscopio();
        	/// calcolo dell'allineamento rispetto ai dati giroscopio
        	//calcolaAllineamentoAssi();
        	/// mappatura cella
        	//mappaCella();
        	/// controllo dell'avanzamento.


        }

}
