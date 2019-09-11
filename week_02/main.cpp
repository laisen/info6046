/*==============================================================================
Play and display sound information
Oscar Lara - scarlara@hotmail.com

*** This implementation works on Debug/x86 only!

This example shows how to play a sound and print sound information on the screen.
FMOD Documentation:
https://www.fmod.com/resources/documentation-api?version=2.0&page=welcome.html
==============================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <ncursesw/ncurses.h>
#include <Windows.h>
#include <fmod.hpp>

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

//FMOD globals
FMOD::System* _system = 0;
FMOD::Sound* _sound = 0;
FMOD::Channel* _channel = 0;
FMOD_RESULT _result = FMOD_OK;

//FMOD functions
void errorCheck(FMOD_RESULT result);
void releaseFMOD();
bool initFMOD();

void handleUserInput();

void consoleUpdate();


void errorCheck(FMOD_RESULT result) {
	if (result != FMOD_OK) {
		wattron(_cs.win, A_STANDOUT);
		wprintw(_cs.win, "FMOD error: %d", result);
		wrefresh(_cs.win);
		releaseFMOD();
		system("pause");
		exit(1);
	}
}

bool initFMOD() {

	_result = FMOD::System_Create(&_system);
	errorCheck(_result);

	_result = _system->init(32, FMOD_INIT_NORMAL, 0);
	errorCheck(_result);

	return true;
}

void releaseFMOD() {

	if (_sound) {
		_result = _sound->release();
		errorCheck(_result);
	}
	if (_system) {
		_result = _system->close();
		errorCheck(_result);
		_result = _system->release();
		errorCheck(_result);
	}
}

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
		bool is_paused = false;
		_result = _channel->getPaused(&is_paused);
		errorCheck(_result);
		//we want to flip pause status on the channel
		_result = _channel->setPaused(!is_paused);
		errorCheck(_result);
	}
	break;
	default:
		break;
	}
}

int main() {

	//Init FMOD here not inside the loop
	initFMOD();

	//Create a sound
	_result = _system->createSound("../common/assets/audio/Sleep_Away.mp3", FMOD_DEFAULT, 0, &_sound);
	errorCheck(_result);

	//play the sound with paused enabled
	_result = _system->playSound(_sound, 0, true, &_channel);
	errorCheck(_result);

	//=================================================
	//Main loop
	while (!_should_exit) {


		bool is_paused = false;
		if (_channel) {
			//get is paused
			_result = _channel->getPaused(&is_paused);
			errorCheck(_result);
		}

		consoleUpdate();
		handleUserInput();
		wprintw(_cs.win, "============================================\n");
		wprintw(_cs.win, "Welcome to INFO-6046 Media Fundamentals\n");
		wprintw(_cs.win, "============================================\n");
		wprintw(_cs.win, "\n");
		wprintw(_cs.win, "Press 1 to play/pause a sound.\n");
		wprintw(_cs.win, "\n");
		wprintw(_cs.win, "Press 'ESC' key to exit.\n");
		wprintw(_cs.win, "Is paused?: %s\n", (is_paused) ? "YES" : "NO");




		//wrefresh(_cs.win);

	}


	//Clean up
	releaseFMOD();

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
