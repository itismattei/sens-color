/*
 * uart.c
 *
 *  Created on: 23/feb/2014
 *      Author: massimo
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <msp430.h>


int fputc(int _c, register FILE *_fp){

	while(!(UCA1IFG&UCTXIFG));
	UCA1TXBUF = (unsigned char) _c;
	return((unsigned char)_c);
}


int fputs(const char *_ptr, register FILE *_fp){

  unsigned int i, len;
  len = strlen(_ptr);
  for(i=0 ; i<len ; i++)  {
    while(!(UCA1IFG&UCTXIFG));
    UCA1TXBUF = (unsigned char) _ptr[i];
  }

  return len;
}
