/*
2017 Timedog


Learning resources used:
https://www.youtube.com/watch?v=hnjhCFA4GnM -- SFML VertexArray tutorial 1
https://www.youtube.com/watch?v=5GkcGw09D8w -- SFML VertexArray tutorial 2
https://en.sfml-dev.org/forums/index.php?topic=11577.0 -- SFML simple paint program
https://www.khanacademy.org/computing/computer-science/cryptography/modarithmetic/a/the-euclidean-algorithm --basic Euclidean algorithm

*/

#include <SFML/Graphics.hpp>
#include <math.h>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include "Arm.h"

using std::cout;
using std::endl;

#define PI 3.14159265359


void GetUserInput(std::vector<Arm> &Arms, int &numArms, std::string &colorAlgo);
float GetSecsToRepeat(int numArms, std::vector<Arm> arms, std::vector<float> armSpeeds);
void InitializeLineStrip(sf::Vector2i screenDimensions, sf::VertexArray &lines, std::vector<Arm> &arms, sf::RenderWindow &window);
void CreateLineStrip(sf::VertexArray &lines, int numArms, std::vector<Arm> arms, float timeRunning);
void ColorAlgorithm(std::vector<sf::Vertex> &Vlines, std::string algoName, float timeRunning, float repeatSecs);
void setBgrdColor(int &bgColorScheme, sf::Color &bgColor, float timeRunning);
void setColorAlgo(std::string &colorAlgo);

float EuclideanAlgo(float num, float denom);
float GCD(std::vector<float> numbers);


void main()
{
	sf::Vector2i screenDimensions(1200, 900);
	sf::Vector2f origin(screenDimensions.x / 2.0f, screenDimensions.y / 2.0f);

	sf::RenderWindow window;
	window.create(sf::VideoMode(screenDimensions.x, screenDimensions.y), "SpiroGreg");
	//int framerate = 60;
	//window.setFramerateLimit(framerate);

	int numArms, screenshotNum = 1;
	int bgColorScheme = 0;
	sf::Color bgColor(0,0,0);
	std::string colorAlgo = "White";
	float secsToRepeat;		//seconds it takes to draw entire spirograph pattern, pattern will repeat after this if not stopped
	std::vector<Arm> arms;	//# rotating arms that make up graph
	std::vector<float> armSpeeds;
	std::vector<sf::Vertex> Vlines;

	GetUserInput(arms, numArms, colorAlgo);
		//For arms of spirograph -- linestrips are lines where the end vertex of a line is the starting vertex of the next line
	sf::VertexArray lines(sf::LinesStrip, numArms + 1);
	bool showLines = true;
	bool takeScreenShot = false;

	secsToRepeat = GetSecsToRepeat(numArms, arms, armSpeeds);
	cout << "Seconds before repeat = " << secsToRepeat << endl;
	InitializeLineStrip(screenDimensions, lines, arms, window); //creates first arm of spirograph
	//window.display();////////////////////////////////////////////////////////////////////////////////////////////////////////////

	sf::Clock clock, refreshClock;
	sf::Time timeRunning, refreshTime;

	while (window.isOpen()){
		sf::Event Event;

		//Event loop
		while (window.pollEvent(Event))
		{
			switch (Event.type){
			case sf::Event::Closed:
				window.close();
				break;
			case sf::Event::KeyPressed:
				if (Event.key.code == sf::Keyboard::Escape) {
					window.close();
				}
				else if (Event.key.code == sf::Keyboard::B) {
					bgColorScheme++;
				}
				else if (Event.key.code == sf::Keyboard::Down) {
					setColorAlgo(colorAlgo);
				}
				else if (Event.key.code == sf::Keyboard::L) {
					if (showLines) {
						showLines = false;
					}
					else {
						showLines = true;
					}
				}
				else if (Event.key.code == sf::Keyboard::P) {
					takeScreenShot = true;
				}
				break;
			}//end switch
		}//end Event Loop


		//sets initial position of the endpoint of the first arm
		lines[1].position = sf::Vector2f(origin.x + (arms[0].getRadius() * (float)std::cos(timeRunning.asSeconds() * arms[0].getAngularV_Rad() - (PI / 2))),
			origin.y + (arms[0].getRadius() * (float)std::sin(timeRunning.asSeconds() * arms[0].getAngularV_Rad() - (PI / 2))));
		lines[1].color = sf::Color::White;

		CreateLineStrip(lines, numArms, arms, timeRunning.asSeconds());					//creates arms of spirograph from user's input data

		//if (refreshTime.asMilliseconds() > 5.0f) {
			if ( secsToRepeat > (timeRunning.asSeconds() - .1f) ){
				Vlines.push_back(sf::Vertex(sf::Vector2f(lines[numArms].position)));	//creates new vertices (to be colored)
				ColorAlgorithm( Vlines, colorAlgo, timeRunning.asMilliseconds(), secsToRepeat );						//defines spirograph color scheme
				refreshClock.restart();
			} else {
				showLines = false;
			}

		//}


		if ( Vlines.size() )
			window.draw(&Vlines[0], Vlines.size(), sf::LinesStrip); //draws colored strip
		if ( showLines )
			window.draw(lines);										//draws arms
		if ( takeScreenShot ) {
			sf::Vector2u windowSize = window.getSize();
			sf::Texture texture;
			texture.create(windowSize.x, windowSize.y);
			texture.update(window);
			sf::Image screenshot = texture.copyToImage();
			//screenshot = window.capture(); //capture() is deprecated
			std::stringstream filename;
			filename << "Spirograph_" << screenshotNum << ".png";
			screenshot.saveToFile(filename.str());
			takeScreenShot = false;
		}
		window.display();

		setBgrdColor(bgColorScheme, bgColor, timeRunning.asMilliseconds());
		window.clear(bgColor);								//clears back buffer 

		refreshTime += refreshClock.getElapsedTime();
		timeRunning += clock.getElapsedTime();
		clock.restart();
	}

}//end main





		//get arm #, radii, and arm speeds from user
void GetUserInput(std::vector<Arm> &arms, int &numArms, std::string &colorAlgo)
{
	Arm* tempArm;
	bool correct = false;
	float tempSpeed = 0, tempRadius = 0, colorScheme = 0;

	while (!correct){
		cout << "Enter number of rotating arms (1-4): ";
		if (!(std::cin >> numArms) || numArms > 4 || numArms < 1)
			cout << "incorrect input! Try again..." << endl;
		else
			correct = true;
	}

	correct = false;

	for (int i = 0; i < numArms; i++){
		while (!correct){	
			cout << "Enter RADIUS for arm #" << i + 1 << " (5 - 200): ";
			if (!(std::cin >> tempRadius) || tempRadius < 5 || tempRadius > 200)
				cout << "incorrect input! Try again..." << endl << endl;
			else
				correct = true;
		}

		correct = false;

		while (!correct){
			cout << "Enter SPEED for arm #" << i + 1 << "(-1080 - 1080): ";
			if (!(std::cin >> tempSpeed) || tempSpeed < -1080 || tempSpeed > 1080)
				cout << "incorrect input! Try again..." << endl << endl;
			else
				correct = true;
		}

		tempArm = new Arm(tempRadius, tempSpeed);
		arms.push_back(*tempArm);
		delete tempArm;
		correct = false;
		cout << endl;
	}

	cout << "White             -- 1" << endl;
	cout << "Fire Gradient     -- 2" << endl;
	cout << "Fuschia Gradient  -- 3" << endl;
	cout << "Rainbow Gradient  -- 4" << endl;
	cout << "Rainbow Discrete  -- 5" << endl;
	cout << "Confetti          -- 6" << endl;
	cout << "Invisible         -- 7" << endl;

	while(!correct){
		cout << "Enter COLOR SCHEME number (1 - 7): ";
		if (!(std::cin >> colorScheme) || colorScheme < 1 || colorScheme > 7)
			cout << "incorrect input! Try again..." << endl;
		else
			correct = true;
	}

	if (colorScheme == 1)
		colorAlgo = "White";
	else if (colorScheme == 2)
		colorAlgo = "Fire Gradient";
	else if (colorScheme == 3)
		colorAlgo = "Fuschia Gradient";
	else if (colorScheme == 4)
		colorAlgo = "Rainbow Gradient";
	else if (colorScheme == 5)
		colorAlgo = "Rainbow Discrete";
	else if (colorScheme == 6)
		colorAlgo = "Confetti";
	else if (colorScheme == 7)
		colorAlgo = "Invisible";
}



float GetSecsToRepeat(int numArms, std::vector<Arm> arms, std::vector<float> armSpeeds)
{
	for (int i = 0; i < numArms; i++)
		armSpeeds.push_back(arms[i].getAngularV_Deg());

	if (armSpeeds.size() > 1)
		return (360 / GCD(armSpeeds));
	else
		return (abs(360 / armSpeeds[0])); //must use abs() because neg. speed will otherwise cause neg. # of seconds
}



void InitializeLineStrip(sf::Vector2i screenDimensions, sf::VertexArray &lines, std::vector<Arm> &arms, sf::RenderWindow &window)
{
	lines[0].position = sf::Vector2f(screenDimensions.x / 2.0f, screenDimensions.y / 2.0f);
	lines[0].color = sf::Color::White;

	lines[1].position = sf::Vector2f(screenDimensions.x / 2.0f, screenDimensions.y / 2.0f - arms[0].getRadius());
	lines[1].color = sf::Color::White;

	window.draw(lines);
	window.display();
}



void CreateLineStrip(sf::VertexArray &lines, int numArms, std::vector<Arm> arms, float timeRunning)
{
	for (int i = 0; i < numArms; i++){
		sf::Vector2f oldCoord = sf::Vector2f(lines[i].position.x, lines[i].position.y);
		float cosResult = (float)std::cos(arms[i].getAngularV_Rad() * timeRunning - (PI / 2));
		float sinResult = (float)std::sin(arms[i].getAngularV_Rad() * timeRunning - (PI / 2));

		lines[i + 1].position = sf::Vector2f(oldCoord.x + arms[i].getRadius() * cosResult,
			oldCoord.y + arms[i].getRadius() * sinResult);
		lines[i + 1].color = sf::Color::White;
	}
}



void ColorAlgorithm(std::vector<sf::Vertex> &Vlines, std::string algoName, float timeRunning, float repeatSecs)
{
	int percentComplete = (int)(timeRunning / repeatSecs);	//percent of pattern cyle completed 
															//out of 1000% max (not 100%)

	if (algoName.compare("Invisible") == 0){ Vlines[Vlines.size() - 1].color = sf::Color::Transparent; return; }
	if (algoName.compare("White") == 0){Vlines[Vlines.size() - 1].color = sf::Color::White; return;	}
	if (algoName.compare("Red") == 0){ Vlines[Vlines.size() - 1].color = sf::Color::Red; return; }
	if (algoName.compare("Cyan") == 0){ Vlines[Vlines.size() - 1].color = sf::Color::Cyan; return; }
	if (algoName.compare("Magenta") == 0){ Vlines[Vlines.size() - 1].color = sf::Color::Magenta; return; }
	if (algoName.compare("Green") == 0){ Vlines[Vlines.size() - 1].color = sf::Color::Green; return; }
	if (algoName.compare("Yellow") == 0){ Vlines[Vlines.size() - 1].color = sf::Color::Yellow; return; }
	if (algoName.compare("Orange") == 0){ Vlines[Vlines.size() - 1].color = sf::Color(255, 255, 0); return; }
	if (algoName.compare("Grey") == 0){ Vlines[Vlines.size() - 1].color = sf::Color(127, 127, 127); return; }


	if (algoName.compare("Fire Gradient") == 0){
		int red = 255, green = 0, blue = 0;
		repeatSecs *= 4.44;
		
		if ((percentComplete % 1000) < 500){		//red to yellow
			green = 0 + (255 *(percentComplete % 1000) / 500.0f);
			Vlines[Vlines.size() - 1].color = sf::Color(red, green, blue);
			return;
		}
		else{													//yellow to red
			green = 255 - (255 * (percentComplete % 1000 - 500) / 500.0f);
			Vlines[Vlines.size() - 1].color = sf::Color(red, green, blue);
			return;
		}
	}//end Fire Gradient


	if (algoName.compare("Fuschia Gradient") == 0){
		int red = 255, green = 0, blue = 128;



		/*********

		example:

		red = $starting_rbg_value$ +/- ( $rgb_change_amount$ * ( (percentComplete % 1000) / $interval_between_different_colors$) );
		i.e.
		red = 255 - (127 * ( (percentComplete % 1000) / 333.0f ) );		//pnk to purp

		$starting_rbg_value$	  = 255			//this should be the value that red ended at in an earlier if statement (IF one exists)
		$rgb_change_amount$		  =	127			//red drops by 127 from the starting amount of 255 over an interval of 333 (or 1/3 a pattern rotation)
		$interval_between_colors$ = 333			//This is out of a possible 1000. So 333 means the color will change from pink to purple as spirograph
		//does 1/3 of a full pattern

		Note:	the "% 1000" next to percentComplete is necessary in the if statements. It is not necessary in the body
		of the statements unless you WANT to keep on drawing colors AFTER 1 full pattern rotation. They are left
		inside the if statements for future flexibility.

		***********/


		if ((percentComplete % 1000) < 333){		//pnk to purp
			red = 255 - (127 * ( (percentComplete % 1000) / 333.0f ) );
			Vlines[Vlines.size() - 1].color = sf::Color(red, green, blue);

			//if (red < 129)
				//cout << "red =" << red << endl;
			return;
		}
		else if ((percentComplete % 1000) < 666){	//purp to blue
			red = 128 - (28 * ((percentComplete % 1000 - 333) / 333.0f));
			blue = 128 + (92 * ((percentComplete % 1000 - 333) / 333.0f));
			green = 0 + (50 * ((percentComplete % 1000 - 333) / 333.0f));
			Vlines[Vlines.size() - 1].color = sf::Color(red, green, blue);
			return;
		}
		else if ((percentComplete % 1000) < 1000){	//blue to pnk
			red = 100 + (155 * ((percentComplete % 1000 - 666) / 333.0f));
			blue = 220 - (92 * ((percentComplete % 1000 - 666) / 333.0f));
			green = 50 - (50 * ((percentComplete % 1000 - 666) / 333.0f));
			Vlines[Vlines.size() - 1].color = sf::Color(red, green, blue);
		}
	}//end Fuschia Gradient


	if (algoName.compare("Rainbow Gradient") == 0){
		int red = 0, green = 0, blue = 0;
		repeatSecs *= 1.25;	//larger value makes gradient fade between colors slower

		if ((Vlines.size() / (int)repeatSecs) % 8 == 0){				//red to orng
			red = 255;
			green = 0 + (165 * (Vlines.size() % (int)repeatSecs) / repeatSecs);
			Vlines[Vlines.size() - 1].color = sf::Color(red, green, blue);
			return;
		}
		else if ((Vlines.size() / (int)repeatSecs) % 8 == 1){			//orng to yell	
			red = 255;
			green = 165 + (90 * (Vlines.size() % (int)repeatSecs) / repeatSecs);
			Vlines[Vlines.size() - 1].color = sf::Color(red, green, blue);
			return;
		}
		else if ((Vlines.size() / (int)repeatSecs) % 8 == 2){			//yell to grn
			red = 255 - (255 * (Vlines.size() % (int)repeatSecs) / repeatSecs);
			green = 255;
			Vlines[Vlines.size() - 1].color = sf::Color(red, green, blue);
			return;
		}
		else if ((Vlines.size() / (int)repeatSecs) % 8 == 3){			//grn to blu
			green = 255; 
			blue = 0 + (255 * (Vlines.size() % (int)repeatSecs) / repeatSecs);
			Vlines[Vlines.size() - 1].color = sf::Color(red, green, blue);
			return;
		}
		else if ((Vlines.size() / (int)repeatSecs) % 8 == 4){			//blu to indi
			blue = 255;
			green = 255 - (155 * (Vlines.size() % (int)repeatSecs) / repeatSecs);
			red = 0 + (190 * (Vlines.size() % (int)repeatSecs) / repeatSecs);
			Vlines[Vlines.size() - 1].color = sf::Color(red, green, blue);
			return;
		}
		else if ((Vlines.size() / (int)repeatSecs) % 8 == 5){			//indi to viol
			red = 190 + (10 * (Vlines.size() % (int)repeatSecs) / repeatSecs);
			green = 100 - (70 * (Vlines.size() % (int)repeatSecs) / repeatSecs);
			blue = 255 -(25 * (Vlines.size() % (int)repeatSecs) / repeatSecs);
			Vlines[Vlines.size() - 1].color = sf::Color(red, green, blue);
			return;
		}
		else if ((Vlines.size() / (int)repeatSecs) % 8 == 6){			//viol to mag
			red = 200 + (55 * (Vlines.size() % (int)repeatSecs) / repeatSecs);
			blue = 230 + (25 * (Vlines.size() % (int)repeatSecs) / repeatSecs);
			green = 30 - (30 * (Vlines.size() % (int)repeatSecs) / repeatSecs);
			Vlines[Vlines.size() - 1].color = sf::Color(red, green, blue);
			return;
		}
		else if ((Vlines.size() / (int)repeatSecs) % 8 == 7){			//mag to red
		
			blue = 255 - (255 * (Vlines.size() % (int)repeatSecs) / repeatSecs);
			red = 255;
			Vlines[Vlines.size() - 1].color = sf::Color(red, green, blue);
			return;
		}
	}//end Rainbow Gradient


	if (algoName.compare("Rainbow Discrete") == 0){

		int vLength = ((Vlines.size() - 1) / (int)repeatSecs) % 160;

		if (vLength < 20)
			Vlines[Vlines.size() - 1].color = sf::Color::Red;
		else if (vLength >= 20 && vLength < 40)
			Vlines[Vlines.size() - 1].color = sf::Color(255, 165, 0);
		else if (vLength >= 40 && vLength < 60)
			Vlines[Vlines.size() - 1].color = sf::Color::Yellow;
		else if (vLength >= 60 && vLength < 80)
			Vlines[Vlines.size() - 1].color = sf::Color::Green;
		else if (vLength >= 80 && vLength < 100)
			Vlines[Vlines.size() - 1].color = sf::Color::Blue;
		else if (vLength >= 100 && vLength < 120)
			Vlines[Vlines.size() - 1].color = sf::Color(75, 0, 130);
		else if (vLength >= 120 && vLength < 140)
			Vlines[Vlines.size() - 1].color = sf::Color(128, 0, 128);
		else if (vLength >= 140 && vLength < 160)
			Vlines[Vlines.size() - 1].color = sf::Color::Magenta;
		return;
	}//end Rainbow Discrete



	if (algoName.compare("Confetti") == 0){
		int red = rand() & 250 + 5;
		int green = rand() % 250 + 5;
		int blue = rand() % 250 + 5;

		Vlines[Vlines.size() - 1].color = sf::Color(red, green, blue);
	}
}//end Color Algo



		//sets background color behind spirograph
void setBgrdColor(int &bgColorScheme, sf::Color &color, float timeRunning)
{
	if (bgColorScheme % 4 == 0)				//black
		color = sf::Color(0, 0, 0);
	else if (bgColorScheme % 4 == 1)		//dark grey
		color = sf::Color(25, 25, 25);
	else if (bgColorScheme % 4 == 2)		//grey
		color = sf::Color(50, 50, 50);
	else if (((int)timeRunning % 65 == 0))		//rainbow flash
	{
		int red = rand() & 60;
		int green = rand() % 60;
		int blue = rand() % 60;

		color = sf::Color(red, green, blue);
	}

}//end BgrdColor


		//placeholder til you can design a pop-up menu for user to choose color
void setColorAlgo(std::string &colorAlgo)
{
	if (colorAlgo == "White")
		colorAlgo = "Fire Gradient";
	else if (colorAlgo == "Fire Gradient")
	colorAlgo = "Fuschia Gradient";
	else if (colorAlgo == "Fuschia Gradient")
		colorAlgo = "Rainbow Gradient";
	else if (colorAlgo == "Rainbow Gradient")
		colorAlgo = "Rainbow Discrete";
	else if (colorAlgo == "Rainbow Discrete")
		colorAlgo = "Confetti";
	else if (colorAlgo == "Confetti")
		colorAlgo = "White";
	else
		colorAlgo = "Green";
}



		//Euclidean algorithm. Requires positive numbers to work. abs() workaround explained below...
float EuclideanAlgo(float num, float denom){

	//a = b*q + r
	float a(abs(num));			//abs() won't make this *particular* program incorrect, - and + speed arms end at same starting
	float d(abs(denom));		//point. Need abs() for negative speed values to work in algo, may be incorrect if used in other apps??
	float q=0, r;

	if (a == 0)
		return d;
	else if (d == 0)
		return a;
	else{
		while (a >= d){
			a -= d;
			q++;
		}
		r = a;
		return EuclideanAlgo(d, r);
	}
}


		//finds Greatest Common Denominator of 2 or more numbers
float GCD(std::vector<float> armSpeeds) 
{
	float gcd = armSpeeds[0]; 

	for (int i = 1; i < armSpeeds.size(); i++)
		gcd = EuclideanAlgo(gcd, armSpeeds[i]);

	return gcd; 
}