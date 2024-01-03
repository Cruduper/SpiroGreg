
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
	SetRadius(rad);
	SetAngularV_Rad(spd);
	SetAngularV_Deg(spd);
}

Arm::~Arm()
{

}

float Arm::GetRadius()
{
	return radius;
}

float Arm::GetAngularV_Rad()
{
	return angVelocRad;
}

float Arm::GetAngularV_Deg()
{
	return angVelocDeg;
}

void Arm::SetRadius(float rad)
{
	radius = rad;
}

void Arm::SetAngularV_Rad(float spd)
{
	angVelocRad = spd * ((2.0f * PI) / 360);
}

void Arm::SetAngularV_Deg(float spd)
{
	angVelocDeg = spd;
}

