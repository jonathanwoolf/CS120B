/*
 * jwool003_lab4_part3.c
 *
 * Created: 8/7/2018 6:38:25 PM
 * Author : Jonathan
 */ 

#include <avr/io.h>
void tick ();
enum states {start, out, middle, in, wait1, wait2} state;
#define A0 (~PINA & 0x01)

int main(void)
{
	DDRA= 0x00; PORTA= 0xFF;
	DDRB= 0xFF; PORTB= 0x00;
	
	/* Replace with your application code */
	while (1)
	{
		tick();
	}
}

void tick()
{
	//	unsigned char tmpB = 0x00;
	//	unsigned short cnt = 0;
	
	switch (state){
		
		case start:
		//PORTB = 0x00;
		if(A0) {state = out;}
		else {state = start;}
		break;
		
		case out:
		PORTB = 0x21;
		if(A0) {state = out;}
		else {state = wait1;}
		break;
		
		case wait1:
		if(A0) {state = middle;}
		else {state = wait1;}
		break;
		
		case middle:
		PORTB = 0x12;
		if(A0) {state = middle;}
		else {state = wait2;}
		break;
		
		case wait2:
		if(A0) {state = in;}
		else {state = wait2;}
		break;
		
		case in:
		PORTB = 0x0C;
		if(A0) {state = in;}
		else {state = start;}
		break;
	}
}