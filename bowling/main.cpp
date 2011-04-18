/*
 * The main entry point for all demos.
 * 
 * Part of the Cyclone physics system.
 * 
 * Copyright (c) Icosagon 2003. All Rights Reserved.
 *
 * This software is distributed under licence. Use of this software
 * implies agreement with all terms and conditions of the accompanying
 * software licence.
 */

#include <gl/glfw.h>

// Include the general application structure.
#include "app.h"

// Include the timing functions
#include "timing.h"

#include "Sound.h"

#include <string>
#include <FTGL/ftgl.h>
#include <windows.h>
#include <fstream>

#define MAX_HIGH_SCORES 10

using namespace std;

// Forward declaration of the function that will return the 
// application object for this particular demo. This should be 
// implemented in the demo's .cpp file.
extern Application* getApplication();

// Store the global application object.
Application* app;

int window_width = 1280, window_height = 800;
float aspect_ratio;
FTGLPixmapFont font("bin/fonts/calibrib.ttf");

Sound game_sounds;

void init() {
	GLuint logo_texture;
	aspect_ratio = (float)window_width/window_height;

	// Initialise GLFW
	if( !glfwInit() ) {
		exit( EXIT_FAILURE );
	}

	int window_size = GLFW_FULLSCREEN;

	if(MessageBox(NULL, "Would You Like To Run In Fullscreen Mode?", "Start FullScreen?", MB_YESNO|MB_ICONQUESTION) == IDNO)
	{
		window_size = GLFW_WINDOW;// Windowed Mode
	}

	// Open an OpenGL window
	if( !glfwOpenWindow( window_width, window_height, 0, 0, 0, 0, 0, 0, window_size ) ) {
		glfwTerminate();
		exit( EXIT_FAILURE );
	}

	glfwSetWindowTitle("Power Bowling");

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(75.0, aspect_ratio, 1.0, 20000.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
    glTranslatef(0.0, 0.0, -5.0);
        
    glEnable(GL_TEXTURE_2D);

    //Load the logo as a texture
    glGenTextures(1, &logo_texture);
    GLFWimage current_image;

    GLuint success = glfwReadImage("bin/images/company.tga", &current_image, 0);

    if(success) {
        glBindTexture(GL_TEXTURE_2D, logo_texture);
                        
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, current_image.Format, current_image.Width, current_image.Height, 0, current_image.Format, GL_UNSIGNED_BYTE, current_image.Data); // Texture specification

        glfwFreeImage(&current_image);
    }

    float time = glfwGetTime();
    while(glfwGetTime() - time <= 1.0) {
        glClear( 0 | GL_DEPTH_BUFFER_BIT );
        
        if(success) {
            glBindTexture(GL_TEXTURE_2D, logo_texture);

            glBegin(GL_QUADS);
                glTexCoord2f(0.0, 0.0);
                glVertex3f(-3.0, -0.5, 0.0);
                glTexCoord2f(0.0, 1.0);
                glVertex3f(-3.0, 0.5, 0.0);
                glTexCoord2f(1.0, 1.0);
                glVertex3f(3.0, 0.5, 0.0);
                glTexCoord2f(1.0, 0.0);
                glVertex3f(3.0, -0.5, 0.0);
            glEnd();
        } 
        
        glfwSwapBuffers();
    }

	alutInit(0, 0) ;
	app = getApplication();
	TimingData::init();
	app->initGraphics();
	game_sounds.initialize();
}

//exits the game
void exit() {
	app->deinit();
    delete app;
    TimingData::deinit();

	// Close window and terminate GLFW
	glfwTerminate();

	// Exit program
	exit( EXIT_SUCCESS );
}

/**
 * Called each frame to update the 3D scene. Delegates to
 * the application.
 */
int update()
{
	int running = 1;
    // Update the timing.
    TimingData::get().update();

    // Delegate to the application.
    running = app->update();

	return running;
}

/**
 * Called each frame to display the 3D scene. Delegates to
 * the application.
 */
void display()
{
    app->display();

    // Update the displayed content.
    glFlush();
    glfwSwapBuffers();
}

/**
 * Called when a key is pressed.
 */
void keyboard()
{
    // Note we omit passing on the x and y: they are rarely needed.
    app->key();
}

//used to display text on the screen
void display_text(string text, FTPoint& position, unsigned int size) {
	font.FaceSize(size);
	font.Render(text.c_str(), -1, position);
}

//displays the game menu
int menu() {
	int selection_value = 0;
	int current_selection = 1;
	int menu, new_game_button, game_controls_button, high_scores_button, quit_button;

	double selection_time = glfwGetTime();
	double enter_time = selection_time;

	/*FTPoint new_game_text_position((window_width*0.41), window_height*0.515);
	FTPoint controls_text_position((window_width*0.37), window_height*0.375);
	FTPoint leaderboard_text_position((window_width*0.385), window_height*0.24);
	FTPoint quit_game_text_position((window_width*0.46), window_height*0.105);*/

	string menu_image = "menu.tga";
	string new_game_button_image = "new_game_button.tga";
	string game_controls_button_image = "game_controls_button.tga";
	string high_scores_button_image = "high_scores_button.tga";
	string quit_button_image = "quit_button.tga";

	menu = app->getTextureValue(menu_image);
	new_game_button = app->getTextureValue(new_game_button_image);
	game_controls_button = app->getTextureValue(game_controls_button_image);
	high_scores_button = app->getTextureValue(high_scores_button_image);
	quit_button = app->getTextureValue(quit_button_image);

	game_sounds.setSoundSourcePosition(0.0, 0.0, 0.0);
	while(selection_value == 0) {
		if(!glfwGetWindowParam( GLFW_OPENED )) {
			selection_value = 4;
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glDisable(GL_LIGHTING);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(75.0, aspect_ratio, 1.0, 20000.0);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(0.0, 0.0, -5.0);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glBindTexture(GL_TEXTURE_2D, menu);
		glBegin(GL_QUADS);
			glColor3f(1.0, 1.0, 1.0);
			glTexCoord2f(0.0, 0.0); glVertex3f(-6.25, -4.0, 0.0);			
			glTexCoord2f(0.0, 1.0); glVertex3f(-6.25, 4.0, 0.0);			
			glTexCoord2f(1.0, 1.0); glVertex3f(6.25, 4.0, 0.0);
			glTexCoord2f(1.0, 0.0); glVertex3f(6.25, -4.0, 0.0);
		glEnd();

		glBindTexture(GL_TEXTURE_2D, new_game_button);
		glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT);
		glBegin(GL_QUADS);
			glColor3f(1.0, 1.0, 1.0);
			//glColor4f(0.0, 0.0, 0.0, 0.5);
			if(current_selection == 1) {
				glColor4f(1.0, 1.0, 0.0, 0.5);
			}
			glTexCoord2f(0.0, 0.0); glVertex3f(-1.75, 0.25, 0.25);			
			glTexCoord2f(0.0, 1.0); glVertex3f(-1.75, 0.75, 0.25);			
			glTexCoord2f(1.0, 1.0); glVertex3f(1.75, 0.75, 0.25);
			glTexCoord2f(1.0, 0.0); glVertex3f(1.75, 0.25, 0.25);
		glEnd();
		glPopAttrib();

		glBindTexture(GL_TEXTURE_2D, game_controls_button);
		glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT);
		glBegin(GL_QUADS);
			glColor3f(1.0, 1.0, 1.0);
			//glColor4f(0.0, 0.0, 0.0, 0.5);
			if(current_selection == 2) {
				glColor4f(1.0, 1.0, 0.0, 0.5);
			}
			glTexCoord2f(0.0, 0.0); glVertex3f(-1.75, -0.75, 0.25);			
			glTexCoord2f(0.0, 1.0); glVertex3f(-1.75, -0.25, 0.25);			
			glTexCoord2f(1.0, 1.0); glVertex3f(1.75, -0.25, 0.25);
			glTexCoord2f(1.0, 0.0); glVertex3f(1.75, -0.75, 0.25);
		glEnd();
		glPopAttrib();

		glBindTexture(GL_TEXTURE_2D, high_scores_button);
		glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT);
		glBegin(GL_QUADS);
			glColor3f(1.0, 1.0, 1.0);
			//glColor4f(0.0, 0.0, 0.0, 0.5);
			if(current_selection == 3) {
				glColor4f(1.0, 1.0, 0.0, 0.5);
			}
			glTexCoord2f(0.0, 0.0); glVertex3f(-1.75, -1.75, 0.25);			
			glTexCoord2f(0.0, 1.0); glVertex3f(-1.75, -1.25, 0.25);			
			glTexCoord2f(1.0, 1.0); glVertex3f(1.75, -1.25, 0.25);
			glTexCoord2f(1.0, 0.0); glVertex3f(1.75, -1.75, 0.25);
		glEnd();
		glPopAttrib();

		glBindTexture(GL_TEXTURE_2D, quit_button);
		glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT);
		glBegin(GL_QUADS);
			glColor3f(1.0, 1.0, 1.0);
			//glColor4f(0.0, 0.0, 0.0, 0.5);
			if(current_selection == 4) {
				glColor4f(1.0, 1.0, 0.0, 0.5);
			}
			glTexCoord2f(0.0, 0.0); glVertex3f(-1.75, -2.75, 0.25);			
			glTexCoord2f(0.0, 1.0); glVertex3f(-1.75, -2.25, 0.25);			
			glTexCoord2f(1.0, 1.0); glVertex3f(1.75, -2.25, 0.25);
			glTexCoord2f(1.0, 0.0); glVertex3f(1.75, -2.75, 0.25);
		glEnd();
		glPopAttrib();

		glfwSwapBuffers();

		if(glfwGetKey( GLFW_KEY_UP ) && glfwGetTime() - selection_time >= 0.25) {
			game_sounds.playSound("menu-change.wav");
			current_selection--;
			selection_time = glfwGetTime();
		} else if(glfwGetKey( GLFW_KEY_DOWN ) && glfwGetTime() - selection_time >= 0.25) {
			game_sounds.playSound("menu-change.wav");
			current_selection++;
			selection_time = glfwGetTime();
		}

		if(current_selection == 0) {
			current_selection = 4;
		} else if(current_selection == 5) {
			current_selection = 1;
		}

		if(glfwGetKey( GLFW_KEY_ENTER ) && glfwGetTime() - enter_time >= 0.25) {
			selection_value = current_selection;
			enter_time = glfwGetTime();
		}
	}

	game_sounds.stopSound("b.wav");
	return selection_value;
}

//displays the game controls on the screen
int game_controls() {
	int control_system, back_button, return_value = 2;
	double delay_time = glfwGetTime();

	string control_system_image = "control-system.tga";
	string back_button_image = "back_button.tga";

	control_system = app->getTextureValue(control_system_image);
	back_button = app->getTextureValue(back_button_image);

	while(return_value == 2) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glDisable(GL_LIGHTING);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(75.0, aspect_ratio, 1.0, 20000.0);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(0.0, 0.0, -5.0);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glBindTexture(GL_TEXTURE_2D, control_system);
		glBegin(GL_QUADS);
			glColor3f(1.0, 1.0, 1.0);
			glTexCoord2f(0.0, 0.0); glVertex3f(-6.25, -4.0, 0.0);			
			glTexCoord2f(0.0, 1.0); glVertex3f(-6.25, 4.0, 0.0);			
			glTexCoord2f(1.0, 1.0); glVertex3f(6.25, 4.0, 0.0);
			glTexCoord2f(1.0, 0.0); glVertex3f(6.25, -4.0, 0.0);
		glEnd();

		glBindTexture(GL_TEXTURE_2D, back_button);
		glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT);
		glBegin(GL_QUADS);
			glColor4f(1.0, 1.0, 0.0, 0.5);
			glTexCoord2f(0.0, 0.0); glVertex3f(-1.75, -3.25, 0.25);			
			glTexCoord2f(0.0, 1.0); glVertex3f(-1.75, -2.75, 0.25);			
			glTexCoord2f(1.0, 1.0); glVertex3f(1.75, -2.75, 0.25);
			glTexCoord2f(1.0, 0.0); glVertex3f(1.75, -3.25, 0.25);
		glEnd();
		glPopAttrib();

		glfwSwapBuffers();

		if(glfwGetKey( GLFW_KEY_ENTER ) && glfwGetTime() - delay_time >= 1.0) {
			return_value = 0;
		}
	}

	return return_value;
}

//displays the high scores
int high_scores() {
	//open the high scores file and load values into an array
	string name, score, high_scores_file_name = "bin/high_scores.txt";
	int i = 0;
	bool error = false;
	
	//holds the name and the scores in the high scores file
	string names[MAX_HIGH_SCORES];
	string scores[MAX_HIGH_SCORES];

	for(int i = 0; i < MAX_HIGH_SCORES; i++) {
		names[i] = "";
		scores[i] = "0";
	}

	fstream read_high_scores;

	read_high_scores.open(high_scores_file_name, ios::in);
	
	if(read_high_scores) {
		while(!read_high_scores.eof()) {
			read_high_scores >> name >> score;

			names[i] = name;
			scores[i] = score;

			i++;
		}

		read_high_scores.close();
	}

	FTPoint name_highscore1_position((window_width*0.35), window_height*0.55);
	FTPoint name_highscore2_position((window_width*0.35), window_height*0.51);
	FTPoint name_highscore3_position((window_width*0.35), window_height*0.47);
	FTPoint name_highscore4_position((window_width*0.35), window_height*0.43);
	FTPoint name_highscore5_position((window_width*0.35), window_height*0.39);
	FTPoint name_highscore6_position((window_width*0.35), window_height*0.35);
	FTPoint name_highscore7_position((window_width*0.35), window_height*0.31);
	FTPoint name_highscore8_position((window_width*0.35), window_height*0.27);
	FTPoint name_highscore9_position((window_width*0.35), window_height*0.23);
	FTPoint name_highscore10_position((window_width*0.35), window_height*0.19);

	FTPoint score_highscore1_position((window_width*0.64), window_height*0.55);
	FTPoint score_highscore2_position((window_width*0.64), window_height*0.51);
	FTPoint score_highscore3_position((window_width*0.64), window_height*0.47);
	FTPoint score_highscore4_position((window_width*0.64), window_height*0.43);
	FTPoint score_highscore5_position((window_width*0.64), window_height*0.39);
	FTPoint score_highscore6_position((window_width*0.64), window_height*0.35);
	FTPoint score_highscore7_position((window_width*0.64), window_height*0.31);
	FTPoint score_highscore8_position((window_width*0.64), window_height*0.27);
	FTPoint score_highscore9_position((window_width*0.64), window_height*0.23);
	FTPoint score_highscore10_position((window_width*0.64), window_height*0.19);

	int high_score_list, back_button, return_value = 3;
	double delay_time = glfwGetTime();

	string high_scores_list_image = "high-score-list.tga";
	string back_button_image = "back_button.tga";

	high_score_list = app->getTextureValue(high_scores_list_image);
	back_button = app->getTextureValue(back_button_image);

	while(return_value == 3) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glDisable(GL_LIGHTING);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(75.0, aspect_ratio, 1.0, 20000.0);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(0.0, 0.0, -5.0);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glBindTexture(GL_TEXTURE_2D, high_score_list);
		glBegin(GL_QUADS);
			glColor3f(1.0, 1.0, 1.0);
			glTexCoord2f(0.0, 0.0); glVertex3f(-6.25, -4.0, 0.0);			
			glTexCoord2f(0.0, 1.0); glVertex3f(-6.25, 4.0, 0.0);			
			glTexCoord2f(1.0, 1.0); glVertex3f(6.25, 4.0, 0.0);
			glTexCoord2f(1.0, 0.0); glVertex3f(6.25, -4.0, 0.0);
		glEnd();

		glBindTexture(GL_TEXTURE_2D, back_button);
		glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT);
		glBegin(GL_QUADS);
			glColor4f(1.0, 1.0, 0.0, 0.5);
			glTexCoord2f(0.0, 0.0); glVertex3f(-1.75, -3.25, 0.25);			
			glTexCoord2f(0.0, 1.0); glVertex3f(-1.75, -2.75, 0.25);			
			glTexCoord2f(1.0, 1.0); glVertex3f(1.75, -2.75, 0.25);
			glTexCoord2f(1.0, 0.0); glVertex3f(1.75, -3.25, 0.25);
		glEnd();
		glPopAttrib();

		glPixelTransferf(GL_RED_BIAS, -1.0f);
		glPixelTransferf(GL_GREEN_BIAS, -1.0f);
		glPixelTransferf(GL_BLUE_BIAS, -1.0f);

		if(scores[0].compare("0") != 0) {
			display_text(names[0], name_highscore1_position, 24);
			display_text(scores[0], score_highscore1_position, 24);
		}

		if(scores[1].compare("0") != 0) {
			display_text(names[1], name_highscore2_position, 24);
			display_text(scores[1], score_highscore2_position, 24);
		}

		if(scores[2].compare("0") != 0) {
			display_text(names[2], name_highscore3_position, 24);
			display_text(scores[2], score_highscore3_position, 24);
		}

		if(scores[3].compare("0") != 0) {
			display_text(names[3], name_highscore4_position, 24);
			display_text(scores[3], score_highscore4_position, 24);
		}

		if(scores[4].compare("0") != 0) {
			display_text(names[4], name_highscore5_position, 24);
			display_text(scores[4], score_highscore5_position, 24);
		}

		if(scores[5].compare("0") != 0) {
			display_text(names[5], name_highscore6_position, 24);
			display_text(scores[5], score_highscore6_position, 24);
		}

		if(scores[6].compare("0") != 0) {
			display_text(names[6], name_highscore7_position, 24);
			display_text(scores[6], score_highscore7_position, 24);
		}

		if(scores[7].compare("0") != 0) {
			display_text(names[7], name_highscore8_position, 24);
			display_text(scores[7], score_highscore8_position, 24);
		}

		if(scores[8].compare("0") != 0) {
			display_text(names[8], name_highscore9_position, 24);
			display_text(scores[8], score_highscore9_position, 24);
		}

		if(scores[9].compare("0") != 0) {
			display_text(names[9], name_highscore10_position, 24);
			display_text(scores[9], score_highscore10_position, 24);
		}

		glfwSwapBuffers();

		if(glfwGetKey( GLFW_KEY_ENTER ) && glfwGetTime() - delay_time >= 1.0) {
			return_value = 0;
		}
	}

	return return_value;
}

/**
 * The main entry point. We pass arguments onto GLFW.
 */
int main()
{
	int running = 0;
	int game_end = 0;
	int select_exit_value = 0;
	/*int exit_box;
	int button;

	double pause_time = 0.0;
	double selection_time = glfwGetTime();

	bool pause_game = false;*/
	bool menu_sound = false;

	/*FTPoint paused_text_position((window_width*0.39), window_height*0.75);
	FTPoint yes_text_position((window_width*0.40), window_height*0.385);
	FTPoint no_text_position((window_width*0.575), window_height*0.385);*/

	//initialize the openGL window
	init();

	/*string exit_box_image = "exit-box.tga";
	string button_image = "button.tga";

	exit_box = app->getTextureValue(exit_box_image);
	button = app->getTextureValue(button_image);*/

	// Main loop
	while (running != 4) {
		if(menu_sound == false) {
			game_sounds.playSound("initial-sound.wav");
			menu_sound = true;
		}

		running = menu();
		select_exit_value = 0;

		while( running == 1 ) {
			game_sounds.stopSound("initial-sound.wav");
			menu_sound = false;
			glDisable(GL_BLEND);

			if(!glfwGetWindowParam( GLFW_OPENED )) {
				running = 4;
			}

			//sets up the camera for the game
			display();

			//get the keys pressed
			keyboard();

			//updates the world
			running = update();
		}

		if(running == 2) {
			game_sounds.stopSound("initial-sound.wav");
			running = game_controls();
		}

		if(running == 3) {
			game_sounds.stopSound("initial-sound.wav");
			running = high_scores();
		}
	}

	//exit the openGL window
	if(running == 4) {
		game_sounds.stopAllSounds();
		exit();
	}
	
	return 0;
}