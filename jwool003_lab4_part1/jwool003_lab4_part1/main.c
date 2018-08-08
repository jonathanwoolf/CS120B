/*
 * jwool003_lab4_part1.c
 *
 * Created: 8/2/2018 3:55:34 PM
 * Author : ucrcse
 */ 

#include <avr/io.h>
#define tempA ~PINA


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

	// initialize to 0s

	unsigned char tmpC = 0x00; // intermediate variable used for port updates

	unsigned char counter = 0;

	while(1)

	{

		// 1) Read Inputs and assign to variables
		tmpC = 0x00;
		counter = tempA & 0x0F; // Mask tempA to only get the bits you are interested in

		// 2) Perform Computation
		if (((tempA >> 4) & 0x01) && ((tempA >> 5) & 0x01))
		{
			if (!(tempA >> 6) & 0x01)
			{
				tmpC = SetBit(tmpC, 7, 1);
			}
			else
			{
				tmpC = SetBit(tmpC, 7, 0);
			}
		}

		if (counter >= 1)
		{

			tmpC = SetBit(tmpC, 5, 1); // Set bit 5 to 0

		}

		if (counter >= 3)
		{

			tmpC = SetBit(tmpC, 4, 1); // Set bit 5 to 0

		}
		
		if (counter <= 4)
		{
			tmpC = SetBit(tmpC, 6, 1); //low fuel indicator
		}
		
		if (counter >= 5)
		{
			tmpC = SetBit(tmpC, 6, 0); //low fuel indicator
			tmpC = SetBit(tmpC, 3, 1); // Set bit 5 to 0

		}

		if (counter >= 7)
		{

			tmpC = SetBit(tmpC, 2, 1); // Set bit 5 to 0

		}

		if (counter >= 10)
		{

			tmpC = SetBit(tmpC, 1, 1); // Set bit 5 to 0

		}

		if (counter >= 13)
		{

			tmpC = SetBit(tmpC, 0, 1); // Set bit 5 to 0

		}

		// 3) write results to port

		PORTB = tmpC;

	}

}