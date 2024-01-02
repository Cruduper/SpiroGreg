#include "Inflection.h"


Inflection::Inflection()
{
	time = 0.0f;
	type = "";

}

Inflection::Inflection(float time, std::string type)
{
	setTime(time);
	setType(type);
}

Inflection::~Inflection()
{

}

float Inflection::getTime()
{
	return time;
}

std::string Inflection::getType()
{
	return type;
}

void Inflection::setTime(float t)
{
	time = t;
}

void Inflection::setType(std::string inflectionType) //types are "coincident" or "reverseCoincident"
{
	type = inflectionType;
}

bool operator<(const Inflection &lhs, const Inflection &rhs) 
{
	return lhs.time < rhs.time;
}