/*
 * interrupt.c
 *
 *  Created on: 23/feb/2014
 *      Author: massimo
 */

#include <msp430.h>
#include <stdbool.h>
#include "init.h"

//INTERRUPT

volatile unsigned int contatore = 0;
extern volatile bool scansione;
// Timer1 A0 interrupt service routine
#pragma vector=TIMER1_A0_VECTOR
__interrupt void TIMER1_A0_ISR(void){

	contatore++;
	if ((contatore & 0xF) == 0)
		/// blink del led verde
		P4OUT ^= 0x80;
	if ((contatore & 1) == 0)
		scansione = true;
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


volatile unsigned int contaTacche = 0;
// Port 2 interrupt service routine
#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
{
	contaTacche++;
	P2IFG &= ~BIT0;                         // Clear P2.0 IFG
	P2IE  &= ~BIT0;                         // Clear P2.0 IE
	asm( " nop " );
	P2IE  |= BIT0;                         	// Set P2.0 IE
	asm( " nop " );
}

