/*
 * spi.c
 *
 *  Created on: 08/mar/2014
 *      Author: massimo
 */

///
/// Registers set for UCB1 SPI interface
/// 4 wires, master mode
///
/*

	P5.5 : UCB1CLK				--->
	P5.4 : UCB1MISO				<---
	P3.7 : UCB1MOSI				--->
	P3.6 : UCB1STE (/CS)  --->

*/

#include <msp430x552x.h>
#include "spi.h"

///
/// the /SS signal must be managed by hand (by programmer)
/// so, it is better to use 3 wire SPI and put convenable /SS signal

/// This function is specific for communication between 2 other MCUs,
/// that use P2.1 as CE
void setUCB0_4Wire(){
	//CS this signal is managed by master. UCB0 /CS e' posto sul pin P2.7

	P2SEL |= BIT7;						/// P2.7 UCB1STE

	/// set alternate port function
	P3SEL |= BIT0 + BIT1; 				/// P3.0 (UCB0MOSI) e P3.1 (UCB0MISO)
	P3SEL |= BIT2;						/// P3.2 (UCB0CLK)

	UCB0CTL1 |= UCSWRST;				/// the state machine is in reset mode
	/// msb, ,master, 4 wires, sync, ck inactive low
	//UCB1CTL0 |= UCMSB + UCMST + UCMODE_1 + UCSYNC;

	/// msb,  4 wires, sync, ck inactive low, slave UCB1STE attivo alto
	UCB0CTL0 |= UCMSB + UCMODE_1 + UCSYNC;
	///
	//UCB0CTL1 |= UCSSEL_2; 				/// clock from SMCLK
	/// bit rate: SMCK / 4
	//UCB0BR0 = 0x4;						/// :4
	//UCB0BR1 = 0;

	UCB0CTL1 &= ~UCSWRST;				/// initialize the state machine+
	/// interrupt enable
	UCB0IE |= UCRXIE;
}

///
/// transmit a byte
void B1tx(unsigned char val){
	/// wait until a tx operation end
	while( UCB0STAT & UCBUSY );
	UCB0TXBUF = val;
	/// wait until a tx operation end
	while( UCB0STAT & UCBUSY );
}

///
/// receive a byte
///
unsigned char B1rx(){
	unsigned char read;
	/// wait until a tx operation end
	while( UCB0STAT & UCBUSY );
	UCB0TXBUF = 0;
	/// wait until a tx operation end
	while( UCB0STAT & UCBUSY );
	/// when write operation is finished, received data is ready.
	read = UCB0RXBUF;
	/// close connection
		return read;
}


