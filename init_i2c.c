///
/// Registers set for UCB0 I2C interface
/// 2 wires
///
/*
 
	P4.1 : UCB0SDA				<-->
	P4.2 : UCB0SCL				--->
	
*/

#include <msp430x552x.h>
#include "init.h"

#define			MULT_READ			0

///
/// this function initialize I2C on B1 UART
void initI2C_B1(unsigned long int fdco, unsigned long int speed, unsigned char devAddr){
	
	unsigned long int baud;
	/// P4.2 and P4.1 in alternative mode
	//P4SEL |= 0x06;                            // Assign I2C pins to USCI_B1
	if (UCB1STAT & UCBBUSY)
		UCB1CTL1 = 0;
	UCB1CTL1 |= UCSWRST;                      // Enable SW reset
	UCB1CTL0 = UCMST + UCMODE_3 + UCSYNC;     // I2C Master, synchronous mode
	UCB1CTL1 = UCSSEL_2 + UCSWRST;            // Use SMCLK, keep SW reset
	///
	/// e.g if fdco = 24MHz, and baud = 200kHz, baud = 24e6/0,2e6 = 110
	baud = fdco / speed;
	UCB1BR0 = baud & 0xFF;
	UCB1BR1 = (baud >> 8) & 0xFF;

	/*switch(devAddr){
		case GYRO_ADDR:
			UCB1I2CSA = GYRO_ADDR;
		break;

		case ACCEL_ADDR:
			UCB1I2CSA = ACCEL_ADDR;
		break;

		case COMPASS_ADDR:
			UCB1I2CSA = COMPASS_ADDR;
		break;
	}*/

	/// P4.2 and P4.1 in alternative mode
	P4SEL |= 0x06;                            // Assign I2C pins to USCI_B1
	UCB1CTL1 &= ~UCSWRST;                     // Clear SW reset, resume operation
	__delay_cycles(50);
	//UCB0IE |= UCRXIE;                         // Enable RX interrupt
}

///
/// this function read a byte at stored address
unsigned char readI2CByte(){

	unsigned char tmp, flag;
	
	/// send STOP
	//UCB0CTL1 |= UCTXSTP;
	flag = 1;
	do{
		//while (UCB0CTL1 & UCTXSTP);             // Ensure stop condition got sent
		UCB1CTL1 &= ~UCTR;											// clear tx mode
		UCB1CTL1 |= UCTXSTT;                    // I2C start condition
		/// wait start transmission
		while (UCB1CTL1 & UCTXSTT);
		/// and send stop condition (page 629 of UG)
		UCB1CTL1 |= UCTXSTP;                		// Generate I2C stop condition
		while (!(UCB1IFG & UCRXIFG)){
			/// a NACK is produced?
			if (UCB1IFG & UCNACKIFG){
				UCB1IFG &= ~UCNACKIFG;
				/// if yes, reset and repeat the cycle
				break;
			}
			else
				flag = 0;
		}						/// wait a char
	}while(flag);
	
	tmp = UCB1RXBUF;
	return tmp;
}

///
/// this function read a byte at specified address
unsigned char readI2CByteFromAddress(unsigned char reg_address, char *status){

	unsigned char tmp = reg_address;
	unsigned int conteggio = 0;
	*status = 0;
	
	while (UCB1CTL1 & UCTXSTP);             // Ensure stop condition got sent
	/// start with a write session
	UCB1CTL1 |= UCTR + UCTXSTT;				// transmit

	/// wait tx buffer free (it already free)
	while(!(UCB1IFG & UCTXIFG));
	/// transmit addr byte
	UCB1TXBUF = tmp;
	/// a NACK is arrived?  Write cycle isn't finished
	if (UCB1IFG & UCNACKIFG){
		/// send a STOP
		UCB1CTL1 |= UCTXSTP;
		UCB1IFG &= ~UCNACKIFG;
		*status = NACK_ERR;
		return 0;
	}
//
	/// as soon data is transferred from buffer to tx reg
	/// it send a RESTART SIGNAL
	while(!(UCB1IFG & UCTXIFG))
		if (++conteggio > 1000){
			UCB1CTL1 |= UCTXSTP;
			*status = NACK_ERR;
			return 0;
		}

	UCB1CTL1 &= ~UCTR;								// receive
	UCB1CTL1 |= UCTXSTT;                    		// I2C start condition (repeated start)
	UCB1IFG &= ~UCTXIFG;							// clear tx flag
  
	/// wait start transmission
	while (UCB1CTL1 & UCTXSTT);
	/// and send stop condition (page 629 of UG)
	UCB1CTL1 |= UCTXSTP;                				// Generate I2C stop condition
	while (!(UCB1IFG & UCRXIFG));						/// wait a response
	
	tmp = UCB1RXBUF;
	*status = OK;
	return tmp;
}

///
/// this function read N bytes at specified address
char readI2C_N_Byte( unsigned char  reg_address, unsigned char numElm, unsigned char buff[] ){

	unsigned char tmp = reg_address, i;
	//unsigned int numGiri = 0;

	/// prima di tutto occorre vedere se il bus e' impegnato
	if (UCB0STAT & UCBBUSY){
		//// e' impegnato, qualcosa e' andato storto
		P4SEL &= 0xF9;	/// rimetto SCL e SDA in funzione digitale
		P4DIR &= 0xFD;  /// SDA in ingresso
		P4OUT &= 0xFB;  /// metto SCL a 0;
		P4DIR |= 0x4;		/// P4.2 basso (SCL)
		/// in che stato e' SDA?
		while(!(P4IN & BIT1)){
		/// un po' di colpetti su SCL per sbloccare l'automa dello slave
			for (i = 0; i < 10; i++){
				__delay_cycles(500);
				P4OUT ^= BIT2;
			}
			/// SDA alto:
			P4OUT |= BIT1;
			P4DIR |= BIT1;
			__delay_cycles(10);
			P4DIR &= 0xFD;	/// SDA in ingresso
		}
		P4SEL |= 0x6;		/// SCL + SDA
		return BUS_BUSY;
	}

	while (UCB1CTL1 & UCTXSTP);             	// Ensure stop condition got sent
	/// start with a write session
	UCB1CTL1 |= UCTR + UCTXSTT;					// transmit

	/// wait tx buffer free (it already free)
	while(!(UCB1IFG & UCTXIFG));
	/// transmit addr byte, BUT, as we are coming a multiple read, address' MSB must be asserted
	/// (pag. 24 L3GD20 U.G.)
	UCB1TXBUF = tmp + MULT_READ;
	/// a NACK is arrived?  Write cycle isn't finished
	if (UCB1IFG & UCNACKIFG){
  	/// send a STOP
		UCB1CTL1 |= UCTXSTP;
		UCB1IFG &= ~UCNACKIFG;
		return NACK_ERR;
	}

	/// as soon data is transferred from buffer to tx reg
	/// it send a RESTART SIGNAL
	while(!(UCB1IFG & UCTXIFG));
	UCB1CTL1 &= ~UCTR;											// receive
	UCB1CTL1 |= UCTXSTT;                    					// I2C start condition
	UCB1IFG &= ~UCTXIFG;										// clear tx flag

	/// wait start transmission
	while (UCB1CTL1 & UCTXSTT);
	for ( i = 0; i < numElm - 1; i++){
		while (!(UCB1IFG & UCRXIFG));					/// wait a response
		buff[i] = UCB1RXBUF;

			//UCB1CTL1 |= UCTXSTP;                		// Generate I2C stop condition
	}
	UCB1CTL1 |= UCTXSTP;                				// Generate I2C stop condition
	while (!(UCB1IFG & UCRXIFG));						/// wait a response
	buff[i] = UCB1RXBUF;
  /// and send stop condition (page 629 of UG)
  //UCB0CTL1 |= UCTXSTP;                		// Generate I2C stop condition
	return OK;
}

/* 																																				*
 * 															WRITE FUNCTIONS														*
 * 																																				*/
///
/// this function write a byte at specified address
 char writeI2CByte(unsigned char reg_address, unsigned char data){

	unsigned char tmp;
	
	tmp = reg_address;

	while (UCB1CTL1 & UCTXSTP);             // Ensure stop condition got sent
	UCB1CTL1 |= UCTXSTT + UCTR;             // I2C start condition
	/// wait start transmission
	/// As stated in UG, page 627, UCTXSTT is cleared when address is transmitted
	/// from master to slave AND slave ACKNOWLEDGE the address
	//while (UCB0CTL1 & UCTXSTT);
  
	/// wait tx buffer free (it already free because as shown in fig. 28.12,
	/// UCTXIFG is set after a start command
	while(!(UCB1IFG & UCTXIFG));
	/// transmit addr byte
	UCB1TXBUF = tmp;
  ///tmp = (address & 0xff);
  /// however a NACK could happen, if a previous write cycle is not finished,
  /// so wait tx buffer free will be an infinite cycle
	while (!(UCB1IFG & UCTXIFG)){
			/// if NACK is produced, UCTXIFG = 0 and thus we are here
		if (UCB1IFG & UCNACKIFG){
			/// there is a NACK. Fig. 28.12 assert that stop signal must be done
			UCB1IFG &= ~UCNACKIFG;
			// A stop condition is built and we check it next time we call this function
			UCB1CTL1 |= UCTXSTP;
			///Exit from this while cycle and try again
			return NACK_ERR;
		}
	}
	//while(!(UCB0IFG & UCTXIFG));
	/// ok, an ACK is arrived and we can transmit
	/// transmit addr byte
	///UCB1TXBUF = tmp;
 
//  /// transmit data
//  while(!(UCB0IFG & UCTXIFG)){
//	 	/// if NACK is produced, UCTXIFG = 0 and thus we are here
//  	if (UCB0IFG & UCNACKIFG){
//  		/// there is a NACK. Fig. 28.12 assert that stop signal must be done
//  		UCB0IFG &= ~UCNACKIFG;
//  		// A stop condition is built and we check it next time we call this function
//  		UCB0CTL1 |= UCTXSTP;
//  		///Exit from this while cycle and try again
//  		return NACK_ERR;
//  	}
//  }
//  /// transmit data byte
	  UCB1TXBUF = data;
  
	  /// check if data is shifted from buffer to tx register
	  /// as soon data is shifted in tx register, it send a STOP signal
	  while(!(UCB1IFG & UCTXIFG));
	  /// and now it must send stop
	  UCB1CTL1 |= UCTXSTP;                // Generate I2C stop condition
	  return OK;
	
}


