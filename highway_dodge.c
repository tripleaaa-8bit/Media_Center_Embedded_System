#include <LPC17xx.H>                    /* NXP LPC17xx definitions            */
#include "string.h"
#include "GLCD.h"
#include "LED.h"
#include "KBD.h"
#include <stdlib.h>
#include <stdio.h>

#define __FI        1                      /* Font index 16x24               */
#define __USE_LCD   0                      /* Uncomment to use the LCD       */

#define NUM_LANES       3
#define NUM_OBSTACLES   5
#define PLAYER_COL      2

typedef struct {
  int row;
  int col;
  int active;
} Obstacle;

/* Busy-wait delay used to slow down the game loop / animations */
void delay(int div) {
  int i;
  int val = 10000000 / div;	/* Larger div => smaller val => shorter delay */
  volatile int count = 0;		/* volatile prevents compiler from optimizing it away */
  for (i = 0; i < val; i++) {	/* Simple loop to burn CPU cycles */
    count++;
  }
}

/* Draw the player car at a given LCD row in the fixed player column */
static void draw_player(int row) {
  GLCD_SetBackColor(White);
  GLCD_SetTextColor(Blue);
  GLCD_DisplayChar(row, PLAYER_COL, 1, 'A');   /* player car 'A' */
}

/* Erase the player car by printing a space in its position */
static void erase_player(int row) {
  GLCD_SetBackColor(White);	/* Background remains white */
  GLCD_DisplayChar(row, PLAYER_COL, 1, ' ');	/* Overwrite with space character */
}

/* Draw a single obstacle car if it's active */
static void draw_obstacle(const Obstacle *o) {
  if (!o->active) return;	/* If obstacle slot is not in use, nothing to draw */
  GLCD_SetBackColor(White);	/* same white background as road */
  GLCD_SetTextColor(Red);	/* Obstacles drawn in red */
  GLCD_DisplayChar(o->row, o->col, 1, '#');    /* draw '#' to represent obstacle car */
}

/* Erase an obstacle by printing a space where it used to be */
static void erase_obstacle(const Obstacle *o) {
  if (!o->active) return;	/* Ignore inactive obstacles */
  GLCD_SetBackColor(White);	/* Background remains white */
  GLCD_DisplayChar(o->row, o->col, 1, ' ');	/* Overwrite with space character */
}

/* "Highway Dodge" lane-change game.
   Move your car up/down with the joystick to avoid oncoming cars. */
int game(void) {
  const int lane_rows[NUM_LANES] = { 2, 4, 6 };  /* three lanes on the road */
  Obstacle obs[NUM_OBSTACLES];
  int i;
  int lane = 1;        /* start in middle lane */
  int score = 0;			/* player's score increments for each obstacle that passes safely */
  int js;							/* joystick input value form get_button() */
  char score_str[20];	/* buffer to store formatted socre string */

  /* initialise obstacle list */
  for (i = 0; i < NUM_OBSTACLES; i++) {
    obs[i].active = 0;
    obs[i].row    = 0;
    obs[i].col    = 0;
  }

  GLCD_Clear(White);
  GLCD_SetBackColor(White);
  GLCD_SetTextColor(Blue);
  GLCD_DisplayString(0, 0, 1, "   Highway Dodge   ");

  GLCD_SetTextColor(Black);
  sprintf(score_str, "Score: 0          ");
  GLCD_DisplayString(1, 0, 1, score_str);

  draw_player(lane_rows[lane]);
  delay(5);

	/* Main game loop: runs until collision occurs and function returns score */
  while (1) {
    js = get_button();

    /* lane change with joystick */
    if (js == KBD_UP && lane > 0) {
      erase_player(lane_rows[lane]);
      lane--;
      draw_player(lane_rows[lane]);
    } else if (js == KBD_DOWN && lane < (NUM_LANES - 1)) {
      erase_player(lane_rows[lane]);
      lane++;
      draw_player(lane_rows[lane]);
    }

    /* maybe spawn a new obstacle at the far right */
    if ((rand() % 8) == 0) {                 /* ~1 in 8 frames */
      for (i = 0; i < NUM_OBSTACLES; i++) {	/* find a free obstacle slot */
        if (!obs[i].active) {								/* if this slot is inactive */
          obs[i].active = 1;								/* mark obstacle as active */
          obs[i].col    = 19;                /* stat at right-most column */
          obs[i].row    = lane_rows[rand() % NUM_LANES];	/* Random lane row for this car */
          draw_obstacle(&obs[i]);						/* draw new obstacle on screen */
          break;														/* stop after using one free slot */
        }
      }
    }

    /* move all active obstacles one step to the left */
    for (i = 0; i < NUM_OBSTACLES; i++) {
      if (obs[i].active) {								/* only move currently active obstacles */
        erase_obstacle(&obs[i]);					/* erase obstacle at old position */
        obs[i].col--;											/* move obstacle one column left */

        if (obs[i].col < 0) {							/* if obstacle moved off left edge */
          /* went off-screen safely -> score point */
          obs[i].active = 0;						/* free this obstacle slot */
          score++;											/* increase player's score*/
          GLCD_SetBackColor(White);
          GLCD_SetTextColor(Black);
          sprintf(score_str, "Score: %-4d      ", score);	/* update score string */
          GLCD_DisplayString(1, 0, 1, score_str);
        } else {
          draw_obstacle(&obs[i]);
        }
      }
    }

    /* collision check: same lane and column as player */
    for (i = 0; i < NUM_OBSTACLES; i++) {
      if (obs[i].active &&									/* obstacle exists */
          obs[i].row == lane_rows[lane] &&	/* obstacle row matches player's lane */
          obs[i].col == PLAYER_COL) {			/* obstacle column matches PLAYER_COL */
        return score;									/* collision, end game and return score */
      }
    }

    delay(12);
  }
}

/* Game-over screen and replay/exit logic*/
int game_over(int score){
  int js_sel;
  int sel_num = 0;
  char score_str[20];
  sprintf(score_str, "      SCORE: %d      ", score);
  GLCD_Clear(Purple);
  GLCD_SetBackColor(Purple);
  GLCD_SetTextColor(White);
  GLCD_DisplayString(0,0,1,"     GAME OVER      ");
  GLCD_DisplayString(1,0,1, score_str);
  GLCD_DisplayString(2, 0, 1, "   UP/DOWN - Scroll  ");
  GLCD_DisplayString(3, 0, 1, "   SELECT - Select   ");
  while(1){
    js_sel = get_button();
    if (sel_num == 0 || sel_num == 1){
      if (js_sel == KBD_DOWN){
        sel_num++;
      }
      else if (js_sel == KBD_UP){
        sel_num--;
      }
    }
    else {
      sel_num = 0;
    }

    if (sel_num == 0){
      GLCD_SetBackColor(LightGrey);
      GLCD_DisplayString(5,0,1,"Replay");
      GLCD_SetBackColor(Purple);
      GLCD_DisplayString(6,0,1,"Exit");
      js_sel = get_button();
      if (js_sel == KBD_SELECT){
        return 0;
      }
    }
    else if (sel_num == 1){
      GLCD_SetBackColor(Purple);
      GLCD_DisplayString(5,0,1,"Replay");
      GLCD_SetBackColor(LightGrey);
      GLCD_DisplayString(6,0,1,"Exit");
      js_sel = get_button();
      if (js_sel == KBD_SELECT){
        return 1;
      }
    }
  }
}

void highway_dodge(void){
  int game_score = 0;
  int reset; 

  while(1){
    game_score = game();
    reset = game_over(game_score);

    if (reset == 1)
      break;
  }
}