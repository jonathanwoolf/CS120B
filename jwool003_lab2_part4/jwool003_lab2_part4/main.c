/*
 * jwool003_lab2_part4.c
 *
 * Created: 8/1/2018 3:47:00 PM
 * Author : ucrcse
 */ 

#include <avr/io.h>

// Bit-access function
unsigned char SetBit(unsigned char x, unsigned char k, unsigned char b) {
	return (b ? x | (0x01 << k) : x & ~(0x01 << k));
}
unsigned char GetBit(unsigned char x, unsigned char k) {
	return ((x & (0x01 << k)) != 0);
}

int main(void)
{
	DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as inputs
	DDRB = 0xFF; PORTB = 0x00; // Configure port B's 8 pins as outputs,
	DDRC = 0xFF; PORTC = 0x00; // Configure port B's 8 pins as outputs,
	// initialize to 0s
	unsigned char mask = 0x00;
	unsigned char mask2 = 0x00;
	while(1)
	{
		// 1) Read Inputs and assign to variables
		mask = PINA & 0xF0; 
		mask2 = PINA & 0x0F;
		PORTB = mask >> 4;
		PORTC = mask2 << 4;
	}
}


