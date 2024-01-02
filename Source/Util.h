#include <SFML/Graphics.hpp>
//#include <string>
//#include <vector>

#ifndef UTIL_H_INCLUDED 
#define UTIL_H_INCLUDED



class Util {
	public:
			// static data members
		static bool isDebugLogOn;

			// static classes
		static float EuclideanAlgo(float num, float denom);
		static float GCD(std::vector<float> armSpeeds);
		static int GCD(int a, int b);
		static int LCM(std::vector<int> numbers);
		static float RoundToXDecimals(float num, int x);
		static void PauseForXMilliseconds(sf::Clock& clock, int x);
		static float DegToRad(float deg);
		static std::string VectorFloatToString(std::vector<float> vec);
		static void DebugLog(std::string input, std::string titleText);
	//private:
};


#endif

