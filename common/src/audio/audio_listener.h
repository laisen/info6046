#pragma once

#include <fmod.hpp>
#include <fmod_errors.h>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

#define BUFFER_SIZE 255


class AudioListener
{
public:
	AudioListener(FMOD::System* system);
	virtual ~AudioListener();

	FMOD_VECTOR last_position;
	FMOD_VECTOR position;
	FMOD_VECTOR up;
	FMOD_VECTOR forward;
	FMOD_VECTOR velocity;

	void step_left();
	void step_right();
	std::string get_position_info();
	void dance_over_x(float some_time);

private:

	float step_size;

};


