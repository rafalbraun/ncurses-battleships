// --okrety.c
// --compile: gcc okrety.c -lncurses -lmenu

#include <time.h>
#include <curses.h>
#include <stdlib.h>
#include <menu.h>
#include <stdlib.h>
#include <stdio.h>
#include "menu.c"

/* (r)eal - to wartość dla kursora, (m)emory - to wartość w pamięci*/
#define rtomx(x) (((x+2)/4))
#define rtomy(y) (((y+1)/2))
#define mtorx(x) (4*(x)-2)
#define mtory(y) (2*(y)-1)

#define MAXLINE 50

#define NOTHING 0
#define PLACED 1
#define SURROUND 2
#define MISSED 3
#define HIT 4

// do poprawki: dozwolic square, wczytywanie z pliku (parsing), hard enemy, sprawdzic czy sie nie wykrzacza, poprawic strzelanie

WINDOW *set_board_player(int height, int width, int starty_main, int startx_main, int **tab, int n);
WINDOW *set_board_enemy(int height, int width, int starty_main, int startx_main, int **tab, int n);
WINDOW *create_newwin(int height, int width, int starty_main, int startx_main);
void destroy_win(WINDOW *local_win);
int players_shot(int **tab, int **tab2, WINDOW *local_win, int n);
int enemys_shot(int **tab, WINDOW *local_win, int n);
int check_if_around_pos0(int **tab, int starty, int startx, int size);
int check_if_around_pos1(int **tab, int starty, int startx, int size);
int save(int **tab, FILE *file);
WINDOW* load(int **tab, FILE *file, int height, int width, int starty_main, int startx_main);
int battle_pvsp(WINDOW *players_win, WINDOW *enemys_win, int **players_board, int **enemys_board, int n); 
int battle_pvsc_easy(WINDOW *players_win, WINDOW *enemys_win, int **players_board, int **enemys_board, int n);  
int battle_pvsc_normal(WINDOW *players_win, WINDOW *enemys_win, int **players_board, int **enemys_board, int n);
int normal_enemys_shot(int **tab, WINDOW *local_win, int n, int memox, int memoy);
int battle_pvsc_hard(WINDOW *players_win, WINDOW *enemys_win, int **players_board, int **enemys_board, int n);  
int parsing(char *name);

/*tab1 to plansza gracza, zas tab2 to plansza przeciwnika*/
int size_of_ship = 4;
int n, i;
int step_up_down = 2;
int step_left_right = 4;
int startx_main = 5;
int starty_main = 5;
int memx=0;
int memy=0;
int torus;
int ch;
int lvl;
FILE *file;
char type;
char save_name[MAXLINE], load_name[MAXLINE];  //nazwy plikow do zapisu i odczytu
int pvsp;

int main(int argc, char *argv[])
{	
	WINDOW *players_win; //deklaracja planszy gracza i przeciwnika
	WINDOW *enemys_win;
	int **players_board;  // tablica wskaznikow do wskaznikow w ktorej przechowywane sa wspolrzedne i oznaczenia pol
	int **enemys_board;
	int height;
	int width;
	int result;
	
	if (argc < 2)              //zapytanie o ilosc pol
	{
		printf("Sposób użycia: <liczba pol> \n");
		exit(1);
	}
	n=atoi(argv[1]);
	
	/* zapytanie o rodzaj planszy - torus czy kwadrat */
	printf("Jaki rodzaj planszy wybierasz <(t)orus/(s)quare> ?\n");
	scanf("%c", &type);
	if(type=='t')
		torus=TRUE;
	else
		if(type=='s')
			torus=FALSE;
		else
		{printf("Nieprawidłowy typ\n");
		exit(1);}
		
		/* nazwa pliku do odczytu */
		printf("Jaki plik ma zostać użyty do odczytu (jesli wcisniesz teraz q plik nie zostanie zdefiniowany)?\n");
		scanf("%s", load_name);
		
		if(load_name[0]!='q')
			if(parsing(load_name)==FALSE)
				{printf("Nieprawidłowy format pliku\n");
				exit(1);}
				
		/* nazwa pliku do zapisu */
		printf("Jaki plik ma zostać użyty do ewentualnego zapisu (jesli wcisniesz teraz q plik nie zostanie zdefiniowany)?\n");
		scanf("%s", save_name);
	/*
	if(load_name == NULL || load_name[0] == '\0')
		{printf("Nieprawdiłowa nazwa pliku\n");
		exit(1);}
	*/
	height = 1 + step_up_down*n;
	width = 1 + step_left_right*n;
	
	/*Deklaracja dwoch tablic n x n do przechowywania danych o statkach*/
	
	players_board=(int **)calloc(n+4,sizeof(int));
	for (i=0 ; i < n+4 ; i++)
			players_board[i] = (int *)calloc(n+4, sizeof(int));		
			
	enemys_board=(int **)calloc(n+4,sizeof(int));
	for (i=0 ; i < n+4 ; i++)
			enemys_board[i] = (int *)calloc(n+4, sizeof(int));	
	
	ch=menu();
	
	initscr();
	cbreak();
	keypad(stdscr, TRUE);
	curs_set(0);
	
	switch(ch)
	{
		case 1:{ lvl=1;	
				 pvsp=TRUE;
				 players_win = set_board_player(height,width,starty_main, COLS - startx_main - width, players_board, n);
				 enemys_win = set_board_player(height,width,starty_main,startx_main, enemys_board, n);	
				 wrefresh(stdscr);
				 result=battle_pvsp(players_win, enemys_win, players_board, enemys_board, n); 
				 break;}
		
		case 2:{ lvl=2;
				 players_win = set_board_player(height,width,starty_main, COLS - startx_main - width, players_board, n);
				 enemys_win = set_board_enemy(height,width,starty_main,startx_main, enemys_board, n);	
				 wrefresh(stdscr);
				 result=battle_pvsc_easy(players_win,enemys_win, players_board, enemys_board, n); 
				 break;}
		
		case 3: {lvl=3;			
				 players_win = set_board_player(height,width,starty_main, COLS - startx_main - width, players_board, n);
				 enemys_win = set_board_enemy(height,width,starty_main,startx_main, enemys_board, n);	
				 wrefresh(stdscr);
				 result=battle_pvsc_normal(players_win,enemys_win, players_board, enemys_board, n); 
				 break;}
		
		case 4: {lvl=4;	
				 players_win = set_board_player(height,width,starty_main, COLS - startx_main - width, players_board, n);
				 enemys_win = set_board_enemy(height,width,starty_main,startx_main, enemys_board, n);
				 wrefresh(stdscr);
				 result=battle_pvsc_hard(players_win,enemys_win, players_board, enemys_board, n);
				 break;}	
		
		
		case 5: {
				if(load_name[0]=='q' && load_name[1]=='\0')
					mvwprintw(stdscr, LINES-3, 0, "Nie zdefiniowano pliku do odczytu");
				else
				{file=fopen(load_name,"r");
				fscanf(file, "%d %d %c ", &n, &ch, &type);}
				
				height = 1 + step_up_down*n;	//powinien rysowac dpiero jak robi plansze
				width = 1 + step_left_right*n;
				
				free(enemys_board);
				free(players_board);
	
				players_board=(int **)calloc(n+4,sizeof(int));
				for (i=0 ; i < n+4 ; i++)
					players_board[i] = (int *)calloc(n+4, sizeof(int));		
			
				enemys_board=(int **)calloc(n+4,sizeof(int));
				for (i=0 ; i < n+4 ; i++)
					enemys_board[i] = (int *)calloc(n+4, sizeof(int));
									
						switch(ch)
						{						
							case 1: {
									pvsp=TRUE;									
									enemys_win=load(enemys_board, file, height, width, starty_main, startx_main);
									players_win=load(players_board, file, height, width, starty_main, COLS - startx_main - width);
									wrefresh(stdscr);
									fclose(file);
									result=battle_pvsp(players_win, enemys_win, players_board, enemys_board, n); 
									break;}
							
							case 2: { 									
									enemys_win=load(enemys_board, file, height, width, starty_main, startx_main);
									players_win=load(players_board, file, height, width, starty_main, COLS - startx_main - width);
									wrefresh(stdscr);
									fclose(file);
									result=battle_pvsc_easy(players_win,enemys_win, players_board, enemys_board, n);
									break;}	
																	
							case 3: {									
									enemys_win=load(enemys_board, file, height, width, starty_main, startx_main);
									players_win=load(players_board, file, height, width, starty_main, COLS - startx_main - width);
									wrefresh(stdscr);
									fclose(file);
									result=battle_pvsc_normal(players_win,enemys_win, players_board, enemys_board, n); 
									break;}
									
							case 4: {
									enemys_win=load(enemys_board, file, height, width, starty_main, startx_main);
									players_win=load(players_board, file, height, width, starty_main, COLS - startx_main - width);
									wrefresh(stdscr);
									fclose(file);
									result=battle_pvsc_hard(players_win,enemys_win, players_board, enemys_board, n); 
									break;}
						}	
				break;}
		
		case 6: {curs_set(1); endwin(); exit(0);}
	}
	
	destroy_win(players_win);
	destroy_win(enemys_win);
	
	curs_set(1);
	
	mvwprintw(stdscr, LINES-3, 0, "																	");
	mvwprintw(stdscr, LINES-1, 0, "																	");
	mvwprintw(stdscr, LINES-2, 0, "																	");
	
	endwin();
	
	if (result==TRUE)
		mvwprintw(stdscr,1,0,"Wygrywasz\n");							
	else
		mvwprintw(stdscr,1,0,"Przegrywasz\n");
		
	getch();
	
	mvwprintw(stdscr,1,0,"								");	
	
	free(enemys_board);
	free(players_board);
	
	return 0;
}

int players_shot(int **tab, int **tab2, WINDOW *local_win, int n)   //done
{
	int ch;
	int starty=1;
	int startx=2;
	FILE *file;
	
	while((ch = getch()) != EOF)
	{	switch(ch)
		{	case KEY_LEFT:
				if (rtomx(startx)!=1)
				{
				mvwaddch(local_win,starty,startx-1,' ');
				mvwaddch(local_win,starty,startx+1,' ');
				startx = startx - step_left_right;
				mvwaddch(local_win,starty,startx-1,'>');
				mvwaddch(local_win,starty,startx+1,'<'); 
				wrefresh(local_win);
				}
				break;
			case KEY_RIGHT:
				if (rtomx(startx)!=n)
				{
				mvwaddch(local_win,starty,startx-1,' ');
				mvwaddch(local_win,starty,startx+1,' ');
				startx = startx + step_left_right;
				mvwaddch(local_win,starty,startx-1,'>');
				mvwaddch(local_win,starty,startx+1,'<');   
				wrefresh(local_win);
				}
				break;
			case KEY_UP:
				if (rtomy(starty) !=1)
				{
				mvwaddch(local_win,starty,startx-1,' ');
				mvwaddch(local_win,starty,startx+1,' ');
				starty = starty - step_up_down;
				mvwaddch(local_win,starty,startx-1,'>');
				mvwaddch(local_win,starty,startx+1,'<');   
				wrefresh(local_win);
				}
				break;
			case KEY_DOWN:
				if (rtomy(starty) !=n)
				{
				mvwaddch(local_win,starty,startx-1,' ');
				mvwaddch(local_win,starty,startx+1,' ');
				starty = starty + step_up_down;
				mvwaddch(local_win,starty,startx-1,'>');
				mvwaddch(local_win,starty,startx+1,'<');  
				wrefresh(local_win);
				}
				break;	
			case 'p':	
				mvwprintw(stdscr,3,0,"Wartosc punktu to: starty = %d  startx = %d  rtomy=%d rtomx=%d wartosc = %d to samo pole na drugiej planszy: %d", starty, startx, rtomy(starty), rtomx(startx), tab[rtomy(starty)][rtomx(startx)], tab2[rtomy(starty)][rtomx(startx)]);
				wrefresh(stdscr);
				break;
			case 's':
				if((file=fopen(save_name,"w"))==NULL)
					mvwprintw(stdscr, LINES-3, 0, "Wystąpił błąd zapisu");			
				if(save_name[0]=='q')
					mvwprintw(stdscr, LINES-3, 0, "Nie zdefiniowano pliku do zapisu.");			
				else
				{fprintf(file, "%d %d %c ", n, lvl, type);
				save(tab, file);
				save(tab2, file);
				fclose(file);
				mvwprintw(stdscr, LINES-3, 0, "Plik zapisano pomyslnie.");}
				break;
			case 10:
				mvwaddch(local_win,starty,startx-1,' ');
				mvwaddch(local_win,starty,startx+1,' ');
				if (tab[rtomy(starty)][rtomx(startx)]==3 || tab[rtomy(starty)][rtomx(startx)]==4)
				{		
					mvwprintw(stdscr,LINES-2,0,"Garcz: już tu strzelales!, wspolrzedne to: y=%d , x=%d ", rtomy(starty), rtomx(startx) );
					wrefresh(stdscr);		
				}
				else
				{
				if (tab[rtomy(starty)][rtomx(startx)]==1)
					{	
						
						{	mvwprintw(stdscr,LINES-2,0,"Garcz:trafiles,wspolrzedne to: y=%d , x=%d ", rtomy(starty), rtomx(startx) );
							wrefresh(stdscr);	}
						
						mvwaddch(local_win,starty,startx,'X');
						
						tab[rtomy(starty)][rtomx(startx)]=4;
						wrefresh(local_win);
						return TRUE;
					}			
					else
						{		
																							
							{	mvwprintw(stdscr,LINES-2,0,"Gracz:chybiles,wspolrzedne to: y=%d , x=%d ", rtomy(starty), rtomx(startx) );
								wrefresh(stdscr);	}
						
							mvwaddch(local_win,starty,startx,'-');
							
							tab[rtomy(starty)][rtomx(startx)]=3;
							wrefresh(local_win);
							return FALSE;
						}
				}
		}
	}
	return EXIT_FAILURE;
}

int shot(WINDOW* local_win, int **tab, int starty, int startx)
{
	if (tab[starty][startx]==1)
					{
						mvwaddch(local_win,mtory(starty),mtorx(startx),'X');
						wrefresh(local_win);												
						
						mvwprintw(stdscr,LINES-1,0,"																								");				
						wrefresh(stdscr);
						
						mvwprintw(stdscr,LINES-1,0,"Wróg:trafiles,wspolrzedne to: y=%d, x=%d (aby zobaczyc kolejny strzal wcisnij dowolny przycisk", starty, startx);				
						wrefresh(stdscr);
						getch();						
						
						tab[starty][startx]=4;
						return TRUE;
					}
				else
					{
						mvwaddch(local_win, mtory(starty), mtorx(startx),'-');
						wrefresh(local_win);					
											
						mvwprintw(stdscr,LINES-1,0,"																								");				
						wrefresh(stdscr);					
											
						mvwprintw(stdscr,LINES-1,0,"Wróg:chybiles,wspolrzedne to: y=%d, x=%d (aby przejsc do strzalu wcisnij dowlony przycisk)", starty, startx);
						wrefresh(stdscr);
						
						tab[starty][startx]=3;
						return FALSE;
					}
}

int enemys_shot(int **tab, WINDOW *local_win, int n)    //done
{
	srand(time(NULL));
	int starty=1;
	int startx=1;
							/*dopóki wartosc pola jest wieksza od 2 i wylosowane wspolrzedne sa ujemne trzeba losowac nowe wspolrzedne*/
		while(tab[starty][startx] > 2)	
		{			
			startx=((rand()%n)+ 1);	
			starty=((rand()%n)+ 1);
		}
		
			if(shot(local_win, tab, starty, startx)==TRUE)
			{	memy=starty;
				memx=startx;
				return TRUE;	}
			else
			return FALSE;
			
	return EXIT_FAILURE;	
}

int hard_enemys_shot(int **tab, WINDOW *local_win, int n)    //done
{
	srand(time(NULL));
	int starty=1;
	int startx=1;
							/*dopóki wartosc pola jest wieksza od 2 i wylosowane wspolrzedne sa ujemne trzeba losowac nowe wspolrzedne*/
		while(tab[starty][startx] != 1)	
		{		
				startx=((rand()%n)+ 1);		
				starty=((rand()%n)+ 1);
		}
		
				if (tab[starty][startx]==1)
					{
						mvwaddch(local_win,mtory(starty),mtorx(startx),'X');
						wrefresh(local_win);												
						
						mvwprintw(stdscr,LINES-1,0,"																								");				
						wrefresh(stdscr);
						
						mvwprintw(stdscr,LINES-1,0,"Wróg:trafiles,wspolrzedne to: y=%d, x=%d (aby zobaczyc kolejny strzal wcisnij dowolny przycisk", starty, startx);					
						wrefresh(stdscr);
						getch();
						
						memx=startx;
						memy=starty;
						
						tab[starty][startx]=4;
						return TRUE;
					}
				else
					{
						mvwaddch(local_win, mtory(starty), mtorx(startx),'-');
						wrefresh(local_win);					
											
						mvwprintw(stdscr,LINES-1,0,"																							");				
						wrefresh(stdscr);					
											
						mvwprintw(stdscr,LINES-1,0,"Wróg:chybiles,wspolrzedne to: y=%d, x=%d (aby przejsc do strzalu wcisnij dowlony przycisk)", starty, startx);
						wrefresh(stdscr);
						
						tab[starty][startx]=3;
						return FALSE;
					}
		
	return EXIT_FAILURE;	
}

int normal_enemys_shot(int **tab, WINDOW *local_win, int n, int memox, int memoy)
{/*
	if(tab[memoy-1][memox]==4)
		memoy--;
	if(tab[memoy+1][memox]==4)
		memoy++;
	if(tab[memoy][memox-1]==4)
		memox--;
	if(tab[memoy][memox+1]==4)
		memoy++;
	*/
	if(memoy!=1 && tab[memoy-1][memox]<3)
	{
			if(shot(local_win, tab, memoy-1, memox)==TRUE)
				return TRUE;
			else		
				return FALSE;
	}
	if(memoy!=n && tab[memoy+1][memox]<3)
	{
			if(shot(local_win, tab, memoy+1, memox)==TRUE)
				return TRUE;
				else		
				return FALSE;
	}
	if(memox!=n && tab[memoy][memox+1]<3)
	{
			if(shot(local_win, tab, memoy, memox+1)==TRUE)
				return TRUE;
				else		
				return FALSE;
	}
	if(memox!=1 && tab[memoy][memox-1]<3)
	{
			if(shot(local_win, tab, memoy, memox-1)==TRUE)
				return TRUE;
				else		
				return FALSE;
	}
	

	
	
	
		if(enemys_shot(tab, local_win, n)==TRUE)
			return TRUE;
			else
			{
				memox=0;
				memoy=0;
			}
			return FALSE;
	
}

WINDOW *set_board_enemy(int height, int width, int starty_main, int startx_main, int **tab, int n)      //done
{
	int enemys_ships=0;
	int size_of_ship=4;	
	int i, x, y, vertical;
	int flagx=0, flagy=0;
	srand(time(NULL));
	WINDOW *local_win;
	
	local_win = create_newwin(height,width,starty_main,startx_main);
	
	while(enemys_ships < 20)
	{	/* x i y przyjmują wartości od 1 do n minus  wielkosc statku */
		/* jednak gdy program sie uruchamia probouje on najwidoczniej wyciagnac wartosci z jakichs dziwnych miejsc */
		x=((rand()%(n)) + 1 - size_of_ship);
		y=((rand()%(n)) + 1 - size_of_ship);

	/*	file_err=fopen("file_err.html","wt");	
		fprintf(file_err, "%d  %d \n", x, y);
		fclose(file_err);*/

		vertical=(rand()%2);
		//startx=mtorx(x);
		//starty=mtory(y);
		
		if(x>0 && y>0)		
		{
		
		for(i=0 ; i<size_of_ship ; i++)
					{ 
						if (tab[y + i][x]==1)
							{flagy=1;}
					}
		
		for(i=0 ; i<size_of_ship ; i++)
					{ 
						if (tab[y][x + i]==1)
							{flagx=1;}
					}

		
				if ( (vertical==0 && check_if_around_pos0(tab, y, x, size_of_ship)!=1 && flagx==0)  ||  (vertical==1 && check_if_around_pos1(tab, y, x, size_of_ship)!=1 && flagy==0) )
				{	
					enemys_ships=enemys_ships + size_of_ship;	
				
					if(vertical==0)
					{	
						tab[y - 1][x - 1]=2;						
						tab[y][x - 1]=2;						
						tab[y + 1][x - 1]=2;						
											
						for (i=0 ; i < (size_of_ship); i++)
						{
						mvwaddch(local_win,mtory(y),mtorx(x)+i*step_left_right,ACS_CKBOARD);                  //..........................................
						wrefresh(local_win);	
						tab[y][x+i]=1;
						tab[y - 1][x+i]=2;					
						tab[y + 1][x+i]=2;		
						//x=x+1;
															
						}
						
												
						tab[y - 1][x + size_of_ship]=2;					
						tab[y][x + size_of_ship]=2;					
						tab[y + 1][x + size_of_ship]=2;
						
					}
					else
					{
						if(vertical==1)					
						{						
						tab[y - 1][x + 1]=2;		
						tab[y - 1][x]=2;		
						tab[y - 1][x - 1]=2;											
						
						for (i=0 ; i < (size_of_ship); i++)
							{
							tab[y+i][x]=1;
							mvwaddch(local_win,mtory(y)+i*step_up_down,mtorx(x),ACS_CKBOARD);	   // .....................................................
							wrefresh(local_win);					
							tab[y+i][x - 1]=2;					
							tab[y+i][x + 1]=2;
							
							//y++;
						
							}
							
						tab[y + size_of_ship][x - 1]=2;
						tab[y + size_of_ship][x]=2;
						tab[y + size_of_ship][x + 1]=2;
															
						}
					}
					wrefresh(local_win);														
				}
				
					flagx=0;
					flagy=0;	
		}									
					if (enemys_ships==4) 
					{size_of_ship=3;}
					if (enemys_ships==10) 
					{size_of_ship=2;}
					if (enemys_ships==16) 
					{size_of_ship=1;}
					if (enemys_ships==20) 
					{			
						wrefresh(local_win);	
						endwin();
						//mvwprintw(stdscr,4,0,"Wartosc to: enemys_ships= %d  ,size_of_ship= %d  blad = %d", enemys_ships, size_of_ship, blad);
						wrefresh(stdscr);
						return local_win;
									}
			
	}
	return local_win;
}

WINDOW *set_board_player(int height, int width, int starty_main, int startx_main, int **tab, int n)
{	/*direction - zmienna pobierająca klawisz od gracza*/ 
	/*flag_s, flag_ty, flag_tx - flagi przekazujące informację czy na drodze statku nie ma przeszkód w postaci innych statków*/
	/*starty, startx - przechowują aktualne współrzędne statku (kursora)*/
	/*players_ships - ilość statków na planszy, size_of_ship - rozmiar aktualnie ustawianego statku*/
	/*vertical - zmienna przyjmująca wartości TRUE/FALSE i mówiąca o pionowym/poziomym ustawieniu statku*/
	/*flagx, flagy - potrzebne do sprawdzenia przy ustawianiu statku czy nie ma żadnych statków w pobliżu*/
	/*local_win - wskaźnik na okno gracza*/
	int direction, flag_s, flag_ty, flag_tx, starty, startx;
	int players_ships=0, vertical=FALSE, size_of_ship=4, flagx=0, flagy=0;
	WINDOW *local_win;
	
	
	local_win = create_newwin(height,width,starty_main,startx_main);
	
	starty=1;
	startx=2;
	
	for (i=0 ; i < (size_of_ship); i++)
					{
						mvwaddch(local_win,starty,startx,ACS_CKBOARD);
						tab[rtomy(starty)][rtomx(startx)]=2;
						startx = startx + step_left_right;
					}
					startx = startx - (size_of_ship)*step_left_right;
					wrefresh(local_win);
	
	
	while( ((direction = getch()) != EOF)  &&  players_ships<20)
	{	
							
		switch(direction)
		{	
			case KEY_LEFT:
				/* pętla sprawdzająca czy na lewo od statku znajduje się inny statek i oznacza taka sytuacja przypisujac zmiennej flaga wartosc 1*/
				for(i=0 ; i<size_of_ship ; i++)
					{ 
						if (tab[rtomy(starty)+i][rtomx(startx)-1]==1)
							flag_s=TRUE;
					}
				
				for(i=0 ; i<size_of_ship ; i++)
					{ 
						if (tab[rtomy(starty)+i][n]==1)
							flag_ty=TRUE;
					}
				
				for(i=0 ; i<size_of_ship ; i++)
					{ 
						if (tab[rtomy(starty)][n-i]==1)
							flag_tx=TRUE;
					}	
					
				if (vertical==TRUE && flag_s!=TRUE)
				{		/*usuwanie statku z planszy*/
						for (i=0 ; i< (size_of_ship) ; i++)
						{	
							mvwaddch(local_win,starty,startx,' ');
							starty = starty + step_up_down;								
						}	
						starty = starty - step_up_down;
						/*ustawienie kursora*/
						if(rtomx(startx)==1 && torus==TRUE && flag_ty!=TRUE)
							startx = mtorx(n);
						else if(rtomx(startx)!=1)
								startx = startx - step_left_right;
						/*rysowanie nowego statku*/
						for (i=0 ; i<(size_of_ship) ; i++)
						{
							mvwaddch(local_win,starty,startx,ACS_CKBOARD);
							starty = starty - step_up_down;
						}
						starty = starty + step_up_down;
						wrefresh(local_win);								
				}
				/* zestaw działań gdy statek jest poziomo*/
				if (vertical==FALSE && tab[rtomy(starty)][rtomx(startx) - 1]!=1 )
				{									
						if(torus==TRUE && rtomx(startx)==1 && flag_tx!=TRUE)
						{for(i=0 ; i<=size_of_ship ; i++)
							{mvwaddch(local_win,starty,startx,' '); startx=startx+step_left_right;}							
						startx=mtorx(n-(size_of_ship-1));
						 for(i=0 ; i<=size_of_ship ; i++)
							{mvwaddch(local_win,starty,startx,ACS_CKBOARD); startx=startx+step_left_right;}
						startx=mtorx(n-(size_of_ship-1));
						wrefresh(local_win);}	
						
						else if(rtomx(startx)!=1)
						{startx = startx + (size_of_ship - 1)*step_left_right;
						mvwaddch(local_win,starty,startx,' ');															
						startx = startx - size_of_ship*step_left_right;
						mvwaddch(local_win,starty,startx,ACS_CKBOARD);} 
						wrefresh(local_win);				
				}
				flag_s=FALSE;
				flag_tx=FALSE;
				flag_ty=FALSE;
				break;
				
			case KEY_RIGHT:
			
				for(i=0 ; i<size_of_ship ; i++)
					{ 
						if (tab[rtomy(starty)+i][rtomx(startx) + 1]==1)
							{flag_s=TRUE;}
					}
				
				for(i=0 ; i<size_of_ship ; i++)
					{ 
						if (tab[rtomy(starty)+i][1]==1)
							{flag_ty=TRUE;}
					}
				
				for(i=0 ; i<size_of_ship ; i++)
					{ 
						if (tab[rtomy(starty)][1+i]==1)
							flag_tx=TRUE;
					}
				
				/*sprawdzenie czy w wypadku gdy statek jest pionowo nie jest przy prawej krawedzi planszy i wartosci flagi*/
				if (vertical==TRUE && flag_s!=TRUE)
					{					
							for (i=0 ; i< (size_of_ship) ; i++)
							{	
								mvwaddch(local_win,starty,startx,' ');
								starty = starty + step_up_down;								
							}															
							starty = starty - step_up_down;
							
							if(rtomx(startx)==n && torus==TRUE && flag_ty!=TRUE)							
								startx=mtorx(1);
							else if(rtomx(startx)!=n)
								startx = startx + step_left_right;
								
							for (i=0 ; i<(size_of_ship) ; i++)
							{
								mvwaddch(local_win,starty,startx,ACS_CKBOARD);
								starty = starty - step_up_down;
							}
							starty = starty + step_up_down;
							wrefresh(local_win);
					}
				/*sprawdzenie czy gdy statek jest poziomo czy nie jest przy krawedzi planszy oraz czy w przesunieciu nie przeszkadza inny statek*/
				if (vertical==FALSE && tab[rtomy(starty)][rtomx(startx) + size_of_ship]!=TRUE) 
				{			
							if((rtomx(startx)==n+1-size_of_ship) && torus==TRUE && flag_tx!=TRUE)
							{for(i=0 ; i<size_of_ship ; i++)
							{mvwaddch(local_win,starty,startx,' '); startx=startx+step_left_right;}							
							startx=mtorx(1);
							for(i=0 ; i<size_of_ship ; i++)
							{mvwaddch(local_win,starty,startx,ACS_CKBOARD); startx=startx+step_left_right;}
							startx=mtorx(1);
							wrefresh(local_win);}	
																					
							else if(rtomx(startx)!=n+1-size_of_ship)
							{
							mvwaddch(local_win,starty,startx,' ');
							startx = startx + size_of_ship*step_left_right;																								
							mvwaddch(local_win,starty,startx,ACS_CKBOARD); 
							wrefresh(local_win);	
							startx = startx - (size_of_ship - 1)*step_left_right;
							mvwaddch(local_win,starty,startx,ACS_CKBOARD);
							wrefresh(local_win);
							}
				}
				flag_s=FALSE;
				flag_tx=FALSE;
				flag_ty=FALSE;
				break;
				
			case KEY_UP:
			
				for(i=0 ; i<size_of_ship ; i++)
					{ 
						if (tab[rtomy(starty) - 1][rtomx(startx)+i]==1)
							flag_s=1;
					}
				
				for(i=0 ; i<size_of_ship ; i++)
					{ 
						if (tab[n][rtomx(startx)+i]==1)
							flag_tx=TRUE;
					}
				
				for(i=0 ; i<size_of_ship ; i++)
					{ 
						if (tab[n-i][rtomx(startx)]==1)
							flag_ty=TRUE;
					}
					
				if (vertical==TRUE && tab[rtomy(starty)-1][rtomx(startx)]!=TRUE)
					{
						if(torus==TRUE && rtomy(starty)==1 && flag_ty!=TRUE)
						{for(i=0 ; i<size_of_ship ; i++)
							{mvwaddch(local_win,starty,startx,' '); starty=starty+step_up_down;}
						starty=mtory(n+1-size_of_ship);
						for(i=0 ; i<size_of_ship ; i++)
							{mvwaddch(local_win,starty,startx,ACS_CKBOARD); starty=starty+step_up_down;}
						starty=mtory(n+1-size_of_ship);	
						wrefresh(local_win);}	
																		
						else if(rtomy(starty)!=1)	
						{starty = starty + step_up_down*(size_of_ship-1);
						mvwaddch(local_win,starty,startx,' ');
						starty = starty - step_up_down*(size_of_ship);
						mvwaddch(local_win,starty,startx,ACS_CKBOARD);
						wrefresh(local_win);}
					}	
				
				if (vertical==FALSE && flag_s!=TRUE)
						{
							for (i=0 ; i< (size_of_ship - 1) ; i++)
								{
								mvwaddch(local_win,starty,startx,' ');	
								startx = startx + step_left_right;
								}
							mvwaddch(local_win,starty,startx,' ');	
							
							if(torus==TRUE && rtomy(starty)==1 && flag_tx!=TRUE)
								starty=mtory(n);
							else if(rtomy(starty)!=1)
									starty = starty - step_up_down;
							
							for (i=0 ; i< (size_of_ship -1) ; i++)
								{
								mvwaddch(local_win,starty,startx,ACS_CKBOARD);
								startx = startx - step_left_right;
								}
							mvwaddch(local_win,starty,startx,ACS_CKBOARD);
							wrefresh(local_win);
						}	
				flag_s=FALSE;
				flag_tx=FALSE;
				flag_ty=FALSE;
				break;
				
			case KEY_DOWN:
			
				for(i=0 ; i<size_of_ship ; i++)
					{ 
						if (tab[rtomy(starty) + 1][rtomx(startx)+i]==1)
							flag_s=1;
					}
				
				for(i=0 ; i<size_of_ship ; i++)
					{ 
						if (tab[1][rtomx(startx)+i]==1)
							flag_tx=TRUE;
					}
				
				for(i=0 ; i<size_of_ship ; i++)
					{ 
						if (tab[1+i][rtomx(startx)]==1)
							flag_ty=TRUE;
					}
					
				/*sprawdzenie czy w wypadku gdy statek jest pionowo nie wyjdzie za krawedz planszy oraz czy na drodze jest inny statek*/
				if (vertical==TRUE && tab[rtomy(starty)+size_of_ship][rtomx(startx)]!=1)
				{	
					if(torus==TRUE && rtomy(starty)==n+1-size_of_ship && flag_ty!=TRUE)
						{for(i=0 ; i<size_of_ship ; i++)
							{mvwaddch(local_win,starty,startx,' '); starty=starty+step_up_down;}
						starty=mtory(1);
						for(i=0 ; i<size_of_ship ; i++)
							{mvwaddch(local_win,starty,startx,ACS_CKBOARD); starty=starty+step_up_down;}
						starty=mtory(1);	
						wrefresh(local_win);}
					else if(rtomy(starty)!=n+1-size_of_ship)
					{
					mvwaddch(local_win,starty,startx,' ');
					starty = starty + step_up_down*(size_of_ship);				
					mvwaddch(local_win,starty,startx,ACS_CKBOARD);
					starty = starty - step_up_down*(size_of_ship-1);
					wrefresh(local_win);
					}
				}					
				if (vertical==FALSE && tab[rtomy(starty) + 1][rtomx(startx)]!=1 &&  flag_s!=TRUE)
					{
							for (i=0 ; i< (size_of_ship - 1) ; i++)
								{
								mvwaddch(local_win,starty,startx,' ');	
								startx = startx + step_left_right;
								}
							mvwaddch(local_win, starty,startx,' ');	
							
							if(torus==TRUE && rtomy(starty)==n && flag_tx!=TRUE)
								starty=rtomy(1);
							else if(rtomy(starty)!=n)
								starty = starty + step_up_down;
							
							for (i=0 ; i< (size_of_ship -1) ; i++)
								{
								mvwaddch(local_win,starty,startx,ACS_CKBOARD);
								startx = startx - step_left_right;
								}
							mvwaddch(local_win,starty,startx,ACS_CKBOARD);	
							wrefresh(local_win);	
					}
				flag_s=FALSE;
				flag_tx=FALSE;
				flag_ty=FALSE;
				break;	
			
			case 'p':
			mvwprintw(stdscr,1,0,"Wartosc punktu to: starty = %d  startx = %d rtormy = %d rtomx=%d  wartosc = %d ", starty,startx,rtomy(starty), rtomx(startx), tab[rtomy(starty)][rtomx(startx)]);//,tab[mtory(9)][mtorx(9)], mtorx(9), mtory(9));                                  //..........................................................................................
			break;
			
			/* Spacja - zmiana położenia statku z poziomego na pionowe i odwrotnie */
			case 32:
				
				for(i=0 ; i<size_of_ship ; i++)
					{ 
						if (tab[rtomy(starty)][rtomx(startx)+i]==1)
							{flagx=1;}
					}
				
				for(i=0 ; i<size_of_ship ; i++)
					{ 
						if (tab[rtomy(starty)+i][rtomx(startx)]==1)
							{flagy=1;}
					}
				
				if(vertical==FALSE && flagy==FALSE && (starty+(size_of_ship-1)*step_up_down <= (n*step_up_down-1)  ) )
				{	
					for (i=0 ; i< (size_of_ship) ; i++)
							{
							mvwaddch(local_win,starty,startx,' ');	
							startx = startx + step_left_right;
							}
							startx = startx - (size_of_ship)*step_left_right;
					for (i=0 ; i< (size_of_ship) ; i++)
							{
							mvwaddch(local_win,starty,startx,ACS_CKBOARD);	
							starty = starty + step_up_down;
							}
							starty = starty - (size_of_ship)*step_up_down;
							mvwaddch(local_win,starty,startx,ACS_CKBOARD);	
					vertical=1;
					wrefresh(local_win);				
				}
				else 
				if(vertical==TRUE && flagx==FALSE && (startx+(size_of_ship-1)*step_left_right <= (n*step_left_right-2)) )
				{
					for (i=0 ; i< (size_of_ship) ; i++)
							{
							mvwaddch(local_win,starty,startx,' ');	
							starty = starty + step_up_down;
							}
							starty = starty - step_up_down*size_of_ship;
					for (i=0 ; i< (size_of_ship) ; i++)
							{
							mvwaddch(local_win,starty,startx,ACS_CKBOARD);	
							startx = startx + step_left_right;
							}
							startx = startx - step_left_right*size_of_ship;
							mvwaddch(local_win,starty,startx,ACS_CKBOARD);	
					vertical=0;
					wrefresh(local_win);
				}
				flagy=FALSE;
				flagx=FALSE;
				break;
			/* Enter - ustawienie statku */	
			case 10:
			if ( (vertical==0 && check_if_around_pos0(tab, rtomy(starty), rtomx(startx), size_of_ship)!=1)  ||  (vertical==1 && check_if_around_pos1(tab, rtomy(starty), rtomx(startx), size_of_ship)!=1) )
			{	
				
					if(vertical==0)
					{	
						
						tab[rtomy(starty)-1][rtomx(startx)]=2;						
						tab[rtomy(starty)-1][rtomx(startx)-1]=2;						
						tab[rtomy(starty)][rtomx(startx)-1]=2;						
						tab[rtomy(starty)+1][rtomx(startx)-1]=2;					
						tab[rtomy(starty)+1][rtomx(startx)]=2;					
						
						for (i=0 ; i < (size_of_ship - 1); i++)
						{
						tab[rtomy(starty)][rtomx(startx)]=1;
						
						startx = startx + step_left_right;
						mvwaddch(local_win,starty,startx,ACS_CKBOARD);	
						
						
						tab[rtomy(starty)-1][rtomx(startx)]=2;						
						tab[rtomy(starty)+1][rtomx(startx)]=2;
						
						
						}
						
						tab[rtomy(starty)][rtomx(startx)]=1;											
						tab[rtomy(starty)-1][rtomx(startx)]=2;					
						tab[rtomy(starty)-1][rtomx(startx)+1]=2;					
						tab[rtomy(starty)][rtomx(startx)+1]=2;					
						tab[rtomy(starty)+1][rtomx(startx)+1]=2;					
						tab[rtomy(starty)+1][rtomx(startx)]=2;
						
					}
					else
					{
						
						tab[rtomy(starty)][rtomx(startx)-1]=2;						
						tab[rtomy(starty)-1][rtomx(startx)-1]=2;						
						tab[rtomy(starty)-1][rtomx(startx)]=2;						
						tab[rtomy(starty)-1][rtomx(startx)+1]=2;						
						tab[rtomy(starty)][rtomx(startx)+1]=2;						
						
						
						for (i=0 ; i < (size_of_ship-1); i++)
						{
							
						tab[rtomy(starty)][rtomx(startx)]=1;	
														
						starty = starty + step_up_down;
						mvwaddch(local_win,starty,startx,ACS_CKBOARD);	
										
						tab[rtomy(starty)][rtomx(startx)-1]=2;					
						tab[rtomy(starty)][rtomx(startx)+1]=2;	
																						
						}
						
						tab[rtomy(starty)][rtomx(startx)]=1;					
						tab[rtomy(starty)][rtomx(startx)+1]=2;
						tab[rtomy(starty)+1][rtomx(startx)+1]=2;
						tab[rtomy(starty)+1][rtomx(startx)]=2;
						tab[rtomy(starty)+1][rtomx(startx)-1]=2;
						tab[rtomy(starty)][rtomx(startx)-1]=2;					
						
					  }
			

					players_ships=players_ships + size_of_ship;
					startx=2;
					starty=1;
					wmove(local_win,starty,startx);
					vertical=0;	
	
					
					if (players_ships==0) 
					{size_of_ship=4;}	
					if (players_ships==4) 
					{size_of_ship=3;
					tab[rtomy(starty)][rtomx(startx)+1]=0;}
					if (players_ships==10) 
					{size_of_ship=2;
					tab[rtomy(starty)][rtomx(startx)+2]=0;}
					if (players_ships==16) 
					{size_of_ship=1;
					tab[rtomy(starty)][rtomx(startx)+3]=0;}
					if (players_ships==20) 
					{
						endwin();
						return local_win;
							}
					
					for (i=0 ; i < (size_of_ship); i++)
					{
						mvwaddch(local_win,starty,startx,ACS_CKBOARD);
						startx = startx + step_left_right;
					}
					startx = startx - (size_of_ship)*step_left_right;
					wrefresh(local_win);
					
				}
												
	
		}//do switcha
	
	}//do while
	getch();
	endwin();
	return local_win;
}

WINDOW *create_newwin(int height, int width, int starty_main, int startx_main)    //done
{	
	WINDOW *local_win;

	int i=0, j=0;

	local_win = newwin(height, width, starty_main, startx_main);
	box(local_win, 0 , 0);	
	for (j=1 ; j<(step_up_down*n) ; j++)
		{	
			if ((j%2) == 0)
			
				for (i=1 ; i<(step_left_right*n) ; i++)
					{
						mvwaddch(local_win,j,i,ACS_HLINE);
						mvwaddch(local_win,j,i,ACS_HLINE);
						mvwaddch(local_win,j,i,ACS_HLINE);
						mvwaddch(local_win,j,i,ACS_HLINE);
					}
			else		
			
				for (i=0 ; i<(step_left_right*n) ; i++)
					{
						if ((i%4) == 0)
							mvwaddch(local_win,j,i,ACS_VLINE);
						else
							mvwaddch(local_win,j,i,' ');
					}
					
		}
	wrefresh(local_win);

	return local_win;
}

int check_if_around_pos0(int **tab, int starty, int startx, int size) 
{
	int i, state=0;
	
	for (i=0 ; i<size ; i++)
	{
		if (tab[starty][startx + i]==2)
			{state=1;}
	}
	
	return state;
}

int check_if_around_pos1(int **tab, int starty, int startx, int size) 
{
	int i, state=0;
	
	for (i=0 ; i<size ; i++)
	{
		if (tab[starty +i][startx]==2)
			{state=1;}
	}
	
	return state;
}

void destroy_win(WINDOW *local_win)    //done
{	
	int i,j;
	
	for (j=1 ; j<2*n ; j++)
		{	
			if ((j%2) == 0)
			
				for (i=1 ; i<4*n ; i++)
					{
						mvwaddch(local_win,j,i,' ');
						mvwaddch(local_win,j,i,' ');
						mvwaddch(local_win,j,i,' ');
						mvwaddch(local_win,j,i,' ');
					}
			else		
			
				for (i=0 ; i<4*n ; i++)
					{
						if ((i%4) == 0)
							mvwaddch(local_win,j,i,' ');
						else
							mvwaddch(local_win,j,i,' ');
					}
					
		}
	
	wborder(local_win, ' ', ' ', ' ',' ',' ',' ',' ',' ');
	wrefresh(local_win);
	delwin(local_win);
}

/* Funkcja zapisuje odpowiednie wartości do pliku */
int save(int **tab, FILE *file)
{
	int x, y;

				for (y=1 ; y<n+1 ; y++)
				{
					for (x=1 ; x<n+1 ; x++)
						{				
							if(tab[y][x]==0)
								fputs("0", file);
							
							if(tab[y][x]==1)
								fputs("1", file);
							
							if(tab[y][x]==2)
								fputs("2", file);
								
							if(tab[y][x]==3)
								fputs("3", file);
							
							if(tab[y][x]==4)
								fputs("4", file);		
						}
				}							
	return 0;
}
/* Funkcja za pomocą dwóch pętli for wpisuje odowiednie wartości z pliku do tablic, rysuje przzy tym plansze i zwraca okno */
WINDOW* load(int **tab, FILE *file, int height, int width, int starty_main, int startx_main)
{
	int x, y, ch;
	WINDOW *local_win;
	
	local_win = create_newwin(height,width,starty_main,startx_main);
	
	for (y=1 ; y<n+1 ; y++)
		for (x=1 ; x<n+1 ; x++)
			{		
				ch=fgetc(file);
					if((tab[y][x]=ch-'0')==1)
						{mvwaddch(local_win,mtory(y),mtorx(x),ACS_CKBOARD); 
						wrefresh(local_win);}			
					
					else if((tab[y][x]=ch-'0')==2 && (ch-'0'==0))
							{mvwaddch(local_win,mtory(y),mtorx(x),' '); 
							wrefresh(local_win);}
							
						else if((tab[y][x]=ch-'0')==3)
								{mvwaddch(local_win,mtory(y),mtorx(x),'-'); 
								wrefresh(local_win);}
								
							else if((tab[y][x]=ch-'0')==4)
									{mvwaddch(local_win,mtory(y),mtorx(x),'X');
									wrefresh(local_win);}
				}
				
	return local_win;
}

int battle_pvsp(WINDOW *players_win, WINDOW *enemys_win, int **players_board, int **enemys_board, int n)  
{
	int enemys_ships=20;  //ilosc statkow do zestrzelenia
	int players_ships=20;
	int state=FALSE;  // zmienna przechowujaca wynik ostatenigo strzalu
	
	while ((enemys_ships>0) && (players_ships>0))
	{	
		/*pierwszy strzal w turze, jesli celny to gracz strzela poki nie chybi*/	
		state=players_shot(enemys_board, players_board, enemys_win, n);
		enemys_ships=enemys_ships - state;	
			
		while(state==TRUE)
		{
				state=FALSE;
				state=players_shot(enemys_board, players_board, enemys_win, n);
				enemys_ships=enemys_ships - state;
				
						/*sprawdzenie czy nie trzba juz zakonczyc gry*/
						if (enemys_ships==0)													
						{	
							return TRUE;	}						
		}	
		
		
		/*tura zaczyna się po chybieniu gracza - komputer losuje pole */
		state=players_shot(players_board, enemys_board, players_win, n);
		players_ships=players_ships - state;
			
			while(state==TRUE)
			{
				state=FALSE;
				state=players_shot(players_board, enemys_board, players_win, n);			
				players_ships=players_ships - state;
				
						if(players_ships==0)									
						{	
							return FALSE;	}				
			}						
	}
	
	return EXIT_FAILURE;
}

int battle_pvsc_normal(WINDOW *players_win, WINDOW *enemys_win, int **players_board, int **enemys_board, int n)  
{
	int enemys_ships=20;  //ilosc statkow do zestrzelenia
	int players_ships=20;
	int state=0;  // zmienna przechowujaca wynik ostatenigo strzalu
	
	while ((enemys_ships>0) && (players_ships>0))
	{	
		/*pierwszy strzal w turze, jesli celny to gracz strzela poki nie chybi*/
		state=players_shot(enemys_board, players_board, enemys_win, n);
		enemys_ships=enemys_ships - state;

			while(state==TRUE)
			{
				state=FALSE;
				state=players_shot(enemys_board, players_board, enemys_win, n);
				enemys_ships=enemys_ships - state;
				
						/*sprawdzenie czy nie trzba juz zakonczyc gry*/
						if (enemys_ships==0)											
						{	
							return TRUE;	}						

			}	
		/*
		if(players_board[memy-1][memx]==4)
		memy--;
		if(players_board[memy+1][memx]==4)
		memy++;
		if(players_board[memy][memx-1]==4)
		memx--;
		if(players_board[memy][memx+1]==4)
		memy++;
		*/
		/*tura zaczyna się po chybieniu gracza - komputer losuje pole */
		if(memx!=0 && memy!=0)
			state=normal_enemys_shot(players_board, players_win, n, memx, memy);
			else			
			state=enemys_shot(players_board, players_win, n);
			
		players_ships=players_ships - state;	
		
			while(state==TRUE)
			{	/*komputer po trafieniu strzela w pobliskie pola*/
				state=FALSE;
				
				if(memx!=0 && memy!=0)
					state=normal_enemys_shot(players_board, players_win, n, memx, memy);
					
					else		
					state=enemys_shot(players_board, players_win, n);
					
				players_ships=players_ships - state;	
					
					if(players_ships==0)
					{	
						return FALSE;	}
			}
							
	}
	
	return EXIT_FAILURE;
}

int battle_pvsc_easy(WINDOW *players_win, WINDOW *enemys_win, int **players_board, int **enemys_board, int n)  
{
	int enemys_ships=20;  //ilość statków do zestrzelenia
	int players_ships=20;
	int state=FALSE;  // zmienna przechowująca wynik ostatniego strzału
	
	while(enemys_ships>0 && players_ships>0)
	{	
		/*pierwszy strzał w turze, jeśli celny to gracz strzela póki nie chybi*/
		state=players_shot(enemys_board, players_board, enemys_win, n);
		enemys_ships=enemys_ships - state;
				
			while(state==TRUE)
			{
				state=FALSE;
				state=players_shot(enemys_board, players_board, enemys_win, n);			
				enemys_ships=enemys_ships - state;
				
						/*sprawdzenie czy nie trzeba juz zakończyc gry*/
						if(enemys_ships==0)
						{	
							return TRUE;	}				
			}	
		
		/*tura zaczyna się po chybieniu gracza - komputer losuje pole */
		state=enemys_shot(players_board, players_win, n);
		players_ships=players_ships - state;
			
			while(state==TRUE)
			{
				state=FALSE;
				state=enemys_shot(players_board, players_win, n);
		
				players_ships=players_ships - state;
					
						if(players_ships==0)
						{	
							return FALSE;	}					
			}
							
	}
	
	return EXIT_FAILURE;
}

int battle_pvsc_hard(WINDOW *players_win, WINDOW *enemys_win, int **players_board, int **enemys_board, int n)  
{
	int enemys_ships=20;  //ilość statków do zestrzelenia
	int players_ships=20;
	int state=FALSE;  // zmienna przechowująca wynik ostatniego strzału
	
	while(enemys_ships>0 && players_ships>0)
	{	
		/*pierwszy strzał w turze, jesli celny to gracz strzela póki nie chybi*/
		state=players_shot(enemys_board, players_board, enemys_win, n);
		enemys_ships=enemys_ships - state;
				
			while(state==TRUE)
			{
				state=FALSE;
				state=players_shot(enemys_board, players_board, enemys_win, n);			
				enemys_ships=enemys_ships - state;
				
						/*sprawdzenie czy nie trzeba juz zakończyc gry*/
						if(enemys_ships==0)
						{	
							return TRUE;	}				
			}	
		
		/*tura zaczyna się po chybieniu gracza - komputer losuje pole */
		state=hard_enemys_shot(players_board, players_win, n);
		players_ships=players_ships - state;
			
			while(state==TRUE)
			{
				state=FALSE;		
				state=hard_enemys_shot(players_board, players_win, n);				
				players_ships=players_ships - state;
					
						if(players_ships==0)
						{	
							return FALSE;	}					
			}
							
	}
	
	return EXIT_FAILURE;
}

/*
ptr - wskaźnik plikowy niezbędny do otwarcia pliku
licznik - przechowuje ilość znaków oznaczających watrości pól na planszy
ch - zmienna, która przechowuje pobierane znaki
n_ex - potrzebna do obliczenia prawidłowości pliku

Funkcja pobiera odpowiednie wartości z początku pliku i na ich podstawie oblicza prawidłowość dalszej części pliku
*/
int parsing(char *name)
{
	FILE *ptr;				
	int ch;					
	int licznik = 0;		
	int n_ex=0;
	//int n, lvl, type;			
	
	ptr=fopen(name, "r");
	
	fscanf(ptr, "%d %d %c ", &n_ex, &lvl, &type); 
	
	while ((ch=getc(ptr)) != EOF)
		licznik++;
	
	fclose(ptr);
	//printf("\nPlik %s zawiera %d znakow, zas n=%d a lvl=%d  2*n*n=%d \n", name, licznik, n, lvl, 2*n*n);
	
	if((2*n_ex*n_ex)==licznik && lvl>0 && lvl<5 && (type=='s' || type=='t'))
		return TRUE;
	else
		return FALSE;

	return 0;	
}
