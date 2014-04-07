/*
 * interrupt.c
 *
 *  Created on: 23/feb/2014
 *      Author: massimo
 */

#include <msp430.h>
#include <stdbool.h>
#include "init.h"
#include "sens.h"

//INTERRUPT

volatile unsigned int contatore = 0;
extern volatile bool scansione, letturaCampioni, leggiSensCol;
// Timer1 A0 interrupt service routine
#pragma vector=TIMER1_A0_VECTOR
__interrupt void TIMER1_A0_ISR(void){
	int rit = 10;
	contatore++;
	if ((contatore & 0xF) == 0)
		/// blink del led verde
		P4OUT ^= 0x80;
	if ((contatore & 1) == 0){
		/// potrebbe essere ora di campionare il sensore di colore

			/// accende il led
			P8OUT &= ~BIT1;
			while(--rit > 0);
			/// apre la finestra di conteggio degli impulsi proporzionali alla luminosita'
			/// durata della finestra: 50 ms
			scansione = true;
			letturaCampioni = false;
			P2IE  |= BIT0;                         	// Set P2.0 IE
			leggiSensCol = false;
	}
	else{
		/// chiude la finestra di conteggio degli impulsi proporzionali alla luminosita'
		scansione = false;
		letturaCampioni = true;
		/// pulsce il flag in modo che non esegua una eventuale interruzione pendente
		/// dovrebbe essere una azione atomica.
		P2IE  &= ~BIT0;
		P2IFG &= ~BIT0;
		/// accende il led
		P8OUT |= BIT1;

	}

}

///
/// Riceve ad interruzione un carattere via seriale dal PC
///

#pragma vector=USCI_A1_VECTOR
__interrupt void USCI_A1_ISR(void)
{
  switch(__even_in_range(UCA1IV,4))
  {
  case 0:break;                             // Vector 0 - no interrupt
  case 2:                                   // Vector 2 - RXIFG
    while (!(UCA1IFG & UCTXIFG));           // USCI_A0 TX buffer ready?

    UCA1TXBUF = UCA1RXBUF;	        		// TX -> RXed character

    break;
  case 4:break;                             // Vector 4 - TXIFG
  default: break;
  }
}

///
/// interruzione che gestisce la SPI come slave
unsigned char spiBuffer[16], TX_PTR = 0, RX_PTR = 0;
volatile int statoCom = 0;
extern volatile tile *tilePTR;
extern volatile survivor *survPTR;

#pragma vector=USCI_B0_VECTOR
__interrupt void USCI_B0_ISR(void){
	switch(__even_in_range(UCB0IV,4))
	  {
	    case 0:break;                             // Vector 0 - no interrupt
	    case 2:                                   // Vector 2 - RXIFG

	    	if (UCB0RXBUF == GET_DARK_TILE){
	    		/// invia il primo dato del giroscopio
				UCB0TXBUF = (tilePTR->isDark);
			}
	    	else {
	    		if (UCB0RXBUF == GET_SURVIVOR){
					/// GET_SURVIVOR
					UCB0TXBUF = survPTR->isSurvivor;
	    		}
	    		else
	    			UCB0TXBUF = 0xFF;
	    	}

			TX_PTR++;
			TX_PTR &= 7;
	      break;
	    case 4:break;                             // Vector 4 - TXIFG
	    default: break;
	  }
}


volatile unsigned int contaImpulsi = 0;
// Port 2 interrupt service routine
#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
{
	/// se la finestra di conteggio e' attiva esegue il conteggio, altrimenti e' in pausa
	if (scansione == true){
		contaImpulsi++;
		P2IFG &= ~BIT0;                         // Clear P2.0 IFG
		P2IE  &= ~BIT0;                         // Clear P2.0 IE
		asm( " nop " );
		P2IE  |= BIT0;                         	// Set P2.0 IE
		asm( " nop " );
	}
}

