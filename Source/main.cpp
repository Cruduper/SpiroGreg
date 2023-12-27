#include <SFML/Graphics.hpp>
#include <math.h>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include "Arm.h"
#include "Inflection.h"




using std::cout;
using std::endl;

#define PI 3.14159265359
//3.141592653589793

void GetUserInput(std::vector<Arm> &Arms, int &numArms, std::string &colorAlgo);
bool AskUserToRepeat();
void GetInflectionPointsSimple(std::vector<float> armSpeeds, float secsToRepeat, std::set<Inflection>& inflectionPoints);
//void GetInflectionPoints(std::vector<float> armSpeeds, float secsToRepeat, std::set<Inflection> &inflectionPoints);
//void CalculateInflections(std::set<Inflection> &inflectionPoints, std::string typeToCalculate, float armSpeedA, float armSpeedB, float secsToRepeat);
float GetSecsToRepeat(std::vector<float> armSpeeds);
std::vector<float> SetArmSpeeds(int numArms, std::vector<Arm> arms);
void InitializeLineStrip(sf::Vector2i screenDimensions, sf::VertexArray &lines, std::vector<Arm> &arms, sf::RenderWindow &window);
void CreateLineStrip(sf::VertexArray &lines, int numArms, std::vector<Arm> arms, float timeRunning);
void UpdateArms(sf::Vector2f origin, std::vector<Arm> &arms, sf::VertexArray &armLines, int numArms, sf::Time timeRunning);
void CalculateInflectionsSimple(std::set<Inflection>& inflectionPoints, float armSpeedA, float armSpeedB, float secsToRepeat);
void GetMatchesFromLists(std::set<Inflection>& inflectionPoints, std::vector<float> listA, std::vector<float> listB, std::string matchType);
void FindMatches(std::vector<float> &matchList, float armSpeedA, float armSpeedB, float secsToRepeat, std::string formula1, std::string formula2);
float GetFormulaResult(std::string formula, float armSpeedA, float armSpeedB, int i);
void setBgrdColor(int &bgColorScheme, sf::Color &bgColor, float timeRunning);
float EuclideanAlgo(float num, float denom);
float GCD(std::vector<float> numbers);
int GCD(int a, int b);
int LCM(std::vector<float> numbers);
float RoundToXDecimals(float num, int x);
float DegToRad(float deg);
void DebugLog(std::string input, std::string = "Debug Log");
void setColorAlgo(std::string& colorAlgo);
void ColorAlgorithmHandler(std::vector<sf::Vertex> &graph, std::vector<sf::Vertex> &graph3DFront, std::vector<sf::Vertex> &graph3DBack, sf::Vector2f graphPosition, std::string algoName, float timeRunning, float repeatSecs, std::set<Inflection> &inflectionPoints, bool is3DGraph, bool& is3DFront);
void ColorAlgoSolid(std::vector<sf::Vertex> &graph, sf::Color color);
void ColorAlgoFireGradient(std::vector<sf::Vertex> &graph, float repeatSecs, int percentComplete);
void ColorAlgoFuschiaGradient(std::vector<sf::Vertex> &graph, float repeatSecs, int percentComplete);
void ColorAlgoRainbowGradient(std::vector<sf::Vertex> &graph, float repeatSecs);
void ColorAlgoRainbowDiscrete(std::vector<sf::Vertex> &graph, float repeatSecs);
void ColorAlgoConfetti(std::vector<sf::Vertex> &graph);
void ColorAlgo3DDefault(std::vector<sf::Vertex>& graph3DFront, std::vector<sf::Vertex>& graph3DBack, bool& is3DFront, const std::set<Inflection>& inflectionPoints, sf::Vector2f graphPosition, float repeatSecs, float timeRunning, int percentComplete);




void main()
{
	sf::Vector2i screenDimensions(1200, 900);
	sf::Vector2f origin(screenDimensions.x / 2.0f, screenDimensions.y / 2.0f);
	sf::RenderWindow window;
	sf::Color bgColor(0,0,0);
	std::string colorAlgo = "White";
	std::vector<Arm> arms;	//# rotating arms that make up graph
	std::vector<float> armSpeeds;
	std::vector<sf::Vertex> graph, graph3DFront, graph3DBack;
	std::set<Inflection> inflectionPoints;
	sf::Clock clock, refreshClock;
	sf::Time timeRunning, refreshTime;
	float secsToRepeat;		//seconds it takes to draw entire spirograph pattern, pattern will repeat after this if not stopped
	int numArms, screenshotNum = 1;
	int bgColorScheme = 0;
	bool showArmLines = true;
	bool takeScreenShot = false;
	bool is3DGraph = true; //! developer debug variable (for now)
	bool is3DFront = true;


	window.create(sf::VideoMode(screenDimensions.x, screenDimensions.y), "SpiroGreg");
	GetUserInput(arms, numArms, colorAlgo);

		//For arms of spirograph -- linestrips are lines where the end vertex of a line is the starting vertex of the next line
	sf::VertexArray armLines(sf::LinesStrip, numArms + 1);
	armSpeeds = SetArmSpeeds(numArms, arms);
	secsToRepeat = GetSecsToRepeat(armSpeeds);

	cout << "Seconds before repeat = " << secsToRepeat << endl;
	GetInflectionPointsSimple(armSpeeds, secsToRepeat, inflectionPoints);
	InitializeLineStrip(screenDimensions, armLines, arms, window); //creates first arm of spirograph


	while (window.isOpen()){
		
		sf::Event Event;
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
					if (showArmLines) {
						showArmLines = false;
					}
					else {
						showArmLines = true;
					}
				}
				else if (Event.key.code == sf::Keyboard::P) {
					takeScreenShot = true;
				}
				break;
			}//end switch
		}//end Event Loop

		UpdateArms(origin, arms, armLines, numArms, timeRunning);

		//if (refreshTime.asMilliseconds() > 5.0f) {
			if ( secsToRepeat > (timeRunning.asSeconds() - .1f) ){
				if (!is3DGraph) {
					graph.push_back(sf::Vertex(sf::Vector2f(armLines[numArms].position)));	//creates new vertices (to be colored)
				}
				ColorAlgorithmHandler(graph, graph3DFront, graph3DBack, sf::Vector2f(armLines[numArms].position), colorAlgo, timeRunning.asMilliseconds(), secsToRepeat, inflectionPoints, is3DGraph, is3DFront);
				refreshClock.restart();
			} else {
				showArmLines = false;
			}
		//}


		if (!is3DGraph)
		{
			if (graph.size())
				window.draw(&graph[0], graph.size(), sf::LinesStrip);
		}
		else
		{
			if (is3DFront)
			{
				if (graph3DFront.size())
					window.draw(&graph3DFront[0], graph3DFront.size(), sf::LinesStrip);
			} 
			else
			{
				if (graph3DBack.size())
					window.draw(&graph3DBack[0], graph3DBack.size(), sf::LinesStrip);
			}
		}
	
		if ( showArmLines )
			window.draw(armLines);
		if ( takeScreenShot ) {
			sf::Vector2u windowSize = window.getSize();
			sf::Texture texture;
			texture.create(windowSize.x, windowSize.y);
			texture.update(window);
			sf::Image screenshot = texture.copyToImage();
			std::stringstream filename;
			filename << "screenshots/SpiroGreg_" << screenshotNum << ".png";
			screenshot.saveToFile(filename.str());
			takeScreenShot = false;
		}
		window.display();

		setBgrdColor(bgColorScheme, bgColor, timeRunning.asMilliseconds());
		window.clear(bgColor);								//clears back buffer 

		refreshTime += refreshClock.getElapsedTime();
		timeRunning += clock.getElapsedTime();

		if (timeRunning.asSeconds() > secsToRepeat)
		{
			if (AskUserToRepeat())
			{
				timeRunning = sf::Time::Zero;
				if (is3DGraph)
				{
					graph3DFront.resize(0);
					graph3DBack.resize(0);
				}
				else {
					graph.resize(0);
				}
				InitializeLineStrip(screenDimensions, armLines, arms, window);
				UpdateArms(origin, arms, armLines, numArms, timeRunning);
			}
		}

		clock.restart();
	}

}//end main





/******************************* FUNCTIONS ***********************************/

void GetUserInput(std::vector<Arm> &arms, int &numArms, std::string &colorAlgo)
{
	Arm* tempArm;
	bool isValid = false;
	std::string debugResponse;
	bool isDebugGraph = false;
	float tempSpeed = 0, tempRadius = 0, colorScheme = 0;

	while (!isValid) {
		cout << "Do you want to enter default debug graph data? (y/n): ";
		if (!(std::cin >> debugResponse) || debugResponse.compare("y") == 0 || debugResponse.compare("yes") == 0)
		{
			isValid = true;
			isDebugGraph = true;
			cout << "setting graph with default variables..." << endl;
		}
		else if (debugResponse.compare("n") == 0 || debugResponse.compare("no") == 0)
		{
			isValid = true;
			cout << endl << endl;
		}
		else
		{
			cout << "incorrect input! Try again..." << endl;
		}
	}

	if (isDebugGraph)
	{
		numArms = 2;
		arms.push_back(*(new Arm(200, 100)));
		arms.push_back(*(new Arm(69, 250)));
		colorAlgo = "Fire Gradient";

	}
	else
	{
		isValid = false;

		while (!isValid) {
			cout << "Enter number of rotating arms (1-4): ";
			if (!(std::cin >> numArms) || numArms > 4 || numArms < 1)
				cout << "incorrect input! Try again..." << endl;
			else
				isValid = true;
		}

		isValid = false;

		for (int i = 0; i < numArms; i++) {
			while (!isValid) {
				cout << "Enter RADIUS for arm #" << i + 1 << " (5 - 200): ";
				if (!(std::cin >> tempRadius) || tempRadius < 5 || tempRadius > 200)
					cout << "incorrect input! Try again..." << endl << endl;
				else
					isValid = true;
			}

			isValid = false;

			while (!isValid) {
				cout << "Enter SPEED for arm #" << i + 1 << "(-1080 - 1080): ";
				if (!(std::cin >> tempSpeed) || tempSpeed < -1080 || tempSpeed > 1080)
					cout << "incorrect input! Try again..." << endl << endl;
				else
					isValid = true;
			}

			tempArm = new Arm(tempRadius, tempSpeed);
			arms.push_back(*tempArm); //! need to implement object destruction so we can not just repeat graph, but create new graphs
			delete tempArm;
			isValid = false;
			cout << endl;
		}

		cout << "White             -- 1" << endl;
		cout << "Fire Gradient     -- 2" << endl;
		cout << "Fuschia Gradient  -- 3" << endl;
		cout << "Rainbow Gradient  -- 4" << endl;
		cout << "Rainbow Discrete  -- 5" << endl;
		cout << "Confetti          -- 6" << endl;
		cout << "Invisible         -- 7" << endl;

		while (!isValid) {
			cout << "Enter COLOR SCHEME number (1 - 7): ";
			if (!(std::cin >> colorScheme) || colorScheme < 1 || colorScheme > 7)
				cout << "incorrect input! Try again..." << endl;
			else
				isValid = true;
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
}


bool AskUserToRepeat()
{
	bool isValid = false;
	std::string input;
	while (!isValid) {
		cout << "Enter 'yes' to repeat the graph drawing: ";
		if (!(std::cin >> input) || !(input.compare("yes") == 0 || input.compare("YES") == 0 ||
			input.compare("Yes") == 0 || input.compare("y") == 0 ||
			input.compare("Y") == 0))
		{
			cout << "incorrect input! Try again..." << endl;
		}
		else {
			isValid = true;
			return true;
		}
	}
	return false;
}

//void GetInflectionPoints(std::vector<float> armSpeeds, float secsToRepeat, std::set<Inflection> &inflectionPoints)
//{
//	std::vector<Inflection> comparisonVector;
//
//	for (int i = 0; i < armSpeeds.size() - 1; i++) {
//			//add special case for i = 0
//		CalculateInflections(inflectionPoints, "normal", armSpeeds[i], armSpeeds[i + 1], secsToRepeat);
//		CalculateInflections(inflectionPoints, "opposite", armSpeeds[i], armSpeeds[i + 1], secsToRepeat);
//			//1. if i != 0, called CalculateInflections again twice but add comparisonVector instead of inflectionPoints
//			//2. if inflectionPoints has any values that are not in comparison vector, delete them from inflectionPoints
//			//2a. if a value in inflectionPoints was "normal" but it shares a value with a comparisonVector entry marked 
//			//		as opposite, change the inflectionPoints entry to be marked as "opposite"
//			//3. after going through all possible comparison vectors, and excising bum entries and marking "opposite" 
//			//		entries correctly, you should have a valid list of inflection points to send to a 3D coloring algorithm
//			//note: make sure the final list gets turned into a set to de-dupe before going to the 3D coloring algorithm
//	}
//}

void GetInflectionPointsSimple(std::vector<float> armSpeeds, float secsToRepeat, std::set<Inflection> &inflectionPoints)
{
	std::vector<float> sineMatchList;
	std::vector<float> negSineMatchList;
	std::vector<float> cosMatchList;
	std::vector<float> negCosMatchList;


	FindMatches(sineMatchList, armSpeeds[0], armSpeeds[1], secsToRepeat, "sineMinus", "sinePlus");
	FindMatches(negSineMatchList, armSpeeds[0], armSpeeds[1], secsToRepeat, "negSineMinus", "negSinePlus");
	FindMatches(cosMatchList, armSpeeds[0], armSpeeds[1], secsToRepeat, "cosMinus", "cosPlus");
	FindMatches(negCosMatchList, armSpeeds[0], armSpeeds[1], secsToRepeat, "negCosMinus", "negCosPlus");
	
	//DebugLog("/start");
	//DebugLog(VectorFloatToString(sineMatchList));
	//DebugLog("/end");
	//DebugLog("/start");
	//DebugLog(VectorFloatToString(cosMatchList));
	//DebugLog("/end");

	GetMatchesFromLists(inflectionPoints, sineMatchList, cosMatchList, "coincident");
	GetMatchesFromLists(inflectionPoints, negSineMatchList, negCosMatchList, "reverseCoincident");
}

//void CalculateInflections(
//		std::set<Inflection> &inflectionPoints,
//		std::string typeToCalculate, 
//		float armSpeedA, 
//		float armSpeedB, 
//		float secsToRepeat) 
//{
//	if (typeToCalculate == "coincident") { //checking for 
//		std::vector<float> cosineNormalTimeValues;
//		std::vector<float> sineNormalTimeValues;
//			// need multiple values here because the equation for t has +/- for both sine and cos :(
//			// see if there's a way to do this that's easier to read!
//		float prospectiveCosTime1;
//		float prospectiveCosTime2;
//		float prospectiveSineTime1;
//		float prospectiveSineTime2;
//		bool cos1Done = false;
//		bool cos2Done = false;
//		bool sine1Done = false;
//		bool sine2Done = false;
//		for (int i = 0; cos1Done == false || sine1Done == false || cos2Done == false || sine2Done == false; i ++) {
//				// cos + case
//			prospectiveCosTime1 = (2 * PI * i) / (armSpeedA + armSpeedB);
//			if (prospectiveCosTime1 > secsToRepeat) {
//				cos1Done == true;
//			} else if (!cos1Done) {
//				cosineNormalTimeValues.push_back(prospectiveCosTime1);
//			}
//				// cos - case
//			prospectiveCosTime2 = (2 * PI * i) / (armSpeedA - armSpeedB);
//			if (prospectiveCosTime2 > secsToRepeat) {
//				cos2Done == true;
//			}
//			else if (!cos2Done) {
//				cosineNormalTimeValues.push_back(prospectiveCosTime2);
//			}
//				// sine + case
//			prospectiveSineTime1 = (2 * PI * i) / (armSpeedA - armSpeedB);
//			if (prospectiveSineTime1 > secsToRepeat) {
//				sine1Done == true;
//			}
//			else if (!sine1Done) {
//				sineNormalTimeValues.push_back(prospectiveSineTime1);
//			}
//				// sine - case
//			prospectiveSineTime2 = (2 * PI * i + PI) / (armSpeedA + armSpeedB);
//			if (prospectiveSineTime2 > secsToRepeat) {
//				sine2Done == true;
//			}
//			else if (!sine2Done) {
//				sineNormalTimeValues.push_back(prospectiveSineTime2);
//			}
//		}
//
//		// add inflection points to returning vector only if both sine and cos contain the same time value
//		// which means that sine and cos both match 
//		for (int i = 0; i < cosineNormalTimeValues.size(); i++ ) {
//			if (std::find(sineNormalTimeValues.begin(), sineNormalTimeValues.end(), cosineNormalTimeValues[i]) != sineNormalTimeValues.end()) {
//				Inflection infl(cosineNormalTimeValues[i], typeToCalculate);
//				inflectionPoints.insert(infl);
//			}
//		}
//	}
//	else if (typeToCalculate == "reverseCoincident") {
//		//Same as above but with different equations. Just make a function!
//	}
//	else {
//		//TODO return error
//	}
//}


void CalculateInflectionsSimple(
	std::set<Inflection> &inflectionPoints,
	float armSpeedA,
	float armSpeedB,
	float secsToRepeat)
{

}


void GetMatchesFromLists(
		std::set<Inflection> &inflectionPoints,
		std::vector<float> listA, 
		std::vector<float> listB, 
		std::string matchType)
{
	for (int i = 0; i < listA.size(); i++)
	{
		for (int j = 0; j < listB.size(); j++)
		{
			if (RoundToXDecimals(listA[i], 3) == RoundToXDecimals(listB[j], 3))
			{
				Inflection newInflection = Inflection::Inflection(RoundToXDecimals(listA[i], 3), matchType);
				inflectionPoints.insert(newInflection);
				break;
			}
		}

	}
}


void FindMatches(
		std::vector<float> &matchList, 
		float armSpeedA, 
		float armSpeedB, 
		float secsToRepeat, 
		std::string formula1, 
		std::string formula2 
	 )
{
	bool isFormula1Done = false, isFormula2Done = false;
	bool hasFormula1Pushed = false, hasFormula2Pushed = false;

	int n = 1;
	float formula1Result;
	float formula2Result;
	for (int i = 1; !isFormula1Done && !isFormula2Done; i++)
	{
		if (!isFormula1Done)
		{
			formula1Result = GetFormulaResult(formula1, armSpeedA, armSpeedB, i);
			if (formula1Result >= secsToRepeat)
			{
				isFormula1Done = true;
				if (!hasFormula2Pushed)
				{
					isFormula2Done = true;
				}
			}
			else if (formula1Result > 0)
			{
				hasFormula1Pushed = true;
				matchList.push_back(formula1Result);
			}
		}
		if (!isFormula2Done)
		{
			formula2Result = GetFormulaResult(formula2, armSpeedA, armSpeedB, i);
			if (formula2Result >= secsToRepeat)
			{
				isFormula2Done = true;
				if (!hasFormula1Pushed)
				{
					isFormula1Done = true;
				}
			}
			else if (formula2Result > 0) 
			{
				hasFormula2Pushed = true;
				matchList.push_back(formula2Result);
			}
		}
	}
}


float GetFormulaResult(std::string formula, float armSpeedA, float armSpeedB, int i)
{
	float rtnStr = 0;
	if (formula.compare("sineMinus") == 0 || formula.compare("cosMinus") == 0)
		rtnStr = (2 * PI * i) / (DegToRad(armSpeedA) - DegToRad(armSpeedB));
	if (formula.compare("sinePlus") == 0 || formula.compare("negCosPlus") == 0)
		rtnStr = (2 * PI * i + PI) / (DegToRad(armSpeedA) + DegToRad(armSpeedB));
	if (formula.compare("cosPlus") == 0 || formula.compare("negSinePlus") == 0)
		rtnStr = (2 * PI * i) / (DegToRad(armSpeedA) + DegToRad(armSpeedB));
	if (formula.compare("negSineMinus") == 0 || formula.compare("negCosMinus") == 0)
		rtnStr = (2 * PI * i + PI) / (DegToRad(armSpeedA) - DegToRad(armSpeedB));
	
	return rtnStr;
}


std::vector<float> SetArmSpeeds(int numArms, std::vector<Arm> arms) 
{
	std::vector<float> armSpeeds;
	
	for (int i = 0; i < numArms; i++)
	{
		armSpeeds.push_back(arms[i].getAngularV_Deg());
	}
	return armSpeeds;
}


float GetSecsToRepeat(std::vector<float> armSpeeds)
{
	/*
		math explanation: 
		armSpeeds are in degrees per second, so the higher the armspeed number, the less time it takes 
		that wavefore to do a revolution. Its easy to make the mistake that larger numbers are faster
		here, and think you should take the Least Common Multiple. Instead, because larger numbers mean
		less time, we actually want to take the GCD. (360 degrees/revolution) / (<GCD> degrees/second). A 
		little dimensional analysis gives a result in terms of seconds/revolution, exactly what we want.
	*/
	if (armSpeeds.size() > 1) {
		return (360 / (float)GCD(armSpeeds));
	} else {
		return (abs(360 / (float)armSpeeds[0])); //must use abs() because neg. speed will otherwise cause neg. # of seconds
	}
}


void InitializeLineStrip(sf::Vector2i screenDimensions, sf::VertexArray &armLines, std::vector<Arm> &arms, sf::RenderWindow &window)
{
	armLines[0].position = sf::Vector2f(screenDimensions.x / 2.0f, screenDimensions.y / 2.0f);
	armLines[0].color = sf::Color::White;

	armLines[1].position = sf::Vector2f(screenDimensions.x / 2.0f, screenDimensions.y / 2.0f - arms[0].getRadius());
	armLines[1].color = sf::Color::White;

	window.draw(armLines);
	window.display();
}


void UpdateArms(sf::Vector2f origin, std::vector<Arm> &arms, sf::VertexArray &armLines, int numArms, sf::Time timeRunning)
{
		//sets position of the end of the first arm
	float xPos = origin.x + (arms[0].getRadius() * (float)std::cos(timeRunning.asSeconds() * arms[0].getAngularV_Rad() - (PI / 2)));
	float yPos = origin.y + (arms[0].getRadius() * (float)std::sin(timeRunning.asSeconds() * arms[0].getAngularV_Rad() - (PI / 2)));
	armLines[1].position = sf::Vector2f(xPos, yPos);
	armLines[1].color = sf::Color::White;

		//creates arms of spirograph from user's input data
	CreateLineStrip(armLines, numArms, arms, timeRunning.asSeconds());		
}


void CreateLineStrip(sf::VertexArray &lines, int numArms, std::vector<Arm> arms, float timeRunning)
{
	for (int i = 0; i < numArms; i++) {
		sf::Vector2f oldCoord = sf::Vector2f(lines[i].position.x, lines[i].position.y);
		float cosAngleRad = arms[i].getAngularV_Rad() * timeRunning - (PI / 2);
		float sinAngleRad = arms[i].getAngularV_Rad() * timeRunning - (PI / 2);

		float cosResult = (float)std::cos(cosAngleRad);
		float sinResult = (float)std::sin(sinAngleRad);

		lines[i + 1].position = sf::Vector2f(oldCoord.x + arms[i].getRadius() * cosResult,
			oldCoord.y + arms[i].getRadius() * sinResult);
		lines[i + 1].color = sf::Color::White;
	}
}


		//! VERY inefficient to call this function every update for solid colors. Create an isSolidColor switch that makes this
		//! run only once for solid colors
void ColorAlgorithmHandler(
		std::vector<sf::Vertex> &graph,
		std::vector<sf::Vertex> &graph3DFront,
		std::vector<sf::Vertex> &graph3DBack,
		sf::Vector2f graphPosition,
		std::string algoName, 
		float timeRunning, 
		float repeatSecs, 
		std::set<Inflection>& inflectionPoints,
		bool is3Dgraph,
		bool& is3DFront)
{
	int percentComplete = (int)(timeRunning / repeatSecs);	//percent of pattern cyle completed out of 1000% max (not 100%)

	if (is3Dgraph)
	{
		ColorAlgo3DDefault(graph3DFront, graph3DBack, is3DFront, inflectionPoints, graphPosition, repeatSecs, timeRunning, percentComplete);
		return;
	}

	if ( (algoName.compare("Invisible") == 0) || (algoName.compare("White") == 0) || (algoName.compare("Red") == 0) 
			|| (algoName.compare("Cyan") == 0) || (algoName.compare("Magenta") == 0) || (algoName.compare("Green") == 0) 
			|| (algoName.compare("Yellow") == 0) || (algoName.compare("Orange") == 0) || (algoName.compare("Grey") == 0) ) { 
		ColorAlgoSolid(graph, sf::Color::Transparent);
		return;
	}

	if (algoName.compare("Fire Gradient") == 0){
		ColorAlgoFireGradient(graph, repeatSecs, percentComplete);
		return;
	}

	if (algoName.compare("Fuschia Gradient") == 0){
		ColorAlgoFuschiaGradient(graph, repeatSecs, percentComplete);
		return;
	}

	if (algoName.compare("Rainbow Gradient") == 0){
		ColorAlgoRainbowGradient(graph, repeatSecs);
		return;
	}

	if (algoName.compare("Rainbow Discrete") == 0){
		ColorAlgoRainbowDiscrete(graph, repeatSecs);
		return;
	}

	if (algoName.compare("Confetti") == 0){
		ColorAlgoConfetti(graph);
		return;
	}
}//end Color Algo


void setBgrdColor(int &bgColorScheme, sf::Color &color, float timeRunning)
{
	if (bgColorScheme % 4 == 0)				//black
		color = sf::Color(0, 0, 0);
	else if (bgColorScheme % 4 == 1)		//dark grey
		color = sf::Color(25, 25, 25);
	else if (bgColorScheme % 4 == 2)		//grey
		color = sf::Color(50, 50, 50);
	else if (((int)timeRunning % 65 == 0)){		//rainbow flash
		int red = rand() & 60;
		int green = rand() % 60;
		int blue = rand() % 60;

		color = sf::Color(red, green, blue);
	}
}


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
float EuclideanAlgo(float num, float denom) 
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
		while (a >= d){
			a -= d;
			q++;
		}
		r = a;
		return EuclideanAlgo(d, r);
	}
}


		//finds Greatest Common Denominator of 2 or more ints in a vector
float GCD(std::vector<float> armSpeeds) 
{
	float gcd = armSpeeds[0]; 

	for (int i = 1; i < armSpeeds.size(); i++) {
		gcd = EuclideanAlgo(gcd, armSpeeds[i]);
	}

	cout << "GCD = " << gcd << endl << endl;
	return gcd; 
}

int GCD(int a, int b) 
{
	if (b == 0)
		return a;
	return GCD(b, a % b);
}

int LCM(std::vector<int> numbers) 
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

float RoundToXDecimals(float num, int x)
{
	float multiplier = std::pow(10.0, x);
	return std::round(num * multiplier) / multiplier;
}

float DegToRad(float deg)
{
	float rad = deg * (PI / 180.0);
	return rad;
}

void DebugLog(std::string input, std::string titleText)
{
	if (input.compare("/start") == 0) {
		cout << endl << "***************" + titleText + " * **************" << endl << endl;
	}
	else if (input.compare("/end") == 0) {
		cout << endl << endl << "*****************************************" << endl;
	}
	else {
		cout << input << endl;
	}
}






/**************************** COLORING ALGORITHMS ****************************/ //!coloring stuff should probably be in its own class

void ColorAlgoSolid(std::vector<sf::Vertex> &graph, sf::Color color)
{
	graph[graph.size() - 1].color = color;
}

void ColorAlgoFireGradient(std::vector<sf::Vertex> &graph, float repeatSecs, int percentComplete)
{
	int red = 255, green = 0, blue = 0;
	repeatSecs *= 4.44;

	if ((percentComplete % 1000) < 500) {		//red to yellow
		green = 0 + (255 * (percentComplete % 1000) / 500.0f);
		graph[graph.size() - 1].color = sf::Color(red, green, blue);
		return;
	}
	else {													//yellow to red
		green = 255 - (255 * (percentComplete % 1000 - 500) / 500.0f);
		graph[graph.size() - 1].color = sf::Color(red, green, blue);
		return;
	}
}

void ColorAlgoFuschiaGradient(std::vector<sf::Vertex> &graph, float repeatSecs, int percentComplete)
{
	int red = 255, green = 0, blue = 128;

	/*********

		//template for equation that appears in the below 'if...else if' branches:
	red = $starting_rbg_value$ +/- ( $rgb_change_amount$ * ( (percentComplete % 1000) / $interval_between_different_colors$) );

		//example:
	red = 255 - (127 * ( (percentComplete % 1000) / 333.0f ) );		//pnk to purp

	$starting_rbg_value$	  = 255		//should be the value that red ended at in the previous statement in the if...else (unless there's no previous )
	$rgb_change_amount$		  =	127		//red drops by 127 from the starting amount of 255 over an interval of 333 (or 1/3 a pattern rotation)
	$interval_between_colors$ = 333		//This is out of a possible 1000. So 333 means the color will change from pink to purple as spirograph
	//does 1/3 of a full pattern

	Note: the "% 1000" next to percentComplete is necessary in the if conditionals. It is not necessary in the BODY
	of the if statements unless you WANT to keep on drawing colors AFTER 1 full pattern rotation. They are left
	inside the if statements for future flexibility.

	***********/

	if ((percentComplete % 1000) < 333) {		//pnk to purp
		red = 255 - (127 * ((percentComplete % 1000) / 333.0f));
		graph[graph.size() - 1].color = sf::Color(red, green, blue);

		//if (red < 129)
			//cout << "red =" << red << endl;
		return;
	}
	else if ((percentComplete % 1000) < 666) {	//purp to blue
		red = 128 - (28 * ((percentComplete % 1000 - 333) / 333.0f));
		blue = 128 + (92 * ((percentComplete % 1000 - 333) / 333.0f));
		green = 0 + (50 * ((percentComplete % 1000 - 333) / 333.0f));
		graph[graph.size() - 1].color = sf::Color(red, green, blue);
		return;
	}
	else if ((percentComplete % 1000) < 1000) {	//blue to pnk
		red = 100 + (155 * ((percentComplete % 1000 - 666) / 333.0f));
		blue = 220 - (92 * ((percentComplete % 1000 - 666) / 333.0f));
		green = 50 - (50 * ((percentComplete % 1000 - 666) / 333.0f));
		graph[graph.size() - 1].color = sf::Color(red, green, blue);
	}
}

void ColorAlgoRainbowGradient(std::vector<sf::Vertex> &graph, float repeatSecs)
{
	int red = 0, green = 0, blue = 0;
	repeatSecs *= 1.25;	//larger value makes gradient fade between colors slower

	if ((graph.size() / (int)repeatSecs) % 8 == 0) {				//red to orng
		red = 255;
		green = 0 + (165 * (graph.size() % (int)repeatSecs) / repeatSecs);
		graph[graph.size() - 1].color = sf::Color(red, green, blue);
		return;
	}
	else if ((graph.size() / (int)repeatSecs) % 8 == 1) {			//orng to yell	
		red = 255;
		green = 165 + (90 * (graph.size() % (int)repeatSecs) / repeatSecs);
		graph[graph.size() - 1].color = sf::Color(red, green, blue);
		return;
	}
	else if ((graph.size() / (int)repeatSecs) % 8 == 2) {			//yell to grn
		red = 255 - (255 * (graph.size() % (int)repeatSecs) / repeatSecs);
		green = 255;
		graph[graph.size() - 1].color = sf::Color(red, green, blue);
		return;
	}
	else if ((graph.size() / (int)repeatSecs) % 8 == 3) {			//grn to blu
		green = 255;
		blue = 0 + (255 * (graph.size() % (int)repeatSecs) / repeatSecs);
		graph[graph.size() - 1].color = sf::Color(red, green, blue);
		return;
	}
	else if ((graph.size() / (int)repeatSecs) % 8 == 4) {			//blu to indi
		blue = 255;
		green = 255 - (155 * (graph.size() % (int)repeatSecs) / repeatSecs);
		red = 0 + (190 * (graph.size() % (int)repeatSecs) / repeatSecs);
		graph[graph.size() - 1].color = sf::Color(red, green, blue);
		return;
	}
	else if ((graph.size() / (int)repeatSecs) % 8 == 5) {			//indi to viol
		red = 190 + (10 * (graph.size() % (int)repeatSecs) / repeatSecs);
		green = 100 - (70 * (graph.size() % (int)repeatSecs) / repeatSecs);
		blue = 255 - (25 * (graph.size() % (int)repeatSecs) / repeatSecs);
		graph[graph.size() - 1].color = sf::Color(red, green, blue);
		return;
	}
	else if ((graph.size() / (int)repeatSecs) % 8 == 6) {			//viol to mag
		red = 200 + (55 * (graph.size() % (int)repeatSecs) / repeatSecs);
		blue = 230 + (25 * (graph.size() % (int)repeatSecs) / repeatSecs);
		green = 30 - (30 * (graph.size() % (int)repeatSecs) / repeatSecs);
		graph[graph.size() - 1].color = sf::Color(red, green, blue);
		return;
	}
	else if ((graph.size() / (int)repeatSecs) % 8 == 7) {			//mag to red

		blue = 255 - (255 * (graph.size() % (int)repeatSecs) / repeatSecs);
		red = 255;
		graph[graph.size() - 1].color = sf::Color(red, green, blue);
		return;
	}
}

void ColorAlgoRainbowDiscrete(std::vector<sf::Vertex> &graph, float repeatSecs)
{
	int vLength = ((graph.size() - 1) / (int)repeatSecs) % 160;

	if (vLength < 20)
		graph[graph.size() - 1].color = sf::Color::Red;
	else if (vLength >= 20 && vLength < 40)
		graph[graph.size() - 1].color = sf::Color(255, 165, 0);
	else if (vLength >= 40 && vLength < 60)
		graph[graph.size() - 1].color = sf::Color::Yellow;
	else if (vLength >= 60 && vLength < 80)
		graph[graph.size() - 1].color = sf::Color::Green;
	else if (vLength >= 80 && vLength < 100)
		graph[graph.size() - 1].color = sf::Color::Blue;
	else if (vLength >= 100 && vLength < 120)
		graph[graph.size() - 1].color = sf::Color(75, 0, 130);
	else if (vLength >= 120 && vLength < 140)
		graph[graph.size() - 1].color = sf::Color(128, 0, 128);
	else if (vLength >= 140 && vLength < 160)
		graph[graph.size() - 1].color = sf::Color::Magenta;
	return;
}

void ColorAlgoConfetti(std::vector<sf::Vertex> &graph)
{
	int red = rand() & 250 + 5;
	int green = rand() % 250 + 5;
	int blue = rand() % 250 + 5;

	graph[graph.size() - 1].color = sf::Color(red, green, blue);
}

void ColorAlgo3DDefault(
		std::vector<sf::Vertex> &graph3DFront, 
		std::vector<sf::Vertex> &graph3DBack, 
		bool& is3DFront,
		const std::set<Inflection> &inflectionPoints, 
		sf::Vector2f graphPosition,
		float repeatSecs, 
		float timeRunning, 
		int percentComplete)
{

	Inflection lowerBoundInflection;
	Inflection upperBoundInflection;
	int indexOfLowerBound = 0;
	float lowerBoundTime = 0;
	bool intervalFound = false;
	std::set<Inflection>::iterator itr;


	for (itr = inflectionPoints.begin(); itr != inflectionPoints.end() || intervalFound; indexOfLowerBound++)
	{
		lowerBoundInflection = *itr;
		itr++;
		upperBoundInflection = *itr;
		if (upperBoundInflection.getTime() > timeRunning)
		{
			itr--;
			lowerBoundTime = lowerBoundInflection.getTime();
			intervalFound = true;
			break;
		}
	}

	//if the lower bound of the segment (inflectionPoints[i]) is even draw front, if odd draw back
	if (indexOfLowerBound % 2 == 0)
	{
		is3DFront = true;

		if (graph3DFront.size() == 0)
		{
			graph3DFront.push_back(sf::Vertex(graphPosition));
		}

		graph3DFront.push_back(sf::Vertex(graphPosition));	//creates new vertices (to be colored)
		graph3DFront[graph3DFront.size() - 1].color = sf::Color::Red;
	}
	else 
	{
		is3DFront = false;

		if (graph3DBack.size() == 0)
		{
			graph3DBack.push_back(sf::Vertex(graphPosition));
		}

		graph3DBack.push_back(sf::Vertex(graphPosition));	//creates new vertices (to be colored)
		graph3DBack[graph3DBack.size() - 1].color = sf::Color::Green;
	}
}