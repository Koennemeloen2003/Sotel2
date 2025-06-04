/*
 * Serial2.c
 *
 * Created: 20/05/2025 11:07:42
 * Author : kaben
 */ 

#define F_CPU 16000000UL

#include <avr/sfr_defs.h>
#include <avr/io.h>

typedef unsigned char byte;

inline void serial_write_byte(byte c)
{
	loop_until_bit_is_set(UCSR0A, UDRE0); UDR0 = c;
}

inline byte serial_read_byte()
{
	loop_until_bit_is_set(UCSR0A, RXC0); return UDR0;
}

byte serial_read_nibble()
{
	byte nibble = serial_read_byte();
	
	if ((nibble >= 'A') && (nibble <= 'F'))
	{
		nibble -= 'A' - 10;
	}
	else if ((nibble >= 'a') && (nibble <= 'f'))
	{
		nibble -= 'a' - 10;
	}
	else if ((nibble >= '0') && (nibble <= '9'))
	{
		nibble -= '0';
	}
	else
	{
		nibble = 0;
	}
	
	return nibble;
}


int main(void)
{

	UBRR0L = 0X10;
	UBRR0H = 0;
	
	UCSR0B = (1 << RXEN0)|(1 << TXEN0); // receive & transmit enabled
	
	UCSR0C = 3 << UCSZ00; // 8 data-bits, 1 stop-bit, no parity
	
	DDRB = 0xFF;
	DDRC = 0xFF;
	DDRD |= (1 << DDD6);
	
	PORTB =0;
	PORTC = 0;
	PORTD = 0;
	
	OCR0A = 0;
	// set PWM for 50% duty cycle
	
	TCCR0A |= (1 << COM0A1);
	// set none-inverting mode
	
	TCCR0A |= (1 << WGM01) | (1 << WGM00);
	// set fast PWM Mode
	
	TCCR0B |= (1 << CS01);
	// set prescaler to 8 and starts PWM
	
	while (1)
	{
		byte input = serial_read_byte();
		
		if (input == 'B')
		{
			byte hi = serial_read_nibble();
			byte lo = serial_read_nibble();
			
			PORTB = (hi << 4) | lo;
			
			serial_write_byte('.');
		}
		else if (input == 'S'){
			byte hi = serial_read_nibble();
			byte lo = serial_read_nibble();
			
			shift_out_595((hi << 4) | lo);
			serial_write_byte('!');
		}
		else if (input=='P'){
			byte hi = serial_read_nibble();
			byte lo = serial_read_nibble();
			OCR0A = (hi << 4) | lo;
			serial_write_byte('|');
		}
		else
		{
			serial_write_byte('?');
		}
	}
}

void shift_out_595(uint8_t data) {
	data = ~data;
	for (int i = 7; i >= 0; i--) {
		// Set DATA bit
		if (data & (1 << i))
		PORTC |= (1 << PC0);   // DATA high
		else
		PORTC &= ~(1<<PC0);  // DATA low

		// Pulse CLOCK
		PORTC |= (1<<PC1);
		PORTC &= ~(1<<PC1);
	}

	// Pulse LATCH
	PORTC |=(1<<PC2);
	PORTC &=~(1<<PC2);
	PORTC &= ~(1<<PC0);
}

