//Header -- function declarations
#include <SFML/Graphics.hpp>

#ifndef ARM_H_INCLUDED //https://stackoverflow.com/questions/3799478/c-ifndef-for-include-files-why-is-all-caps-used-for-the-header-file
#define ARM_H_INCLUDED

class Arm{

	public:
		Arm();
		Arm(float rad, float spd);
		~Arm();
		float GetRadius();
		float GetAngularV_Rad();
		float GetAngularV_Deg();
		void SetRadius(float rad);
		void SetAngularV_Rad(float spd);
		void SetAngularV_Deg(float spd);

	private:
		float radius;
		float angVelocRad;
		float angVelocDeg;

};

#endif //needed to terminate the #ifndef