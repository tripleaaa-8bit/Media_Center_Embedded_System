/*----------------------------------------------------------------------------
 * Name:    Blinky.c
 * Purpose: LED Flasher and Graphic Demo
 * Note(s): 
 *----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2008-2011 Keil - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------*/
                  
#include <LPC17xx.H>                    /* NXP LPC17xx definitions            */
#include "string.h"
#include "GLCD.h"
#include "LED.h"
#include "KBD.h"

#define __FI        1                      /* Font index 16x24               */
#define __USE_LCD   0										/* Uncomment to use the LCD */

extern int gallery(void);
extern void restart_game_menu(void);
extern int mp3_player (void); 
	
int main_menu (void) {                       /* Main Program                       */
	uint32_t joystick_motion;
	int sel_num = 0;

	GLCD_Clear(Purple);                         /* Clear graphical LCD display   */
	GLCD_SetBackColor(Purple);
	GLCD_SetTextColor(White);
	GLCD_DisplayString(0, 0, 1, "||||Media Center|||||");
	GLCD_SetTextColor(White);
	GLCD_DisplayString(1, 0, 1, "      Main Menu      ");
  GLCD_DisplayString(2, 0, 1, "   UP/DOWN - Scroll  ");
  GLCD_DisplayString(3, 0, 1, "   SELECT - Select   ");

	while (1) { 
		joystick_motion = get_button(); 
	
		//only three menu options
		if (sel_num<3 && sel_num>=0){
			if (joystick_motion == KBD_DOWN){
				sel_num++;
			}
			else if (joystick_motion == KBD_UP){
				sel_num --;
			}
		}
		else {
			sel_num = 0;
		}
	
		if (sel_num == 0){
			GLCD_SetTextColor(White);
			GLCD_SetBackColor(LightGrey);
			GLCD_DisplayString(5, 0, __FI, "1)Photo gallery");
			GLCD_SetBackColor(Purple);
			GLCD_DisplayString(6, 0, __FI, "2)Audio");
			GLCD_DisplayString(7, 0, __FI, "3)Game selection");

			joystick_motion = get_button(); 
			if (joystick_motion == KBD_SELECT){
					GLCD_Clear(Purple);
					gallery();
					break;
			}
		}
		
		else if (sel_num == 1) {
			GLCD_SetTextColor(White);
			GLCD_SetBackColor(Purple);
			GLCD_DisplayString(5, 0, __FI, "1)Photo gallery");
			GLCD_SetBackColor(LightGrey);
			GLCD_DisplayString(6, 0, __FI, "2)Audio");
			GLCD_SetBackColor(Purple);
			GLCD_DisplayString(7, 0, __FI, "3)Game selection");
			
			joystick_motion = get_button(); 
			if (joystick_motion == KBD_SELECT){
					GLCD_Clear(Purple);
					mp3_player();
					break;
			}
		}
		
		else if (sel_num == 2){
			GLCD_SetTextColor(White);
			GLCD_SetBackColor(Purple);
			GLCD_DisplayString(5, 0, __FI, "1)Photo gallery");
			GLCD_DisplayString(6, 0, __FI, "2)Audio");
			GLCD_SetBackColor(LightGrey);
			GLCD_DisplayString(7, 0, __FI, "3)Game selection");

			joystick_motion = get_button(); 			
			if (joystick_motion == KBD_SELECT){
					GLCD_Clear(Purple);
					restart_game_menu();
					break;
			}			
		}
	}
} 

int main(void){
	LED_Init ();
  GLCD_Init();
	KBD_Init();
	
	while(1){
		main_menu();
	}
}
