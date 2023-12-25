//Header -- function declarations
#include <SFML/Graphics.hpp>
#include <string>

#ifndef INFLECTION_H_INCLUDED //https://stackoverflow.com/questions/3799478/c-ifndef-for-include-files-why-is-all-caps-used-for-the-header-file
#define INFLECTION_H_INCLUDED

class Inflection {

	public:
		Inflection();
		Inflection(float time, std::string type);
		~Inflection();
		float getTime();
		std::string getType();
		void setTime(float time);
		void setType(std::string type);
		friend bool operator<(const Inflection& lhs, const Inflection& rhs);

	private:
		float time;
		std::string type;
};

bool operator<(const Inflection& lhs, const Inflection& rhs);


#endif
