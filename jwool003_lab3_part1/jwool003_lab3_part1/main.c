/*
 * jwool003_lab3_part1.c
 *
 * Created: 8/1/2018 1:47:51 PM
 * Author : ucrcse
 */ 

#include <avr/io.h>

enum states {start, off, on, wait1, wait2} state;
void tick();

int main(void)
{
    DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as inputs
    DDRB = 0xFF; PORTB = 0x00; // Configure port B's 8 pins as outputs,
    
	state = start;
	
	while(1)
	{
		tick();
	}
}

void tick() 
{
	unsigned char A0 = PINA & 0x01; //bitmask for the flag bit
	
	switch(state)
	{ //transitions
		case start:
			state = off;
			break;
		case off:
			PORTB = 0x01;
			state = A0? off : wait1;
			break;
		case on: 
			PORTB = 0x02;
			state = A0? on : wait2;
			break;
		case wait1:
			PORTB = 0x01;
			state = A0? on : wait1;
			break;
		case wait2:
			PORTB = 0x02;
			state = A0? off : wait2;
			break;
		default:
			state = off;
			break;
	}	
} 


