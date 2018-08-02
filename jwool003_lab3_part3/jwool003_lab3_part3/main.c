/*
 * jwool003_lab3_part3.c
 *
 * Created: 8/1/2018 5:24:59 PM
 * Author : Jonathan Woolf Peter Kim
 */ 

#include <avr/io.h>



enum states {start, wait , wait2, unlock} state;
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
	unsigned char buttonZ = PINA & 0x04; // Z is #
	unsigned char buttonPA7 = PINA & 0x80;
	unsigned char underByte = PINA & 0x87; //bitmask for the flag bit
	
	switch(state)
	{ //transitions
		case start:
			PORTB = 0;
			if(underByte == buttonZ){ state = wait;}
			else { state = start;} 
			break;
		case wait:
			if(underByte == buttonZ){ state = wait;}
			else if(underByte == 0x02) { state = unlock;}
			else { state = start;}
			break;
		case wait2:
			if(underByte == buttonZ){ state = wait2;}
			else if(underByte == 0x02) { state = start;}
			else { state = unlock;}
			break;
		case unlock:
			PORTB = 1;
			if(buttonPA7){ state = start;}
			else if(underByte == buttonZ){ state = wait2;}
			else { state = unlock;}
			break;
		default:
			state = start;
			break;
	}
}
