#include <avr/io.h>



enum states {start, reset, increment, decrement, wait} state;
void tick();

int main(void)

{

	DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as inputs
	DDRB = 0xFF; PORTB = 0x00; // Configure port B's 8 pins as outputs,

	PORTB = 0x00;
	state = start;

	while(1)
	{
		tick();
	}
}



void tick()
{
	unsigned char underByte = ~PINA & 0x03; //bitmask for the flag bit
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
		if(underByte == 0x03) { state = reset;}
		else {state = underByte? wait : start;}
		break;
		case increment:
		if(PORTB < 9)
		{
			PORTB++;
		}
		state = wait;
		break;
		case decrement:
		if(PORTB > 0)
		{
			PORTB--;
		}
		state = wait;
		break;
		case reset:
		PORTB = 0;
		state = underByte? reset : start;
		break;
		default:
		state = start;
		break;
	}
}