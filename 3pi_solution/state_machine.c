/*
* state_machine.c
*
* Created: 11/29/2015 1:41:39 PM
*  Author: nss
*/

#include "initialize.h"
#include "line-follow-pid.h"
#include "bargraph.h"
#include <pololu/3pi.h>
#include <string.h>

typedef enum { STATE_LINE_FOLLOW=0, STATE_INPUT_DECODE, STATE_MAZE_SOLVE} threepi_state_t;
threepi_state_t state = STATE_LINE_FOLLOW;

char* input_decode_string = "";

void select_state(unsigned int* sensors) {
	if(sensors[0] > 900) {
		strcat(input_decode_string, "L");
		} else if(sensors[4] > 900) {
		strcat(input_decode_string, "R");
	}
	//lcd_goto_xy(0,0);
	//print(input_decode_string);
	//delay_ms(200);
}

int set_position(unsigned int* sensors) {
	int pos = read_line(sensors, IR_EMITTERS_ON);
	if(state == STATE_INPUT_DECODE) {
			if(sensors[0] > 900 && sensors[1] > 900 && sensors[2] > 900 && sensors[3] > 900 && sensors[4] > 900){
				return pos;
			}
		return (1000*sensors[1]+2000*sensors[2]+3000*sensors[3])/(sensors[1]+sensors[2]+sensors[3]);
		} else {
		return pos; 
	}
}

void state_machine() {
	initialize();
	
	unsigned int sensors[5]; // an array to hold sensor values
	unsigned int last_position=0;
	while(1) {
		clear();
		int position = set_position(sensors);
		//if we see a horizontal line
		if(sensors[0] > 900 && sensors[1] > 900 && sensors[2] > 900 && sensors[3] > 900 && sensors[4] > 900) {
			if(state == STATE_INPUT_DECODE) {
				state = STATE_LINE_FOLLOW;
				} else {
				input_decode_string = "";
				state = STATE_INPUT_DECODE;
			}
			delay_ms(300);
		}

		switch(state) {
			case STATE_LINE_FOLLOW:
			if(get_ms() % 500 < 1000) {
				clear();
				lcd_goto_xy(0,0);
				print("FOL");
			}
			lcd_goto_xy(0,1);
			display_readings(sensors);
			last_position = line_follow_pid(sensors, position, last_position,0);
			break;
			
			case STATE_INPUT_DECODE:
			if(get_ms() % 500 < 1000) {
				clear();
				lcd_goto_xy(0,0);
				print("DECODE");
			}
			lcd_goto_xy(0,1);
			display_readings(sensors);
			last_position = line_follow_pid(sensors, position, last_position,1);
			//set_motors(0,0);
			//select_state(sensors);
			break;
			
			case STATE_MAZE_SOLVE:
			break;
		}
	}
}
