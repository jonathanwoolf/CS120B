/*
 * jwool003_customProject.c
 *
 * Created: 8/23/2018 5:27:32 PM
 * Author : Jonathan
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include "timer.h"
#include "io.c"
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
unsigned char score = 0; // The player's score
unsigned char score1 = 0; 
unsigned char score10 = 0;
unsigned char score100 = 0;
unsigned char highscore = 0; // The system's high score
unsigned char highscore1 = 0; 
unsigned char highscore10 = 0;
unsigned char highscore100 = 0;
	
const unsigned char course[35] = {0x81, 0x81, 0xE7, 0x81, 0x81, 0x81, 0xE2, 0x62, 0x24, 0x34, 0x2E, 0xA4, 0x24, 0x22, 0x14, 0x12, 0x0A, 0x09, 0xAB, 0xC3, 0x55, 0xBA, 0xAA, 0xAB, 0x55, 0x33, 0xAA,
0xE7, 0x55, 0xC3, 0xAA, 0xE7, 0x55, 0xC3, 0xAA}; // This is the obstacle course

unsigned char customChar1[8] = {0x0A, 0x15, 0x1B, 0x15, 0x15, 0x15, 0x0A, 0x0E}; // Inverted custom character
unsigned char customChar2[8] = {0x15, 0x0A, 0x04, 0x0A, 0x0A, 0x0A, 0x15, 0x11}; // This is the non-inverted one ;)

// ====================
// SM1: Moves your character on the matrix
// ====================
enum Character_States {button, wait} Character_State;
int Character_Tick(int state) 
{		
	switch (Character_State) {
		case button:
// 			if(overDrive) // THIS IS VERY BROKEN
// 			{
// 				LCD_Cursor(2); LCD_WriteData('H' + '0');
// 				LCD_Cursor(3); LCD_WriteData('E' + '0');
// 				LCD_Cursor(4); LCD_WriteData('Y' + '0');
// 			}
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
				startGame = 1; // The game will now start generating obstacles.. the LCD SoundSystem guy is losing his edge
				Character_State = wait;
			}
			else {Character_State = button;} // No user input occurred
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
	static unsigned char flag = 0; // Flag to switch between courses
	static unsigned char i = 0; // Iterate through course
	
	switch (Obstacles_State){
		case start:
		flag = 0;
		column_sel = 0xFE; // Initializes start point for course
		column_val = course[i]; // assign course to column_val
		if(startGame) { Obstacles_State = obstacleCourse;} // Start generating obstacles when user hits both buttons
		else { Obstacles_State = start;} // Wait for user to start the game
		break;
	case obstacleCourse:   // Generate obstacles
		if(column_sel == 0x7F) // If obstacle reaches the character return to the top of the matrix and iterate through course
		{
			column_sel = 0xFE;
			if(flag == 0) // Display standard course
			{
				column_val = course[i];
			} 
			else // Display inverted course after standard course is complete
			{
				column_val = ~course[i];
			}
			i++;
		}
		if(i == 36) { i = 6; flag = !flag;} // If the end of the course is reached, reset iterator at beginning of standard course
		else 
		{
			column_sel = (column_sel << 1) | 0x01; // Move obstacle column down toward the character and "or" with 0x01 in order to only display one column at a time
		}
		Obstacles_State = obstacleCourse; // Continue to iterate obstacle course
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
			if((column_sel == char_sel) && (column_val == (column_val | char_location))) { startGame = 0; char_location = 0x10; Synch_State = gameOver;} // Collision detected GAME OVER
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
enum Score_State {scoreStart, scoreDisplay} Score_State;
int Score_Task(int state)
{	
	switch(Score_State){
		case scoreStart: 
			Score_State = scoreStart; // Default state when the game is not running
			if(startGame)
			{ 
				score100 = 0; // Sets all score values to 0
				score10 = 0;
				score1 = 0;
				score = 0;
				LCD_Cursor(2); LCD_WriteData(score100 + '0'); // Write each score value to the display from 100th place to 1th place
				LCD_Cursor(3); LCD_WriteData(score10 + '0'); 
				LCD_Cursor(4); LCD_WriteData(score1 + '0'); 
				Score_State = scoreDisplay;
			}
			break;
		case scoreDisplay: // Displays game and iterates score
			if(startGame)
			{
				score++; // Iterate actual score
				score1++; // Iterate 1th place score
				if(score1 > 9) {score1 -= 10; score10++;} // Iterate 10th place score
				if(score10 > 9) {score10 -= 10; score100++;} // Iterate 100th place score
				LCD_Cursor(2); LCD_WriteData(score100 + '0'); //Write each score value to the display from 100th place to 1th place
				LCD_Cursor(3); LCD_WriteData(score10 + '0');
				LCD_Cursor(4); LCD_WriteData(score1 + '0');
				Score_State = scoreDisplay;
			}
			if(!startGame)
			{
				if(score > highscore) // If the high score has been beaten replace it with the current score 
				{
					highscore100 = score100;
					highscore10 = score10;
					highscore1 = score1;
					highscore = score;
				}
				
				eeprom_write_byte((uint8_t*)3, highscore100); // Write high score to memory
				eeprom_write_byte((uint8_t*)4, highscore10);
				eeprom_write_byte((uint8_t*)5, highscore1);
				eeprom_write_byte((uint8_t*)6, highscore);
				
				Score_State = scoreStart;
			}
			break;
		default:
			Score_State = scoreStart;
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
	
	LCD_init(); // Initialize the LCD and remove the cursor
	LCD_WriteCommand(0x0C);
	
	highscore100 = eeprom_read_byte((uint8_t*)3); // Read high score from memory
	highscore10 = eeprom_read_byte((uint8_t*)4);
	highscore1 = eeprom_read_byte((uint8_t*)5);
	highscore = eeprom_read_byte((uint8_t*)6);
	if(highscore100 > 9) // Reset high score to 0 if memory has been corrupted
	{ 
		highscore100 = 0;
		highscore10 = 0;
		highscore1 = 0;
		highscore = 0;
	}
	LCDBuildChar(1, customChar1); // Create inverted custom character
	LCDBuildChar(2, customChar2); // Create non-inverted custom character
	LCD_Cursor(1); LCD_WriteData(1); // Output high score and custom characters
	LCD_Cursor(2); LCD_WriteData(highscore100 + '0');
	LCD_Cursor(3); LCD_WriteData(highscore10 + '0');
	LCD_Cursor(4); LCD_WriteData(highscore1 + '0');
	LCD_Cursor(5); LCD_WriteData(2);
	
	TimerSet(tasksPeriod); // Set the period for the entire system
	TimerOn();
	
	// Initialize schedule tasks 
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
    while(1){} //Scheduler occurs in TimerISR() function which is called automatically within while loop
}

