/*
 * interrupt.c
 *
 *  Created on: 23/feb/2014
 *      Author: massimo
 */

#include <msp430.h>
#include "init.h"

//INTERRUPT

volatile unsigned int contatore = 0;

// Timer1 A0 interrupt service routine
#pragma vector=TIMER1_A0_VECTOR
__interrupt void TIMER1_A0_ISR(void)
{
	//P1OUT ^= 1;
	contatore++;
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

