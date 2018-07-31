/*
 * jwool003_lab2_part1.c
 *
 * Created: 7/31/2018 1:05:08 PM
 * Author : ucrcse
 */ 

#include <avr/io.h>


int main(void)
{
   DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as inputs, initialize to 1s
   DDRB = 0x00; PORTB = 0xFF; // Configure port B's 8 pins as outputs, initialize to 0s
   DDRC = 0xFF; PORTC = 0x00; // Configure port C's 8 pins as outputs, initialize to 0s
   unsigned char totalBits = 0; // You are UNABLE to read from output pins. Instead you
   // Should use a temporary variable for all bit manipulation.

    while (1) 
    {
		totalBits = (PINA & 0x01) + ((PINA >> 1) & 0x01) + ((PINA >> 2) & 0x01) + 
		((PINA >> 3) & 0x01) + ((PINA >> 4) & 0x01) + ((PINA >> 5) & 0x01) + ((PINA >> 6) & 0x01) +
		((PINA >> 7) & 0x01) + (PINB & 0x01) + ((PINB >> 1) & 0x01) + ((PINB >> 2) & 0x01) +
		((PINB >> 3) & 0x01) + ((PINB >> 4) & 0x01) + ((PINB >> 5) & 0x01) + ((PINB >> 6) & 0x01) +
		((PINB >> 7) & 0x01);
		
		PORTC = totalBits;
    }
}

