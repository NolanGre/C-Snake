#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <windows.h>
#include <stdbool.h>
#include <time.h>

#define AREA_SIZE 18  // x2 if horizontal

#define FRAME_RATE 1000   // actually, snake speed

#define X 0   // macro for snake units coordinate
#define Y 1


typedef struct Score {   // for top score

	unsigned short user_place;
	char user_name[30];
	size_t user_score;

} Score;


typedef struct Snake {

	size_t score;
	int xy[2][100];		// X = [0]; Y = [1]
	// ratio x to y = 1:2. In the future we will multiply by 2
	// max snake lenght = 100

	int direction;

} Snake;


bool isCanMove(int direction_buf, int direction);
int moveSnake(Snake* snake, int apple_coords[], bool* apple_exist);
int youDied(Snake snake);
void createApple(Snake* snake, int* apple_coords, bool* apple_exist);

void startSnake() {

	int direction_buf;  // buffer
	int row, col;
	getmaxyx(stdscr, row, col);

	WINDOW* snake_win;
	snake_win = newwin(AREA_SIZE, AREA_SIZE * 2, (row - AREA_SIZE) / 2, (col - (AREA_SIZE * 2)) / 2);
	nodelay(snake_win, TRUE);
	keypad(snake_win, TRUE);

	box(snake_win, ACS_VLINE, ACS_HLINE);
	
	Snake snake;
	snake.score = 1;  // start with one unit
	snake.direction = KEY_UP;   // initially move UP

	snake.xy[X][0] = AREA_SIZE - 1; // start location (center)
	snake.xy[Y][0] = (AREA_SIZE - 1) / 2;

	int apple_coords[2];   
	bool apple_exist = false;  // apple doesn't exist
	srand(time(NULL));   // seed for rand()

	/*mvprintw(getmaxy(stdscr) >> 3, (getmaxx(stdscr) - 14) / 2, "Your score: %d", snake.score);
	refresh();*/
	
	mvwprintw(snake_win, snake.xy[Y][0], snake.xy[X][0], "[]");
	
	while (1) {

		direction_buf = wgetch(snake_win);

		werase(snake_win);
		box(snake_win, ACS_VLINE, ACS_HLINE);

		if (direction_buf != ERR) {  
			if (isCanMove(direction_buf, snake.direction)) {

				snake.direction = direction_buf;    // change direction
			}
		}

		// uncomment for spawning apple after move 
		/*while (!apple_exist) createApple(&snake, apple_coords, &apple_exist);
		mvwprintw(snake_win, apple_coords[Y], apple_coords[X], "$$");*/


		if (moveSnake(&snake, apple_coords, &apple_exist) == 1) {	// code 1 - you died
			
			youDied(snake);

			delwin(snake_win);
			clear();
			return;    
		}

		while (!apple_exist) createApple(&snake, apple_coords, &apple_exist);
		mvwprintw(snake_win, apple_coords[Y], apple_coords[X], "$$");
		
		// print snake
		for (int i = 1; i < snake.score; i++) 
			mvwprintw(snake_win, snake.xy[Y][i], snake.xy[X][i], "()");
	
		mvwprintw(snake_win, snake.xy[Y][0], snake.xy[X][0], "[]");


		mvprintw(getmaxy(stdscr) >> 3, (getmaxx(stdscr) - 14) / 2, "Your score: %d", snake.score);
		mvwprintw(stdscr, 1, 1, "Snake X: %2d  Y: %2d", snake.xy[X][0], snake.xy[Y][0]);
		mvwprintw(stdscr, 2, 1, "Apple X: %2d  Y: %2d", apple_coords[X], apple_coords[Y]);
		wrefresh(stdscr);
		wrefresh(snake_win);
		
		// there can be your frame rate formula
		Sleep(FRAME_RATE - (snake.score * snake.score) - 500);
	}
}   

bool isCanMove(int direction_buf, int direction) {

	// snake can only move on 4 keys
	if (direction_buf < KEY_DOWN || direction_buf > KEY_RIGHT) return false;  

	// snake can't move in the opposite direction
	if (direction_buf == KEY_DOWN && direction == KEY_UP) return false;
	else if(direction_buf == KEY_UP && direction == KEY_DOWN) return false;
	else if(direction_buf == KEY_LEFT && direction == KEY_RIGHT) return false;
	else if (direction_buf == KEY_RIGHT && direction == KEY_LEFT) return false;

	return true;
}


void createApple(Snake* snake, int* apple_coords, bool* apple_exist ) {

		apple_coords[X] = (rand() % (AREA_SIZE * 2 - 2)) + 1;    // if AREA_SIZE 18: range 1 to 34
		if (apple_coords[X] % 2 == 0) apple_coords[X]--;   // make an odd number 
		apple_coords[Y] = (rand() % (AREA_SIZE - 2)) + 1;  // if AREA_SIZE 18: range 1 to 16

		// if apple spawn in snake, return false
		for (int i = 0; i < snake->score; i++)
			if (snake->xy[X][i] == apple_coords[X] && snake->xy[Y][i] == apple_coords[Y])
				return;

		*apple_exist = true;
}

int moveSnake(Snake* snake, int apple_coords[], bool* apple_exist) {

	int head_temp[] = {snake->xy[X][0], snake->xy[Y][0]}; // remember the old position

	// move head unit
	switch (snake->direction) {

		case KEY_DOWN: snake->xy[Y][0]++; break;
		case KEY_UP: snake->xy[Y][0]--; break;
		case KEY_LEFT: snake->xy[X][0] -= 2; break;
		case KEY_RIGHT: snake->xy[X][0] += 2; break;
	}

	// boundary collision check
	if (snake->xy[X][0] == -1 || snake->xy[X][0] == AREA_SIZE * 2 - 1) return 1;  // code 1 - you died
	if (snake->xy[Y][0] == 0 || snake->xy[Y][0] == AREA_SIZE - 1) return 1;

	// apple eating check
	if (snake->xy[X][0] == apple_coords[X] && snake->xy[Y][0] == apple_coords[Y]) {
	
		*apple_exist = false;
		snake->score++;  // increas score

		snake->xy[X][snake->score - 1] = snake->xy[X][0]; 
		snake->xy[Y][snake->score - 1] = snake->xy[Y][0];
		// P.S. To add a unit instantly, use "= snake->xy[Y/X][snake->score - 2];"
	}

	// move the body of the snake (from end)
	for (int i = snake->score - 1; i > 0; i--) {

		if (!(*apple_exist) && i == snake->score - 1) continue; // if apple eated

		snake->xy[X][i] = snake->xy[X][i - 1];
		snake->xy[Y][i] = snake->xy[Y][i - 1];

		if (i == 1) {
			snake->xy[X][i] = head_temp[X];
			snake->xy[Y][i] = head_temp[Y];
		}

		// check for collision
		if (snake->xy[X][0] == snake->xy[X][i] && snake->xy[Y][0] == snake->xy[Y][i])
			return 1;   // you died
	}

	return 0;
}


int youDied(Snake snake) {


	WINDOW* death_win;
	death_win = newwin(5, 16, (getmaxy(stdscr) - 5) / 2, (getmaxx(stdscr) - 16) / 2);
	
	box(death_win, ACS_VLINE, ACS_HLINE);

	mvwprintw(death_win, 2, 4, "You Died");

	wgetch(death_win);

	delwin(death_win);
	clear();
	return 1;
}




void openScore() {

	int row, col;
	getmaxyx(stdscr, row, col);

	WINDOW* snakeScore = newwin(12, 20, (row - 12) / 2, (col - 20) / 2);
	box(snakeScore, ACS_VLINE, ACS_HLINE);	
	wrefresh(snakeScore); 
	mvprintw(snakeScore, 1, 1, "You Take Score");

	getch();


}