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

#include <string>
#include <FTGL/ftgl.h>
#include <windows.h>

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

	app = getApplication();
	TimingData::init();
	app->initGraphics();

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
    while(glfwGetTime() - time <= 2.0) {
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

	success = glfwReadImage("bin/images/logo.tga", &current_image, 0);

    if(success) {
        glBindTexture(GL_TEXTURE_2D, logo_texture);
                        
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, current_image.Format, current_image.Width, current_image.Height, 0, current_image.Format, GL_UNSIGNED_BYTE, current_image.Data); // Texture specification

        glfwFreeImage(&current_image);
    }

    time = glfwGetTime();
    while(glfwGetTime() - time <= 4.0) {
        glClear( 0 | GL_DEPTH_BUFFER_BIT );

        if(success) {
            glBindTexture(GL_TEXTURE_2D, logo_texture);

            glBegin(GL_QUADS);
                glTexCoord2f(0.0, 0.0);
                glVertex3f(-5.0, -3.0, 0.0);
                glTexCoord2f(0.0, 1.0);
                glVertex3f(-5.0, 3.0, 0.0);
                glTexCoord2f(1.0, 1.0);
                glVertex3f(5.0, 3.0, 0.0);
                glTexCoord2f(1.0, 0.0);
                glVertex3f(5.0, -3.0, 0.0);
            glEnd();
        } 
        
        glfwSwapBuffers();
    }
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
	double enter_time = glfwGetTime();

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

	while(selection_value == 0) {
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
			current_selection--;
			selection_time = glfwGetTime();
		} else if(glfwGetKey( GLFW_KEY_DOWN ) && glfwGetTime() - selection_time >= 0.25) {
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

	return selection_value;
}

/**
 * The main entry point. We pass arguments onto GLUT.
 */
int main()
{
	int running = 0;
	int game_end = 0;
	int select_exit_value = 0;
	int exit_box;
	int button;

	double pause_time = 0.0;

	bool pause_game = false;

	FTPoint paused_text_position((window_width*0.39), window_height*0.75);
	FTPoint yes_text_position((window_width*0.40), window_height*0.42);
	FTPoint no_text_position((window_width*0.575), window_height*0.42);

	//initialize the openGL window
	init();

	string exit_box_image = "exit-box.tga";
	string button_image = "button.tga";

	exit_box = app->getTextureValue(exit_box_image);
	button = app->getTextureValue(button_image);

	// Main loop
	while (running != 4) {
		running = menu();
		select_exit_value = 0;

		while( running == 1 ) {
			glDisable(GL_BLEND);

			running = glfwGetWindowParam( GLFW_OPENED );

			//updates the world
			running = update();

			keyboard();

			//sets up the camera for the game
			display();

			//Check if ESC key	was pressed or window was closed. if yes add the window to ask if you want to quit
			pause_time = glfwGetTime();
			while(glfwGetKey( GLFW_KEY_ESC ) || pause_game) {
				pause_game = true;

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

				glBindTexture(GL_TEXTURE_2D, exit_box);
				glBegin(GL_QUADS);
					glColor3f(1.0, 1.0, 1.0);
					glTexCoord2f(0.0, 0.0); glVertex3f(-2.25, -1.0, 0.0);			
					glTexCoord2f(0.0, 1.0); glVertex3f(-2.25, 1.0, 0.0);			
					glTexCoord2f(1.0, 1.0); glVertex3f(2.25, 1.0, 0.0);
					glTexCoord2f(1.0, 0.0); glVertex3f(2.25, -1.0, 0.0);
				glEnd();

				glBindTexture(GL_TEXTURE_2D, button);
				glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT);
				glBegin(GL_QUADS);
					glColor3f(1.0, 1.0, 1.0);
					if(select_exit_value == 1) {
						glColor4f(1.0, 1.0, 0.0, 0.5);
					}
					glTexCoord2f(0.0, 0.0); glVertex3f(-1.5, -0.75, 0.25);			
					glTexCoord2f(0.0, 1.0); glVertex3f(-1.5, -0.25, 0.25);			
					glTexCoord2f(1.0, 1.0); glVertex3f(-0.5, -0.25, 0.25);
					glTexCoord2f(1.0, 0.0); glVertex3f(-0.5, -0.75, 0.25);
				glEnd();
				glPopAttrib();

				glBindTexture(GL_TEXTURE_2D, button);
				glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT);
				glBegin(GL_QUADS);
					glColor3f(1.0, 1.0, 1.0);
					if(select_exit_value == 0) {
						glColor4f(1.0, 1.0, 0.0, 0.5);
					}
					glTexCoord2f(0.0, 0.0); glVertex3f(1.5, -0.75, 0.25);			
					glTexCoord2f(0.0, 1.0); glVertex3f(1.5, -0.25, 0.25);			
					glTexCoord2f(1.0, 1.0); glVertex3f(0.5, -0.25, 0.25);
					glTexCoord2f(1.0, 0.0); glVertex3f(0.5, -0.75, 0.25);
				glEnd();
				glPopAttrib();

				display_text("Game Paused", paused_text_position, 50);
				display_text("Yes", yes_text_position, 24);
				display_text("No", no_text_position, 24);

				glfwSwapBuffers();

				if(glfwGetKey( GLFW_KEY_RIGHT )) {
					select_exit_value = 0;
				} else if(glfwGetKey( GLFW_KEY_LEFT )) {
					select_exit_value = 1;
				}

				if(glfwGetKey( GLFW_KEY_ENTER ) && select_exit_value == 1) {
					pause_game = false;
					running = 0;
					//rest the game before exiting
				} else if(glfwGetKey( GLFW_KEY_ENTER ) && select_exit_value == 0) {
					pause_game = false;

					glEnable(GL_LIGHTING);
					glDisable(GL_BLEND);

					glfwSetTime(pause_time);
				}

				if(!glfwGetWindowParam( GLFW_OPENED )) {
					pause_game = false;
					running = 4;
				}
			}
		}
	}

	//exit the openGL window
	if(running == 4) {
		exit();
	}
	
	return 0;
}