/*
 * jwool003_lab3_part3.c
 *
 * Created: 8/1/2018 5:24:59 PM
 * Author : Jonathan Woolf Peter Kim
 */ 

#include <avr/io.h>

enum states {start, wait, unlock} state;
unsigned char i = 0;
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
	//unsigned char buttonX = PINA & 0x01; // X is PA0
	//unsigned char buttonY = PINA & 0x02; // Y is PA1
	unsigned char buttonZ = PINA & 0x04; // Z is # / PA2
	unsigned char buttonPA7 = PINA & 0x80;
	unsigned char underByte = PINA & 0x87; //bitmask for the last 4 bits
	
	unsigned char code[4] = {4, 1, 2, 1};
	switch(state)
	{ //transitions
		case start:
			PORTB = 0;
			i = 0;
			unsigned char sequence[] = {0, 0, 0, 0};
			if(underByte == buttonZ){ state = wait;}
			else { state = start;} 
			break;
		case wait:
			if(i < 4)
			{
				sequence[i] = underByte;
				i++;
				state = wait;
			}
			else if((sequence[0] == code[0]) && (sequence[1] == code[1]) && (sequence[2] == code[2]) && (sequence[3] == code[3])) { state = unlock;}
			else { state = start;}
			break;
		case unlock:
			PORTB = 1;
			if(buttonPA7){ state = start;}
			else { state = unlock;}
			break;
		default:
			state = start;
			break;
	}
}