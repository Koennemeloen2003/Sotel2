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
		
	while (1)
	{
		if((UCSR0A & (1<<RXC0))){
		byte input = serial_read_byte();
		
		if (input == 'B')
		{	
			veranderBportpins();
		}
		else if (input == 'D'){ 
			displayGetal();

		}
		else if (input == 'U'){
			sigmentUitzetten();
		}
		else if (input == 'A'){
			sigmentAanzetten();
			
		}
		else if (input=='F'){
			kiesFreq();
			
		}
		else if (input=='N'){
			kiesNoot();
		}
		else if (input=='M'){
			input = serial_read_byte();
			if (input =='1'){
				liedje1();
			}
			serial_write_byte('|');
		}
		else
		{
			
			serial_write_byte('?');
		}
		}
	}
}


