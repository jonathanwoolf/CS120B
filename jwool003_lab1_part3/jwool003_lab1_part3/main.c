/*
 * jwool003_lab1_part3.c
 *
 * Created: 7/30/2018 9:44:50 PM
 * Author : ucrcse
 */ 

#include <avr/io.h>

int main(void)
{
    DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as inputs
    DDRC  = 0xFF; PORTC = 0x00; // Configure port C's 8 pins as outputs
    // initialize spaces to 4 and the rest to 0s
    unsigned char spaces = 0x04; // How many spaces total there are
    unsigned char cntavail = 0x00; 	// How many available spaces there are
    unsigned char occupado = 0x00; // Temporary variable to hold the value of A
    while(1)
    {
        // (1) Read inputs and returns a 1 for PA0 through PA3 if occupied MOFO
        occupado = (PINA & 0x01) + ((PINA >> 0x01) & 0x01) + ((PINA >> 0x02) & 0x01) + ((PINA >> 0x03) & 0x01);
        // (2) Perform computation
        cntavail = spaces - occupado; //finds out the available amount of spaces
        // (3) Write output
		if(cntavail == 0) {
			PORTC = 0x80; //Set PC7 = 1
		}
		else{
		PORTC = cntavail;
		}
    }
    return 0;
}

