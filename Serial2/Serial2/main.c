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

typedef unsigned char byte;




byte display;




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


void init_Serial()
{
	UBRR0L = 0X10;
	UBRR0H = 0;
	
	UCSR0B = (1 << RXEN0)|(1 << TXEN0); // receive & transmit enabled
	
	UCSR0C = 3 << UCSZ00; // 8 data-bits, 1 stop-bit, no parity
	
}

void init_ports()
{
	DDRB = 0xFF;
	DDRC = 0xFF;
	DDRD =0;
	
	PORTB =0;
	PORTC = 0;
	PORTD = 0;
	DDRB |= (1 << PB1); // D9 = output
	
}

void play_tone(uint16_t freq) {
	if (freq == 0) {
		// Stop PWM
		TCCR1A = 0;
		TCCR1B = 0;
		PORTB &= ~(1 << PB1); // Zet D9 laag
		return;
	}

	uint16_t top = (F_CPU / (2UL * 8UL * freq)) - 1;  // Prescaler 8
	ICR1 = top;
	OCR1A = top / 2;  // 50% duty cycle

	// Fast PWM, TOP = ICR1
	TCCR1A = (1 << COM1A1) | (1 << WGM11);
	TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11); // Prescaler = 8
}

const uint16_t toneFreqs[11] = {
	0,      // '0' ? stil
	262,    // '1' ? C4
	294,    // '2' ? D4
	330,    // '3' ? E4
	349,    // '4' ? F4
	392,    // '5' ? G4
	440,    // '6' ? A4
	494,    // '7' ? B4
	523,    // '8' ? C5
	587,	// '9' ? D5
};

const uint16_t getallen[16]={
	0xBE,
	0xA0,
	0x3B,
	0xB9,
	0xA5,
	0x9D,
	0x9F,
	0xB0,
	0xBF,
	0xBD,
	0xB7,
	0x8F,
	0x0B,
	0xAB,
	0x1F,
	0x17
};

const uint16_t bitjes[8]={
	0x01,
	0x02,
	0x04,
	0x08,
	0x10,
	0x20,
	0x40,
	0x80
	};

void liedje1()
{
	play_tone(3);
	_delay_ms(167);
	play_tone(0);
	play_tone(3);
	_delay_ms(167);
	play_tone(0);
	_delay_ms(167);
	play_tone(3);
	_delay_ms(167);
	play_tone(0);
	play_tone(1);
	_delay_ms(167);
	play_tone(0);
	play_tone(3);
	_delay_ms(334);
	play_tone(0);
	play_tone(5);
	_delay_ms(334);
	play_tone(0);
	_delay_ms(334);
}

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
			byte hi = serial_read_nibble();
			byte lo = serial_read_nibble();
			
			PORTB = (hi << 4) | lo;
			
			serial_write_byte('.');
		}
		else if (input == 'D'){
			byte lo = serial_read_nibble();
			display =getallen[lo];
			shift_out_595(display);
			serial_write_byte('!');
		}
		else if (input == 'S'){
			byte lo = serial_read_nibble();
			display = display & ~bitjes[lo-1];
			shift_out_595(display);		
			serial_write_byte('!');
		}
		else if (input == 'H'){
			byte lo = serial_read_nibble();
			display = display | bitjes[lo-1];
			shift_out_595(display);
			serial_write_byte('|');
		}
		else if (input=='P'){
			byte hi = serial_read_nibble();
			byte mid = serial_read_nibble();
			byte lo = serial_read_nibble();
			uint16_t freq =(hi << 8) |(mid << 4) | lo;
			play_tone(freq);
			_delay_ms(300);         // Laat toon even horen
			play_tone(0);           // Stop toon

			serial_write_byte('|');
		}
		else if (input=='O'){
			byte lo = serial_read_nibble();
			play_tone(toneFreqs[lo]);
			_delay_ms(300);         // Laat toon even horen
			play_tone(0);           // Stop toon

			serial_write_byte('|');
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

