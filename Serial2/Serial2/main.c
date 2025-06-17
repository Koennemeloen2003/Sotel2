/*
 * Serial2.c
 *
 * Created: 20/05/2025 11:07:42
 * Author : kaben
 */ 

#define F_CPU 16000000UL
#include <avr/interrupt.h>
#include <math.h>
#include <stdint.h>
#include <avr/sfr_defs.h>
#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>
#include "Functies.h"
#include "Vars.h"



int main(void)
{
	init_Serial();
	init_ports();
	SPI_SlaveInit();
		
	while (1)
	{
		if((UCSR0A & (1<<RXC0))){
		byte input = serial_read_byte();
		if (input == 'D'){ 
			byte lo = serial_read_nibble();
			displayGetal(lo);
		}
		else if (input == 'U'){
			byte lo = serial_read_nibble();
			sigmentUitzetten(lo);
		}
		else if (input == 'A'){
			byte lo = serial_read_nibble();
			sigmentAanzetten(lo);
			
		}
		else if (input=='F'){
			byte hi = serial_read_nibble();
			byte mid = serial_read_nibble();
			byte lo = serial_read_nibble();
			kiesFreq((hi << 8) |(mid << 4) | lo);
			
		}
		else if (input=='N'){
			byte lo = serial_read_nibble();
			kiesNoot(lo);
		}
		else if (input=='B'){
			byte hi = serial_read_nibble();
			byte lo = serial_read_nibble();
			display = (hi << 4) | lo;
			byteToDisplay(display);
		}
		else if (input=='M'){
			input = serial_read_byte();
			if (input =='1'){
				liedje1();
			}
		}
			serial_write_byte('|');
		else
		{
			
			serial_write_byte('?');
		}
		}
		if ((SPSR & (1<<SPIF))){
			byte input = SPDR;
			if (input == 'D'){
				byte lo = SPI_read_byte();
				displayGetal(lo);
			}
			
		}
	}
}


