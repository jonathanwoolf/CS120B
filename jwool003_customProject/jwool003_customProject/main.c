/*
 * jwool003_customProject.c
 *
 * Created: 8/23/2018 5:27:32 PM
 * Author : Jonathan
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "timer.h"
#include "io.c"
#include "io.h"
#include "bit.h"
#include "scheduler.h"

// === Inputs ===
#define overDrive (~PIND & 0x08)
#define rightButton (~PIND & 0x20)
#define leftButton (~PIND & 0x10)

// === Global Variables ===
unsigned char column_val; // sets the pattern displayed on columns
unsigned char column_sel; // grounds column to display pattern
unsigned char char_location = 0x10; // Location of the player character
unsigned char char_sel = 0x7F; // The bottom column where the character resides
unsigned char startGame = 0; // starts the game
/*unsigned short score = 0; // The player's score*/

const unsigned char course[35] = {0x00, 0x00, 0xE7, 0x00, 0x00, 0x81, 0xE2, 0x62, 0x24, 0x34, 0x2E, 0xA4, 0x24, 0x22, 0x14, 0x12, 0x0A, 0x09, 0xAB, 0xC3, 0x55, 0xBA, 0xAA, 0xAB, 0x55, 0x00, 0xAA,
0xE7, 0x55, 0xC3, 0xAA, 0xE7, 0x55, 0xC3, 0xAA};

// ====================
// SM1: Moves your character on the matrix
// ====================
enum Character_States {button, wait} Character_State;
int Character_Tick(int state) 
{
	switch (Character_State) {
		case button:
			if(leftButton && !rightButton) // Character moves to the left to avoid obstacles
			{ 
				if(char_location != 0x01) { char_location = char_location >> 1;} // Obviously a right shift must occur
				Character_State = wait;
			}
			else if(!leftButton && rightButton) // Character moves to the right to avoid obstacles
			{ 
				if(char_location != 0x80) { char_location = char_location << 1;} // Obviously a left shift must occur NIMROD
				Character_State = wait;
			}
			else if(leftButton && rightButton) // So are you starting the game or just too stupid to hit the buttons one at a time?
			{
				startGame = 1; // The game will now start generating obstacles.. the LCD Soundsystem guy is losing his edge
				Character_State = wait;
			}
			else {Character_State = button;} // No user input occured
			break;
		case wait:
			if(!leftButton && !rightButton) // Wait for buttons to stop being pressed
			{
				Character_State = button;
			}
			else {Character_State = wait;} 
			break;
		default:
			Character_State = button; // A default state can only transition and will never allow you to set a value	        
			break;
	}
	return Character_State; // Return value of type int
};

// ====================
// SM2: Move the obstacles on the matrix
// ====================
enum Obstacles_States {start, obstacleCourse, delay} Obstacles_State;
int Obstacles_Task(int state)
{
	//static char count = 0; 
	static unsigned char i = 0; // Iterate through course
	
	switch (Obstacles_State){
		case start:
			column_sel = 0xFE; // Initializes start point for course
			column_val = course[i]; // assign course to column_val
			if(startGame) { Obstacles_State = obstacleCourse;} // Start generating obstacles when user hits both buttons
			else { Obstacles_State = start;} // Wait for user to start the game
			break;
		case obstacleCourse:   // Generate obstacles
			if(column_sel == 0x7F) { column_sel = 0xFE; column_val = course[i]; i++;} // If obstacle reaches the character return to the top of the matrix and iterate through course
			if(i == 36) { i = 4;} // If the end of the course is reached, reset iterator enough to avoid major delay
			else {
				column_sel = (column_sel << 1) | 0x01; // Move obstacle column down toward the character and "or" with 0x01 in order to only display one column at a time
			}
			Obstacles_State = obstacleCourse;
			if(!startGame) { i = 0; Obstacles_State = start;} // Go to start and await soft reset 
 			//Obstacles_State = (overDrive == 1)? obstacleCourse : delay;
			break;
		case delay: // Fix
// 			if(count < 2) { count++; Obstacles_State = delay;}
// 			else { count = 0; Obstacles_State = obstacleCourse;}
			break;
		default:
			Obstacles_State = obstacleCourse; // A default state can only transition and will never allow you to set a value
			break;
	}
	return Obstacles_State;		
};

// ====================
// SM3: Synch output to the matrix
// ====================
enum Synch_States {display, gameOver} Synch_State;
int Synch_Task(int state)
{
	static unsigned char flag = 1; // Flag bit used to set display output
	
	switch(Synch_State){
		case display: // Displays game and iterates score
			if((column_sel == char_sel) && (column_val == (column_val | char_location))) { startGame = 0; Synch_State = gameOver;} // Collision detected GAME OVER
			else if(flag){ flag = 0; PORTA = char_location; PORTB = char_sel; Synch_State = display;} // Flip between displaying character and obstacles
			else{ flag = 1; PORTA = column_val; PORTB = column_sel; Synch_State = display;}
			break;
		case gameOver: // Illuminate all LEDs and wait for soft reset
			PORTA = 0xFF;
			PORTB = 0x00;
			Synch_State = gameOver;
			if(startGame){ Synch_State = display;} // Reset detected: reset score and return to display state
			break;
		default: 
			Synch_State = display; // A default state can only transition and will never allow you to set a value
			break;
	}
	return Synch_State;
}

// ====================
// SM4: Output score and special character to LCD
// ====================
enum Score_State {scoreDisplay} Score_State;
int Score_Task(int state)
{
	static unsigned short score = 0; // The player's score
	
	switch(Score_State){
		case scoreDisplay: // Displays game and iterates score
			if(startGame) // Increment score every 1000ms
			{
				LCD_Cursor(1);
				LCD_WriteData(score + 0);
				LCD_WriteData(0 + 0);
				score = score + 1;
			}
			else if(!startGame){ score = 0;}
			Score_State = scoreDisplay;
			break;
		default:
			Score_State = scoreDisplay;
			break;
	}
	return Score_State;
}

int main(void)
{	
	DDRA = 0xFF;  PORTA = 0x00; // Output value to selected column(s)
	DDRB = 0xFF;  PORTB = 0x00; // Output selected column(s)
	DDRC = 0xFF;  PORTC = 0x00; // LCD Output
	DDRD = 0xC0;  PORTD = 0x3F; // Button Input
	
	LCD_init();
	LCD_Cursor(1);
	LCD_WriteData(0 + '0');
	
	TimerSet(tasksPeriod);
	TimerOn();
	
	unsigned char i = 0;
	tasks[i].state = -1;
	tasks[i].period = 50;
	tasks[i].elapsedTime = 0;
	tasks[i].TickFct = &Character_Tick;
	i++;
	tasks[i].state = start;
	tasks[i].period = 100;
	tasks[i].elapsedTime = 0;
	tasks[i].TickFct = &Obstacles_Task;
	i++;
	tasks[i].state = -1;
	tasks[i].period = 1;
	tasks[i].elapsedTime = 0;
	tasks[i].TickFct = &Synch_Task;
	i++;
	tasks[i].state = -1;
	tasks[i].period = 1000;
	tasks[i].elapsedTime = 0;
	tasks[i].TickFct = &Score_Task;
    while(1){}
}

