/*
 * jwool003_lab3_part2.c
 * jonathan woolf and Peter Kim
 * Created: 8/1/2018 3:56:10 PM
 * Author : ucrcse
 */ 

#include <avr/io.h>



enum states {start, reset, increment, decrement, wait} state;
void tick();

int main(void)

{

    DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as inputs
    DDRC = 0xFF; PORTC = 0x00; // Configure port B's 8 pins as outputs,

	PORTC = 0x07;
	state = start;

	while(1)
	{
		tick();
	}
}



void tick() 
{
	unsigned char underByte = PINA & 0x03; //bitmask for the flag bit
	switch(state)
	{ //transitions
		case start:
			if(underByte == 0x00)
			{
				state = start;
			}
			else if(underByte == 0x01)
			{
				state = increment;
			}
			else if(underByte == 0x02)
			{
				state = decrement;
			}
			else if(underByte == 0x03)
			{
				state = reset;
			}
			break;
		case wait:
			state = underByte? wait : start;
			break;
		case increment:
			if(PORTC < 9)
			{
				PORTC++;
			}
			state = wait;
			break;
		case decrement: 
			if(PORTC > 0)
			{
				PORTC--;
			}
			state = wait;
			break;
		case reset:
			PORTC = 0;
			if(underByte == 0x00) 
			{
				PORTC = 7;
			}
			state = underByte? reset : start;
			break;
		default:
			state = start;
			break;
	}	
} 