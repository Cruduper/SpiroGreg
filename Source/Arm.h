//Header -- function declarations
#include <SFML/Graphics.hpp>

#ifndef ARM_H_INCLUDED //https://stackoverflow.com/questions/3799478/c-ifndef-for-include-files-why-is-all-caps-used-for-the-header-file
#define ARM_H_INCLUDED

class Arm{

	public:
		Arm();
		Arm(float rad, float spd);
		~Arm();
		float getRadius();
		float getAngularV_Rad();
		float getAngularV_Deg();
		void setRadius(float rad);
		void setAngularV_Rad(float spd);
		void setAngularV_Deg(float spd);

	private:
		float radius;
		float angVelocRad;
		float angVelocDeg;

};

#endif //needed to terminate the #ifndef