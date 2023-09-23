
#include "Arm.h"

#define PI 3.14159265359


Arm::Arm()
{
	radius = 0.0f;
	angVelocRad = 0.0f;
	angVelocDeg = 0.0f;
}

Arm::Arm(float rad, float spd)
{
	setRadius(rad);
	setAngularV_Rad(spd);
	setAngularV_Deg(spd);
}

Arm::~Arm()
{

}

float Arm::getRadius()
{
	return radius;
}

float Arm::getAngularV_Rad()
{
	return angVelocRad;
}

float Arm::getAngularV_Deg()
{
	return angVelocDeg;
}

void Arm::setRadius(float rad)
{
	radius = rad;
}

void Arm::setAngularV_Rad(float spd)
{
	angVelocRad = spd * ((2.0f * PI) / 360);
}

void Arm::setAngularV_Deg(float spd)
{
	angVelocDeg = spd;
}

