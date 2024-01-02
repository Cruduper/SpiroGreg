#include "Util.h"
#include <iostream>
//#include <vector>

using std::cout;
using std::endl;
#define PI 3.14159265359



float Util::EuclideanAlgo(float num, float denom)
{
	//a = b*q + r
	float a(abs(num));			//abs() won't make this *particular* program incorrect, - and + speed arms end at same starting
	float d(abs(denom));		//point. Need abs() for negative speed values to work in algo, may be incorrect if used in other apps??
	float q = 0;
	float r;

	if (a == 0)
		return d;
	else if (d == 0)
		return a;
	else {
		while (a >= d) {
			a -= d;
			q++;
		}
		r = a;
		return EuclideanAlgo(d, r);
	}
}


//finds Greatest Common Denominator of 2 or more ints in a vector
float Util::GCD(std::vector<float> armSpeeds)
{
	float gcd = armSpeeds[0];

	for (int i = 1; i < armSpeeds.size(); i++) {
		gcd = EuclideanAlgo(gcd, armSpeeds[i]);
	}

	cout << "GCD = " << gcd << endl << endl;
	return gcd;
}


int Util::GCD(int a, int b)
{
	if (b == 0)
		return a;
	return GCD(b, a % b);
}


int Util::LCM(std::vector<int> numbers)
{
	int lcm = numbers[0];
	int n = numbers.size();

	for (int i = 1; i < n; i++)
	{
		lcm = (lcm * numbers[i]) / GCD(lcm, numbers[i]);
	}
	cout << "LCM = " << lcm << endl << endl;
	return lcm;
}

float Util::RoundToXDecimals(float num, int x)
{
	float multiplier = std::pow(10.0, x);
	return std::round(num * multiplier) / multiplier;
}


void Util::PauseForXMilliseconds(sf::Clock& clock, int x)
{
	sf::Time startTime = clock.getElapsedTime();

	for (int i = 0; (clock.getElapsedTime() - startTime) < sf::milliseconds(x); )
	{
		i++;
	}
}


float Util::DegToRad(float deg)
{
	float rad = deg * (PI / 180.0);
	return rad;
}


std::string Util::VectorFloatToString(std::vector<float> vec)
{
	std::string rtnStr = "";
	bool first = true;

	for (float num : vec) {
		if (!first) {
			rtnStr += ", ";
		}
		else {
			first = false;
		}
		rtnStr += std::to_string(num);
	}

	return rtnStr;
}


void Util::DebugLog(std::string input, std::string titleText)
{
	if (Util::isDebugLogOn)
	{
		if (input.compare("/start") == 0) {
			std::cout << endl << "*************** " + titleText + " ***************" << endl << endl;
		}
		else if (input.compare("/end") == 0) {
			cout << endl << endl << "*****************************************" << endl;
		}
		else {
			cout << input << endl;
		}
	}
}