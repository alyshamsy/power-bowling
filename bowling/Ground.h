#ifndef Ground_H
#define Ground_H

#include <gl/glfw.h>
#include <cyclone/cyclone.h>
#include "app.h"
#include "timing.h"
#include <iostream>
#include <stdio.h>

class Ground : public cyclone::CollisionBox
{
public:
    Ground();
	~Ground();

	void render(GLuint texture);
	void setState(const cyclone::Vector3 &position, const cyclone::Quaternion &orientation, const cyclone::Vector3 &extents, const cyclone::Vector3 &velocity);
	void calculateMassProperties(cyclone::real invDensity);

};

#endif