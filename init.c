/*
 * init.c
 *
 *  Created on: 23/dic/2013
 *      Author: robotis
 */

#include <msp430x552x.h>

/// initialize UART 1
/// usata per le comunicazioni col PC via USB seriale
void initUART1(unsigned long bitRate, unsigned long fdco){

	unsigned long baud;

	P4SEL |= BIT4 + BIT5;                       // P4.4,5 alternate function TXD/RXD
	UCA1CTL1 |= UCSWRST;                      // **Put state machine in reset**
	UCA1CTL1 |= UCSSEL_2;                     // SMCLK
	baud = fdco / bitRate;
	if (baud < 255){
		UCA1BR0 = baud;                             // @FDCO MHz: 115200 (see User's Guide)
		UCA1BR1 = 0;
	}
	else{
		/*UCA1BR1 = baud >> 8;
		UCA1BR0 = baud & 0xFF;*/
		UCA1BRW = baud;
	}
	///UCA1MCTL = UCBRS_0 + UCBRF_13 + UCOS16;   // Modln UCBRSx=0, UCBRFx=0,
												// over sampling
	UCA1CTL1 &= ~UCSWRST;                     	// **Initialize USCI state machine**
	UCA1IE |= UCRXIE;                         // Enable USCI_A0 RX interrupt

}


/// initialize UART 0
/// usata per le comunicazioni con altra scheda MSP
void initUART0(unsigned long bitRate, unsigned long fdco){

	unsigned long baud;

		P3SEL |= BIT3 + BIT4;                       // P3.3,4 alternate function TXD/RXD
		UCA0CTL1 |= UCSWRST;                      // **Put state machine in reset**
		UCA0CTL1 |= UCSSEL_2;                     // SMCLK
		baud = fdco / bitRate;
		if (baud < 255){
			UCA0BR0 = baud;                             // @FDCO MHz: 115200 (see User's Guide)
			UCA0BR1 = 0;
		}
		else{
			/*UCA1BR1 = baud >> 8;
			UCA1BR0 = baud & 0xFF;*/
			UCA0BRW = baud;
		}
		///UCA1MCTL = UCBRS_0 + UCBRF_13 + UCOS16;   // Modln UCBRSx=0, UCBRFx=0,
														// over sampling
		UCA0CTL1 &= ~UCSWRST;                     		// **Initialize USCI state machine**
		UCA0IE |= UCRXIE;                         		// Enable USCI_A0 RX interrupt
}



/// initialize port 1
void initPort1(void){

	P1DIR |= BIT0;
	P1OUT &= ~BIT0;
	/// blink led per scopi di debug
	P4DIR = 0x80;
	P4OUT = 0x80;
	P1DIR |= BIT0;
	P1OUT &= ~BIT0;
}

/// ADC init with INTeRnAL REFERENCE
void initADC(unsigned char channel){

	/// set reference register for internal reference: enable, 1.5V
	REFCTL0 = REFMSTR + REFVSEL_0 + REFON;
	/// disable the AD unit
	ADC12CTL0 = 0;
	/// set the AD to have internal ck, not divided, with single channel operation
	ADC12CTL1 =  ADC12SHP;
	/// 12 bit conversion
	ADC12CTL2 = ADC12RES_2;
	if (channel < 12){
		if (channel == 10){
		/// use of internal temperature sensor. It requires 100us sampling time, with a clock of 5MHz:
		/// 100 us * 5MHz = 500, => ADC12SHT0x = 1010b: 512 clock sample time
			ADC12CTL0 |= ADC12SHT0_10 + ADC12ON;
		/// Vref and channel 10 (Temperature sensor)
			ADC12MCTL0 = ADC12SREF_1 + ADC12INCH_10;

		}
		__delay_cycles(300);                      // 35us delay to allow Ref to settle
		                                          // based on default DCO frequency.
		                                          // See Datasheet for typical settle
		                                          // time.
		/// Enable of conversion
		ADC12CTL0 |= ADC12ENC;

	}

}

/// TIMER A1 INIT
/// genera un periodo di 50 ms @
void initTIMER(unsigned long FDCO){

	unsigned int divisore;
	///FDCO		22118400
	/// impostare il timer
	TA1CCTL0 = CCIE;                          		// CCR0 interrupt enabled
	/// 22118400 / 640 = 34560 count; Fck = 22118400 / 64 = 345600;  345600 / 20 = 17280 => 50 ms
	/// oppure 24576000 / 640 = 38400; Fck = 24576000 / 64 = 384000 => 384000 / 20 = 19200 => 50 ms
	divisore = FDCO / 640;
	TA1CCR0 = divisore >> 1;						/// 50 ms @22,1184 MHz oppure 50 ms @ 24,576MHz
	TA1EX0	= 7;									/// source clock divided by 8
	TA1CTL = TASSEL_2 + ID_3 + MC_1 + TACLR;        // SMCLK, upmode, divide by 8, clear TAR
}

///
/// inizualizza lacune funzioni della MCU
///
void initMCU(void){
	;
}

///
/// inizializza la porta 2 a funzionare ad interruzione sul pin 0
/// gestisco ad interruzione la lettura del colore
void initP20int(){
	P2IES &= ~BIT0;                           // P2.0 Lo/Hi edge
	P2IFG &= ~BIT0;                           // P2.0 IFG cleared
	P2IE  |=  BIT0;                           // P2.0 interrupt enabled
}
