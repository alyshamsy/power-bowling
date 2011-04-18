#ifndef Gutter_H
#define Gutter_H

#include <gl/glfw.h>
#include <cyclone/cyclone.h>
#include "app.h"
#include "timing.h"
#include <iostream>
#include <stdio.h>

class Gutter : public cyclone::CollisionBox
{
public:
	bool isOverlapping;

    Gutter();
	~Gutter();

	void render(GLuint texture);
	void setState(const cyclone::Vector3 &position, const cyclone::Quaternion &orientation, const cyclone::Vector3 &extents, const cyclone::Vector3 &velocity);
	void calculateMassProperties(cyclone::real invDensity);

};

#endif