#include "audio_listener.h"


AudioListener::AudioListener(FMOD::System* system)
{
	this->last_position = {0.0f, 0.0f, 0.0f};
	this->position = {0.0f, 0.0f, 0.0f};
	this->up = { 0.0f, 1.0f, 0.0f };
	this->forward = { 0.0f, 0.0f, -1.0f };
	this->velocity = {0.0f, 0.0f, 0.0f};
	this->step_size = 1.0f;
}

AudioListener::~AudioListener()
{
}

void AudioListener::step_left()
{
	this->position.x -= this->step_size;
	//calculate velocity
	this->velocity.x = (this->position.x - this->last_position.x);
	this->velocity.y = (this->position.y - this->last_position.y);
	this->velocity.z = (this->position.z - this->last_position.z);

	//update last position
	this->last_position = this->position;

}

void AudioListener::step_right()
{
	this->position.x += this->step_size;
	//calculate velocity
	this->velocity.x = (this->position.x - this->last_position.x);
	this->velocity.y = (this->position.y - this->last_position.y);
	this->velocity.z = (this->position.z - this->last_position.z);

	//update last position
	this->last_position = this->position;
}

std::string AudioListener::get_position_info()
{
	std::ostringstream ss;
	ss.precision(2);

	ss << "X: " << this->position.x << " | Y: " << this->position.y << " | Z: " << this->position.z;

	return ss.str();
}

void AudioListener::dance_over_x(float some_time)
{

	this->position.x = (float)sin(some_time * 0.5f) * 20.0f;

	//calculate velocity
	this->velocity.x = (this->position.x - this->last_position.x) / (some_time * 0.5f);
	this->velocity.y = (this->position.y - this->last_position.y) / (some_time * 0.5f);
	this->velocity.z = (this->position.z - this->last_position.z) / (some_time * 0.5f);

	//update last position
	this->last_position = this->position;

}
