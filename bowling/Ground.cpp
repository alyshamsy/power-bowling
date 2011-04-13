#include "Ground.h"

Ground::Ground()
{
    body = new cyclone::RigidBody();
}

Ground::~Ground()
{
    delete body;
}

/** Draws the block. */
void Ground::render(GLuint texture)
{
    // Get the OpenGL transformation
    GLfloat mat[16];
    body->getGLTransform(mat);

    //glColor3f(0.6f, 0.5f, 0.2f);
	//if (body->getAwake()) glColor3f(1.0,0.0,1.0);
    //else glColor3f(1.0,1.0,0.0);

    glPushMatrix();
		glMultMatrixf(mat);
		glScalef(halfSize.x, halfSize.y, halfSize.z);
		//glutSolidCube(1.0f);
		glBindTexture(GL_TEXTURE_2D, texture);
		glBegin(GL_QUADS);
		{
			glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 0.0f, -1.0f);   //A
			glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, 0.0f, 0.0f);  //B
			glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, 0.0f, 0.0f); //C
			glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 0.0f, -1.0f);  //D
		}
		glEnd();
    glPopMatrix();
}

/** Sets the block to a specific location. */
void Ground::setState(const cyclone::Vector3 &position, 
                const cyclone::Quaternion &orientation, 
                const cyclone::Vector3 &extents,
                const cyclone::Vector3 &velocity)
{
    body->setPosition(position);
    body->setOrientation(orientation);
    body->setVelocity(velocity);
    body->setRotation(cyclone::Vector3(0,0,0));
    halfSize = extents;

    cyclone::real mass = 150.0f;
    body->setMass(mass);

    cyclone::Matrix3 tensor;
    tensor.setBlockInertiaTensor(halfSize, mass);
    body->setInertiaTensor(tensor);

    body->setLinearDamping(0.95f);
    body->setAngularDamping(0.8f);
    body->clearAccumulators();
    body->setAcceleration(0,-10.0f,0);

    //body->setCanSleep(false);
    body->setAwake();

    body->calculateDerivedData();
}

///>CalculateBlockTensor
/**
    * Calculates and sets the mass and inertia tensor of this block,
    * assuming it has the given constant density.
    */
void Ground::calculateMassProperties(cyclone::real invDensity)
{
    // Check for infinite mass
    if (invDensity <= 0)
    {
        // Just set zeros for both mass and inertia tensor
        body->setInverseMass(0);
        body->setInverseInertiaTensor(cyclone::Matrix3());
    }
    else
    {
        // Otherwise we need to calculate the mass
        cyclone::real volume = halfSize.magnitude() * 2.0;
        cyclone::real mass = volume / invDensity;

        body->setMass(mass);

        // And calculate the inertia tensor from the mass and size
        mass *= 0.333f;
        cyclone::Matrix3 tensor;
        tensor.setInertiaTensorCoeffs(
            mass * halfSize.y*halfSize.y + halfSize.z*halfSize.z,
            mass * halfSize.y*halfSize.x + halfSize.z*halfSize.z,
            mass * halfSize.y*halfSize.x + halfSize.z*halfSize.y
            );
        body->setInertiaTensor(tensor);
    }

}
///<CalculateBlockTensor