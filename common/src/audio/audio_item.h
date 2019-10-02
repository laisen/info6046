#pragma once

#include <fmod.hpp>
#include <fmod_errors.h>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

#define BUFFER_SIZE 255

enum eAIConfigSetting
{
	VOLUME,
	PITCH,
	PAN,
	PAUSE,
	PLAYING,
	NONE
};

class AudioItem
{
public:
	AudioItem(FMOD::System* system);
	virtual ~AudioItem();

	FMOD::Sound* sound;
	FMOD::Channel* channel;
	FMOD::ChannelGroup* channel_group;
	char name[BUFFER_SIZE];
	std::string path;
	float volume;
	float pitch;
	float pan;
	bool is_paused;
	bool is_playing;
	eAIConfigSetting selectedConfigSetting;

	/*
	2 modes to create sounds
	1. sample loads all audio file into memory
	2. stream sound loads on demand you pay the price of a bit CPU overhead
	*/
	bool create_and_play_sound(bool is_streamed_sound, bool start_paused);
	std::string get_info();
	std::string get_selected_config_setting();

private:
	FMOD_RESULT _result;	
	FMOD::System* _system;
	void error_check();


};

