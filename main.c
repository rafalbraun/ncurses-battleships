#include <time.h>
#include <curses.h>
#include <stdlib.h>
#include <menu.h>
#include <stdlib.h>
#include <stdio.h>
#include "constants.h"

#define EMPTY 48
#define SHIP_PLACED 49
#define SHIP_TOOCLOSE 50

WINDOW *create_newwin(int height, int width, int starty, int statrx);
int** allocate_board();
bool ship_inside(int cursorx, int cursory, ORIENTATION o, int ship_size);
bool field_inside(int cursorx, int cursory);
bool ship_collision(int** board, int cursorx, int cursory, ORIENTATION o, int ship_size);
void destroy_win(WINDOW* local_win);
void print_ship(WINDOW* local_win, int cursorx, int cursory, int ship_size, ORIENTATION o, int sign);
void place_ship(int** players_board, int cursorx, int cursory, int ship_size, ORIENTATION o);
void save_board(int **board);
void destroy_board(int **players_board);
void placing_mode(WINDOW *my_win, int** players_board, int SHIP_SIZE);
void select_mode(WINDOW *my_win, int** players_board);
void print_normal(WINDOW *my_win, int cursorx, int cursory);
void print_pointer(WINDOW *my_win, int cursorx, int cursory);
void fill_board_randomly(int** board);

int CURSORXOLD = 1, CURSORYOLD = 1;
int CURSORX = 1, CURSORY = 1;
ORIENTATION orientation = HORIZONTAL;
ORIENTATION orientationold = HORIZONTAL;

int main() {
	WINDOW *my_win;
	int startx, starty, width, height;
	
	initscr();
	cbreak();
	keypad(stdscr, TRUE);
	curs_set(0); // cursor invisible
	//start_color();
	//init_pair(1, COLOR_YELLOW, COLOR_GREEN);

	height = 3;
	width = 10;
	startx = (LINES - height)/2;
	starty = (COLS = width)/2;
	printw("Press F1 to exit");
	refresh();
	
	HEIGHT = 29;
	WIDTH = 39;

	//int SHIP_SIZE = 3;
	my_win = create_newwin(HEIGHT, WIDTH, starty, startx);
	
	//int** players_board = allocate_board();
	//select_mode(my_win, players_board);
	// placing_mode(my_win, players_board, 4);
	// placing_mode(my_win, players_board, 3);
	// placing_mode(my_win, players_board, 3);
	// placing_mode(my_win, players_board, 2);
	// placing_mode(my_win, players_board, 2);
	// placing_mode(my_win, players_board, 2);
	// placing_mode(my_win, players_board, 1);
	// placing_mode(my_win, players_board, 1);
	// placing_mode(my_win, players_board, 1);
	// placing_mode(my_win, players_board, 1);

	int** enemys_board = allocate_board();
	fill_board_randomly(enemys_board);
	// main loop 
	//while(true) { //!game_finished()) {
		select_mode(my_win, enemys_board);
		//enemy_shot(players_board);
		//refresh_board(my_win, players_board);
	//}

	endwin();

	return 0;
}

void select_mode(WINDOW *my_win, int** players_board) {
	int ch;
	//print_ship(my_win, CURSORX, CURSORY, SHIP_SIZE, orientation, ACS_CKBOARD);

	print_pointer(my_win,CURSORX,CURSORY);
	wrefresh(my_win);

	while ((ch = getch()) != KEY_F(2)) {
		print_normal(my_win,CURSORX,CURSORY);
		wrefresh(my_win);
		switch(ch) {
			case KEY_LEFT:
				CURSORX -= 2;
				break;
			case KEY_RIGHT:
				CURSORX += 2;
				break;
			case KEY_UP:
				CURSORY -= 2;
				break;
			case KEY_DOWN:
				CURSORY += 2;
				break;
			case 10:
				if (players_board[CURSORY][CURSORX] == SHIP_PLACED) {
					mvwaddch(my_win,CURSORY,CURSORX,ACS_CKBOARD);
				} else {
					mvwaddch(my_win,CURSORY,CURSORX,ACS_BULLET);
				}
				wrefresh(my_win);
				break;
		}
		//printw("qdqdqwd");
		//attron(COLOR_PAIR(1));
		print_pointer(my_win,CURSORX,CURSORY);
		//attroff(COLOR_PAIR(0));

		wrefresh(my_win);
	}
}

void print_normal(WINDOW *my_win, int cursorx, int cursory) {
	mvwaddch(my_win,cursory-1,cursorx,ACS_HLINE);
	mvwaddch(my_win,cursory+1,cursorx,ACS_HLINE);
	mvwaddch(my_win,cursory,cursorx-1,ACS_VLINE);
	mvwaddch(my_win,cursory,cursorx+1,ACS_VLINE);
	//mvwaddch(my_win,cursory,cursorx,' ');
}

void print_pointer(WINDOW *my_win, int cursorx, int cursory) {
	mvwaddch(my_win,cursory-1,cursorx,ACS_TTEE);
	mvwaddch(my_win,cursory+1,cursorx,ACS_BTEE);
	mvwaddch(my_win,cursory,cursorx-1,ACS_LTEE);
	mvwaddch(my_win,cursory,cursorx+1,ACS_RTEE);
	//mvwaddch(my_win,cursory,cursorx,ACS_PLUS);
}

void placing_mode(WINDOW *my_win, int** players_board, int SHIP_SIZE) {
	int ch;
	print_ship(my_win, CURSORX, CURSORY, SHIP_SIZE, orientation, ACS_CKBOARD);

	while ((ch = getch()) != KEY_F(2)) {
		switch(ch) {
			case KEY_LEFT:
				if (ship_inside(CURSORX-2, CURSORY, orientation, SHIP_SIZE) && !ship_collision(players_board, CURSORX-2, CURSORY, orientation, SHIP_SIZE)) {
					CURSORX -= 2;
				}
				break;
			case KEY_RIGHT:
				if (ship_inside(CURSORX+2, CURSORY, orientation, SHIP_SIZE) && !ship_collision(players_board, CURSORX+2, CURSORY, orientation, SHIP_SIZE)) {
					CURSORX += 2;
				}
				break;
			case KEY_UP:
				if (ship_inside(CURSORX, CURSORY-2, orientation, SHIP_SIZE) && !ship_collision(players_board, CURSORX, CURSORY-2, orientation, SHIP_SIZE)) {
					CURSORY -= 2;
				}
				break;
			case KEY_DOWN:
				if (ship_inside(CURSORX, CURSORY+2, orientation, SHIP_SIZE) && !ship_collision(players_board, CURSORX, CURSORY+2, orientation, SHIP_SIZE)) {
					CURSORY += 2;
				}
				break;
			case 32:
				orientation = (orientation == VERTICAL) ? HORIZONTAL : VERTICAL;
				if (!ship_inside(CURSORX, CURSORY, orientation, SHIP_SIZE) && !ship_collision(players_board, CURSORX, CURSORY, orientation, SHIP_SIZE)) {
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
				orientation = orientationold = HORIZONTAL;
				return;
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
}

void place_ship(int** players_board, int cursorx, int cursory, int ship_size, ORIENTATION o) {
	//printw("Placed ship, x: %d, y: %d", cursorx, cursory);
	// if (collision(cursorx, cursory, o, ship_size)) {
	// 	return;
	// }
	if (o == HORIZONTAL) {
		// for (int i=-1; i<=ship_size; i++) {
		// 	for (int j=-1; j<2; j++) {
		// 		if (field_inside(cursorx+2*i, cursory+2*j)) {
		// 			players_board[cursory+2*j][cursorx+2*i] = SHIP_TOOCLOSE;
		// 		}
		// 	}
		// }
	 	for (int i=0; i<ship_size; i++) {
	 		players_board[cursory][cursorx+2*i] = SHIP_PLACED;
	 	}
	 	//players_board[cursory][cursorx-2] = SHIP_TOOCLOSE;
	 	//players_board[cursory][cursorx+2*ship_size+2] = SHIP_TOOCLOSE;
	}
	if (o == VERTICAL) {
		// for (int i=-1; i<=ship_size; i++) {
		// 	for (int j=-1; j<2; j++) {
		// 		if (field_inside(cursorx+2*j, cursory+2*i)) {
		// 			players_board[cursory+2*i][cursorx+2*j] = SHIP_TOOCLOSE;
		// 		}
		// 	}
		// }
		for (int i=0; i<ship_size; i++) {
			players_board[cursory+2*i][cursorx] = SHIP_PLACED;
		}		
	}
}

void print_ship(WINDOW* local_win, int cursorx, int cursory, int ship_size, ORIENTATION o, int sign) {
	//mvwaddch(local_win,cursory,cursorx,sign);
	if (o == HORIZONTAL) {
		/*
		for (int i=-1; i<=ship_size; i++) {
			for (int j=-1; j<2; j++) {
				mvwaddch(local_win, cursory+2*j, cursorx+2*i, 'x');
			}
		}*/
		for (int i=0; i<ship_size; i++) {
			mvwaddch(local_win,cursory,cursorx+2*i,sign);
		}
		//mvwaddch(local_win, cursory, cursorx-2, 'x');
		//mvwaddch(local_win, cursory, cursorx+2*ship_size, 'x');
	}
	if (o == VERTICAL) {
		/*
		for (int i=-1; i<=ship_size; i++) {
			for (int j=-1; j<2; j++) {
				mvwaddch(local_win, cursory+2*i, cursorx+2*j, 'x');
			}
		}*/
		for (int i=0; i<ship_size; i++) {
			mvwaddch(local_win,cursory+2*i,cursorx,sign);
		}		
	}
	wrefresh(local_win);
}

bool ship_collision(int** board, int cursorx, int cursory, ORIENTATION o, int ship_size) {
	if (o == HORIZONTAL) {
		for (int i=0; i<ship_size; i++) {
			if (board[cursory][cursorx + 2*i] == SHIP_PLACED) {
				return TRUE;
			}
		}
	}
	if (o == VERTICAL) {
		for (int i=0; i<ship_size; i++) {
			if (board[cursory + 2*i][cursorx] == SHIP_PLACED) {
				return TRUE;
			}
		}
	}
	return FALSE;
}

bool ship_inside(int cursorx, int cursory, ORIENTATION o, int ship_size) {
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

bool field_inside(int cursorx, int cursory) {
	if (cursorx >= WIDTH || cursory >= HEIGHT || cursorx <= 0 || cursory <= 0) {
		return FALSE;
	}
	return TRUE;
}

// bool collision(int** board, int cursorx, int cursory, ORIENTATION o, int ship_size) {
// 	if (o == HORIZONTAL) {
// 		for (int i=0; i<ship_size; i++) {
// 			if (field_inside(cursorx + 2*i, cursory) && 
// 				board[cursorx + 2*i][cursory] == SHIP_TOOCLOSE &&
// 				board[cursorx + 2*i][cursory] == SHIP_PLACED) {
// 				return FALSE;
// 			}
// 		}
// 	}
// 	if (o == VERTICAL) {
// 		for (int i=0; i<ship_size; i++) {
// 			if (field_inside(cursorx, cursory) && 
// 				board[cursorx][cursory + 2*i] == SHIP_TOOCLOSE &&
// 				board[cursorx][cursory + 2*i] == SHIP_PLACED) {
// 				return FALSE;
// 			}
// 		}
// 	}
// 	return TRUE;
// }

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
			}
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
	//printw("Allocated board, WIDTH: %d, HEIGHT: %d", WIDTH, HEIGHT);
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
}zw

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

void destroy_board(int **players_board) {
	for (int i=0; i < HEIGHT; i++) {
		free(players_board[i]);
	}
	free(players_board);
}

void fill_board_randomly(int** board) {
	board[1][1] = SHIP_PLACED;
	board[3][3] = SHIP_PLACED;
	board[5][5] = SHIP_PLACED;
	board[7][7] = SHIP_PLACED;


}