#include "Pins.h"
#include "BackWall.h"
#include "Gutter.h"
#include "Ground.h"
#include "ModelLoader.h"
#include "TextureLoader.h"
#include <string>
#include <FTGL/ftgl.h>
#include <sstream>

#define MAX_PINS 10

using namespace std;

/**
 * The main bowling class definition.
 */
class Bowling : public RigidBodyApplication
{
	int window_width, window_height;

	/** Tracks if a block has been hit. */
	bool hit;
	bool ball_active;
	bool bowled;
	bool show_score;
	bool shot_reset;
	bool display_score;
	bool end_of_game;

	float current_time;
	float spin;
	float movement;
	float power;
	float fire_timeout;
	float ground_change;

	float friction_value;

	int shot;
	int score;
	int current_frame_score;
	int bowling_shot;
	int ground_type;

	int gutter_texture;
	int wall_texture;
	int grass_texture;
	int ice_texture;
	int wood_texture;
	int current_floor_texture;

	unsigned pin_contact;

	/** Handle random numbers. */
	cyclone::Random random;

    /** Holds the bodies. */
    Pins pins[MAX_PINS];

    /** Holds the projectile. */
    cyclone::CollisionSphere ball;

	//Holds the backwall
	BackWall* wall;

	//Holds the gutters
	Gutter* left_gutter;
	Gutter* right_gutter;

	//holds the ground
	Ground* ground;

	//Initialize Text displayer
	FTGLPixmapFont font;

	//Models to be loaded
	ModelLoader bowling_ball;
	ModelLoader pin;

	//textures to be loaded
	TextureLoader textures;

	//list of texture images & texture file names
	GLuint* texture_images;
	string* texture_file_names;

	//model lists to be generated
	GLuint ball_list;
	GLuint pin_list;
	GLuint left_direction_list;
	GLuint right_direction_list;
	GLuint left_spin_list;
	GLuint right_spin_list;

	//Holds the score
	int game_score[20];
	int frame_score[10];

	void initialize();

    /** Processes the contact generation code. */
	virtual void generateContacts();

    /** Processes the objects in the simulation forward in time. */
    virtual void updateObjects(cyclone::real duration);

    /** Resets the position of all the blocks. */
    virtual void reset();

	/** Processes the physics. */
	virtual void update();

	//fire the bowling ball
	void bowl();

	//reset the shot
	void reset_shot();

	//resets the frame
	void reset_frame();

	//calculates the score
	void calculate_score();

	//draw the bar for spin and sideways movement
	void draw_power_bar();
	void draw_direction_bar(int scale);
	void draw_left_spin_bar();
	void draw_right_spin_bar();

	//draw the score
	void draw_score();

	//the 2 viewports to be drawn
	void display_normal_screen();
	void display_shot_screen();

	void display_end_of_game_screen();

	//Helper functions
	void display_text(string text, FTPoint& position, unsigned int size);
	string intToString(int a);
	GLfloat** generate_vector(int rows, int cols);
	void delete_vector(GLfloat** my_vector, int row);
	

	//functions to load models and textures
	int load_models();
	int load_textures(string& texture_file);
	void create_call_lists();
	int generate_model_display_list(ModelLoader& model, GLuint model_call_list);
	int generate_direction_display_list(GLuint& texture, GLuint model_call_list);
	int generate_spin_display_list(GLuint& texture, GLuint model_call_list);

public:
    /** Creates a new demo object. */
    Bowling();
	~Bowling();

	virtual int getTextureValue(string& texture_name);

    /** Display the particle positions. */
    virtual void display();

	/** Handle a keypress. */
    virtual void key();
};