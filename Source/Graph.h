#include "Arm.h"
#include "Inflection.h"
//#include "Util.h"
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <set>



#ifndef GRAPH_H_INCLUDED 
#define GRAPH_H_INCLUDED

using namespace std;

class Graph {
	public:
		Graph(vector<Arm>& armsArg, set<Inflection>& inflectionPtsArg);
		~Graph();
	private:
		vector<sf::Vertex> graph;
		vector<vector<sf::Vertex>> graph3DFront2, graph3DBack2;

		vector<Arm> arms;	//# rotating arms that make up graph
		sf::VertexArray armLines;
		int numArms; //why not use arms.size() after initialization???
		bool armsVisible = true;

		set<Inflection> inflectionPoints;
		sf::Time timeRunning;
		float secsToRepeat;		//seconds it takes to draw entire spirograph pattern
		bool is3DGraph = true; //! developer debug variable (for now)

		void FindInflectionPoints();
		void FindMatchesFromLists(vector<float> listA, vector<float> listB, string matchType);
		void FindMatches(vector<float>& matchList,float armSpeedA,float armSpeedB,float secsToRepeat,string formula1,string formula2
		);
		float GetFormulaResult(string formula, float armSpeedA, float armSpeedB, int i);
		vector<float> GetArmSpeeds();
		float FindSecsToRepeat();
		void InitializeLineStrip(sf::Vector2i screenDimensions, sf::VertexArray& armLines, vector<Arm>& arms, sf::RenderWindow& window);
		void UpdateArms(sf::Vector2f origin, vector<Arm>& arms, sf::VertexArray& armLines, int numArms, sf::Time timeRunning);
		void CreateLineStrip(sf::VertexArray& lines, int numArms, vector<Arm> arms, float timeRunning);
};

#endif
