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
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>

#define BUFFER_SIZE 255

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

ConsoleSettings _cs;

//FMOD globals
FMOD::System* _system = 0;
//FMOD functions
void errorCheck(FMOD_RESULT result);
void releaseFMOD();
bool initFMOD();

struct AudioItem
{
	
private:
	FMOD_RESULT result;
	std::string path;
	char name[BUFFER_SIZE];
	float volume;
	float pitch;
	float pan;
	bool is_paused;
	bool is_playing;

public:
	FMOD::Sound* sound;
	FMOD::Channel* channel;
	//constructor
	AudioItem() {
		result = FMOD_OK;
		path = "";
		name[BUFFER_SIZE -1 ] = { '\0' };
		volume = 0.5f;
		pitch = 0.0f;//check documentation for pitch allowed values
		pan = 0.0f; //chec documentation for pan allowed values
		is_paused = true;
		is_playing = false;
		sound = 0;
		channel = 0;
	}

	/*
	2 modes to create sounds
	1. sample loads all audio file into memory
	2. stream sound loads on demand you pay the price of a bit CPU overhead
	*/
	bool create_and_play_sound(bool is_streamed_sound) {
		if (_system) {
			result = _system->createSound(path.c_str(), (is_streamed_sound) ? FMOD_CREATESTREAM : FMOD_DEFAULT, 0, &sound);
			errorCheck(result);
			result = _system->playSound(sound, 0, is_paused, &channel);
			errorCheck(result);
			return true;
		}

		return false;
	}

	std::string get_name() {
		if (sound) {
			result = sound->getName(name, BUFFER_SIZE);
			errorCheck(result);
		}
		return name;
	}

	float get_volume() {
		if (channel) {
			result = channel->getVolume(&volume);
			errorCheck(result);
		}

		return volume;
	}

	void set_path(std::string new_path) {
		path = new_path;
	}

	bool get_is_paused() {
		if (channel) {
			result = channel->getPaused(&is_paused);
			errorCheck(result);
		}
		return is_paused;
	}
	void set_is_paused(bool new_is_paused) {
		if (channel) {
			is_paused = new_is_paused;
			result = channel->setPaused(is_paused);
			errorCheck(result);
		}
	}

	//create get info method
	std::string get_info() {
		std::ostringstream ss;
		
		//get name
		ss << "Name: " << this->get_name() << std::endl;
		ss << "Volume: " << this->get_volume() << std::endl;
		ss << "Is paused: " << ((this->get_is_paused()) ? "TRUE" : "FALSE") << std::endl;

		std::string result = ss.str();
		return result;
	}
	

	


};

//global audio item objects
std::vector<AudioItem> _audio_items;
int _current_audio_itme_index = 0;

bool initAudioItems() {

	//read config file with audio paths.
	std::ifstream input_file;
	input_file.open("./config_files/audio_files_paths.txt");
	if (!input_file) {
		wprintw(_cs.win, "Unable to open audio_files_paths.txt\n");
		wrefresh(_cs.win);
		system("pause");
		return false;
	}

	std::string line;
	while (std::getline(input_file, line))
	{
		if (line.length() > 0) {
			AudioItem ai;
			ai.set_path(line.c_str());
			ai.create_and_play_sound(true);
			_audio_items.push_back(ai);
		}
	}

}

//================================================================
//Globals

bool _should_exit = false;




//FMOD::Sound* _sound = 0;
//FMOD::Channel* _channel = 0;
FMOD_RESULT _result = FMOD_OK;



int _release_counter = 0;

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



	//_release_counter++;
	//if (_release_counter >= 3) {
	//	_sound = 0;
	//	_system = 0;
	//	exit(1);
	//}

	std::vector<AudioItem>::iterator it = _audio_items.begin();
	for (it; it != _audio_items.end(); it++)
	{
		if (it->sound) {
			_result = it->sound->release();
			errorCheck(_result);
		}
	}

	//if (_sound) {
	//	_result = _sound->release();
	//	errorCheck(_result);
	//}
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
		_audio_items[_current_audio_itme_index].set_is_paused(!_audio_items[_current_audio_itme_index].get_is_paused());//
	//{
		//bool is_paused = false;
		//_result = _channel->getPaused(&is_paused);
		//errorCheck(_result);
		////we want to flip pause status on the channel
		//_result = _channel->setPaused(!is_paused);
		//errorCheck(_result);
	//}
		break;
	case 'n':
		_current_audio_itme_index = (_current_audio_itme_index >= _audio_items.size() - 1) ? 0 : (_current_audio_itme_index + 1);
		break;
	default:
		break;
	}
}

int main() {

	//Init FMOD here not inside the loop
	initFMOD();

	//Init audio item objects
	initAudioItems();

	////Create a sound
	//_result = _system->createSound("../common/assets/audio/Sleep_Away.mp3", FMOD_DEFAULT, 0, &_sound);
	//errorCheck(_result);

	////play the sound with paused enabled
	//_result = _system->playSound(_sound, 0, true, &_channel);
	//errorCheck(_result);

	//=================================================
	//Main loop
	while (!_should_exit) {


		bool is_paused = false;
		unsigned int pos;


		//if (_channel) {
		//	//get is paused
		//	_result = _channel->getPaused(&is_paused);
		//	errorCheck(_result);
		//	_result = _channel->getPosition(&pos, FMOD_TIMEUNIT_MS);
		//	errorCheck(_result);
		//}

		//const int buffer_size = 255;
		//char name[buffer_size] = {'\0'};
		//unsigned int len;

		//if (_sound) {
		//	_result = _sound->getName(name, buffer_size);
		//	errorCheck(_result);
		//	_result = _sound->getLength(&len, FMOD_TIMEUNIT_MS);
		//	errorCheck(_result);
		//}


		consoleUpdate();
		handleUserInput();
		wprintw(_cs.win, "============================================\n");
		wprintw(_cs.win, "Welcome to INFO-6046 Media Fundamentals\n");
		wprintw(_cs.win, "============================================\n");
		wprintw(_cs.win, "\n");
		wprintw(_cs.win, "Press 1 to play/pause a sound.\n");
		wprintw(_cs.win, "\n");
		wprintw(_cs.win, "Press 'ESC' key to exit.\n");
		wprintw(_cs.win, "\n");
		wprintw(_cs.win, "Current sound info:\n%s", _audio_items[_current_audio_itme_index].get_info().c_str());
		//wprintw(_cs.win, "Sound name: %s\n", name);
		//wprintw(_cs.win, "Position:%02d:%02d / %02d:%02d\n", pos / 1000 / 60, pos / 1000 % 60,len / 1000 / 60, len / 1000 % 60);
		//wprintw(_cs.win, "Is paused?: %s\n", (is_paused) ? "YES" : "NO");




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
