/*
 * init.h
 *
 *  Created on: 23/dic/2013
 *      Author: massimo
 */



#ifndef INIT_H_
#define INIT_H_

unsigned long int setDCO(unsigned long int mhz);
void initPort1(void);

void initUART1(unsigned long bitRate, unsigned long fdco);
void initADC(unsigned char channel);
void initTIMER(unsigned long);

double leggiTemepratura();
void double2string(double a, char* b);

void receiveCommand();

void initMCU(void);

void setUCB0_4Wire();

void initI2C_B1(unsigned long int fdco, unsigned long int speed, unsigned char devAddr);
unsigned char readI2CByteFromAddress(unsigned char reg_address, char *status);
char writeI2CByte(unsigned char data, unsigned char reg_address);
char writeI2CSingleByte(unsigned char data);
char readI2C_N_Byte( unsigned char reg_address, unsigned char numElm, unsigned char buff[] );


///////////////////////////////////////////////
#define		NACK_ERR		1
#define		OK				0
#define		BUS_BUSY		2
#define		NOT_PRESENT		3
#define		OFF				4
#define		ERROR			5


//////////////////////////////////////////////
///
/// CLOCK FREQUENCY
///
//////////////////////////////////////////////
/* MHz */
#define		Fdc0	22118400
#define		Fdc01	24576000
///
/// FREQUENZA ATTUALE
#define		FDCO	24576000


#endif /* INIT_H_ */
