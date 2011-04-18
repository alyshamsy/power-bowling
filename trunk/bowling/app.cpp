/*
 * The definition file for the default application object.
 * 
 * Part of the Cyclone physics system.
 * 
 * Copyright (c) Icosagon 2003. All Rights Reserved.
 *
 * This software is distributed under licence. Use of this software
 * implies agreement with all terms and conditions of the accompanying
 * software licence.
 */
#include <cstring>
#include <gl/glfw.h>
#include "app.h"
#include "timing.h"

void Application::initGraphics() 
{
    glClearColor(0.9f, 0.95f, 1.0f, 1.0f);
	glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);

    setView();
}

void Application::setView()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (double)width/(double)height, 1.0, 500.0);
    glMatrixMode(GL_MODELVIEW);
}

void Application::display() 
{
    glClear(GL_COLOR_BUFFER_BIT);  

    glBegin(GL_LINES);
    glVertex2i(1, 1);
    glVertex2i(639, 319);
    glEnd();
}

const char* Application::getTitle()
{
    return "Cyclone GLFW Demo"; 
}

void Application::deinit()
{
}

int Application::update()
{
	return 1;
}

void Application::key()
{
}

int Application::getTextureValue(string& texture_name) {
	return 0;
}


MassAggregateApplication::MassAggregateApplication(unsigned int particleCount)
:
world(particleCount*10)
{
	particleArray = new cyclone::Particle[particleCount];
	for (unsigned i = 0; i < particleCount; i++)
	{
		world.getParticles().push_back(particleArray + i);
	}

	groundContactGenerator.init(&world.getParticles());
	world.getContactGenerators().push_back(&groundContactGenerator);
}

MassAggregateApplication::~MassAggregateApplication()
{
	delete[] particleArray;
}

void MassAggregateApplication::initGraphics()
{
	// Call the superclass 
	Application::initGraphics();
}

void MassAggregateApplication::display()
{
	// Clear the view port and set the camera direction
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	//gluLookAt(0.0, 3.5, 8.0,  0.0, 3.5, 0.0,  0.0, 1.0, 0.0);

	glColor3f(0,0,0);

	GLUquadric* sphere_quad;
	sphere_quad = gluNewQuadric();
	
	cyclone::ParticleWorld::Particles &particles = world.getParticles();
	for (cyclone::ParticleWorld::Particles::iterator p = particles.begin();
		p != particles.end();
		p++)
	{
		cyclone::Particle *particle = *p;
		cyclone::Vector3 &pos = particle->getPosition();
		glPushMatrix();
		glTranslatef(pos.x, pos.y, pos.z);
		gluSphere(sphere_quad, 0.1f, 20, 10);
		glPopMatrix();
	}

	gluDeleteQuadric(sphere_quad);
}

int MassAggregateApplication::update()
{
	// Clear accumulators
	world.startFrame();

	// Find the duration of the last frame in seconds
	float duration = (float)TimingData::get().lastFrameDuration * 0.001f;
	if (duration <= 0.0f) return 0;

	// Run the simulation
	world.runPhysics(duration);

	return Application::update();
}

RigidBodyApplication::RigidBodyApplication()
:
    theta(0.0f), 
    phi(15.0f),
    resolver(maxContacts*8),

    renderDebugInfo(false),
    pauseSimulation(true),
    autoPauseSimulation(false)
{
    cData.contactArray = contacts;
}

int RigidBodyApplication::update()
{
    // Find the duration of the last frame in seconds
    float duration = (float)TimingData::get().lastFrameDuration * 0.001f;
    if (duration <= 0.0f) return 0;
    else if (duration > 0.05f) duration = 0.05f;

    // Exit immediately if we aren't running the simulation
    if (pauseSimulation) 
    {
        return Application::update();
    }
    else if (autoPauseSimulation)
    {
        pauseSimulation = true;
        autoPauseSimulation = false;
    }

    // Update the objects
    updateObjects(duration);

	// Perform the contact generation
	generateContacts();

    // Resolve detected contacts
    resolver.resolveContacts(
        cData.contactArray, 
        cData.contactCount, 
        duration
        );

    return Application::update();
}

void RigidBodyApplication::display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
	/*
    gluLookAt(18.0f, 0, 0,  0, 0, 0,  0, 1.0f, 0);
    glRotatef(-phi, 0, 0, 1);
    glRotatef(theta, 0, 1, 0);
    glTranslatef(0, -5.0f, -5.0);
	*/
	glTranslatef(0, -5.0f, -25.0);
}

void RigidBodyApplication::drawDebug()
{
	if (!renderDebugInfo) return;

    // Recalculate the contacts, so they are current (in case we're
    // paused, for example).
    generateContacts();

    // Render the contacts, if required
    glBegin(GL_LINES);
    for (unsigned i = 0; i < cData.contactCount; i++)
    {
		// Interbody contacts are in green, floor contacts are red.
        if (contacts[i].body[1]) {
            glColor3f(0,1,0);
        } else {
            glColor3f(1,0,0);
        }

        cyclone::Vector3 vec = contacts[i].contactPoint;
        glVertex3f(vec.x, vec.y, vec.z);

        vec += contacts[i].contactNormal;
        glVertex3f(vec.x, vec.y, vec.z);
    }

    glEnd();
}

void RigidBodyApplication::key()
{
	/*
    switch(key)
    {
    case 'R': case 'r':
        // Reset the simulation
        reset();
        return;

    case 'C': case 'c':
        // Toggle rendering of contacts
        renderDebugInfo = !renderDebugInfo;
        return;

    case 'P': case 'p':
        // Toggle running the simulation
        pauseSimulation = !pauseSimulation;
        return;

    case ' ':
        // Advance one frame
        autoPauseSimulation = true;
        pauseSimulation = false;
    }
	*/

	autoPauseSimulation = true;
    pauseSimulation = false;

	if(glfwGetKey('R') == GLFW_PRESS) {
		reset();
        return;
	}

    Application::key();
}