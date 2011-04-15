#include "Bowling.h"

#define DEG2RAD 0.0174532778f
cyclone::Random global_random;

string keysPressed;
void  GLFWCALL getInputs( int key, int action );
void  GLFWCALL emptyCallBack( int key, int action );

// Method definitions
Bowling::Bowling():RigidBodyApplication(), game_font("bin/fonts/trebuc.ttf")
{
	window_width = 1280, window_height = 800;
	aspect_ratio = (float)window_width/window_height;

	bowled = false;
	shot_reset = false;
	display_score = false;
	end_of_game = false;
	start_game = false;

	spin = 0.0f;
	movement = 0.0f;
	power = 5.0f;
	current_time = 0.0;
	fire_timeout = 0.0;
	ground_change = 0.0;

	friction_value = 0.7;

	shot = 0;
	score = 0;
	current_frame_score = 0;

	bowling_shot = 0;

	ground_type = 1;

	for(int i = 0; i < 20; i++) {
		game_score[i] = -1;
	}

	for(int i = 0; i < 10; i++) {
		frame_score[i] = -1;
	}

	for(int i = 0; i < MAX_HIGH_SCORES; i++) {
		names[i] = "";
		scores[i] = 0;
	}

	// Create the ball.
	ball.body = new cyclone::RigidBody();
	ball.radius = 0.5f;
	ball.body->setMass(30.0f);
	ball.body->setDamping(0.8f, 0.8f);
	cyclone::Matrix3 it;
	it.setDiagonal(5.0f, 5.0f, 5.0f);
	ball.body->setInertiaTensor(it);
	ball.body->setAcceleration(cyclone::Vector3::GRAVITY);

	ball.body->setCanSleep(false);
	ball.body->setAwake(true);

	wall = new BackWall();
	left_gutter = new Gutter();
	right_gutter = new Gutter();
	ground = new Ground();

	initialize();

    // Set up the initial block
    reset();
}

Bowling::~Bowling() {
	delete wall;
	delete left_gutter;
	delete right_gutter;
	delete ground;

	wall = NULL;
	left_gutter = NULL;
	right_gutter = NULL;
	ground = NULL;

	delete texture_images;
	texture_images = NULL;

	delete[] texture_file_names;
	texture_file_names = NULL;
}

void Bowling::initialize() {
	//load all models
	load_models();

	//load all textures
	string texture_file_name = "bin/images/textures.txt";
	load_textures(texture_file_name);

	//load the high scores file and store in an array
	load_high_scores();

	//create all call lists
	create_call_lists();

	string gutter = "concrete.tga";
	string wall = "back.tga";
	string new_wall = "new_back.tga";
	string grass = "green.tga";
	string ice = "ice.tga";
	string wood = "wood-floor.tga";

	gutter_texture = getTextureValue(gutter);
	wall_texture = getTextureValue(wall);
	new_wall_texture = getTextureValue(new_wall);
	grass_texture = getTextureValue(grass);
	ice_texture = getTextureValue(ice);
	wood_texture = getTextureValue(wood);
	current_floor_texture = wood_texture;

	right_alley_list = 0;
	left_alley_list = 0;
}

//handle to read models.txt and load models in parallel
int Bowling::load_models() {
	string ball_model = "ball.obj";
	string pin_model = "bowling-pin.obj";

	bowling_ball.LoadModel(ball_model);
	pin.LoadModel(pin_model);

	return 0;
}

//read the texture file and load all the textures
int Bowling::load_textures(string& texture_file) {
	int number_of_textures = 0;
	string texture_file_name;
	fstream texture_loader;

	texture_loader.open(texture_file, ios::in);

	if(!texture_loader) {
		return 1;
	}

	texture_loader.ignore(1024, '\n');
	texture_loader >> number_of_textures;

	texture_file_names = new string[number_of_textures];
	texture_images = new GLuint[number_of_textures];

	for(int i = 0; i < number_of_textures; i++) {
		texture_loader >> texture_file_name;
		texture_file_names[i] = texture_file_name;
	}

	textures.LoadTextures(texture_file_names, number_of_textures, texture_images);

	return 0;
}

int Bowling::load_high_scores() {
	//open the high scores file and load values into an array
	string name, high_scores_file_name = "bin/high_scores.txt";
	int score, i = 0;
	fstream read_high_scores;

	read_high_scores.open(high_scores_file_name, ios::in);
	
	if(!read_high_scores) {
		return 1;
	}

	while(!read_high_scores.eof()) {
		read_high_scores >> name >> score;

		names[i] = name;
		scores[i] = score;

		i++;
	}

	read_high_scores.close();

	return 0;
}

int Bowling::save_high_scores() {
	//open the high scores file and load values into an array
	string name, high_scores_file_name = "bin/high_scores.txt";
	int score, number_of_high_scores = 0;
	ofstream save_high_scores;

	save_high_scores.open(high_scores_file_name, ios::out);
	
	if(!save_high_scores) {
		return 1;
	}

	for(int i = 0; i < MAX_HIGH_SCORES; i++) {
		if(scores[i] == 0) {
			number_of_high_scores = i;
			break;
		}
	}

	for(int i = 0; i < number_of_high_scores; i++) {
		name = names[i];
		score = scores[i];

		if(i < number_of_high_scores - 1) {
			save_high_scores << name << ' ' << score << endl;
		} else {
			save_high_scores << name << ' ' << score;
		}
	}

	save_high_scores.close();

	return 0;
}

int Bowling::update_high_scores(string name, int score) {
	string current_names[MAX_HIGH_SCORES];
	int current_top_scores[MAX_HIGH_SCORES];

	int current_rank = 0;

	for(int i = 0; i < MAX_HIGH_SCORES; i++) {
		current_names[i] = names[i];
		current_top_scores[i] = scores[i];
	}

	for(int i = 0; i < MAX_HIGH_SCORES; i++) {
		if(score >= current_top_scores[i]) {
			current_rank = i;
			break;
		}
	}

	for(int i = 0; i < current_rank; i++) {
		names[i] = current_names[i];
		scores[i] = current_top_scores[i];
	}

	names[current_rank] = name;
	scores[current_rank] = score;

	for(int i = current_rank; i < MAX_HIGH_SCORES - 1; i++) {
		names[i + 1] = current_names[i];
		scores[i + 1] = current_top_scores[i];
	}

	return 0;
}

//create and generate all display lists
void Bowling::create_call_lists() {
	//create the model display lists
	ball_list = glGenLists(1);
	pin_list = glGenLists(1);

	//create the texture display lists
	left_direction_list = glGenLists(1);
	right_direction_list = glGenLists(1);
	left_spin_list = glGenLists(1);
	right_spin_list = glGenLists(1);
	controls_display_list = glGenLists(1);

	//generate all the model display lists
	generate_model_display_list(bowling_ball, ball_list);
	generate_model_display_list(pin, pin_list);

	string left_direction = "left-arrow.tga";
	string right_direction = "right-arrow.tga";
	string left_spin = "left-spin-arrow.tga";
	string right_spin = "right-spin-arrow.tga";
	string controls_display = "controls.tga";

	GLuint left_direction_texture = getTextureValue(left_direction);
	GLuint right_direction_texture = getTextureValue(right_direction);
	GLuint left_spin_texture = getTextureValue(left_spin);
	GLuint right_spin_texture = getTextureValue(right_spin);
	GLuint controls_display_texture = getTextureValue(controls_display);

	//generate the textures display lists
	generate_direction_display_list(left_direction_texture, left_direction_list);
	generate_direction_display_list(right_direction_texture, right_direction_list);
	generate_spin_display_list(left_spin_texture, left_spin_list);
	generate_spin_display_list(right_spin_texture, right_spin_list);
	generate_controls_display_list(controls_display_texture, controls_display_list);
}

//generate the call list from the model and the call list value
int Bowling::generate_model_display_list(ModelLoader& model, GLuint model_call_list) {
	if(model_call_list != 0) {
		string current_material;
		string current_texture;

		GLfloat** vertex_vector = generate_vector(3, 3);
		GLfloat** normals_vector = generate_vector(3, 3);
		GLfloat** texture_vector = generate_vector(3, 2);

		glNewList(model_call_list, GL_COMPILE);
			for(int i = 0; i < model.current_model.faces->size(); i++) {
				//load materials and bind textures
				//if the current material is the same as in previous iteration do not relaoad the material values
				if(current_material.compare(model.current_model.faces->at(i).texture_material) != 0) {
					current_material = model.current_model.faces->at(i).texture_material;
					if(current_material.compare("(null)") != 0) {
						int material_index = 0;
						while(current_material.compare(model.current_model.materials->at(material_index).newmtl) != 0)
							material_index++;

						//assign material values
						glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, model.current_model.materials->at(material_index).Ns);
						glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, (GLfloat*) &model.current_model.materials->at(material_index).Ka);
						glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, (GLfloat*) &model.current_model.materials->at(material_index).Kd);
						glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, (GLfloat*) &model.current_model.materials->at(material_index).Ks);

						//bind texture
						current_texture = model.current_model.materials->at(material_index).map_Kd;
						int texture_index = 0;
						while(current_texture.compare(texture_file_names[texture_index]) != 0) {
							texture_index++;
						}

						glBindTexture(GL_TEXTURE_2D, texture_images[texture_index]);
					}
				}
				
				//load vectors
				int j = 0, vertex_index = 0, normals_index = 0, texture_index = 0, index = 0;
				while(j < model.current_model.faces->at(i).face.size()) {
					if(j%3 == 0) {
						index = model.current_model.faces->at(i).face.at(j) - 1;
						
						vertex_vector[vertex_index][0] = model.current_model.vertices->at(index).x;
						vertex_vector[vertex_index][1] = model.current_model.vertices->at(index).y;
						vertex_vector[vertex_index][2] = model.current_model.vertices->at(index).z;

						vertex_index++;
					} else if(j%3 == 1) {
						index = model.current_model.faces->at(i).face.at(j) - 1;

						texture_vector[texture_index][0] = model.current_model.texture_vertices->at(index).u;
						texture_vector[texture_index][1] = model.current_model.texture_vertices->at(index).v;	

						texture_index++;
					} else if(j%3 == 2) {
						index = model.current_model.faces->at(i).face.at(j) - 1;

						normals_vector[normals_index][0] = model.current_model.normal_vertices->at(index).x;
						normals_vector[normals_index][1] = model.current_model.normal_vertices->at(index).y;
						normals_vector[normals_index][2] = model.current_model.normal_vertices->at(index).z;

						normals_index++;
					}
					j++;
				}

				//draw the triangle
				glBegin(GL_TRIANGLES);
					//first point
					glTexCoord2fv(texture_vector[0]);
					glNormal3fv(normals_vector[0]);
					glVertex3fv(vertex_vector[0]);
					//second point
					glTexCoord2fv(texture_vector[1]);
					glNormal3fv(normals_vector[1]);
					glVertex3fv(vertex_vector[1]);
					//third point
					glTexCoord2fv(texture_vector[2]);
					glNormal3fv(normals_vector[2]);
					glVertex3fv(vertex_vector[2]);
				glEnd();
			}
		glEndList();

		delete_vector(vertex_vector, 3);
		delete_vector(normals_vector, 3);
		delete_vector(texture_vector, 3);
	}

	return 0;
}

//generate the display list for the direction textures
int Bowling::generate_direction_display_list(GLuint& texture, GLuint model_call_list) {
	if(model_call_list != 0) {
		glNewList(model_call_list, GL_COMPILE);
			glBindTexture(GL_TEXTURE_2D, texture);
			glBegin(GL_QUADS);	
				glTexCoord2f(1.0, 1.0); 
				glVertex3f(0.5, 0.5, 0.0);
				glTexCoord2f(1.0, 0.0); 
				glVertex3f(0.5, 0.0, 0.0);
				glTexCoord2f(0.0, 0.0); 
				glVertex3f(0.0, 0.0, 0.0);
				glTexCoord2f(0.0, 1.0); 
				glVertex3f(0.0, 0.5, 0.0);
			glEnd();
		glEndList();
	}

	return 0;
}

//generate the display list for the spin textures
int Bowling::generate_spin_display_list(GLuint& texture, GLuint model_call_list) {
	if(model_call_list != 0) {
		glNewList(model_call_list, GL_COMPILE);
			glBindTexture(GL_TEXTURE_2D, texture);
			glBegin(GL_QUADS);	
				glTexCoord2f(1.0, 1.0); 
				glVertex3f(0.5, 0.5, 0.0);
				glTexCoord2f(1.0, 0.0); 
				glVertex3f(0.5, -0.5, 0.0);
				glTexCoord2f(0.0, 0.0); 
				glVertex3f(0.0, -0.5, 0.0);
				glTexCoord2f(0.0, 1.0); 
				glVertex3f(0.0, 0.5, 0.0);
			glEnd();
		glEndList();
	}

	return 0;
}

//generate the display list for the controls texture
int Bowling::generate_controls_display_list(GLuint& texture, GLuint model_call_list) {
	if(model_call_list != 0) {
		glNewList(model_call_list, GL_COMPILE);
			glBindTexture(GL_TEXTURE_2D, texture);
			glBegin(GL_QUADS);	
				glTexCoord2f(1.0, 1.0); 
				glVertex3f(4.0, 0.6, 0.0);
				glTexCoord2f(1.0, 0.0); 
				glVertex3f(4.0, -0.6, 0.0);
				glTexCoord2f(0.0, 0.0); 
				glVertex3f(-4.0, -0.6, 0.0);
				glTexCoord2f(0.0, 1.0); 
				glVertex3f(-4.0, 0.6, 0.0);
			glEnd();
		glEndList();
	}

	return 0;
}

void Bowling::generateContacts()
{
	hit = false;

	// Create the ground plane data
	cyclone::CollisionPlane plane;
	plane.direction = cyclone::Vector3(0,1,0);
	plane.offset = 0;

	// Set up the collision data structure
	cData.reset(maxContacts);
	cData.friction = (cyclone::real)friction_value;
	cData.restitution = (cyclone::real)0.8;
	cData.tolerance = (cyclone::real)0.1;

	// Perform collision detection
	cyclone::Matrix4 transform, otherTransform;
	cyclone::Vector3 position, otherPosition;
	for (Pins *pin = pins; pin < pins+MAX_PINS; pin++)
	{
		if (!pin->exists) continue;

		cyclone::Matrix3 it;
		it.setBlockInertiaTensor(pin->halfSize, 4.0f);

		// Check for collisions with the ground plane
		if (!cData.hasMoreContacts()) return;
		cyclone::CollisionDetector::boxAndHalfSpace(*pin, plane, &cData);

		if (ball_active)
		{
			// And with the sphere
			if (!cData.hasMoreContacts()) return;
			if (cyclone::CollisionDetector::boxAndSphere(*pin, ball, &cData))
			{
				hit = true;
				pin_contact = cData.contactCount-1;
				pin->collide = true;
				pin->body->setAcceleration(cyclone::Vector3::GRAVITY);

				pin->body->setInertiaTensor(it);
			}

			if(cyclone::CollisionDetector::boxAndSphere(*wall, ball, &cData)) {
				ball.body->setAcceleration(0.0, -9.81, 0.0);
			}

			cyclone::CollisionDetector::boxAndSphere(*left_gutter, ball, &cData);
			cyclone::CollisionDetector::boxAndSphere(*right_gutter, ball, &cData);
		}

		cyclone::CollisionDetector::boxAndBox(*pin, *wall, &cData);
		cyclone::CollisionDetector::boxAndBox(*pin, *left_gutter, &cData);
		cyclone::CollisionDetector::boxAndBox(*pin, *right_gutter, &cData);

		// Check for collisions with each other box
		for (Pins *other = pin+1; other < pins+MAX_PINS; other++)
		{
			if (!other->exists) continue;

			if (!cData.hasMoreContacts()) return;
			cyclone::CollisionDetector::boxAndBox(*pin, *other, &cData);
			other->body->setAcceleration(cyclone::Vector3::GRAVITY);
			other->body->setInertiaTensor(it);
		}
	}

	// Check for sphere ground collisions
	if (ball_active)
	{
		if (!cData.hasMoreContacts()) return;
		cyclone::CollisionDetector::sphereAndHalfSpace(ball, plane, &cData);
	}
}

void Bowling::reset()
{
	reset_frame();

	srand((unsigned int)glfwGetTime());
	ground_type = rand()%3 + 1;

	end_of_game = false;
	start_game = false;
	bowling_shot = 0;
	current_time = 0.0;
	fire_timeout = 0.0;
	glfwSetTime(0.0);
}

void Bowling::reset_frame() {
	bowled = false;
	shot_reset = true;

	spin = 0.0f;
	movement = 0.0f;
	power = 5.0f;
	current_time = glfwGetTime();;

	shot = 0;
	score = 0;
	current_frame_score = 0;

	const cyclone::Vector3 wall_position(0,0,-10); 
	const cyclone::Quaternion wall_orientation(1,0,0,0);
	const cyclone::Vector3 wall_extent(6,6,1);
	const cyclone::Vector3 wall_velocity(0,0,0);
	wall->setState(wall_position, wall_orientation, wall_extent, wall_velocity);

	const cyclone::Vector3 left_gutter_position(-5,0,15); 
	const cyclone::Vector3 right_gutter_position(5,0,15);
	const cyclone::Quaternion gutter_orientation(1,0,0,0);
	const cyclone::Vector3 gutter_extent(1,1,25);
	const cyclone::Vector3 gutter_velocity(0,0,0);
	left_gutter->setState(left_gutter_position, gutter_orientation, gutter_extent, gutter_velocity);
	right_gutter->setState(right_gutter_position, gutter_orientation, gutter_extent, gutter_velocity);

	const cyclone::Vector3 ground_position(0,0,15); 
	const cyclone::Quaternion ground_orientation(0,0,0,0);
	const cyclone::Vector3 ground_extent(6,0,25);
	const cyclone::Vector3 ground_velocity(0,0,0);
	ground->setState(ground_position, ground_orientation, ground_extent, ground_velocity);

	// Make sure all the pins exist
	for (Pins *pin = pins; pin < pins+MAX_PINS; pin++)
	{
		pin->collide = false;
		pin->exists = true;
	}

	// Set the pins block
	int mid = MAX_PINS*0.5;
	for(int i = 0; i < MAX_PINS; i++) {
		pins[i].halfSize = cyclone::Vector3(1,1,1);
		
		if(i == 9) {
			pins[i].body->setPosition(3, 1, -6); //keeps the blocks on the ground
		} else if(i == 8) {
			pins[i].body->setPosition(1, 1, -6);
		} else if(i == 7) {
			pins[i].body->setPosition(-1, 1, -6);
		} else if(i == 6) {
			pins[i].body->setPosition(-3, 1, -6);
		} else if(i == 5) {
			pins[i].body->setPosition(2, 1, -5);
		} else if(i == 4) {
			pins[i].body->setPosition(0, 1, -5);
		} else if(i == 3) {
			pins[i].body->setPosition(-2, 1, -5);
		} else if(i == 2) {
			pins[i].body->setPosition(1, 1, -4);
		} else if(i == 1) {
			pins[i].body->setPosition(-1, 1, -4);
		} else {
			pins[i].body->setPosition(0, 1, -3);
		}

		pins[i].body->setOrientation(1,0,0,0);
		pins[i].body->setVelocity(0,0,0);
		pins[i].body->setRotation(0,0,0);
		pins[i].body->setMass(10.0f);
		cyclone::Matrix3 it;
		it.setBlockInertiaTensor(pins[0].halfSize, pins[i].body->getMass());
		pins[i].body->setInertiaTensor(it);
		pins[i].body->setDamping(0.9f, 0.9f);
		pins[i].body->calculateDerivedData();
		pins[i].calculateInternals();

		//pins[i].body->setAcceleration(cyclone::Vector3::GRAVITY);
		pins[i].body->setAwake(false);
		//pins[i].body->setCanSleep(true);
	}

	// Set up the ball
	ball.body->setPosition(0.0f, 1.0f, 17.0f);
	ball.body->setOrientation(1, 0, 0, 0);
	ball.body->setVelocity(0.0, 0.0f, 0.0f);
	ball.body->setAcceleration(0.0, 0.0, 0.0);
	ball.body->setRotation(0,0,0);
	ball.body->calculateDerivedData();
	ball.body->setAwake(true);
	ball.calculateInternals();

	hit = false;

	// Reset the contacts
	cData.contactCount = 0;
}

void Bowling::reset_shot() {
	for (Pins *pin = pins; pin < pins+MAX_PINS; pin++)
	{
		if(pin->collide == true) {
			pin->exists = false;
		}
	}
	
	bowled = false;
	shot_reset = true;

	spin = 0.0f;
	movement = 0.0f;
	power = 5.0f;
	current_time = glfwGetTime();

	const cyclone::Vector3 wall_position(0,0,-10); 
	const cyclone::Quaternion wall_orientation(1,0,0,0);
	const cyclone::Vector3 wall_extent(6,6,1);
	const cyclone::Vector3 wall_velocity(0,0,0);
	wall->setState(wall_position, wall_orientation, wall_extent, wall_velocity);

	const cyclone::Vector3 left_gutter_position(-5,0,15); 
	const cyclone::Vector3 right_gutter_position(5,0,15);
	const cyclone::Quaternion gutter_orientation(1,0,0,0);
	const cyclone::Vector3 gutter_extent(1,1,25);
	const cyclone::Vector3 gutter_velocity(0,0,0);

	left_gutter->setState(left_gutter_position, gutter_orientation, gutter_extent, gutter_velocity);
	right_gutter->setState(right_gutter_position, gutter_orientation, gutter_extent, gutter_velocity);
	
	// Set the pins block
	//int mid = MAX_PINS*0.5;
	for(int i = 0; i < MAX_PINS; i++) {
		if(pins[i].exists == true) {
			pins[i].halfSize = cyclone::Vector3(1,1,1);
		
			if(i == 9) {
				pins[i].body->setPosition(3, 0, -6); //keeps the blocks on the ground
			} else if(i == 8) {
				pins[i].body->setPosition(1, 0, -6);
			} else if(i == 7) {
				pins[i].body->setPosition(-1, 0, -6);
			} else if(i == 6) {
				pins[i].body->setPosition(-3, 0, -6);
			} else if(i == 5) {
				pins[i].body->setPosition(2, 0, -5);
			} else if(i == 4) {
				pins[i].body->setPosition(0, 0, -5);
			} else if(i == 3) {
				pins[i].body->setPosition(-2, 0, -5);
			} else if(i == 2) {
				pins[i].body->setPosition(1, 0, -4);
			} else if(i == 1) {
				pins[i].body->setPosition(-1, 0, -4);
			} else {
				pins[i].body->setPosition(0, 0, -3);
			}

			pins[i].body->setOrientation(1,0,0,0);
			pins[i].body->setVelocity(0,0,0);
			pins[i].body->setRotation(0,0,0);
			pins[i].body->setMass(10.0f);
			cyclone::Matrix3 it;
			it.setBlockInertiaTensor(pins[0].halfSize, pins[i].body->getMass());
			pins[i].body->setInertiaTensor(it);
			pins[i].body->setDamping(0.9f, 0.9f);
			pins[i].body->calculateDerivedData();
			pins[i].calculateInternals();

			//pins[i].body->setAcceleration(cyclone::Vector3::GRAVITY);
			pins[i].body->setAwake(true);
			//pins[i].body->setCanSleep(true);
		}
	}

	// Set up the ball
	ball.body->setPosition(0.0f, 0.0f, 17.0f);
	ball.body->setOrientation(1, 0, 0, 0);
	ball.body->setVelocity(0.0, 0.0f, 0.0f);
	ball.body->setAcceleration(0.0, 0.0, 0.0);
	ball.body->setRotation(0,0,0);
	ball.body->calculateDerivedData();
	ball.body->setAwake(true);
	ball.calculateInternals();

	hit = false;

	// Reset the contacts
	cData.contactCount = 0;
}

void Bowling::calculate_score() {
	int shot_score = 0;
	int value = 0;
	int prev_frame_score = 0;

	for (Pins *pin = pins; pin < pins+MAX_PINS; pin++) {
		if(pin->collide == true) {
			shot_score++;
		}
	}

	if(bowling_shot % 2 == 0) {
		current_frame_score = shot_score;
		score = shot_score - score;

		value = (bowling_shot*0.5)-1;
		if(value != 0) {
			prev_frame_score = frame_score[value - 1];
			current_frame_score += prev_frame_score;
		}

		frame_score[value] = current_frame_score;
	} else {
		score = shot_score;
	}

	game_score[bowling_shot-1] = score;

	display_score = true;
}

void Bowling::bowl() {
	ball_active = true;
	bowled = true;
	shot_reset = false;

	current_time = glfwGetTime();
	bowling_shot++;
	shot++;

	//wake the pins
	/*for(int i = 0; i < MAX_PINS; i++) {
		pins[i].body->setAwake(true);
	}*/

	// Set up the ball
	ball.body->setPosition(0.0f, 1.0f, 17.0f);
	ball.body->setOrientation(1, 0, 0, 0);
	ball.body->setVelocity(movement, 0.0f, power);
	ball.body->setAcceleration(spin, -9.81, 0.0);
	ball.body->setRotation(0,0,0);
	ball.body->calculateDerivedData();
	ball.body->setAwake(true);
	ball.calculateInternals();

	hit = false;
}

int Bowling::update()
{
	RigidBodyApplication::update();
	
	//reset the next shot or the next frame
	if(glfwGetTime() - current_time >= 10.0 && bowled == true) {
		calculate_score();

		if(shot < 2) {
			reset_shot();
		} else {
			reset_frame();
		}

		if(bowling_shot == 20) {
			int final_score = frame_score[9]; //frame_score[0];

			FTPoint high_score_name_text_position((window_width*0.455), window_height*0.445);

			FTPoint end_game_text_position((window_width*0.15), window_height*0.75);
			FTPoint info_text_position((window_width*0.30), window_height*0.45);
			FTPoint score_text_position((window_width*0.48), window_height*0.30);

			glDisable(GL_LIGHTING);
			glDisable(GL_TEXTURE_2D);

			double end_game_time = glfwGetTime();
			
			end_of_game = true;
			while(glfwGetTime() - end_game_time <= 3.0) {	
				display();
				glfwSwapBuffers();
			}
			
			//check the score and if it is a high score then ask for name and call update_high_scores
			if(final_score > scores[MAX_HIGH_SCORES - 1]) {
				bool name_entered = false;
				int high_score_name_box;
				string high_score_name_box_image = "high-score-box.tga";
				string high_score_name = "";

				high_score_name_box = getTextureValue(high_score_name_box_image);

				glfwSetKeyCallback(getInputs);

				while(!name_entered) {
					glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

					glMatrixMode(GL_PROJECTION);
					glLoadIdentity();
					gluPerspective(75.0, aspect_ratio, 1.0, 20000.0);

					glMatrixMode(GL_MODELVIEW);
					glLoadIdentity();
					glTranslatef(0.0, 0.0, -5.0);

					glBindTexture(GL_TEXTURE_2D, high_score_name_box);
					glBegin(GL_QUADS);
						glColor3f(1.0, 1.0, 1.0);
						glTexCoord2f(0.0, 0.0); glVertex3f(-2.5, -1.5, 0.0);			
						glTexCoord2f(0.0, 1.0); glVertex3f(-2.5, 1.5, 0.0);			
						glTexCoord2f(1.0, 1.0); glVertex3f(2.5, 1.5, 0.0);
						glTexCoord2f(1.0, 0.0); glVertex3f(2.5, -1.5, 0.0);
					glEnd();

					high_score_name = keysPressed;
					display_text(high_score_name, high_score_name_text_position, 24);

					glfwSwapBuffers();

					if(glfwGetKey( GLFW_KEY_ENTER )) {
						name_entered = true;
					}
				}
				glfwSetKeyCallback(emptyCallBack);
				update_high_scores(high_score_name, final_score);
			}

			//add fireworks particles and high score check
			end_game_time = glfwGetTime();
			string game_score = intToString(final_score);

			while(glfwGetTime() - end_game_time <= 5.0) {
				glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				display_text("Congratulations!!!", end_game_text_position, 100);
				display_text("You Bowled:", info_text_position, 75);
				display_text(game_score, score_text_position, 75);

				glfwSwapBuffers();
			}

			glEnable(GL_LIGHTING);
			glEnable(GL_TEXTURE_2D);

			save_high_scores();
			reset();

			return 0;
		}
	}

	if(ground_type == 1) {
		current_floor_texture = wood_texture;
		ball.body->setDamping(0.8f, 0.8f);
	} else if(ground_type == 2) {
		current_floor_texture = ice_texture;
		ball.body->setDamping(0.995f, 0.995f);
	} else if(ground_type == 3) {
		current_floor_texture = grass_texture;
		ball.body->setDamping(0.6f, 0.6f);
	} 

	return 1;
}

void Bowling::updateObjects(cyclone::real duration)
{
	wall->calculateInternals();
	left_gutter->calculateInternals();
	right_gutter->calculateInternals();

    for (Pins *pin = pins; pin < pins+MAX_PINS; pin++)
    {
        if (pin->exists)
        {
            pin->body->integrate(duration);
            pin->calculateInternals();
        }
    }

	if (ball_active)
	{
		ball.body->integrate(duration);
		ball.calculateInternals();
	}
}

void Bowling::display_text(string text, FTPoint& position, unsigned int size) {
	game_font.FaceSize(size);
	game_font.Render(text.c_str(), -1, position);
}

string Bowling::intToString(int a) {
	string output;
	stringstream out;
	out << a;
	output = out.str();

	return output;
}

//generates a 2D array with the number of rows and columns
GLfloat** Bowling::generate_vector(int rows, int cols) {
  GLfloat** my_vector = new GLfloat* [rows];
  for (int j = 0; j < rows; j ++)
     my_vector[j] = new GLfloat[cols];
  
  return my_vector;
}

// Deletes a 2D array pointed by 'my_vector' that has 'row' number rows
void Bowling::delete_vector(GLfloat** my_vector, int row) {
  for (int j = 0; j < row; j ++)
     delete [] my_vector[j];

  delete [] my_vector;
}

int Bowling::getTextureValue(string& texture_name) {
	int texture_index = 0;
	while(texture_name.compare(texture_file_names[texture_index]) != 0) {
		texture_index++;
	}

	return texture_images[texture_index];
}

void Bowling::draw_power_bar() 
{
	FTPoint power_text_position((window_width*0.44), window_height*0.78);
	display_text("Shot Power", power_text_position, 32);

	glPolygonMode(GL_FRONT, GL_LINE);
	
	glColor3f(0.0, 0.0, 0.0);
	glBegin(GL_QUADS);	
		glVertex3f((0.0 - 0.02), (1.0 + 0.02), 0.0); 
		glVertex3f((0.0 - 0.02), (0.0 + 0.02), 0.0);
		glVertex3f((40.0 + 0.02), (0.0 + 0.02), 0.0);
		glVertex3f((40.0 + 0.02), (1.0 + 0.02), 0.0);
	glEnd();

	glPolygonMode(GL_FRONT, GL_FILL);

	if(power < 0.0) {
		glColor3f(0.0, 1.0, 0.0);
		glDisable(GL_LIGHTING);
		glBegin(GL_QUADS);
			for(float i = 0.0; i < -power; i +=(0.5)) {
				glVertex3f(i, 1.0, 0.0);
				glVertex3f(i, 0.0, 0.0);
				glVertex3f(i+(0.5), 0.0, 0.0);
				glVertex3f(i+(0.5), 1.0, 0.0);
			}
		glEnd();
		glEnable(GL_LIGHTING);
	}
}

void Bowling::draw_direction_bar(int scale)
{
	FTPoint direction_text_position((window_width*0.42), window_height*0.68);
	display_text("Shot Direction", direction_text_position, 32);

	glPolygonMode(GL_FRONT, GL_LINE);
	
	glColor3f(0.0, 0.0, 0.0);
	glBegin(GL_QUADS);	
		//glTexCoord2f(0.0, 1.0); 
		glVertex3f((-25.0 - 0.02)*scale, (0.5 + 0.02)*scale, 0.0);
		//glTexCoord2f(0.0, 0.0); 
		glVertex3f((-25.0 - 0.02)*scale, (0.0 + 0.02)*scale, 0.0);
		//glTexCoord2f(1.0, 0.0); 
		glVertex3f((25.0 + 0.02)*scale, (0.0 + 0.02)*scale, 0.0);
		//glTexCoord2f(1.0, 1.0); 
		glVertex3f((25.0 + 0.02)*scale, (0.5 + 0.02)*scale, 0.0);
	glEnd();

	glPolygonMode(GL_FRONT, GL_FILL);
	glDisable(GL_LIGHTING);
	if(movement >= 0.0) {
		glColor3f(0.0, 0.0, 1.0);
		glBegin(GL_QUADS);
			for(float i = 0.0; i < movement*scale; i +=(0.5*scale)) {
				glVertex3f(i, 0.5*scale, 0.0);
				glVertex3f(i, 0.0, 0.0);
				glVertex3f(i+(0.5*scale), 0.0, 0.0);
				glVertex3f(i+(0.5*scale), 0.5*scale, 0.0);
			}
		glEnd();
	} else {
		glColor3f(0.0, 0.0, 1.0);
		glBegin(GL_QUADS);
			for(float i = 0.0; i > movement*scale; i -=(0.5*scale)) {
				glVertex3f(i, 0.5*scale, 0.0);
				glVertex3f(i, 0.0, 0.0);
				glVertex3f(i+(0.5*scale), 0.0, 0.0);
				glVertex3f(i+(0.5*scale), 0.5*scale, 0.0);
			}
		glEnd();
	}
	glEnable(GL_LIGHTING);
}

void Bowling::draw_left_spin_bar() 
{
	FTPoint left_spin_text_position((window_width*0.17), window_height*0.585);
	display_text("Left Spin", left_spin_text_position, 24);

	glPolygonMode(GL_FRONT, GL_LINE);
	
	glColor3f(0.0, 0.0, 0.0);
	glBegin(GL_QUADS);	
		glVertex3f((-19.5 - 0.02), (4.25 + 0.02), 0.0);
		glVertex3f((-19.5 - 0.02), (3.75 + 0.02), 0.0);
		glVertex3f((0.5 + 0.02), (3.75 + 0.02), 0.0);
		glVertex3f((0.5 + 0.02), (4.25 + 0.02), 0.0);
	glEnd();

	glPolygonMode(GL_FRONT, GL_FILL);

	if(spin > 0.0) {
		glColor3f(1.0, 0.0, 0.0);
		glDisable(GL_LIGHTING);
		glBegin(GL_QUADS);
			for(float i = 0.0; i > -spin; i -=(0.5)) {
				glVertex3f(i, 4.25, 0.0);
				glVertex3f(i, 3.75, 0.0);
				glVertex3f(i+(0.5), 3.75, 0.0);
				glVertex3f(i+(0.5), 4.25, 0.0);
			}
		glEnd();
		glEnable(GL_LIGHTING);
	}
}

void Bowling::draw_right_spin_bar() 
{
	FTPoint right_spin_text_position((window_width*0.75), window_height*0.585);
	display_text("Right Spin", right_spin_text_position, 24);

	glPolygonMode(GL_FRONT, GL_LINE);
	
	glColor3f(0.0, 0.0, 0.0);
	glBegin(GL_QUADS);	
		glVertex3f((0.0 - 0.02), (4.25 + 0.02), 0.0);
		glVertex3f((0.0 - 0.02), (3.75 + 0.02), 0.0);
		glVertex3f((20.0 + 0.02), (3.75 + 0.02), 0.0);
		glVertex3f((20.0 + 0.02), (4.25 + 0.02), 0.0);
	glEnd();

	glPolygonMode(GL_FRONT, GL_FILL);

	if(spin < 0.0) {
		glColor3f(1.0, 0.0, 0.0);
		glDisable(GL_LIGHTING);
		glBegin(GL_QUADS);
			for(float i = 0.0; i < -spin; i +=(0.5)) {
				glVertex3f(i, 4.25, 0.0);
				glVertex3f(i, 3.75, 0.0);
				glVertex3f(i+(0.5), 3.75, 0.0);
				glVertex3f(i+(0.5), 4.25, 0.0);
			}
		glEnd();
		glEnable(GL_LIGHTING);
	}
}

void Bowling::draw_score() {
	glPolygonMode(GL_FRONT, GL_LINE);

	glColor3f(0.0, 0.0, 0.0);
	glBegin(GL_QUADS);
		for(int i = 0; i < 20; i++) {
			glVertex3f(i, 1.0, 0.0);
			glVertex3f(i, 0.0, 0.0);
			glVertex3f(i+1, 0.0, 0.0);
			glVertex3f(i+1, 1.0, 0.0);
		}
	glEnd();

	glBegin(GL_QUADS);
		for(int i = 0; i < 20; i+=2) {
			glVertex3f(i, 0.0, 0.0);
			glVertex3f(i, -1.0, 0.0);
			glVertex3f(i+2, -1.0, 0.0);
			glVertex3f(i+2, 0.0, 0.0);
		}
	glEnd();
	glPolygonMode(GL_FRONT, GL_FILL);

	if(display_score) {
		string shot_score;
		//string frame_score;
		for(int i = 0; i < 20; i++) {
			if(i < bowling_shot && game_score[i] >= 0) {
				FTPoint score_text_position((window_width*(0.1+(i*0.041))), window_height*0.94);
				shot_score = intToString(game_score[i]);
				display_text(shot_score, score_text_position, 32);
			}
		}

		string this_frame_score;
		int bound = bowling_shot*0.5;
		for(int i = 0; i < 10; i++) {
			if(i < bound && frame_score[i] >= 0) {
				FTPoint frame_text_position((window_width*(0.145+(i*0.08))), window_height*0.87);
				this_frame_score = intToString(frame_score[i]);
				display_text(this_frame_score, frame_text_position, 32);
			}
		}
	}
}

void Bowling::draw_left_side_alleys() {
	if(left_alley_list == 0) {
		left_alley_list = glGenLists(1);
		glNewList(left_alley_list, GL_COMPILE);
			//draw the pins
			glPushMatrix();
				glTranslatef(0.0f, -1.0f, 0.0f);
				for (Pins *pin = pins; pin < pins+MAX_PINS; pin++)
				{
					pin->render(pin_list);
				}
			glPopMatrix();

			//draw the back wall
			glPushMatrix();
				wall->render(new_wall_texture);
			glPopMatrix();
	
			//draw the gutters
			glPushMatrix();
				left_gutter->render(gutter_texture);
				glTranslatef(10.0, 0.0, 0.0);
				left_gutter->render(gutter_texture);
			glPopMatrix();

			//draw the ground
			glPushMatrix();
				ground->render(current_floor_texture);
			glPopMatrix();
		glEndList();
	} else {
		glCallList(left_alley_list);
	}
}

void Bowling::draw_right_side_alleys() {
	if(right_alley_list == 0) {
		right_alley_list = glGenLists(1);
		glNewList(right_alley_list, GL_COMPILE);
			//draw the pins
			glPushMatrix();
				glTranslatef(0.0f, -1.0f, 0.0f);
				for (Pins *pin = pins; pin < pins+MAX_PINS; pin++)
				{
					pin->render(pin_list);
				}
			glPopMatrix();

			//draw the back wall
			glPushMatrix();
				wall->render(new_wall_texture);
			glPopMatrix();
	
			//draw the gutters
			glPushMatrix();
				right_gutter->render(gutter_texture);
				glTranslatef(-10.0, 0.0, 0.0);
				right_gutter->render(gutter_texture);
			glPopMatrix();

			//draw the ground
			glPushMatrix();
				ground->render(current_floor_texture);
			glPopMatrix();
		glEndList();
	} else {
		glCallList(right_alley_list);
	}
}

void Bowling::display_end_of_game_screen() {
	//draw the pins
	glPushMatrix();
		glTranslatef(0.0f, -1.0f, 0.0f);
		for (Pins *pin = pins; pin < pins+MAX_PINS; pin++)
		{
			if (pin->exists) {
				//make pins sleeps
				pin->render(pin_list);
			}
		}
	glPopMatrix();

	//draw the bowling alleys on the right side
	glPushMatrix();
		glTranslatef(12.0, 0.0, 0.0);
		draw_right_side_alleys();
	glPopMatrix();

	glPushMatrix();
		glTranslatef(24.0, 0.0, 0.0);
		draw_right_side_alleys();
	glPopMatrix();

	//draw the bowling alleys on the left side
	glPushMatrix();
		glTranslatef(-12.0, 0.0, 0.0);
		draw_left_side_alleys();
	glPopMatrix();

	glPushMatrix();
		glTranslatef(-24.0, 0.0, 0.0);
		draw_left_side_alleys();
	glPopMatrix();

	//draw the back wall
	glPushMatrix();
		wall->render(wall_texture);
	glPopMatrix();
	
	//draw the gutters
	glPushMatrix();
		left_gutter->render(gutter_texture);
		right_gutter->render(gutter_texture);
	glPopMatrix();

	//draw the ground
	glPushMatrix();
		ground->render(current_floor_texture);
	glPopMatrix();
	
	//draw the controls
	/*glPushMatrix();
		glTranslatef(0.0, 0.0, 25.0);
		glCallList(controls_display_list);
	glPopMatrix();*/

	//draw the score boxes
	glPushMatrix();
		glTranslatef(-10.0, 11.5, 15.0);
		draw_score();
	glPopMatrix();
	//glEnable(GL_TEXTURE_2D);
}

void Bowling::display_normal_screen() {
	//draw the pins
	glPushMatrix();
		glTranslatef(0.0f, -1.0f, 0.0f);
		for (Pins *pin = pins; pin < pins+MAX_PINS; pin++)
		{
			if (pin->exists) {
				//make pins sleeps
				pin->render(pin_list);
			}
		}
	glPopMatrix();

	//draw the ball
	if (ball_active)
	{
		//glColor3f(1.0f, 0.0f, 0.0f);
		glPushMatrix();
			cyclone::Vector3 pos = ball.body->getPosition();
			glTranslatef(pos.x, pos.y, pos.z);
			glCallList(ball_list);
		glPopMatrix();
	}

	//draw the bowling alleys on the right side
	glPushMatrix();
		glTranslatef(12.0, 0.0, 0.0);
		draw_right_side_alleys();
	glPopMatrix();

	glPushMatrix();
		glTranslatef(24.0, 0.0, 0.0);
		draw_right_side_alleys();
	glPopMatrix();

	//draw the bowling alleys on the left side
	glPushMatrix();
		glTranslatef(-12.0, 0.0, 0.0);
		draw_left_side_alleys();
	glPopMatrix();

	glPushMatrix();
		glTranslatef(-24.0, 0.0, 0.0);
		draw_left_side_alleys();
	glPopMatrix();

	//draw the back wall
	glPushMatrix();
		wall->render(wall_texture);
	glPopMatrix();
	
	//draw the gutters
	glPushMatrix();
		left_gutter->render(gutter_texture);
		right_gutter->render(gutter_texture);
	glPopMatrix();

	//draw the ground
	glPushMatrix();
		ground->render(current_floor_texture);
	glPopMatrix();

	//draw the controls
	glPushMatrix();
		glTranslatef(0.0, 1.1, 19.0);
		glCallList(controls_display_list);
	glPopMatrix();

	//draw the left direction image
	glPushMatrix();
		glTranslatef(-7.0, 6.25, 18.0);
		glCallList(left_direction_list);
	glPopMatrix();

	//draw the right direction image
	glPushMatrix();
		glTranslatef(6.5, 6.25, 18.0);
		glCallList(right_direction_list);
	glPopMatrix();

	//draw the left spin image
	glPushMatrix();
		glTranslatef(-9.5, 6.0, 15.0);
		glCallList(left_spin_list);
	glPopMatrix();

	//draw the right spin image
	glPushMatrix();
		glTranslatef(9.0, 6.0, 15.0);
		glCallList(right_spin_list);
	glPopMatrix();

	glDisable(GL_TEXTURE_2D);
	//draw the score boxes
	glPushMatrix();
		glTranslatef(-10.0, 11.5, 15.0);
		draw_score();
	glPopMatrix();

	//draw the power bar
	glPushMatrix();
		glTranslatef(-5.0, 7.5, 18.0);
		glScalef(0.25, 0.25, 0.25);
		draw_power_bar();
	glPopMatrix();

	//draw the sideways movement bar
	glPushMatrix();
		glTranslatef(0.0, 6.5, 18.0);
		glScalef(0.25, 0.25, 0.25);
		draw_direction_bar(1.0);
	glPopMatrix();

	//draw the right spin bar
	glPushMatrix();
		glTranslatef(3.5, 5.0, 15.0);
		glScalef(0.25, 0.25, 0.25);
		draw_right_spin_bar();
	glPopMatrix();

	//draw the left spin bar
	glPushMatrix();
		glTranslatef(-3.5, 5.0, 15.0);
		glScalef(0.25, 0.25, 0.25);
		draw_left_spin_bar();
	glPopMatrix();
	glEnable(GL_TEXTURE_2D);
}

void Bowling::display_shot_screen() {
	glPushMatrix();
		glEnable(GL_NORMALIZE);
		glTranslatef(0.0f, -1.0f, 0.0f);
		for (Pins *pin = pins; pin < pins+MAX_PINS; pin++)
		{
			if (pin->exists) {
				//make pins sleeps
				pin->render(pin_list);
			}
		}
		glDisable(GL_NORMALIZE);
	glPopMatrix();

	if (ball_active)
	{
		//glColor3f(1.0f, 0.0f, 0.0f);
		glPushMatrix();
			cyclone::Vector3 pos = ball.body->getPosition();
			glTranslatef(pos.x, pos.y, pos.z);
			glCallList(ball_list);
		glPopMatrix();
	}

	//draw the bowling alleys on the right side
	glPushMatrix();
		glTranslatef(12.0, 0.0, 0.0);
		draw_right_side_alleys();
	glPopMatrix();

	glPushMatrix();
		glTranslatef(24.0, 0.0, 0.0);
		draw_right_side_alleys();
	glPopMatrix();

	//draw the bowling alleys on the left side
	glPushMatrix();
		glTranslatef(-12.0, 0.0, 0.0);
		draw_left_side_alleys();
	glPopMatrix();

	glPushMatrix();
		glTranslatef(-24.0, 0.0, 0.0);
		draw_left_side_alleys();
	glPopMatrix();

	//draw the back wall
	glPushMatrix();
		glEnable(GL_NORMALIZE);
		wall->render(wall_texture);
		glDisable(GL_NORMALIZE);
	glPopMatrix();
	
	//draw the gutters
	glPushMatrix();
		glEnable(GL_NORMALIZE);
		left_gutter->render(gutter_texture);
		right_gutter->render(gutter_texture);
		glDisable(GL_NORMALIZE);
	glPopMatrix();

	//draw the ground
	glPushMatrix();
		glEnable(GL_NORMALIZE);
		ground->render(current_floor_texture);
		glDisable(GL_NORMALIZE);
	glPopMatrix();
}

bool Bowling::display_instructions() {
	bool start = false;
	int instructions, start_button;
	double delay_time = glfwGetTime();

	string instructions_image = "instructions.tga";
	string start_button_image = "lets_bowl_button.tga";

	instructions = getTextureValue(instructions_image);
	start_button = getTextureValue(start_button_image);

	glClearColor(0.0, 0.0, 0.0, 1.0);

	while(!start) {
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(75.0, aspect_ratio, 1.0, 20000.0);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(0.0, 0.0, -5.0);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glBindTexture(GL_TEXTURE_2D, instructions);
		glBegin(GL_QUADS);
			glColor3f(1.0, 1.0, 1.0);
			glTexCoord2f(0.0, 0.0);
			glVertex3f(-5.0, -3.0, 0.0);
			glTexCoord2f(0.0, 1.0);
			glVertex3f(-5.0, 3.0, 0.0);
			glTexCoord2f(1.0, 1.0);
			glVertex3f(5.0, 3.0, 0.0);
			glTexCoord2f(1.0, 0.0);
			glVertex3f(5.0, -3.0, 0.0);
		glEnd();   

		glBindTexture(GL_TEXTURE_2D, start_button);
		glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT);
		glBegin(GL_QUADS);
			glColor4f(1.0, 1.0, 0.0, 0.5);
			glTexCoord2f(0.0, 0.0); glVertex3f(-1.75, -2.5, 0.25);			
			glTexCoord2f(0.0, 1.0); glVertex3f(-1.75, -2.0, 0.25);			
			glTexCoord2f(1.0, 1.0); glVertex3f(1.75, -2.0, 0.25);
			glTexCoord2f(1.0, 0.0); glVertex3f(1.75, -2.5, 0.25);
		glEnd();
		glPopAttrib();

		glfwSwapBuffers();

		if(glfwGetKey( GLFW_KEY_ENTER ) && glfwGetTime() - delay_time >= 1.0) {
			start = true;
		}

		glDisable(GL_BLEND);
	}

	return start;
}

void Bowling::display()
{
	while(!start_game) {
		start_game = display_instructions();
	}

	glClearColor(0.0, 0.0, 0.0, 1.0);
	GLfloat light_ambient[] = { 0.8, 0.8, 0.8, 1.0 };
	GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };	
	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	const static GLfloat lightPosition[] = {0.0f, 2.0f, 2.0f, 0.0};

	wall->calculateInternals();
	left_gutter->calculateInternals();
	right_gutter->calculateInternals();
	ground->calculateInternals();

	RigidBodyApplication::display();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

	//draw view ports and call the appropriate function
	//display_normal_screen();
	//display_shot_screen();

	if(bowled == true) {
		//2nd view
  		glViewport(0,0, window_width, window_height);
		glScissor(0,0, window_width, window_height);
		glEnable(GL_SCISSOR_TEST);
		glClear( GL_DEPTH_BUFFER_BIT );

		glTranslatef(0, 0, 15.0 - (float)ball.body->getPosition().z);

		display_shot_screen();
	} else {
		//single player view
		glViewport(0,0, window_width, window_height);
		glScissor(0,0, window_width, window_height);
		glEnable(GL_SCISSOR_TEST);
		
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		display_normal_screen();
	}

	if(end_of_game == true) {
		glViewport(0,0, window_width, window_height);
		glScissor(0,0, window_width, window_height);
		glEnable(GL_SCISSOR_TEST);
		
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		display_end_of_game_screen();
	}
	
	glDisable(GL_LIGHTING);

	RigidBodyApplication::drawDebug();
}

void Bowling::key()
{
	if(glfwGetKey('G') == GLFW_PRESS && glfwGetTime() - ground_change > 0.5 && shot_reset == true) {
		ground_change = glfwGetTime();
		
		if(ground_type == 3) {
			ground_type = 1;
		} else {
			ground_type++;
		}
	}

	if(glfwGetKey(GLFW_KEY_SPACE) == GLFW_PRESS && glfwGetTime() - fire_timeout > 1.0 && shot_reset == true) {
		fire_timeout = glfwGetTime();
		bowl();
	}

	if(glfwGetKey(GLFW_KEY_RIGHT) == GLFW_PRESS && movement < 25.0f) {
		//throw ball with movement to the right
		movement += 0.5;
	} else if(glfwGetKey(GLFW_KEY_LEFT) == GLFW_PRESS && movement >= -25.0f) {
		//throw ball with movement to the left
		movement -= 0.5;
	}

	if(glfwGetKey('D') == GLFW_PRESS && spin > -20.0f) {
		//right spin
		spin -= 0.5;
	} else if(glfwGetKey('A') == GLFW_PRESS && spin < 20.0f) {
		//left spin
		spin += 0.5;
	}

	if(glfwGetKey(GLFW_KEY_UP) == GLFW_PRESS && power > -40.0f) {
		//increase power
		power -= 0.5;
	} else if(glfwGetKey(GLFW_KEY_DOWN) == GLFW_PRESS && power < 5.0f) {
		//reduce power
		power += 0.5;
	}

	RigidBodyApplication::key();
}

/**
 * Called by the common demo framework to create an application
 * object (with new) and return a pointer.
 */
Application* getApplication()
{
    return new Bowling();
}

void  GLFWCALL emptyCallBack( int key, int action ){

}

void  GLFWCALL getInputs( int key, int action ){
	static int letterCount = 0;

	if( action != GLFW_PRESS ) {
        return;
    }

	if(letterCount < 8) {
		//	glfwSetKeyCallback( keyInputs.getInputs );
		switch( key ){
			case GLFW_KEY_ESC: break;
			case 'A':keysPressed+='A';letterCount++;break;
			case 'B':keysPressed+='B';letterCount++;break;
			case 'C':keysPressed+='C';letterCount++;break;
			case 'D':keysPressed+='D';letterCount++;break;
			case 'E':keysPressed+='E';letterCount++;break;
			case 'F':keysPressed+='F';letterCount++;break;
			case 'G':keysPressed+='G';letterCount++;break;
			case 'H':keysPressed+='H';letterCount++;break;
			case 'I':keysPressed+='I';letterCount++;break;
			case 'J':keysPressed+='J';letterCount++;break;
			case 'K':keysPressed+='K';letterCount++;break;
			case 'L':keysPressed+='L';letterCount++;break;
			case 'M':keysPressed+='M';letterCount++;break;
			case 'N':keysPressed+='N';letterCount++;break;
			case 'O':keysPressed+='O';letterCount++;break;
			case 'P':keysPressed+='P';letterCount++;break;
			case 'Q':keysPressed+='Q';letterCount++;break;
			case 'R':keysPressed+='R';letterCount++;break;
			case 'S':keysPressed+='S';letterCount++;break;
			case 'T':keysPressed+='T';letterCount++;break;
			case 'U':keysPressed+='U';letterCount++;break;
			case 'V':keysPressed+='V';letterCount++;break;
			case 'W':keysPressed+='W';letterCount++;break;
			case 'X':keysPressed+='X';letterCount++;break;
			case 'Y':keysPressed+='Y';letterCount++;break;
			case 'Z':keysPressed+='Z';letterCount++;break;
			case GLFW_KEY_DEL:
			case GLFW_KEY_BACKSPACE:
				if(keysPressed.size()>0) {
					keysPressed.erase(keysPressed.size()-1);
					letterCount--;
				}
			break;
		}
	 } else{
		switch(key) {
			case GLFW_KEY_BACKSPACE:
			case GLFW_KEY_DEL:
				keysPressed.erase(keysPressed.size()-1);
				letterCount--;
		}
	}
}