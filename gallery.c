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

extern SUPER_MARIO_pixel_data[];
extern SONIC_pixel_data[];
extern STEVE_pixel_data[];


#define __FI        1                      /* Font index 16x24               */

/*******************************************************************************
* Draw character on given position                                             *
*   Parameter:      x:        horizontal position                              *
*                   y:        vertical position                                *
*                   cw:       character width in pixel                         *
*                   ch:       character height in pixels                       *
*                   c:        pointer to character bitmap                      *
*   Return:                                                                    *
*******************************************************************************/

/*----------------------------------------------------------------------------
  Main Program
 *----------------------------------------------------------------------------*/
int image_delay(int div){
	int i;
	int val = 10000000/div;
	int count;
	for (i = 0; i<val; i++){
		count ++;
	}
}

int gallery (void) {
	uint32_t joystick_motion;
	int sel_num=0;
	
	GLCD_Clear(Purple);                         
	GLCD_SetBackColor(Purple);
	GLCD_SetTextColor(White);
	GLCD_DisplayString(0, 0, 1, "    Photo Gallery    ");
	GLCD_DisplayString(1, 0, 1, " LEFT/RIGHT - Scroll ");
	GLCD_DisplayString(9, 0, 1, "    SELECT - Exit    ");	

	 while(1){
		joystick_motion = get_button();
		
		if (joystick_motion == KBD_RIGHT){
			if (sel_num == 2){
				sel_num = 0;
			}
			else{
				sel_num++;
			}
		}
		else if (joystick_motion == KBD_LEFT){
			if (sel_num == 0){
				sel_num = 2;
			}
			else{
				sel_num --;		
			}	
		}
		else if (joystick_motion == KBD_SELECT){
			break;
		}
     
		if (sel_num == 0){
			GLCD_Bitmap (  70,   50, 100,  125, (unsigned char *)SONIC_pixel_data);
		}
			
		else if (sel_num == 1){
			GLCD_Bitmap (  70,   50, 100,  125, (unsigned char *)SUPER_MARIO_pixel_data);
		}
			
		else if (sel_num == 2) {
			GLCD_Bitmap (  70,   50, 100,  125, (unsigned char *)STEVE_pixel_data);
		}
		image_delay(10);
	}

}
