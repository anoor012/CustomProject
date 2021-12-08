/*	Author: Ali Noor DEMO:  https://youtu.be/UZSyki6ShEY
 *	lab: Final
 *  Partner(s) Name: 
 *	Lab Section:21
 *	Assignment: Lab #Final  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/eeprom.h>
#include <avr/io.h>
#include <io.h>
#include <scheduler.h>
#include <timer.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

enum LCDStates{start1, wait, begin, begin_wait, begin_wait2};
enum Level1States{wait1, init1};
enum ChooseStates{wait2, init2};
enum Level2States{wait3, init3};
unsigned char gameStart = 0x00;
unsigned char level = 0x00;
uint8_t levelsDone;
unsigned char Special[8] = { 0x01, 0x03, 0x07, 0x0D, 0x0F, 0x02, 0x05, 0x0A };
unsigned char Special2[8] = { 0x04, 0x1F, 0x11, 0x11, 0x1F, 0x1F, 0x1F, 0x1F };
#define temp (~PINA & 0x08)
#define temp1 (~PINA & 0x10)
#define temp2 (~PINA & 0x20)
//unsigned char i = 0x00; //Time of game
//unsigned char begun = 0x00;
unsigned char pos = 0x00;

void eepromWrite(unsigned char addr, unsigned char data) {
	eeprom_write_byte(addr, data);
}

unsigned char eepromRead(unsigned char addr) {
	return eeprom_read_byte(addr);
}

//void SetHighScore() {
//	eeprom_write_byte((uint8_t*)15, 1);
//	levelsDone = eeprom_read_byte((uint8_t*)15);
//}

void InitADC(void) {
	ADMUX |= (1<<REFS0);
	ADCSRA |= (1<<ADPS1)|(1<<ADPS0)|(1<<ADEN);
}

uint16_t ReadADC(uint8_t ADCchannel) {
	ADMUX = (ADMUX & 0xF0) | (ADCchannel & 0x0F);
	ADCSRA |= (1<<ADSC);
	while(ADCSRA & (1<<ADSC));
	return ADC;
}

int joystick() {
	
	int position;
	unsigned short x;
	x = ReadADC(1);

	if(x >= 600) {
		position = 1;
	}
	else if(x <= 500) {
		position = 2;
	}
	else {
		position = 0;
	}

	return position;
}

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
				state = begin_wait2;
			}
			break;
		case begin_wait2 :
			if(temp1) {
				state = wait;
			}
			else {
				state = begin_wait2;
			}
			break;
		default :
			break;
	}
	switch(state) {
		case start1 : 
			break;
		case wait :
			gameStart = 0x00;
			LCD_ClearScreen();
			LCD_DisplayString(1, "Start Game (B1) Levels Done: ");
			//calls function
			LCD_Special_Char(0, Special);
			//LCD_Special_Char(1, Special2);
			LCD_Cursor(0x10);
			LCD_WriteData(0x00);
			LCD_Cursor(0x1D);
			levelsDone = eepromRead(0x00);
			LCD_WriteData(levelsDone + '0');
			break;
		case begin :
			LCD_ClearScreen();
			break;
		case begin_wait :
			if(level == 0x00) {
				gameStart = 0x01;
				LCD_DisplayString(2, "Level 1 (B1)    Level 2 (B1)");
				LCD_Special_Char(0, Special);
				LCD_Special_Char(1, Special2);
				LCD_Cursor(0x01);
				LCD_WriteData(0x00);
				LCD_Cursor(0x11);
				LCD_WriteData(0x01);
				LCD_Cursor(0x01);
			}
			break;
		case begin_wait2 :
			break;
		default :
			break;
	}
	return state;
}

unsigned int current = 1;

int chooseTick (int state) {
	switch(state) {
		case wait1 :
			if(gameStart == 0x01) {
				state = init1;
			}
			else if(temp1) {
				state = wait1;
			}
			break;
		case init1 :
			if(gameStart == 0x01) {
				state = init1;
			}
			else {
				state = wait1;
			}	
			break;
		default :
			break; 
	}
	switch(state) {
		case wait1 :
			level = 0;
			break;
		case init1 :
			pos = joystick();

			if(pos == 1 && current == 1) {
				current += 16;
			}
			else if(pos == 2 && current == 17) {
				current -= 16;
			}

			LCD_Cursor(current);
			delay_ms(250);

			if(temp && current == 1) {
				eepromWrite(0x00, 1);
				level = 0x01;
			}
			else if(temp && current == 17) {
				eepromWrite(0x00, 2);
				level = 0x02;
			}

			break;
		default :
			break;
	}

	return state;
}

int level1tick(int state) {
	switch(state) {
		case wait2 :
			if(level != 0x01) {
				state = wait2;
			}
			else if(level == 0x01) {
				state = init2;
			}
			break;
		case init2 :
			if(level == 0x01) {
				state = init2;
			}
			else if (level == 0x00) {
				state = wait2;
			}
			break;
		default :
			break;
	}
	switch(state) {
		case wait2 :
			break;
		case init2 :
			LCD_ClearScreen();
			LCD_DisplayString(1, "Level 1 Started!");
			break;
		default :
			break;
	}

	return state;
}

int level2tick(int state) {
	switch(state) {
		case wait3 :
			if(level != 0x02) {
				state = wait3;
			}
			else if(level == 0x02) { 
				state = init3;
			}
			break;
		case init3 :
			if(level == 0x02) {
				state = init3;
			}
			else if(level == 0x00) {
				state = wait3;
			}
			break;
		default :
			break;
	}
	switch(state) {
		case wait3 :
			break;
		case init3 :
			LCD_ClearScreen();
			LCD_DisplayString(1, "Level 2 Started!");
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
//	SetHighScore();
	LCD_init();
	InitADC();
	static task task_1;
	static task task_2;
	static task task_3;
	static task task_4;
	task *tasks[] = {&task_1, &task_2, &task_3, &task_4};
	const unsigned short numTasks = sizeof(tasks) / sizeof(task*);

	task_1.state = start1;
	task_1.period = 600;
	task_1.elapsedTime = task_1.period;
	task_1.TickFct = &LCDTick;

	task_2.state = wait1;
	task_2.period = 600;
	task_2.elapsedTime = task_2.period;
	task_2.TickFct = &chooseTick;
	
	task_3.state = wait2;
	task_3.period = 600;
	task_3.elapsedTime = task_3.period;
	task_3.TickFct = &level1tick;
	
	task_4.state = wait3;
	task_4.period = 600;
	task_4.elapsedTime = task_4.period;
	task_4.TickFct = &level2tick;

	TimerSet(600);
	TimerOn();
	unsigned short i;


    while (1) {
	for(i = 0; i < numTasks; ++i) {
		if(tasks[i]->elapsedTime == tasks[i]->period) {
			tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
			tasks[i]->elapsedTime = 0;
		}
		tasks[i]->elapsedTime += 600;
	}
	while(!TimerFlag);
	TimerFlag = 0;
    }

    return 1;
}
