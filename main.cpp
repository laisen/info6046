///*
//==================================================================================================================================================================
//|Copyright 2019 Oscar Lara	- scarlara@hotmail.com																												   |
//|																																								   |
//|See individual libraries separate legal notices																												   |
//|																																								   |
//|Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files(the "Software"),				   |
//|to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,				   |
//|and / or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions :					   |
//|																																								   |
//|The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.									   |
//|																																								   |
//|THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,			   |
//|FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,	   |
//|WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.		   |
//==================================================================================================================================================================
//*/
//


#include <stdio.h>
#include <assert.h>
#include <vector>
#include <cstdio>


#include <glad/glad.h>
#include <GLFW\glfw3.h>

#include "common/src/audio/audio_item.h"


#include <ft2build.h>
#include FT_FREETYPE_H  


//Globals
unsigned int _vertex_shader, _fragment_shader, _program;
GLfloat _current_y_position = 0.0f;
GLfloat _y_offset = 40.0f;
char _text_buffer[512];
std::vector<AudioItem> _vec_audio_items;
std::vector<AudioItem>::iterator _it_selected_autio_item;



//GLFW
int _window_width = 640;
int _window_height = 480;
GLFWwindow* _main_window = NULL;

//Free text
FT_Library _ft;
FT_Face _face;
unsigned int _text_program;
unsigned int _uniform_tex;
unsigned int _attribute_coord;
unsigned int _dp_vbo;
unsigned int _uniform_color;

//FMOD
FMOD::System* _system = 0;
FMOD_RESULT _result = FMOD_OK;


//=============================================================
struct point {
	GLfloat x;
	GLfloat y;
	GLfloat s;
	GLfloat t;
};


//=============================================================

//Functions
bool init_gl();
bool init_text();
bool init_shaders();
void render_text(const char* text);
bool init_fmod();
void release_fmod();
bool load_audio_from_file();
void step_settings(bool is_step_up);

void error_check(FMOD_RESULT result) {
	if (result != FMOD_OK) {
		fprintf(stderr, "FMOD error: %s", FMOD_ErrorString(result));		
		
		exit(1);
	}
}


static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);	
}



static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
	if (key == GLFW_KEY_N && action == GLFW_PRESS) {
		_it_selected_autio_item = (_it_selected_autio_item != (_vec_audio_items.end() - 1))? _it_selected_autio_item + 1: _vec_audio_items.begin();
	}
	if (key == GLFW_KEY_V && action == GLFW_PRESS) {
		_it_selected_autio_item->selectedConfigSetting = VOLUME;
	}
	if (key == GLFW_KEY_P && action == GLFW_PRESS) {
		_it_selected_autio_item->selectedConfigSetting = PITCH;
	}
	if (key == GLFW_KEY_B && action == GLFW_PRESS) {
		_it_selected_autio_item->selectedConfigSetting = PAN;
	}
	if (key == GLFW_KEY_M && action == GLFW_PRESS) {
		_it_selected_autio_item->selectedConfigSetting = PAUSE;
	}
	if (key == GLFW_KEY_C && action == GLFW_PRESS) {
		_it_selected_autio_item->selectedConfigSetting = PLAYING;
	}
	if (key == GLFW_KEY_D && action == GLFW_PRESS) {
		
		//toggle bypass


	}

	//use up/down arrows to change
	if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
		step_settings(true);

	}
	if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
		step_settings(false);
	}
			
		
}

void step_settings(bool is_step_up){

	float step_size = (is_step_up)?0.1f:-0.1f;

	switch (_it_selected_autio_item->selectedConfigSetting)
	{
	
	case PAUSE:
		_result = _it_selected_autio_item->channel->getPaused(&_it_selected_autio_item->is_paused);
		error_check(_result);
		//just switch is paused
		_result = _it_selected_autio_item->channel->setPaused(!_it_selected_autio_item->is_paused);
		error_check(_result);

		_result = _it_selected_autio_item->channel->getPaused(&_it_selected_autio_item->is_paused);
		error_check(_result);
		break;


	default:
		break;
	}
}


void handle_audio_files() {
	std::vector<AudioItem>::iterator it = _vec_audio_items.begin();
	for (it; it != _vec_audio_items.end(); it++) {
		render_text(it->get_info().c_str());
	}
}

int main() {



	fprintf(stdout, "Init opengl ...\n");
	assert(init_gl());

	fprintf(stdout, "Init text...\n");
	assert(init_text());

	fprintf(stdout, "Init shaders...\n");
	assert(init_shaders());

	fprintf(stdout, "Init fmod...\n");
	assert(init_fmod());

	fprintf(stdout, "Loading audio files from config file...\n");
	assert(load_audio_from_file());

	//=======================================================================================
	fprintf(stdout, "Ready ...!\n");	
	

	//Main loop
	while (!glfwWindowShouldClose(_main_window)) {		

		//Reset text y position
		_current_y_position = 30.0f;

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(_program);		

		

		render_text("=====================================================");
		render_text("Media Fundamentals play sound...");
		render_text("=====================================================");
		render_text("Press ESC to Exit!");
		render_text("");

		render_text("Selected audio item:");
		render_text(_it_selected_autio_item->get_info().c_str());		
		render_text(_it_selected_autio_item->get_selected_config_setting().c_str());
		render_text("");
		handle_audio_files();
		
		render_text("=====================================================");
		//print numbers, strings using buffer
		//sprintf(_text_buffer, "i am a float %.02f", 2.15f);
		//render_text(_text_buffer);
		//render_text("=====================================================");
		


		glfwSwapBuffers(_main_window);
		glfwPollEvents();

		glfwGetWindowSize(_main_window, &_window_width, &_window_height);

	}

	glfwDestroyWindow(_main_window);
	glfwTerminate();

	release_fmod();
	return 0;
}



bool init_gl() {

	glfwSetErrorCallback(error_callback);

	if (!glfwInit()) {
		fprintf(stderr, "Unable to init glfw\n");
		return false;
	}
		
	//Full screen
	//_main_window = glfwCreateWindow(1920, 1080, "Media Fundamentals... play sound", glfwGetPrimaryMonitor(), NULL);
	_main_window = glfwCreateWindow(1920, 1080, "Media Fundamentals... play sound", NULL, NULL);

	if (!_main_window)
	{
		glfwTerminate();
		fprintf(stderr, "Unable to create main window glfw\n");
		return false;
	}
	
	glfwGetWindowSize(_main_window, &_window_width, &_window_height);

	glfwSetKeyCallback(_main_window, key_callback);
	glfwMakeContextCurrent(_main_window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		fprintf(stderr, "Unable to init glad\n");
		return false;
	}
	glfwSwapInterval(1);
	glfwWindowHint(GLFW_SAMPLES, 4);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);
	//glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_TRUE);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glClearColor(0.7f, 0.7f, 0.7f, 1.0f);

	return true;
}

bool init_text() {

	if (FT_Init_FreeType(&_ft)) {
		fprintf(stderr, "Unable to init text...\n");
		return false;
	}
	if (FT_New_Face(_ft, "../../common/assets/fonts/arial.ttf", 0, &_face)) {
		fprintf(stderr, "Unable to init text...\n");
		return false;
	}

	//Font size
	FT_Set_Pixel_Sizes(_face, 0, 36);

	if (FT_Load_Char(_face, 'X', FT_LOAD_RENDER))
	{
		fprintf(stderr, "unable to load character\n");
		return false;
	}

	return true;
}

bool init_shaders() {
	//=======================================================================================
//Shaders
	int params = -1;

	glGetProgramiv(_program, GL_LINK_STATUS, &params);
	if (!params) {
		fprintf(stderr, "Unable to link shader program");
		return 1;
	}
	//--	
	FILE* f = fopen("../../common/src/shaders/simple_text.vert", "rb");
	long filesize = ftell(f);
	fseek(f, 0, SEEK_END);
	filesize = ftell(f);
	fseek(f, 0, SEEK_SET);
	char* simple_text_vert = (char*)malloc(filesize + 1);
	fread(simple_text_vert, filesize, 1, f);
	fclose(f);
	simple_text_vert[filesize] = 0;
	//--
	f = fopen("../../common/src/shaders/simple_text.frag", "rb");
	fseek(f, 0, SEEK_END);
	filesize = ftell(f);
	fseek(f, 0, SEEK_SET);
	char* simple_text_frag = (char*)malloc(filesize + 1);
	fread(simple_text_frag, filesize, 1, f);
	fclose(f);
	simple_text_frag[filesize] = 0;

	//text vertex shader
	unsigned int simple_text_vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(simple_text_vs, 1, &simple_text_vert, NULL);
	glCompileShader(simple_text_vs);
	glGetShaderiv(simple_text_vs, GL_COMPILE_STATUS, &params);
	if (!params) {
		fprintf(stderr, "Unable to compile simple text vertex shader");
		return 1;
	}
	unsigned int simple_text_fg = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(simple_text_fg, 1, &simple_text_frag, NULL);
	glCompileShader(simple_text_fg);
	glGetShaderiv(simple_text_fg, GL_COMPILE_STATUS, &params);
	if (!params) {
		fprintf(stderr, "Unable to compile simple text fragment shader");
		return 1;
	}

	_text_program = glCreateProgram();
	glAttachShader(_text_program, simple_text_vs);
	glAttachShader(_text_program, simple_text_fg);
	glLinkProgram(_text_program);

	glGetProgramiv(_text_program, GL_LINK_STATUS, &params);
	if (!params) {
		fprintf(stderr, "Unable to link simple text shader program");
		return 1;
	}

	_uniform_tex = glGetUniformLocation(_text_program, "tex");
	_attribute_coord = glGetAttribLocation(_text_program, "coord");
	_uniform_color = glGetUniformLocation(_text_program, "color");
	glGenBuffers(1, &_dp_vbo);

	glUseProgram(_program);
	return true;
}

void render_text(const char *text) {
	//render_text(const char *text, float x, float y, float sx, float sy)
	float sx = 2.0f / _window_width;
	float sy = 2.0f / _window_height;
	
	//GLfloat _current_y_position = 30.0f;
	GLfloat xoffset = 8 * sx;
	float x = -1 + xoffset;
	float y = 1 - _current_y_position * sy;


	glUseProgram(_text_program);
	GLfloat black[4] = { 0, 0, 0, 1 };
	glUniform4fv(_uniform_color, 1, black);
	const char *p;
	FT_GlyphSlot g = _face->glyph;

	GLuint tex;
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glUniform1i(_uniform_tex, 0);

	/* We require 1 byte alignment when uploading texture data */
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	/* Clamping to edges is important to prevent artifacts when scaling */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	/* Linear filtering usually looks best for text */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	/* Set up the VBO for our vertex data */
	glEnableVertexAttribArray(_attribute_coord);
	glBindBuffer(GL_ARRAY_BUFFER, _dp_vbo);
	glVertexAttribPointer(_attribute_coord, 4, GL_FLOAT, GL_FALSE, 0, 0);


	/* Loop through all characters */
	for (p = text; *p; p++) {
		/* Try to load and render the character */
		if (FT_Load_Char(_face, *p, FT_LOAD_RENDER))
			continue;


		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			g->bitmap.width,
			g->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			g->bitmap.buffer
		);

		/* Calculate the vertex and texture coordinates */
		float x2 = x + g->bitmap_left * sx;
		float y2 = -y - g->bitmap_top * sy;
		float w = g->bitmap.width * sx;
		float h = g->bitmap.rows * sy;

		point box[4] = {
			{ x2, -y2, 0, 0 },
		{ x2 + w, -y2, 1, 0 },
		{ x2, -y2 - h, 0, 1 },
		{ x2 + w, -y2 - h, 1, 1 },
		};

		
		/* Draw the character on the screen */
		glBufferData(GL_ARRAY_BUFFER, sizeof(box), box, GL_DYNAMIC_DRAW);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		/* Advance the cursor to the start of the next character */
		x += (g->advance.x >> 6) * sx;
		y += (g->advance.y >> 6) * sy;
	}
	glDisableVertexAttribArray(_attribute_coord);
	glDeleteTextures(1, &tex);
	glEnableVertexAttribArray(0);

	_current_y_position = (_current_y_position > 800.0f) ? 30.0f : (_current_y_position + _y_offset);
}

bool init_fmod() {
	//Create system
	_result = FMOD::System_Create(&_system);
	error_check(_result);
	//Init system
	_result = _system->init(32, FMOD_INIT_NORMAL, 0);
	error_check(_result);
	


	return true;
}

void release_fmod() {



	std::vector<AudioItem>::iterator it = _vec_audio_items.begin();
	for (it; it != _vec_audio_items.end(); it++) {
		
		if (it->sound) {
			_result = it->sound->release();
			error_check(_result);
		}
	}


	
	if (_system) {
		_system->close();
		_system->release();
	}


}

bool load_audio_from_file() {

	std::ifstream input_file;
	input_file.open("../../common/assets/config/audio_files.txt");
	if (!input_file) {
		fprintf(stderr, "Unable to open audio_files.txt\n");
		return false;
	}
	std::string line;
	
	

	while (std::getline(input_file, line))
	{
		if (line.length() > 0) {
			AudioItem ai(_system);
			ai.path = line.c_str();
			ai.create_and_play_sound(true, true);
			_vec_audio_items.push_back(ai);
		}
	}

	_it_selected_autio_item = _vec_audio_items.begin();
	
	



	//TODO: EXERCISE IN CLASS USE A CHANNEL GROUP, TO MUTE, AND ADD A DSP INSTEAD.

	return true;
}