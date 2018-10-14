#include <time.h>
#include <curses.h>
#include <stdlib.h>
#include <menu.h>
#include <stdlib.h>
#include <stdio.h>
#include "constants.h"

#define EMPTY 48;
#define SHIP_PLACED 49;
#define SHIP_TOOCLOSE 50;

WINDOW *create_newwin(int height, int width, int starty, int statrx);
void destroy_win(WINDOW* local_win);
bool inside(int cursorx, int cursory, ORIENTATION o,int ship_size);
void print_ship(WINDOW* local_win, int cursorx, int cursory, int ship_size, ORIENTATION o, int sign);
int** allocate_board();
void place_ship(int** players_board, int cursorx, int cursory, int ship_size, ORIENTATION o);
void save_board(int **board);

int CURSORXOLD = 1, CURSORYOLD = 1;
int CURSORX = 1, CURSORY = 1;
ORIENTATION orientation = HORIZONTAL;
ORIENTATION orientationold = HORIZONTAL;

int main() {
	WINDOW *my_win;
	int startx, starty, width, height;
	int ch;

	initscr();
	cbreak();
	keypad(stdscr, TRUE);
	curs_set(0); // cursor invisible
	
	height = 3;
	width = 10;
	startx = (LINES - height)/2;
	starty = (COLS = width)/2;
	printw("Press F1 to exit");
	refresh();
	
	//HEIGHT = 17;
	//WIDTH = 29;
	HEIGHT = 29;
	WIDTH = 39;

	int SHIP_SIZE = 3;
	my_win = create_newwin(HEIGHT, WIDTH, starty, startx);
	print_ship(my_win, CURSORX, CURSORY, SHIP_SIZE, orientation, ACS_CKBOARD);

	int** players_board = allocate_board();

	while ((ch = getch()) != KEY_F(2)) {
		switch(ch) {
			case KEY_LEFT:
				if (inside(CURSORX-2, CURSORY, orientation, SHIP_SIZE)) {
					CURSORX -= 2;
				}
				break;
			case KEY_RIGHT:
				if (inside(CURSORX+2, CURSORY, orientation, SHIP_SIZE)) {
					CURSORX += 2;
				}
				break;
			case KEY_UP:
				if (inside(CURSORX, CURSORY-2, orientation, SHIP_SIZE)) {
					CURSORY -= 2;
				}
				break;
			case KEY_DOWN:
				if (inside(CURSORX, CURSORY+2, orientation, SHIP_SIZE)) {
					CURSORY += 2;
				}
				break;
			case 32:
				orientation = (orientation == VERTICAL) ? HORIZONTAL : VERTICAL;
				if (!inside(CURSORX, CURSORY, orientation, SHIP_SIZE)) {
					orientation = orientationold;
				}
				break;
			case 's':
				printw("GAME SAVED");
				save_board(players_board);
				break;
			case 10:
				//printw(" PLACING SHIP");
				place_ship(players_board, CURSORX, CURSORY, SHIP_SIZE, orientation);
				CURSORXOLD = CURSORX = 1;
				CURSORYOLD = CURSORY = 1;
				orientation = HORIZONTAL;
				break;
		}
		print_ship(my_win, CURSORXOLD,CURSORYOLD, SHIP_SIZE, orientationold, ' ');
		print_ship(my_win, CURSORX, CURSORY, SHIP_SIZE, orientation, ACS_CKBOARD);
		//mvwaddch(my_win,CURSORYOLD,CURSORXOLD,' ');
		//mvwaddch(my_win,CURSORY,CURSORX,ACS_CKBOARD);
		CURSORXOLD = CURSORX;
		CURSORYOLD = CURSORY;
		orientationold = orientation;
		//printw("qdqdqwd");

		wrefresh(my_win);
	} 

	endwin();
	return 0;
}

void place_ship(int** players_board, int cursorx, int cursory, int ship_size, ORIENTATION o) {
	printw("Placed ship, x: %d, y: %d", cursorx, cursory);
	if (o == HORIZONTAL) {
	 	for (int i=0; i<ship_size; i++) {
	 		players_board[cursory][cursorx+2*i] = SHIP_PLACED;
	 	}
	}
	if (o == VERTICAL) {
		for (int i=0; i<ship_size; i++) {
			players_board[cursory+2*i][cursorx] = SHIP_PLACED;
		}		
	}
}

void print_ship(WINDOW* local_win, int cursorx, int cursory, int ship_size, ORIENTATION o, int sign) {
	//mvwaddch(local_win,cursory,cursorx,sign);
	if (o == HORIZONTAL) {
		for (int i=0; i<ship_size; i++) {
			mvwaddch(local_win,cursory,cursorx+2*i,sign);
		}
	}
	if (o == VERTICAL) {
		for (int i=0; i<ship_size; i++) {
			mvwaddch(local_win,cursory+2*i,cursorx,sign);
		}		
	}
	wrefresh(local_win);
}

bool inside(int cursorx, int cursory, ORIENTATION o, int ship_size) {
	//printw("cx: %d, cy: %d, w: %d, h: %d", cursorx, cursory, width, height);
	if (o == HORIZONTAL) {
		for (int i=0; i<ship_size; i++) {
			if (cursorx+2*i >= WIDTH || cursory >= HEIGHT || cursorx+2*i <= 0 || cursory <= 0) {
				return FALSE;
			}
		}
	}
	if (o == VERTICAL) {
		for (int i=0; i<ship_size; i++) {
			if (cursorx >= WIDTH || cursory+2*i >= HEIGHT || cursorx <= 0 || cursory+2*i <= 0) {
				return FALSE;
			}
		}		
	}
	return TRUE;
}

bool collision(int cursorx, int cursory, ORIENTATION o, int ship_size) {

}

WINDOW* create_newwin(int height, int width, int starty, int startx) {
	WINDOW* local_win;
	local_win = newwin(height, width, starty, startx);
	box(local_win, 0, 0);
	for(int i=0; i<height; i++) {
		for(int j=0; j<width; j++) {

			// lines
			if (i % 2 == 0) {
				mvwaddch(local_win,i,j,ACS_HLINE);
			}
			if (j % 2 == 0) {
				mvwaddch(local_win,i,j,ACS_VLINE);
			}

			// plus
			if (i % 2 == 0 && j % 2 ==0) {
				mvwaddch(local_win,i,j,ACS_PLUS);
			}
			/*			
			// 
			if (i % 2 == 0 && j == width-1) {
				mvwaddch(local_win,i,j,ACS_RTEE);
			}
			if (i % 2 == 0 && j == 0) {
				mvwaddch(local_win,i,j,ACS_LTEE);
			}
			if (j % 2 == 0 && i == 0) {
				mvwaddch(local_win,i,j,ACS_TTEE);
			}
			if (j % 2 == 0 && i == height-1) {
				mvwaddch(local_win,i,j,ACS_BTEE);
			}

			// corners
			if (i == 0 && j == 0) {
				mvwaddch(local_win,i,j,ACS_ULCORNER);
			}
			if (i == 0 && j == width-1) {
				mvwaddch(local_win,i,j,ACS_URCORNER);
			}
			if (i == height-1 && j == 0) {
				mvwaddch(local_win,i,j,ACS_LLCORNER);
			}
			if (i == height-1 && j == width-1) {
				mvwaddch(local_win,i,j,ACS_LRCORNER);
			}*/
		}
	}

	//mvwaddch(local_win,CURSORX,CURSORY,ACS_CKBOARD);
	
	wrefresh(local_win);
	return local_win;
}

void destroy_win(WINDOW* local_win) {
	wborder(local_win,' ',' ',' ',' ',' ',' ',' ',' ');
	wrefresh(local_win);
	delwin(local_win);
}

int** allocate_board() {
	printw("Allocated board, WIDTH: %d, HEIGHT: %d", WIDTH, HEIGHT);
	int **players_board = (int **)calloc(HEIGHT, sizeof(int *));
	for (int i=0; i < HEIGHT; i++) {
		players_board[i] = (int *)calloc(WIDTH, sizeof(int));
	}
	for (int i=0; i < HEIGHT; i++) {
		for (int j=0; j < WIDTH; j++) {
			players_board[i][j] = EMPTY;
		}
	}
	return players_board;
}

void save_board(int **board) {
	FILE* file = fopen("./save.out", "w");
	if (file == NULL) {
		printf("Could not open file.");
	}
	for (int i=0; i<HEIGHT; i++) {
		for (int j=0; j<WIDTH; j++) {
			fputc(board[i][j], file);
		}
		fputc('\n', file);
	}
	fclose(file);
	printf("File saved.\n");
}
