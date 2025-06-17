/*
 * Functies.h
 *
 * Created: 17-6-2025 12:52:24
 *  Author: kaben
 */ 


#ifndef FUNCTIES_H_
#define FUNCTIES_H_

#include "Vars.h"

void init_ports()
{
	DDRC = 0xFF;
	DDRD =0;
	
	PORTB =0;
	PORTC = 0;
	PORTD = 0;
	DDRB |= (1 << PB1); // D9 = output
	
}


void SPI_SlaveInit(void)
{
	DDRB &= ~(1<<DDB3); // MOSI input
	DDRB &= ~(1<<DDB5); // SCK input
	DDRB &= ~(1<<DDB2); // SS input
	DDRB |=  (1<<DDB4); // MISO output
	SPCR = (1<<SPE);
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

inline byte SPI_read_byte()
{
	loop_until_bit_is_set(SPSR, SPIF); return SPDR;
}

byte SPI_read_nibble()
{
	byte nibble = SPI_read_byte();
	
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


void init_Serial()
{
	UBRR0L = 0X10;
	UBRR0H = 0;
	
	UCSR0B = (1 << RXEN0)|(1 << TXEN0); // receive & transmit enabled
	
	UCSR0C = 3 << UCSZ00; // 8 data-bits, 1 stop-bit, no parity
	
}


void displayGetal(byte x){
	display =getallen[x];
	shift_out_595(display);
	serial_write_byte('!');
}

void veranderBportpins(){
	byte hi = serial_read_nibble();
	byte lo = serial_read_nibble();
	
	PORTB = ((hi << 4) | lo) & ~0x02;
	
	serial_write_byte('.');
}

void sigmentUitzetten(byte x){
	display = display & ~bitjes[x-1];
	shift_out_595(display);
	serial_write_byte('!');
	
}

void sigmentAanzetten(byte x){
	display = display | bitjes[x-1];
	shift_out_595(display);
	serial_write_byte('|');
	
}

void kiesFreq(uint16_t freq){

	
	play_tone(freq);
	_delay_ms(300);         // Laat toon even horen
	play_tone(0);           // Stop toon
	serial_write_byte('|');
	
}

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

void kiesNoot(byte x){
	
	play_tone(toneFreqs[x]);
	_delay_ms(300);         // Laat toon even horen
	play_tone(0);           // Stop toon

	serial_write_byte('|');
	
}

void byteToDisplay(byte x){
	shift_out_595(x);
	
	serial_write_byte(']');
}




#endif /* FUNCTIES_H_ */