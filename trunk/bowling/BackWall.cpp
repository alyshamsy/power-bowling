#include "BackWall.h"

BackWall::BackWall()
{
    body = new cyclone::RigidBody();

	Ka[0] = Ka[1] = Ka[2] = 0.8;
	Kd[0] = Kd[1] = Kd[2] = 0.8;
	Ks[0] = Ks[1] = Ks[2] = 0.8;
}

BackWall::~BackWall()
{
    delete body;
}

/** Draws the block. */
void BackWall::render(GLuint texture)
{
    // Get the OpenGL transformation
    GLfloat mat[16];
    body->getGLTransform(mat);

    //glColor3f(0.0f, 0.0f, 0.0f);
	if (body->getAwake()) glColor3f(1.0,0.0,1.0);
    else glColor3f(1.0,1.0,0.0);

    glPushMatrix();
		glMultMatrixf(mat);
		glScalef(halfSize.x, halfSize.y, halfSize.z);
		
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, (GLfloat*) &Ka);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, (GLfloat*) &Kd);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, (GLfloat*) &Ks);
		
		glBindTexture(GL_TEXTURE_2D, texture);
		glBegin(GL_QUADS);
		{
			glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, 0.0f);   //A
			glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, 0.0f, 0.0f);  //B
			glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, 0.0f, 0.0f); //C
			glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, 0.0f);  //D

			glVertex3f(1.0f, 1.0f, 0.0f);     //A
			glVertex3f(1.0f, 1.0f, -2.0f);  //F
			glVertex3f(1.0f, 0.0f, -2.0f); //G
			glVertex3f(1.0f, 0.0f, 0.0f);    //B
		
			glVertex3f(-1.0f, 1.0f, -2.0f);  //E
			glVertex3f(1.0f, 1.0f, -2.0f);   //F
			glVertex3f(1.0f, 0.0f, -2.0f);  //G
			glVertex3f(-1.0f, 0.0f, -2.0f); //H
		
			glVertex3f(-1.0f, 1.0f, 0.0f);     //D
			glVertex3f(-1.0f, 1.0f, -2.0f);  //E
			glVertex3f(-1.0f, 0.0f, -2.0f); //H
			glVertex3f(-1.0f, 0.0f, 0.0f);    //C
		
			glVertex3f(-1.0f, 0.0f, -2.0f); //H
			glVertex3f(1.0f, 0.0f, -2.0f);  //G
			glVertex3f(1.0f, 0.0f, 0.0f);     //B
			glVertex3f(-1.0f, 0.0f, 0.0f);    //C
		
			glVertex3f(-1.0f, 1.0f, -2.0f);  //E
			glVertex3f(1.0f, 1.0f, -2.0f);   //F
			glVertex3f(1.0f, 1.0f, 0.0f);      //A		
			glVertex3f(-1.0f, 1.0f, 0.0f);     //D
		}
		glEnd();
    glPopMatrix();
}

/** Sets the block to a specific location. */
void BackWall::setState(const cyclone::Vector3 &position, 
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
void BackWall::calculateMassProperties(cyclone::real invDensity)
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