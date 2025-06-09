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

typedef unsigned char byte;
#define TABLE_SIZE 64
#define PI 3.14159265

volatile uint8_t sine_table[TABLE_SIZE];
volatile uint8_t index = 0;




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

ISR(TIMER1_COMPA_vect)
{
	OCR0A = sine_table[index];  // Update PWM duty cycle
	index++;
	if (index >= TABLE_SIZE) index = 0;
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
	
}

void init_sine_table()
{
	for (uint8_t i = 0; i < TABLE_SIZE; i++) {
		float angle = (2 * PI * i) / TABLE_SIZE;
		// 8-bit waarde (0-255)
		sine_table[i] = (uint8_t)((sin(angle) * 127.5) + 127.5);
	}
}

void init_pwm()
{
	DDRD |= (1 << DDD6);  // PD6 (OC0A) als output

	// Fast PWM, non-inverted
	TCCR0A |= (1 << COM0A1) | (1 << WGM00) | (1 << WGM01);
	TCCR0B |= (1 << CS00); // Geen prescaler
	OCR0A = 0; // Beginwaarde
}

void init_timer1()
{
	TCCR1B |= (1 << WGM12); // CTC mode

	// Prescaler: 64
	TCCR1B |= (1 << CS11) | (1 << CS10);

	// Sample rate = TABLE_SIZE * frequentie
	OCR1A = (F_CPU / (64UL * 0)) - 1;

	TIMSK1 |= (1 << OCIE1A); // Enable compare interrupt
}
void change_timer1(float Freq)
{
	// Sample rate = TABLE_SIZE * frequentie
	uint16_t sample_rate = TABLE_SIZE * Freq;
	OCR1A = (F_CPU / (64UL * sample_rate)) - 1;
}

void liedje1()
{
	change_timer1(659.255);
	_delay_ms(167);
	change_timer1(659.255);
	_delay_ms(167);
	change_timer1(0);
	_delay_ms(167);
	change_timer1(659.255);
	_delay_ms(167);
	change_timer1(0);
	change_timer1(523.251);
	_delay_ms(167);
	change_timer1(659.255);
	_delay_ms(334);
	change_timer1(0);
}

int main(void)
{
	cli();
	init_Serial();
	init_ports();
	init_sine_table();
	init_pwm();
	init_timer1();
	sei();
	


	
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
			byte mid = serial_read_nibble();
			byte lo = serial_read_nibble();
			change_timer1((hi << 8) |(mid << 4) | lo);
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

