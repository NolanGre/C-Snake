#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <curses.h>

#define MENU_ITEMS 4   // snake, score, credits, exit

unsigned short mainMenu(int row, int col);
void startSnake();
void openScore();

const char menu_template[MENU_ITEMS][10] = {
	"Snake",
	"Score",
	"Credits",
	"Exit"
};


int main() {

	getchar();       // PAUSE
	initscr();

	curs_set(0);   // hide cursor
	noecho();    // hide keyboard
	cbreak();    // mode that doesn't buffering stdin
	keypad(stdscr, 1);   // keypad mode true

	int row, col;
	getmaxyx(stdscr, row, col);

	mvprintw(row / 2, (col - 13) / 2, "Press any key");
	getch();

	while (1) {
		switch (mainMenu(row, col)) {

			case 0: startSnake(); break;
			case 1: openScore(); break;
			case 2: /*showCredits();*/ break;
			case 3:
				endwin();
				return 0;
		}
	}
}


unsigned short mainMenu(int row, int col) {  // console: y,x

	unsigned short menu_switch = 0;  // set first item

	WINDOW* menu_win;
	menu_win = newwin(MENU_ITEMS + 4, 25, (row - 4 - MENU_ITEMS) / 2, (col - 25) / 2);   // center window alignment

	while (1) {

		refresh();
		box(menu_win, ACS_VLINE, ACS_HLINE);  // draw a frame

		mvwprintw(menu_win, 1, 11, "Menu");   //menu head
		mvwprintw(menu_win, 2, 1, "-----------------------");

		for (int i = 0; i < MENU_ITEMS; i++) {

			wrefresh(menu_win);
			if (menu_switch == i) mvwaddch(menu_win, 3 + i, 2, '>');
			else mvwaddch(menu_win, 3 + i, 2, '-');

			mvwprintw(menu_win, 3 + i, 3, "%s", menu_template[i]);

		}
		wrefresh(menu_win);

		int action = getch();
		
		if (action == KEY_UP && menu_switch) menu_switch--;    // ³f possible, move the pointer up
		else if (action == KEY_DOWN && menu_switch != MENU_ITEMS - 1) menu_switch++;
	    // ³f possible, move the pointer down

		else if (action == ' ' || action == '\n') {    // user make choice

			delwin(menu_win);
			clear();
			return menu_switch;
		}
	}
}