#include "ColorManager.h"
#include "Inflection.h"


ColorManager::ColorManager()
{

}

ColorManager::~ColorManager()
{

}

//! VERY inefficient to call this function every update for solid colors. Create an isSolidColor switch that makes this
//! run only once for solid colors
void ColorManager::ColorAlgorithmHandler(
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
		//ColorAlgo3DDefault(newPixel, is3DFront, inflectionPoints, repeatSecs, timeRunning, percentComplete);
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
void ColorManager::CycleColorAlgo(std::string& colorAlgo)
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


void ColorManager::ColorAlgoSolid(std::vector<sf::Vertex>& graph, sf::Color color)
{
	graph[graph.size() - 1].color = color;
}

void ColorManager::ColorAlgoFireGradient(std::vector<sf::Vertex>& graph, float repeatSecs, int percentComplete)
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

void ColorManager::ColorAlgoFuschiaGradient(std::vector<sf::Vertex>& graph, float repeatSecs, int percentComplete)
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

void ColorManager::ColorAlgoRainbowGradient(std::vector<sf::Vertex>& graph, float repeatSecs)
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

void ColorManager::ColorAlgoRainbowDiscrete(std::vector<sf::Vertex>& graph, float repeatSecs)
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

void ColorManager::ColorAlgoConfetti(std::vector<sf::Vertex>& graph)
{
	int red = rand() & 250 + 5;
	int green = rand() % 250 + 5;
	int blue = rand() % 250 + 5;

	graph[graph.size() - 1].color = sf::Color(red, green, blue);
}

void ColorManager::ColorAlgo3DDefault(
	sf::Vertex& newPixel,
	bool& is3DFront,
	std::set<Inflection>& inflectionPoints,
	float repeatSecs,
	float timeRunning,
	int percentComplete)
{

		//! this entire section (dealing with bounds) should be part of a Graph or Graph-related object
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
			//DebugLog("/start", "Upper Is Greater! Lower = " + std::to_string(indexOfLowerBound));
			//DebugLog("timeRunning: " + std::to_string(timeRunningAsSeconds));
			//DebugLog("lowerBoundTime: " + std::to_string(lowerBoundTime));
			//DebugLog("upperBoundTime: " + std::to_string(upperBoundTime));
			//DebugLog("indexOfLowerBound % 2 = " + std::to_string(indexOfLowerBound % 2));
			//DebugLog("/end");
			if (indexOfLowerBound == 1)
			{
				indexOfLowerBound = 1;
			}
			break;
		}

		//DebugLog("/start", "Interval " + std::to_string(indexOfLowerBound));
		//DebugLog("timeRunning: " + std::to_string(timeRunningAsSeconds));
		//DebugLog("lowerBoundTime: " + std::to_string(lowerBoundTime));
		//DebugLog("upperBoundTime: " + std::to_string(upperBoundInflection.getTime()));
		//DebugLog("indexOfLowerBound % 2 = " + std::to_string(indexOfLowerBound % 2));
		//DebugLog("/end");
	}


	if (indexOfLowerBound % 2 == 0)
	{
		if (!is3DFront) {
			is3DFront = true;
		}

		newPixel.color = sf::Color::Red;
	}
	else
	{
		if (is3DFront) {
			is3DFront = false;
		}

		newPixel.color = sf::Color::Blue;
	}
}
