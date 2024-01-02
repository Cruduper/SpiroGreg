
//#include "Inflection.h"
#include "Util.h"
#include "Graph.h"


using namespace std;
#define PI 3.14159265359


Graph::Graph(vector<Arm>& armsArg, set<Inflection>& inflectionPtsArg)
{
	arms = armsArg;
	numArms = arms.size();
	inflectionPoints = inflectionPtsArg;
	//vector<sf::Vertex> newVec; 		//! make these lines into 	Initialize3DGraph();
	//graph3DFront2.push_back(newVec);		//! make these lines into 	Initialize3DGraph();
	//graph3DBack2.push_back(newVec);			//! make these lines into 	Initialize3DGraph();
}

Graph::~Graph()
{

}

//set arms --> numArms is gotten here
//get, set ArmsVisible()
//unsure if armspeeds is needed?

void Graph::FindInflectionPoints()
{
	vector<float> sineMatchList;
	vector<float> negSineMatchList;
	vector<float> cosMatchList;
	vector<float> negCosMatchList;
	float armSpeedA, armSpeedB;

	if (arms[0].getAngularV_Rad() > arms[1].getAngularV_Rad()) {
		armSpeedA = arms[0].getAngularV_Rad();
		armSpeedB = arms[1].getAngularV_Rad();
	}
	else {
		armSpeedA = arms[1].getAngularV_Rad();
		armSpeedB = arms[0].getAngularV_Rad();
	}

	FindMatches(sineMatchList, armSpeedA, armSpeedB, secsToRepeat, "sineMinus", "sinePlus");
	FindMatches(negSineMatchList, armSpeedA, armSpeedB, secsToRepeat, "SineMinus180", "SinePlus180");
	FindMatches(cosMatchList, armSpeedA, armSpeedB, secsToRepeat, "cosMinus", "cosPlus");
	FindMatches(negCosMatchList, armSpeedA, armSpeedB, secsToRepeat, "CosMinus180", "CosPlus180");

	//Util::DebugLog("/start");
	//Util::DebugLog("");
	//string str = "armSpeed0: " + to_string(armSpeedA) + ", armSpeed1: " + to_string(armSpeedB);
	//Util::DebugLog(str);
	//Util::DebugLog("");
	//Util::DebugLog("");
	//Util::DebugLog(VectorFloatToString(sineMatchList));
	//Util::DebugLog("/end");
	//Util::DebugLog("/start");
	//Util::DebugLog(VectorFloatToString(cosMatchList));
	//Util::DebugLog("/end");

	FindMatchesFromLists(sineMatchList, cosMatchList, "coincident");
	FindMatchesFromLists(negSineMatchList, negCosMatchList, "reverseCoincident");
}


void Graph::FindMatchesFromLists( vector<float> listA, vector<float> listB, string matchType)
{
	for (int i = 0; i < listA.size(); i++)
	{
		for (int j = 0; j < listB.size(); j++)
		{
			if (Util::RoundToXDecimals(listA[i], 3) == Util::RoundToXDecimals(listB[j], 3))
			{
				Inflection newInflection = Inflection(Util::RoundToXDecimals(listA[i], 3), matchType);
				inflectionPoints.insert(newInflection);
				break;
			}
		}
	}
}


void Graph::FindMatches(
	vector<float>& matchList,
	float armSpeedA,
	float armSpeedB,
	float secsToRepeat,
	string formula1,
	string formula2
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


float Graph::GetFormulaResult(string formula, float armSpeedA, float armSpeedB, int i)
{
	float rtnStr = 0;
	if (formula.compare("sineMinus") == 0 || formula.compare("cosMinus") == 0)
		rtnStr = (2 * PI * i) / (armSpeedA - armSpeedB);
	if (formula.compare("sinePlus") == 0 || formula.compare("CosPlus180") == 0)
		rtnStr = (2 * PI * i + PI) / (armSpeedA + armSpeedB);
	if (formula.compare("cosPlus") == 0 || formula.compare("SinePlus180") == 0)
		rtnStr = (2 * PI * i) / (armSpeedA + armSpeedB);
	if (formula.compare("SineMinus180") == 0 || formula.compare("CosMinus180") == 0)
		rtnStr = (2 * PI * i + PI) / (armSpeedA - armSpeedB);

	return rtnStr;
}


vector<float> Graph::GetArmSpeeds()
{
	vector<float> armSpeeds;

	for (int i = 0; i < arms.size(); i++)
	{
		armSpeeds.push_back(arms[i].getAngularV_Deg());
	}
	return armSpeeds;
}


float Graph::FindSecsToRepeat()
{
	/*
		math explanation:
		armSpeeds are in degrees per second, so the higher the armspeed number, the less time it takes
		that wavefore to do a revolution. Its easy to make the mistake that larger numbers are faster
		here, and think you should take the Least Common Multiple. Instead, because larger numbers mean
		less time, we actually want to take the GCD. (360 degrees/revolution) / (<GCD> degrees/second). A
		little dimensional analysis gives a result in terms of seconds/revolution, exactly what we want.
	*/

	vector<float> armSpeeds = GetArmSpeeds();

	if (armSpeeds.size() > 1) {
		return (360 / (float)Util::GCD(armSpeeds));
	}
	else {
		return (abs(360 / (float)armSpeeds[0])); //must use abs() because neg. speed will otherwise cause neg. # of seconds
	}
}


void Graph::InitializeLineStrip(sf::Vector2i screenDimensions, sf::VertexArray& armLines, vector<Arm>& arms, sf::RenderWindow& window)
{
	armLines[0].position = sf::Vector2f(screenDimensions.x / 2.0f, screenDimensions.y / 2.0f);
	armLines[0].color = sf::Color::White;

	armLines[1].position = sf::Vector2f(screenDimensions.x / 2.0f, screenDimensions.y / 2.0f - arms[0].getRadius());
	armLines[1].color = sf::Color::White;

	window.draw(armLines);
	window.display();
}


void Graph::UpdateArms(sf::Vector2f origin, vector<Arm>& arms, sf::VertexArray& armLines, int numArms, sf::Time timeRunning)
{
	//sets position of the end of the first arm
	float xPos = origin.x + (arms[0].getRadius() * (float)cos(timeRunning.asSeconds() * arms[0].getAngularV_Rad() - (PI / 2)));
	float yPos = origin.y + (arms[0].getRadius() * (float)sin(timeRunning.asSeconds() * arms[0].getAngularV_Rad() - (PI / 2)));
	armLines[1].position = sf::Vector2f(xPos, yPos);
	armLines[1].color = sf::Color::White;

	//creates arms of spirograph from user's input data
	CreateLineStrip(armLines, numArms, arms, timeRunning.asSeconds());
}


void Graph::CreateLineStrip(sf::VertexArray& lines, int numArms, vector<Arm> arms, float timeRunning)
{
	for (int i = 0; i < numArms; i++) {
		sf::Vector2f oldCoord = sf::Vector2f(lines[i].position.x, lines[i].position.y);
		float cosAngleRad = arms[i].getAngularV_Rad() * timeRunning - (PI / 2);
		float sinAngleRad = arms[i].getAngularV_Rad() * timeRunning - (PI / 2);

		float cosResult = (float)cos(cosAngleRad);
		float sinResult = (float)sin(sinAngleRad);

		lines[i + 1].position = sf::Vector2f(oldCoord.x + arms[i].getRadius() * cosResult,
			oldCoord.y + arms[i].getRadius() * sinResult);
		lines[i + 1].color = sf::Color::White;
	}
}