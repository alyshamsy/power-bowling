#ifndef Brick_Wall_H
#define Brick_Wall_H

#include <gl/glfw.h>
#include <cyclone/cyclone.h>
#include "app.h"
#include "timing.h"
#include <iostream>
#include <stdio.h>

#define number_of_colors 3

class BackWall : public cyclone::CollisionBox
{
public:
	bool isOverlapping;

    BackWall();
	~BackWall();

	void render(GLuint texture);
	void setState(const cyclone::Vector3 &position, const cyclone::Quaternion &orientation, const cyclone::Vector3 &extents, const cyclone::Vector3 &velocity);
	void calculateMassProperties(cyclone::real invDensity);

private:
	GLfloat Ka[number_of_colors];
	GLfloat Kd[number_of_colors];
	GLfloat Ks[number_of_colors];
};

#endif