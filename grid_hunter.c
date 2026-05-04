#include <LPC17xx.H>                    /* NXP LPC17xx definitions            */
#include "string.h"
#include "GLCD.h"
#include "LED.h"
#include "KBD.h"
#include <stdlib.h>
#include <stdio.h>

#define __FI        1                      /* Font index 16x24               */
#define __USE_LCD   0                      /* Uncomment to use the LCD       */

/* ========= Helper delay used for cursor and simple timing ========= */
/* Busy-wait delay: bigger div -> shorter delay     */
static void cursor_delay(int div){
    int i;
    int val = 10000000 / div;	/* Choose loop count based on desired speed */
    volatile int count = 0;
    for (i = 0; i < val; i++){
        count++;
    }
}

/* ========= 3x3 grid ========= */

/* Map logical grid row [0..2] to LCD line number */
static int get_ln_pos(int ln){
    int ln_num;
    if (ln == 1){
        ln_num = 5;	/* Middle row of grid drawn on LCD line 5 */
    }
    else if (ln == 0){
        ln_num = 2;	/* Top row of grid drawn on LCD line 2 */
    }
    else { /* ln == 2 */
        ln_num = 8;	/* Bottom row of grid drawn on LCD line 8 */
    }
    return ln_num;
}

/* Map logical grid column [0..2] to LCD column number */
static int get_col_pos(int col){
    int col_num;
    if (col == 1){
        col_num = 9;	/* Middle column of grid drawn on LCD line 5 */
    }
    else if (col == 0){
        col_num = 4;	/* Top column of grid drawn on LCD line 5 */
    }
    else { /* col == 2 */
        col_num = 16;	/* Bottom column of grid drawn on LCD line 5 */
    }
    return col_num;
}

/* Draw the two horizontal grid lines */
static void draw_rows(void){
    GLCD_SetTextColor(White);
    GLCD_DisplayString(3,0,1, "  ---------------- ");
    GLCD_DisplayString(6,0,1, "  ---------------- ");
}

/* Draw the two vertical grid lines */
static void draw_columns(void){
    int i;
    for (i = 1; i < 9 ; i++){
        GLCD_DisplayChar(i,6,1, '|');
    }
    for (i = 1; i < 9 ; i++){
        GLCD_DisplayChar(i,13,1, '|');
    }
}

static void draw_grid(void){
    draw_rows();
    draw_columns();
}

/* Draw the selector cursor at a given cell */
static void draw_cursor(int ln_pos, int col_pos){
    GLCD_DisplayChar(ln_pos, col_pos, 1, '*');
}

/* Erase cursor at given cell */
static void erase_cursor(int ln_pos, int col_pos){
    GLCD_DisplayChar(ln_pos, col_pos, 1, ' ');
}

/* Draw / erase target at a given logical cell */
static void draw_target(int row, int col){
    int ln = get_ln_pos(row);
    int cn = get_col_pos(col);
    GLCD_SetTextColor(Yellow);
    GLCD_DisplayChar(ln, cn, 1, 'X');
}

static void erase_target(int row, int col){
    int ln = get_ln_pos(row);
    int cn = get_col_pos(col);
    GLCD_DisplayChar(ln, cn, 1, ' ');
}

/* ========== Core Grid Hunter Memory Game ========== */
/*
   Rules:
   - Each round, 1–3 targets 'X' flash briefly on the 3x3 grid.
     * Rounds 1–3: 3 X
     * Rounds 4–7: 2 X's
     * Rounds 8–10: 1 X
   - X's then disappear.
   - Move the cursor '*' with the joystick (UP/DOWN/LEFT/RIGHT).
   - Press SELECT when your cursor is on ANY of the X positions.
   - 10 rounds total; score goes up by 1 for each correct hit.
*/

static int play_grid_hunter(void){
    int cursor_row = 1, cursor_col = 1; /*current cursor position in coords */
    int prev_ln, prev_col;							/* previous LCD line/col for cursor (for erasing) */
    int ln, cn;													/* current LCD line/col for cursor */
    int rounds = 0;											/* round counter */
    int score = 0;
    int js;
    char buf[32];

    /* arrays for up to 3 targets per round */
    int tRow[3], tCol[3];
    int numTargets;	/* how many X's in this round */
    int flashDiv;	/* how long X's are visible */
    int i, j;
    int unique;	/* flag used when picking unique target positions */
    int hit;	/* 1 if player selected any correct target, else 0 */

    GLCD_Clear(Purple);
    GLCD_SetBackColor(Purple);
    GLCD_SetTextColor(White);
    GLCD_DisplayString(0,0,1,"   GRID HUNTER ");
    GLCD_DisplayString(1,0,1,"Memori ze X's & pick");

    GLCD_SetBackColor(Black);
    GLCD_ClearLn(2,1);
    draw_grid();

    /* initial cursor in centre cell */
    cursor_row = 1;
    cursor_col = 1;
    ln = get_ln_pos(cursor_row);
    cn = get_col_pos(cursor_col);
    draw_cursor(ln, cn);

    while (rounds < 10){

        /* difficulty: number of X's and flash time */
        if (rounds < 3){
            numTargets = 3;       /* easy: 3 targets */
            flashDiv   = 1;       /* longer show time */
        }
        else if (rounds < 7){
            numTargets = 2;       /* medium: 2 targets */
            flashDiv   = 2;       /* shorter time */
        }
        else {
            numTargets = 1;       /* hard: 1 target */
            flashDiv   = 3;       /* shortest time */
        }

        /* generate unique target cells */
        for (i = 0; i < numTargets; i++){
            unique = 0;
            while (!unique){
                tRow[i] = rand() % 3;
                tCol[i] = rand() % 3;
                unique = 1;
                for (j = 0; j < i; j++){
                    if (tRow[i] == tRow[j] && tCol[i] == tCol[j]){
                        unique = 0;
                        break;
                    }
                }
            }
        }

        /* hide cursor while flashing targets */
        erase_cursor(ln, cn);

        /* flash all X's briefly */
        for (i = 0; i < numTargets; i++){
            draw_target(tRow[i], tCol[i]);
        }

        cursor_delay(flashDiv);   /* brief visible time */

        for (i = 0; i < numTargets; i++){
            erase_target(tRow[i], tCol[i]);
        }

        /* redraw cursor at current position after targets disappear */
        ln = get_ln_pos(cursor_row);
        cn = get_col_pos(cursor_col);
        draw_cursor(ln, cn);

        /* show round and score info on bottom lines */
        GLCD_SetBackColor(Purple);
        GLCD_SetTextColor(White);
        sprintf(buf, "Round: %d/10      ", rounds + 1);
        GLCD_DisplayString(9, 0, 1, buf);
        sprintf(buf, "Score: %d         ", score);
        GLCD_DisplayString(8, 0, 1, buf);

        /* inner loop: wait until user presses SELECT */
        while (1){
            js = get_button();

            prev_ln  = ln;
            prev_col = cn;

            if (js == KBD_UP && cursor_row > 0){
                cursor_row--;
            } 
            else if (js == KBD_DOWN && cursor_row < 2){
                cursor_row++;
            } 
            else if (js == KBD_LEFT && cursor_col > 0){
                cursor_col--;
            } 
            else if (js == KBD_RIGHT && cursor_col < 2){
                cursor_col++;
            } 
            else if (js == KBD_SELECT){
                /* evaluate this round: hit if cursor matches ANY X */
                hit = 0;
                for (i = 0; i < numTargets; i++){
                    if (cursor_row == tRow[i] && cursor_col == tCol[i]){
                        hit = 1;
                        break;
                    }
                }

                if (hit){
                    score++;
                    GLCD_SetBackColor(Purple);
                    GLCD_SetTextColor(Green);
                    GLCD_DisplayString(2,0,1,"       HIT!         ");
                } else {
                    GLCD_SetBackColor(Purple);
                    GLCD_SetTextColor(Red);
                    GLCD_DisplayString(2,0,1,"       MISS         ");
                }
                cursor_delay(5);
                GLCD_SetBackColor(Purple);
                GLCD_ClearLn(2,1);
                break;  /* go to next round */
            }

            /* update cursor drawing if moved */
            ln = get_ln_pos(cursor_row);
            cn = get_col_pos(cursor_col);
            if (ln != prev_ln || cn != prev_col){
                erase_cursor(prev_ln, prev_col);
                draw_cursor(ln, cn);
            }

            cursor_delay(2);
        }

        rounds++;
    }

    return score;
}

/* ========== Game over screen and replay handling ========== */

static int grid_hunter_game_over(int score){
    int js_sel;
    int sel_num = 0;
    char buf[32];

    GLCD_Clear(Purple);
    GLCD_SetBackColor(Purple);
    GLCD_SetTextColor(White);
    GLCD_DisplayString(0,0,1,"     GAME OVER      ");
    sprintf(buf, "    SCORE: %2d/10   ", score);
    GLCD_DisplayString(1,0,1, buf);
    GLCD_DisplayString(2,0,1,"   UP/DOWN - Scroll ");
    GLCD_DisplayString(3,0,1,"   SELECT - Select  ");

    /* initial menu state */
    GLCD_SetBackColor(LightGrey);
    GLCD_DisplayString(5,0,1,"Replay");
    GLCD_SetBackColor(Purple);
    GLCD_DisplayString(6,0,1,"Exit");

    while (1){
        js_sel = get_button();

        if (sel_num == 0 || sel_num == 1){
            if (js_sel == KBD_DOWN){
                sel_num = 1;	/* move selection to exit */
            } else if (js_sel == KBD_UP){
                sel_num = 0;	/* move selection to replay */
            }
        } else {
            sel_num = 0;
        }

        if (sel_num == 0){
            GLCD_SetBackColor(LightGrey);
            GLCD_DisplayString(5,0,1,"Replay");
            GLCD_SetBackColor(Purple);
            GLCD_DisplayString(6,0,1,"Exit");
            if (js_sel == KBD_SELECT){
                return 0;   /* replay */
            }
        } else {
            GLCD_SetBackColor(Purple);
            GLCD_DisplayString(5,0,1,"Replay");
            GLCD_SetBackColor(LightGrey);
            GLCD_DisplayString(6,0,1,"Exit");
            if (js_sel == KBD_SELECT){
                return 1;   /* exit to game menu */
            }
        }
    }
}

/* Public entry point used by game_menu.c .  */
int grid_hunter(void){
    int score;
    int exit_flag;

    while (1){
        score = play_grid_hunter();
        exit_flag = grid_hunter_game_over(score);

        if (exit_flag){
            /* return to game menu */
            return 0;
        }
        /* otherwise loop and replay */
    }
}