/*==============================================================================
Play and display sound information
Oscar Lara - scarlara@hotmail.com

This example shows how to play a sound and print sound information on the screen.
FMOD Documentation:
https://www.fmod.com/resources/documentation-api?version=2.0&page=welcome.html
==============================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <ncursesw/ncurses.h>


//================================================================
//Console Settings
struct  ConsoleSettings
{
	int max_rows = 30;
	int max_cols = 100;
	int start_row = 5;
	int start_col = 10;
	int current_row = 0;
	int current_col = 0;
	WINDOW* win;
	ConsoleSettings() {
		initscr();
		win = newwin(max_rows, max_cols, start_row, start_col);
		//hide cursor
		curs_set(0);
		//clear screen - first time is called
		refresh();
		//wgetch character will not be printed on the screen
		noecho();
		//wgetch will work in a non-blocking manner
		nodelay(win, true);

		wattron(win, A_NORMAL);
	}
};

//================================================================
//Globals
ConsoleSettings _cs;
bool _should_exit = false;

void handleUserInput();

void consoleUpdate();


void handleUserInput() {



	//Read user input
	int c = wgetch(_cs.win);
	switch (c)
	{
	case 27: //ESC key pressed
		_should_exit = true;
		break;
	case '1':
	{

	}
	break;
	default:
		break;
	}
}

int main() {

	//=================================================
	//Main loop
	while (!_should_exit) {
	
		consoleUpdate();
		handleUserInput();

		wprintw(_cs.win, "Welcome to INFO-6046 Media Fundamentals\n");

	}


	//Clean up

	delwin(_cs.win);
	endwin();

	refresh();

	return 0;
}

void consoleUpdate() {

	//reset cursor
	wmove(_cs.win, 0, 0);
	wrefresh(_cs.win);

}
