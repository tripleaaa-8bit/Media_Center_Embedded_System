#include <LPC17xx.H>                    /* NXP LPC17xx definitions            */
#include "string.h"
#include "GLCD.h"
#include "LED.h"
#include "KBD.h"
#include <stdlib.h>

extern int highway_dodge(void);
extern int grid_hunter(void);

int game_menu(){
	int joystick_motion;
	int sel_num = 0;
	
	GLCD_Clear(Purple);                         /* Clear graphical LCD display   */
  GLCD_SetBackColor(Purple);
  GLCD_SetTextColor(White);
	GLCD_DisplayString(0, 0, 1, "      Game Menu      ");
  GLCD_DisplayString(1, 0, 1, "   UP/DOWN - Scroll  ");
  GLCD_DisplayString(2, 0, 1, "   SELECT - Select   ");
	
	while (1) { 
		joystick_motion = get_button(); 
		if (sel_num<3 && sel_num>=0){
			if (joystick_motion == KBD_DOWN){
				sel_num++;
			}
			else if (joystick_motion == KBD_UP){
				sel_num --;
			}
		}
		else	{
			sel_num = 0;
		}
	
		if (sel_num == 0){
			GLCD_SetTextColor(White);
			GLCD_SetBackColor(LightGrey);
			GLCD_DisplayString(4, 0, 1, "1)Grid Hunter");
			GLCD_SetBackColor(Purple);
			GLCD_DisplayString(5, 0, 1, "2)Highway Dodge");
			GLCD_DisplayString(6, 0, 1, "Exit");
			joystick_motion = get_button(); 
			if (joystick_motion == KBD_SELECT){
					grid_hunter();
					return 0;
			}
		}
		
		else if (sel_num == 1) {
			GLCD_SetTextColor(White);
			GLCD_SetBackColor(Purple);
			GLCD_DisplayString(4, 0, 1, "1)Grid Hunter");
			GLCD_SetBackColor(LightGrey);
			GLCD_DisplayString(5, 0, 1, "2)Highway Dodge");
			GLCD_SetBackColor(Purple);
			GLCD_DisplayString(6, 0, 1, "Exit");	
			joystick_motion = get_button(); 
			if (joystick_motion == KBD_SELECT){
					highway_dodge();
					return 0;
			}
		}
		
		else if (sel_num == 2){
			GLCD_SetTextColor(White);
			GLCD_SetBackColor(Purple);
			GLCD_DisplayString(4, 0, 1, "1)Grid Hunter");
			GLCD_DisplayString(5, 0, 1, "2)Highway Dodge");
			GLCD_SetBackColor(LightGrey);
			GLCD_DisplayString(6, 0, 1, "Exit");
			joystick_motion = get_button(); 			
			if (joystick_motion == KBD_SELECT){
					return 1;
			}			
		} 
	}
}

void restart_game_menu(){
	int exit_game_menu;
	while(1){
		exit_game_menu = game_menu();
		if (exit_game_menu == 1){
			break;
		}
	}
}