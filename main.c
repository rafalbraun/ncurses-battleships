#include <time.h>
#include <curses.h>
#include <stdlib.h>
#include <menu.h>
#include <stdlib.h>
#include <stdio.h>
#include "constants.h"

WINDOW *create_newwin(int height, int width, int starty_main, int startx_main);
void destroy_win(WINDOW *local_win);

enum state {
	NOTHING,
	PLACED,
	SURROUND,
	MISSED,
	HIT
};

int HEIGHT;
int WIDTH;

WINDOW *players_window;
int **players_board;

int main(int argc, char* argv[]) {

	HEIGHT = BOARD_SIZE * STEP_VERTICAL;
	WIDTH = BOARD_SIZE * STEP_HORIZONTAL;

	initscr();
	cbreak();
	keypad(stdscr, TRUE);
	curs_set(0);

	//printf("SHIP_SIZE: %d \n", SHIP_SIZE);

	//create_board();
}

void create_board() {
	players_board=(int **)calloc(BOARD_SIZE, sizeof(int));
	for (int i=0 ; i < BOARD_SIZE ; i++) {
		players_board[i] = (int *)calloc(BOARD_SIZE, sizeof(int));
	}
	
	WINDOW *local_win;
	local_win = create_newwin(HEIGHT, WIDTH, STARTY, STARTX);
	getch();
	destroy_win(local_win);
}

WINDOW *create_newwin(int height, int width, int starty_main, int startx_main)    //done
{	
	WINDOW *local_win;

	int i=0, j=0;

	local_win = newwin(height, width, STARTY, STARTX);
	box(local_win, 0 , 0);
	for (j=1 ; j<HEIGHT ; j++) {	
		if ((j%2) == 0) {		
			for (i=1 ; i<WIDTH ; i++) {
				mvwaddch(local_win,j,i,ACS_HLINE);
				mvwaddch(local_win,j,i,ACS_HLINE);
				mvwaddch(local_win,j,i,ACS_HLINE);
				mvwaddch(local_win,j,i,ACS_HLINE);
			}
		} else {
			for (i=0 ; i<WIDTH ; i++) {
				if ((i%4) == 0) {
					mvwaddch(local_win,j,i,ACS_VLINE);
				}
				else {
					mvwaddch(local_win,j,i,' ');
				}
			}
		}
	}
	wrefresh(local_win);

	return local_win;
}

void destroy_win(WINDOW *local_win) {
	for (int j=1 ; j<HEIGHT ; j++) {
		if ((j%2) == 0) {
			for (int i=1 ; i<WIDTH ; i++) {
				mvwaddch(local_win,j,i,' ');
				mvwaddch(local_win,j,i,' ');
				mvwaddch(local_win,j,i,' ');
				mvwaddch(local_win,j,i,' ');
			}
		} else {
			for (int i=0 ; i<WIDTH ; i++) {
				if ((i%4) == 0) {
					mvwaddch(local_win,j,i,' ');
				} else {
					mvwaddch(local_win,j,i,' ');
				}
			}
		}
	}

	wborder(local_win, ' ', ' ', ' ',' ',' ',' ',' ',' ');
	wrefresh(local_win);
	delwin(local_win);
}