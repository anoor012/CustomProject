/*	Author: Ali Noor DEMO: https://youtu.be/r3-3gFsFDr8
 *	lab: Final
 *  Partner(s) Name: 
 *	Lab Section:21
 *	Assignment: Lab #Final  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include <io.h>
#include <scheduler.h>
#include <timer.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

enum LCDStates{start1, wait, begin, begin_wait};
unsigned char Special[8] = { 0x01, 0x03, 0x07, 0x0D, 0x0F, 0x02, 0x05, 0x0A };
unsigned char Special2[8] = { 0x04, 0x1F, 0x11, 0x11, 0x1F, 0x1F, 0x1F, 0x1F };
#define temp (~PINA & 0x01)
#define temp1 (~PINA & 0x02)
//unsigned char i = 0x00; //Time of game
//unsigned char begun = 0x00;

int LCDTick(int state) {
	switch(state) {
		case start1 :
			state = wait;
			break;
		case wait :
			if((temp) && (temp1)) {
				state = wait;
			}
			else if(temp) {
				state = begin;
			}
			else {
				state = wait;
			}
			break;
		case begin :
			if(temp) {
				state = begin;
			}
			else {
				state = begin_wait;
			}
			break;
		case begin_wait :
			if(temp1) {
				state = wait;
			}
			else {
				state = begin_wait;
			}
			break;
		default :
			break;
	}
	switch(state) {
		case start1 : 
			break;
		case wait :
			LCD_ClearScreen();
			LCD_WriteCommand(0x80);
			LCD_DisplayString(17, "Start Game (B1)");
			LCD_Special_Char(0, Special);
			LCD_WriteData(0x00);
			LCD_WriteCommand(0x70);
			LCD_Special_Char(0, Special2);
			LCD_WriteData(0x00);
			break;
		case begin :
			LCD_ClearScreen();
			break;
		case begin_wait :
			LCD_DisplayString(1, "Game Started");
			break;
		default :
			break;
	}
	return state;
}

int main(void) {
    
    /* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0xFF;
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
    /* Insert your solution below */
	LCD_init();
	static task task_1;
	task *tasks[] = {&task_1};
	const unsigned short numTasks = sizeof(tasks) / sizeof(task*);
	const char Start = 0;

	task_1.state = Start;
	task_1.period = 40;
	task_1.elapsedTime = task_1.period;
	task_1.TickFct = &LCDTick;

	TimerSet(10);
	TimerOn();
	unsigned short i;


    while (1) {
	for(i = 0; i < numTasks; ++i) {
		if(tasks[i]->elapsedTime == tasks[i]->period) {
			tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
			tasks[i]->elapsedTime = 0;
		}
		tasks[i]->elapsedTime += 1;
	}
	while(!TimerFlag);
	TimerFlag = 0;
    }

    return 1;
}
