#include "audio_item.h"



AudioItem::AudioItem(FMOD::System* system)
{
	this->_system = system;
	this->_result = FMOD_OK;
	this->sound = 0;
	this->channel = 0;
	this->channel_group = 0;

	this->volume = 1.0f;
	this->pitch = 1.0f;
	this->pan = 0.0f;
	this->is_paused = true;
	this->is_playing = false;
	this->path = "";
	this->selectedConfigSetting = NONE;

}

AudioItem::~AudioItem()
{

}

void AudioItem::error_check() {
	if (this->_result != FMOD_OK) {
		fprintf(stderr, "FMOD error: %s", FMOD_ErrorString(this->_result));
		exit(1);
	}
}

bool AudioItem::create_and_play_sound(bool is_streamed_sound, bool start_paused)
{
	//create sound
	this->_result = this->_system->createSound(this->path.c_str(), (is_streamed_sound) ? FMOD_CREATESTREAM : FMOD_DEFAULT, 0, &this->sound);
	this->error_check();
	//get name
	this->_result = this->sound->getName(this->name, BUFFER_SIZE);
	this->error_check();

	//play sound
	this->_result = this->_system->playSound(this->sound, this->channel_group, start_paused, &this->channel);
	this->error_check();
	return true;
}

std::string AudioItem::get_info()
{
	std::ostringstream ss;
	ss.precision(2);
	ss << "Name: " << this->name << " Volume: " << this->volume << " | Pan: " << this->pan << " | Pitch: " << this->pitch << " | Paused: " << ((this->is_paused)?"YES":"NO") << " | Playing: " << ((this->is_playing) ? "YES" : "NO");
	return ss.str();	
}

std::string AudioItem::get_selected_config_setting()
{
	std::string selected_confoig_setting = "Selected config setting: ";
	switch (this->selectedConfigSetting)
	{
	case NONE:
		selected_confoig_setting += "NONE";
		break;
	case VOLUME:
		selected_confoig_setting += "VOLUME";
		break;
	case PAN:
		selected_confoig_setting += "PAN";
		break;
	case PITCH:
		selected_confoig_setting += "PITCH";
		break;
	case PAUSE:
		selected_confoig_setting += "PAUSE";
		break;
	case PLAYING:
		selected_confoig_setting += "PLAYING";
		break;
	default:
		selected_confoig_setting += "INVALID OPTION";
		break;
	}
	return selected_confoig_setting;
}

