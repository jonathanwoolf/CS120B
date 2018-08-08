/*
 * jwool003_lab5_part1.c
 *
 * Created: 8/7/2018 6:39:59 PM
 * Author : Jonathan
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;
void tick();
enum states {start, one, two, three} state;

void TimerOn() {
	TCCR1B = 0x0B;
	OCR1A = 125;
	TIMSK1 = 0x02;
	TCNT1 = 0;
	_avr_timer_cntcurr = _avr_timer_M;
	SREG |= 0x80;
}

void TimerOff() {
	TCCR1B - 0x00;
}

void TimerISR() {
	TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect){
	_avr_timer_cntcurr--;
	if(_avr_timer_cntcurr == 0) {
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

void main()
{
	
	/* Replace with your application code */
	DDRB = 0xFF;
	PORTB = 0x00;
	TimerSet(1000);
	TimerOn();
	
	while (1)
	{
		tick();
		// 		tmpB = ~tmpB;
		// 		PORTB = tmpB;
		// 		while (!TimerFlag);
		// 		TimerFlag = 0;
		
	}
}

void tick()
{
	//	unsigned char tmpB = 0x00;
	//	unsigned short cnt = 0;
	
	switch (state){
		
		case start:
		PORTB = 0x00;
		state = one;
		break;
		
		case one:
		PORTB = 0x01;
		while (!TimerFlag);
		TimerFlag = 0;
		state=two;
		break;
		
		case two:
		PORTB = 0x02;
		while (!TimerFlag);
		TimerFlag = 0;
		state=three;
		break;
		
		case three:
		PORTB = 0x04;
		while (!TimerFlag);
		TimerFlag = 0;
		state=one;
		break;
		
	}
	
	
	
}
