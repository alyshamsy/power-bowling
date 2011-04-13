#include "Pins.h"

Pins::Pins():exists(false), collide(false)
{
    body = new cyclone::RigidBody();
}

Pins::~Pins()
{
    delete body;
}

/** Draws the block. */
void Pins::render(GLuint& pin_list)
{
    // Get the OpenGL transformation
    GLfloat mat[16];
    body->getGLTransform(mat);

    /*if (body->getAwake()) glColor3f(1.0f, 1.0f, 1.0f);
    else glColor3f(0.7,0.7,1.0);*/

    glPushMatrix();
		glMultMatrixf(mat);
		glScalef(halfSize.x, halfSize.y, halfSize.z);
		glCallList(pin_list);
    glPopMatrix();
}

/** Sets the block to a specific location. */
void Pins::setState(const cyclone::Vector3 &position, 
                const cyclone::Quaternion &orientation, 
                const cyclone::Vector3 &extents,
                const cyclone::Vector3 &velocity)
{
    body->setPosition(position);
    body->setOrientation(orientation);
    body->setVelocity(velocity);
    body->setRotation(cyclone::Vector3(0,0,0));
    halfSize = extents;

    cyclone::real mass = halfSize.x * halfSize.y * halfSize.z * 8.0f;
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
void Pins::calculateMassProperties(cyclone::real invDensity)
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