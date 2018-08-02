/*
 * jwool003_lab4_part1.c
 *
 * Created: 8/2/2018 3:55:34 PM
 * Author : ucrcse
 */ 

#include <avr/io.h>

int main(void)
{
	DDRB = 0xFF; PORTB = 0x00; // Configure port B's 8 pins as outputs
	while(1)
	{
		PORTB = 0x0F; // Writes port B's 8 pins with 00001111
	}
}


