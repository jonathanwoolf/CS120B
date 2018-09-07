/*
 * jwool003_customProject.c
 * CubeRunner: Lit Edition
 * Created: 8/23/2018 5:27:32 PM
 * Author : Jonathan Woolf 861172158
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
unsigned char masRapido = 0; // Speeds up course
unsigned char score = 0; // The player's score
unsigned char score1 = 0; 
unsigned char score10 = 0;
unsigned char score100 = 0;
unsigned char highscore = 0; // The system's high score
unsigned char highscore1 = 0; 
unsigned char highscore10 = 0;
unsigned char highscore100 = 0;
	
const unsigned char course[36] = {0x81, 0x81, 0xE7, 0x81, 0x81, 0x81, 0xEA, 0x62, 0x35, 0xC3, 0x46, 0xB4, 0x34, 0x92, 0x15, 0x32, 0x8A, 0x63, 0x89, 0xAB, 0xC7, 0x55, 0x99, 0xBA, 0xAB, 0x55, 0x33, 0x6A,
0xE5, 0x57, 0xCB, 0xAA, 0xE6, 0x55, 0xCB, 0xBA}; // This is the obstacle course

unsigned char customChar1[8] = {0x0A, 0x15, 0x1B, 0x15, 0x15, 0x15, 0x0A, 0x0E}; // Inverted custom character
unsigned char customChar2[8] = {0x15, 0x0A, 0x04, 0x0A, 0x0A, 0x0A, 0x15, 0x11}; // This is the non-inverted one ;)

void transmit_data(unsigned char data) // Outputs to the lower nibble of PORTA and controls which columns are selected
{
	for (int i = 8; i >= 0 ; --i) {
		// Sets SRCLR to 1 allowing data to be set
		// Also clears SRCLK in preparation of sending data
		PORTA = 0x08;
		// set SER = next bit of data to be sent.
		PORTA |= ((data >> i) & 0x01);
		// set SRCLK = 1. Rising edge shifts next bit of data into the shift register
		PORTA |= 0x02;
	}
	// set RCLK = 1. Rising edge copies data from “Shift” register to “Storage” register
	PORTA |= 0x04;
	// clears all lines in preparation of a new transmission
	PORTA = 0x00;
}

void transmit_data2(unsigned char data) // Outputs to the upper nibble of PORTA and controls what value (displayed in green) is assigned to selected columns
{
	for (int i = 8; i >= 0 ; --i) {
		// Sets SRCLR to 1 allowing data to be set
		// Also clears SRCLK in preparation of sending data
		PORTA = 0x80;
		// set SER = next bit of data to be sent.
		PORTA |= (((data >> i) & 0x01)) << 4;
		// set SRCLK = 1. Rising edge shifts next bit of data into the shift register
		PORTA |= 0x20;
	}
	// set RCLK = 1. Rising edge copies data from “Shift” register to “Storage” register
	PORTA |= 0x40;
	// clears all lines in preparation of a new transmission
	PORTA = 0x00;
}

// ====================
// SM1: Moves your character on the matrix
// ====================
enum Character_States {button, wait} Character_State;
int Character_Task(int state) 
{		
	switch (Character_State) {
		case button:
			if(overDrive) // Sets flag that increases the speed of obstacles and the rate in which score is increased
			{
				masRapido = !masRapido; // Overdrive flag is turned on and off each time the button is pressed
				tasks[1].period = masRapido? 70 : 100; // This flag is then used to set the obstacle state machine's period
				Character_State = wait;
			}
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
		default: // A default state can only transition and will never allow you to set a value	 
			Character_State = button;        
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
	static unsigned char inverseFlag = 0; // Flag to switch between courses
	static unsigned char i = 0; // Iterate through course
	
	switch (Obstacles_State){
		case start:
			column_sel = 0xFE; // Initializes start point for course
			column_val = course[i]; // assign course to column_val
			if(startGame) { Obstacles_State = obstacleCourse;} // Start generating obstacles when user hits both buttons
			else { Obstacles_State = start;} // Wait for user to start the game
			break;
	case obstacleCourse:   // Generate obstacles
		if(column_sel == 0x7F) // If obstacle reaches the character return to the top of the matrix and iterate through course
		{
			column_sel = 0xFE;
			if(i == 36) { i = 6; inverseFlag = !inverseFlag;} // If the end of the course is reached, reset iterator at beginning of standard course
			if(!inverseFlag) // Display standard course
			{
				column_val = course[i];
			} 
			else if(inverseFlag) // Display inverted course after standard course is complete
			{
				column_val = ~course[i];
			}
			i++;
		}
		else if(column_sel != 0x7F)
		{
			column_sel = (column_sel << 1) | 0x01; // Move obstacle column down toward the character and "or" with 0x01 in order to only display one column at a time
		}
		Obstacles_State = obstacleCourse; // Continue to iterate through obstacle course
		if(!startGame) // Go to start and await soft reset
		{ 
			i = 0; 
			inverseFlag = 0; 
			Obstacles_State = start;
		} 
		break;
	default: // A default state can only transition and will never allow you to set a value
		Obstacles_State = start; 
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
			// Clear all LED matrix ports
			transmit_data(0x00); 
			transmit_data2(0xFF); 
			PORTB = 0xFF;
			if((column_sel == char_sel) && (column_val == (column_val | char_location))) // Collision detected GAME OVER
			{ 
				startGame = 0;
				char_location = 0x10; 
				Synch_State = gameOver;
			} 
			else if(flag) // Flip between displaying character and obstacles
			{ 
				flag = 0; 
				transmit_data(char_location); 
				transmit_data2(char_sel); 
				Synch_State = display;
			} 
			else
			{
				 flag = 1; 
				 transmit_data(column_val); 
				 PORTB = column_sel; 
				 Synch_State = display;
			}
			break;
		case gameOver: // Illuminate all LEDs and wait for soft reset
			transmit_data(0xFF);
			PORTB = 0x00;
			masRapido = 0;
			Synch_State = gameOver;
			if(startGame) // Reset detected: reset score and character location and return to display state
			{ 
				char_location = 0x10; 
				Synch_State = display;
			} 
			break;
		default: // A default state can only transition and will never allow you to set a value
			Synch_State = display; 
			break;
	}
	return Synch_State;
}

// ====================
// SM4: Output score and special character to LCD
// ====================
enum Score_State {scoreStart, scoreDisplay, highScoreDisplay} Score_State;
int Score_Task(int state)
{	
	static unsigned char flag = 1; // Flag bit used to iterate between score and high score
	static unsigned char count = 0; // Add delay to score incrementer
	static unsigned char tmpScore; // Holds old high score;
	switch(Score_State){
		case scoreStart: 
			tmpScore = highscore;
			count = 0; // Reset count to 0
			score100 = 0; // Sets all score values to 0
			score10 = 0;
			score1 = 0;
			score = 0;
			LCD_ClearScreen();
			LCD_Cursor(1); LCD_WriteData(1);
			LCD_Cursor(2); LCD_WriteData(score100 + '0'); // Write each score value to the display from 100th place to 1th place
			LCD_Cursor(3); LCD_WriteData(score10 + '0');
			LCD_Cursor(4); LCD_WriteData(score1 + '0');
			LCD_Cursor(5); LCD_WriteData(2);
			Score_State = startGame? scoreDisplay : scoreStart; // Transition into scoreDisplay if game has started
			break;
		case scoreDisplay: // Displays and iterates score
			if(startGame && count == 1)
			{
				count = 0;
				if(masRapido) // Iterate actual score and 1th place score on overDrive
				{ 
					score = score + 3; 
					score1 = score1 + 3;
				} 
				else if(!masRapido) // Iterate actual score and 1th place score during standard gameplay
				{ 
					score++; 
					score1++;
				} 
				if(score1 > 9)  // Iterate 10th place score
				{
					score1 -= 10; 
					score10++;
				}
				if(score10 > 9) // Iterate 100th place score
				{
					score10 -= 10; 
					score100++;
				} 
					LCD_Cursor(1); LCD_WriteData(1);
					LCD_Cursor(2); LCD_WriteData(score100 + '0'); // Write each score value to the display from 100th place to 1th place
					LCD_Cursor(3); LCD_WriteData(score10 + '0');
					LCD_Cursor(4); LCD_WriteData(score1 + '0');
					LCD_Cursor(5); LCD_WriteData(2);
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
					eeprom_write_byte((uint8_t*)3, highscore100); // Write high score to memory
					eeprom_write_byte((uint8_t*)4, highscore10);
					eeprom_write_byte((uint8_t*)5, highscore1);
					eeprom_write_byte((uint8_t*)6, highscore);
				}
				Score_State = highScoreDisplay;
			}
			count++;
			break;
		case highScoreDisplay: // End game screen flashes between high score and recent score
			if(flag && (tmpScore >= score))
			{
				flag = 0;
				LCD_Cursor(2); LCD_WriteData(highscore100 + '0'); // Write each high score value to the display from 100th place to 1th place
				LCD_Cursor(3); LCD_WriteData(highscore10 + '0');
				LCD_Cursor(4); LCD_WriteData(highscore1 + '0');
		
			}
			else if(flag && (score > tmpScore))
			{
				flag = 0;
				LCD_ClearScreen();
				LCD_DisplayString(1, "High Score!");
			}
			else
			{
				flag = 1;
				LCD_ClearScreen();
				LCD_Cursor(1); LCD_WriteData(1); 
				LCD_Cursor(2); LCD_WriteData(score100 + '0'); // Write each score value to the display from 100th place to 1th place
				LCD_Cursor(3); LCD_WriteData(score10 + '0');
				LCD_Cursor(4); LCD_WriteData(score1 + '0');
				LCD_Cursor(5); LCD_WriteData(2);
			}
			Score_State = startGame? scoreStart : highScoreDisplay; // Return to starting state or continue to display high score
			break;
		default: // A default state can only transition and will never allow you to set a value
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
	if(highscore100 > 10) // Reset high score to 0 if memory has been corrupted
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
	
	// Initialize scheduled tasks 
	unsigned char i = 0;
	tasks[i].state = -1;
	tasks[i].period = 50;
	tasks[i].elapsedTime = 0;
	tasks[i].TickFct = &Character_Task;
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
	tasks[i].period = 500;
	tasks[i].elapsedTime = 0;
	tasks[i].TickFct = &Score_Task;
    while(1){} //Scheduler occurs in TimerISR() function which is called automatically within while loop
}

