#ifndef Pins_H
#define Pins_H

#include <gl/glfw.h>
#include "BackWall.h"
#include <iostream>
#include <stdio.h>

class Pins : public cyclone::CollisionBox
{
public:
    bool exists;
	bool collide;

	Pins();
	~Pins();
	
	void render(GLuint& pin_list);
	void setState(const cyclone::Vector3 &position, const cyclone::Quaternion &orientation, const cyclone::Vector3 &extents, const cyclone::Vector3 &velocity);
	void calculateMassProperties(cyclone::real invDensity);
};

#endif