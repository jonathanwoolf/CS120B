/*
 * jwool003_lab2_part2.c
 *
 * Created: 7/31/2018 1:37:10 PM
 * Author : ucrcse
 */ 

#include <avr/io.h>

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
	DDRC = 0xFF; PORTC = 0x00; // Configure port B's 8 pins as outputs,
	// initialize to 0s
	unsigned char tmpC = 0x00; // intermediate variable used for port updates
	unsigned char fuelLevel = 0;
	unsigned char counter = 0;
	unsigned char i = 0;
	while(1)
	{
		// 1) Read Inputs and assign to variables
		fuelLevel = PINA & 0x0F; // Mask PINA to only get the bits you are interested in
		// 2) Perform Computation
		// if PA0 is 1, set PB1PB0=01, else =10
		counter = 0;
		for (i = 0; i < 4; i++)
		{
			if (GetBit(fuelLevel, i))  // True if PAi is 1
			{
				if(i == 0)
				{
					counter = counter + 1;
				}
				counter = counter + (2 * i);
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
		if (counter >= 5)
		{
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
		PORTC = tmpC;
	}
}
