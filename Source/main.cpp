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
#include "Util.h"
#include "Graph.h"


bool AskAboutDebugInputs();
void GetUserInput(std::vector<Arm>, std::string& colorAlgo, bool isGraph3D);
string GetColorAlgoFromInput(int colorInput);
bool AskUserToRepeat();
void ShowPauseScreen(float timeAtPause, std::vector<Arm>& arms);
void setBgrdColor(int& bgColorScheme, sf::Color& bgColor, float timeRunning);
void InitializeArmLines(sf::Vector2i screenDimensions, sf::VertexArray& lines, float arm0Radius);
void UpdateArmLines(sf::Vector2f origin, std::vector<Arm> arms, sf::VertexArray& armLines, int numArms, sf::Time timeRunning);
void ChangeArmLinePositions(sf::VertexArray& armLines, int numArms, std::vector<Arm> arms, float timeRunning);
void Add3DSegment(vector<vector<sf::Vertex>>& graph3DFront, vector<vector<sf::Vertex>>& graph3DBack, bool is3DFront);
void Add3DPixel(vector<vector<sf::Vertex>>& graph3DFront, vector<vector<sf::Vertex>>& graph3DBack, bool is3DFront, sf::Vertex& newPixel);

//************** temp
void CycleColorAlgo(std::string& colorAlgo);
void ColorAlgorithmHandler(std::vector<sf::Vertex>& graph, sf::Vertex& newPixel, std::string algoName, float timeRunning, float repeatSecs, std::set<Inflection>& inflectionPoints, bool is3DGraph, bool& is3DFront);
void ColorAlgoSolid(std::vector<sf::Vertex>& graph, sf::Color color);
void ColorAlgoFireGradient(std::vector<sf::Vertex>& graph, float repeatSecs, int percentComplete);
void ColorAlgoFuschiaGradient(std::vector<sf::Vertex>& graph, float repeatSecs, int percentComplete);
void ColorAlgoRainbowGradient(std::vector<sf::Vertex>& graph, float repeatSecs);
void ColorAlgoRainbowDiscrete(std::vector<sf::Vertex>& graph, float repeatSecs);
void ColorAlgoConfetti(std::vector<sf::Vertex>& graph);
void ColorAlgo3DDefault(sf::Vertex& newPixel, bool& is3DFront, const std::set<Inflection>& inflectionPoints, float repeatSecs, float timeRunning, int percentComplete);
//************** temp


using std::cout;
using std::endl;
#define PI 3.14159265359	//3.141592653589793
bool Util::isDebugLogOn = false;



void main()
{
	sf::Vector2i screenDimensions(1200, 900);
	sf::Vector2f origin(screenDimensions.x / 2.0f, screenDimensions.y / 2.0f);
	sf::RenderWindow window;
	Graph* graph;
	sf::Color bgColor(0,0,0);
	sf::Clock clock, refreshClock;
	sf::Time refreshTime;
	int screenshotNum = 1;
	int bgColorScheme = 0;
	bool showArmLines = true;
	bool takeScreenShot = false;
	bool is3DFront = true;
	bool isPaused = false;
	std::vector<Arm> tempArmVector;
	std::string colorAlgo = "White";


	window.create(sf::VideoMode(screenDimensions.x, screenDimensions.y), "SpiroGreg");

	bool isDebugGraph = AskAboutDebugInputs();
	if (isDebugGraph)
	{
		colorAlgo = "Fire Gradient";
		tempArmVector.push_back(*(new Arm(200, 90))); //45
		tempArmVector.push_back(*(new Arm(69, 338)));  //269
		graph = new Graph(tempArmVector, true);
	}
	else
	{
		GetUserInput(tempArmVector, colorAlgo, false);
		graph = new Graph(tempArmVector, false);
	}

		// alias variables for better code readability
	std::vector<Arm> arms = graph->GetArms();
	float numArms = graph->GetNumArms();
	sf::Time timeRunning = graph->GetTimeRunning();
	float secsToRepeat = graph->GetSecsToRepeat();
	bool isGraph3D = graph->GetIs3DGraph();
	vector<sf::Vertex> graph2D = graph->graph2D;
	vector<vector<sf::Vertex>> graph3DFront = graph->graph3DFront;
	vector<vector<sf::Vertex>> graph3DBack = graph->graph3DBack;
	set<Inflection> inflectionPoints = graph->GetInflectionPoints();
	

		//For arms of spirograph -- linestrips are lines where the end vertex of a line is the starting vertex of the next line
	sf::VertexArray armLines(sf::LinesStrip, numArms + 1);
	cout << "Seconds before repeat = " << secsToRepeat << endl;
	InitializeArmLines(screenDimensions, armLines, arms[0].GetRadius()); //creates first arm of spirograph

	window.display();
	clock.restart();

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
				else if (Event.key.code == sf::Keyboard::P) {
					cout << endl << endl;
					if (isPaused) {
						isPaused = false;
						clock.restart();
					}
					else {
						isPaused = true;
						ShowPauseScreen(graph->GetTimeRunningAsSeconds(), graph->GetArms());
					}
				}
				else if (Event.key.code == sf::Keyboard::Down) {
					CycleColorAlgo(colorAlgo);
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
			}
		}
		//end Event Loop



		if (!isPaused) {
			UpdateArmLines(origin, arms, armLines, numArms, timeRunning);

			if (secsToRepeat > (timeRunning.asSeconds() - .1f)) {
				
				sf::Vertex newPixel = sf::Vertex(sf::Vector2f(armLines[numArms].position));
				
				if (isGraph3D == false) {
					graph2D.push_back(sf::Vertex(sf::Vector2f(armLines[numArms].position)));	//creates new vertices (to be colored)
				}

				bool inflectionSwitch = is3DFront;
				ColorAlgorithmHandler(graph2D, newPixel, colorAlgo, timeRunning.asMilliseconds(), secsToRepeat, inflectionPoints, isGraph3D, is3DFront);
				if (isGraph3D)
				{
					if (inflectionSwitch != is3DFront) {
						Add3DSegment(graph3DFront, graph3DBack, is3DFront);
					}
					Add3DPixel(graph3DFront, graph3DBack, is3DFront, newPixel);
				}
				
				//! future: implement 'chaotic' anti-smooth graph drawing functionality here
				//! with code similar to this. Does this code affect color algorithm?
				//		int iRand = (rand() % 230) + 1;
				//		PauseForXMilliseconds(clock, iRand);
				refreshClock.restart();
			}
			else {
				showArmLines = false;
			}

			if (isGraph3D == false)
			{
				if (graph2D.size())
					window.draw(&graph2D[0], graph2D.size(), sf::LinesStrip);
			}
			else
			{
				if (graph3DBack.size())
				{ 
					for (std::vector<sf::Vertex> vertexVector : graph3DBack) {
						if (vertexVector.size())
						{
							window.draw(&vertexVector[0], vertexVector.size(), sf::LinesStrip);
						}
					}
				}
				if (graph3DFront.size())
				{
					for (std::vector<sf::Vertex> vertexVector : graph3DFront) {
						if (vertexVector.size())
						{
							window.draw(&vertexVector[0], vertexVector.size(), sf::LinesStrip);
						}
					}
				}
			}

			if (showArmLines)
				window.draw(armLines);
			if (takeScreenShot) {
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
					graph->Initialize2DGraph();
					graph->Initialize3DGraph();
					InitializeArmLines(screenDimensions, armLines, arms[0].GetRadius());
					window.draw(armLines);
					UpdateArmLines(origin, arms, armLines, numArms, timeRunning);
				}
			}

			clock.restart();
		}
		//end frame update loop
	}
}





/******************************* FUNCTIONS ***********************************/

bool AskAboutDebugInputs()
{
	bool isValid = false;
	std::string debugResponse;
	bool isDebugGraph = false;

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
	return isDebugGraph;
}



void GetUserInput(std::vector<Arm> tempArmVector, std::string& colorAlgo, bool isGraph3D)
{

	bool isValid;
	int numArms;
	Arm* tempArm;
	float tempSpeed = 0;
	float tempRadius = 0; 
	int colorInput = 0;


	//! TODO ask user if Graph should be 2D or 3D and set isGraph3D accordingly

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
		tempArmVector.push_back(*tempArm); //! need to implement object destruction so we can not just repeat graph, but create new graphs
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
		if (!(std::cin >> colorInput) || colorInput < 1 || colorInput > 7)
			cout << "incorrect input! Try again..." << endl;
		else
			isValid = true;
	}

	colorAlgo = GetColorAlgoFromInput(colorInput);
}


string GetColorAlgoFromInput(int colorInput)
{
	if (colorInput == 1)
		return "White";
	else if (colorInput == 2)
		return "Fire Gradient";
	else if (colorInput == 3)
		return "Fuschia Gradient";
	else if (colorInput == 4)
		return "Rainbow Gradient";
	else if (colorInput == 5)
		return "Rainbow Discrete";
	else if (colorInput == 6)
		return "Confetti";
	else if (colorInput == 7)
		return "Invisible";
	else
		return "White";
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


void ShowPauseScreen(float timeAtPause, std::vector<Arm>& arms)
{
	Util::DebugLog("/start", "PAUSE SCREEN");
	Util::DebugLog("");
	std::string str;
	str += " sin(arm0): " + std::to_string(std::sin(arms[0].GetAngularV_Rad() * timeAtPause)) + "\n";
	str += " cos(arm0): " + std::to_string(std::cos(arms[0].GetAngularV_Rad() * timeAtPause)) + "\n";
	str += " sin(arm1): " + std::to_string(std::sin(arms[1].GetAngularV_Rad() * timeAtPause)) + "\n";
	str += " cos(arm1): " + std::to_string(std::cos(arms[1].GetAngularV_Rad() * timeAtPause)) + "\n";
	Util::DebugLog(str);	
	Util::DebugLog("\n Pause time (s): " + std::to_string(timeAtPause));
	Util::DebugLog("/end");
}


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


void InitializeArmLines(sf::Vector2i screenDimensions, sf::VertexArray& armLines, float arm0Radius)
{
	armLines[0].position = sf::Vector2f(screenDimensions.x / 2.0f, screenDimensions.y / 2.0f);
	armLines[0].color = sf::Color::White;

	armLines[1].position = sf::Vector2f(screenDimensions.x / 2.0f, screenDimensions.y / 2.0f - arm0Radius);
	armLines[1].color = sf::Color::White;
}


void UpdateArmLines(sf::Vector2f origin, std::vector<Arm> arms, sf::VertexArray& armLines, int numArms, sf::Time timeRunning)
{
	//sets position of the end of the first arm
	float xPos = origin.x + (arms[0].GetRadius() * (float)std::cos(timeRunning.asSeconds() * arms[0].GetAngularV_Rad() - (PI / 2)));
	float yPos = origin.y + (arms[0].GetRadius() * (float)std::sin(timeRunning.asSeconds() * arms[0].GetAngularV_Rad() - (PI / 2)));
	armLines[1].position = sf::Vector2f(xPos, yPos);
	armLines[1].color = sf::Color::White;

	//creates arms of spirograph from user's input data
	ChangeArmLinePositions(armLines, numArms, arms, timeRunning.asSeconds());
}


void ChangeArmLinePositions(sf::VertexArray& armLines, int numArms, std::vector<Arm> arms, float timeRunning)
{
	for (int i = 0; i < numArms; i++) {
		sf::Vector2f oldCoord = sf::Vector2f(armLines[i].position.x, armLines[i].position.y);
		float cosAngleRad = arms[i].GetAngularV_Rad() * timeRunning - (PI / 2);
		float sinAngleRad = arms[i].GetAngularV_Rad() * timeRunning - (PI / 2);

		float cosResult = (float)std::cos(cosAngleRad);
		float sinResult = (float)std::sin(sinAngleRad);

		armLines[i + 1].position = sf::Vector2f(oldCoord.x + arms[i].GetRadius() * cosResult,
			oldCoord.y + arms[i].GetRadius() * sinResult);
		armLines[i + 1].color = sf::Color::White;
	}
}

void Add3DSegment(vector<vector<sf::Vertex>>& graph3DFront, vector<vector<sf::Vertex>>& graph3DBack, bool is3DFront)
{
	std::vector<sf::Vertex> newVec;

	if (is3DFront) {
		graph3DFront.push_back(newVec);
	}
	else {
		graph3DBack.push_back(newVec);
	}
}


void Add3DPixel(vector<vector<sf::Vertex>>& graph3DFront, vector<vector<sf::Vertex>>& graph3DBack, bool is3DFront, sf::Vertex& newPixel)
{
	if (is3DFront) {
		graph3DFront.back().push_back(newPixel);
	}
	else {
		graph3DBack.back().push_back(newPixel);
	}
}



















































/**************************** COLORING ALGORITHMS ****************************/ //!coloring stuff should probably be in its own class

		//! VERY inefficient to call this function every update for solid colors. Create an isSolidColor switch that makes this
		//! run only once for solid colors
void ColorAlgorithmHandler(
	std::vector<sf::Vertex>& graph,
	sf::Vertex& newPixel,
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
		ColorAlgo3DDefault(newPixel, is3DFront, inflectionPoints, repeatSecs, timeRunning, percentComplete);
		return;
	}

	if ((algoName.compare("Invisible") == 0) || (algoName.compare("White") == 0) || (algoName.compare("Red") == 0)
		|| (algoName.compare("Cyan") == 0) || (algoName.compare("Magenta") == 0) || (algoName.compare("Green") == 0)
		|| (algoName.compare("Yellow") == 0) || (algoName.compare("Orange") == 0) || (algoName.compare("Grey") == 0)) {
		ColorAlgoSolid(graph, sf::Color::Transparent);
		return;
	}

	if (algoName.compare("Fire Gradient") == 0) {
		ColorAlgoFireGradient(graph, repeatSecs, percentComplete);
		return;
	}

	if (algoName.compare("Fuschia Gradient") == 0) {
		ColorAlgoFuschiaGradient(graph, repeatSecs, percentComplete);
		return;
	}

	if (algoName.compare("Rainbow Gradient") == 0) {
		ColorAlgoRainbowGradient(graph, repeatSecs);
		return;
	}

	if (algoName.compare("Rainbow Discrete") == 0) {
		ColorAlgoRainbowDiscrete(graph, repeatSecs);
		return;
	}

	if (algoName.compare("Confetti") == 0) {
		ColorAlgoConfetti(graph);
		return;
	}
}//end Color Algo


		//placeholder til you can design a pop-up menu for user to choose color
void CycleColorAlgo(std::string& colorAlgo)
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


void ColorAlgoSolid(std::vector<sf::Vertex>& graph, sf::Color color)
{
	graph[graph.size() - 1].color = color;
}


void ColorAlgoFireGradient(std::vector<sf::Vertex>& graph, float repeatSecs, int percentComplete)
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


void ColorAlgoFuschiaGradient(std::vector<sf::Vertex>& graph, float repeatSecs, int percentComplete)
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


void ColorAlgoRainbowGradient(std::vector<sf::Vertex>& graph, float repeatSecs)
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


void ColorAlgoRainbowDiscrete(std::vector<sf::Vertex>& graph, float repeatSecs)
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


void ColorAlgoConfetti(std::vector<sf::Vertex>& graph)
{
	int red = rand() & 250 + 5;
	int green = rand() % 250 + 5;
	int blue = rand() % 250 + 5;

	graph[graph.size() - 1].color = sf::Color(red, green, blue);
}


void ColorAlgo3DDefault(
	sf::Vertex& newPixel,
	bool& is3DFront,
	const std::set<Inflection>& inflectionPoints,
	float repeatSecs,
	float timeRunning,
	int percentComplete)
{

	Inflection lowerBoundInflection;
	Inflection upperBoundInflection;
	int indexOfLowerBound = 0;
	float lowerBoundTime = 0;
	float upperBoundTime = 0;
	bool intervalFound = false;
	float timeRunningAsSeconds = timeRunning / 1000;
	std::set<Inflection>::iterator itrLow = inflectionPoints.begin();
	std::set<Inflection>::iterator itrHigh = inflectionPoints.begin();

	for (; itrLow != inflectionPoints.end() || intervalFound; indexOfLowerBound++, itrLow++)
	{
		if (itrHigh != inflectionPoints.end()) {
			itrHigh++;
		}

		lowerBoundInflection = *itrLow;

		if (itrHigh != inflectionPoints.end()) {
			upperBoundInflection = *itrHigh;
			upperBoundTime = upperBoundInflection.getTime();
		}
		else {
			upperBoundInflection = lowerBoundInflection;
			upperBoundTime = repeatSecs;
		}

		if (upperBoundTime > timeRunningAsSeconds)
		{
			lowerBoundTime = lowerBoundInflection.getTime();
			intervalFound = true;
			Util::DebugLog("/start", "Upper Is Greater! Lower = " + std::to_string(indexOfLowerBound));
			Util::DebugLog("timeRunning: " + std::to_string(timeRunningAsSeconds));
			Util::DebugLog("lowerBoundTime: " + std::to_string(lowerBoundTime));
			Util::DebugLog("upperBoundTime: " + std::to_string(upperBoundTime));
			Util::DebugLog("indexOfLowerBound % 2 = " + std::to_string(indexOfLowerBound % 2));
			Util::DebugLog("/end");
			if (indexOfLowerBound == 1)
			{
				indexOfLowerBound = 1;
			}
			break;
		}

		Util::DebugLog("/start", "Interval " + std::to_string(indexOfLowerBound));
		Util::DebugLog("timeRunning: " + std::to_string(timeRunningAsSeconds));
		Util::DebugLog("lowerBoundTime: " + std::to_string(lowerBoundTime));
		Util::DebugLog("upperBoundTime: " + std::to_string(upperBoundInflection.getTime()));
		Util::DebugLog("indexOfLowerBound % 2 = " + std::to_string(indexOfLowerBound % 2));
		Util::DebugLog("/end");
	}

	if (indexOfLowerBound % 2 == 0)
	{
		if (!is3DFront) {
			is3DFront = true;
		}

		newPixel.color = sf::Color::Green;
	}
	else
	{
		if (is3DFront) {
			is3DFront = false;
		}

		newPixel.color = sf::Color::Red;
	}
}