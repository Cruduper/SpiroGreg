#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <set>
#include "Arm.h"
#include "Inflection.h"

#ifndef COLORMANAGER_H_INCLUDED 
#define COLORMANAGER_H_INCLUDED

class ColorManager {

	public:
		ColorManager();
		~ColorManager();
		void ColorAlgorithmHandler(
			std::vector<sf::Vertex>& graph,
			sf::Vertex& newPixel,
			std::string algoName,
			float timeRunning,
			float repeatSecs,
			std::set<Inflection>& inflectionPoints,
			bool is3Dgraph,
			bool& is3DFront
		);
				//placeholder til you can design a pop-up menu for user to choose color
		void CycleColorAlgo(std::string& colorAlgo);


	

	private:
		void ColorAlgoSolid(std::vector<sf::Vertex>& graph, sf::Color color);
		void ColorAlgoFireGradient(std::vector<sf::Vertex>& graph, float repeatSecs, int percentComplete);
		void ColorAlgoFuschiaGradient(std::vector<sf::Vertex>& graph, float repeatSecs, int percentComplete);
		void ColorAlgoRainbowGradient(std::vector<sf::Vertex>& graph, float repeatSecs);
		void ColorAlgoRainbowDiscrete(std::vector<sf::Vertex>& graph, float repeatSecs);
		void ColorAlgoConfetti(std::vector<sf::Vertex>& graph);
		void ColorAlgo3DDefault(
			sf::Vertex& newPixel,
			bool& is3DFront,
			std::set<Inflection>& inflectionPoints,
			float repeatSecs,
			float timeRunning,
			int percentComplete
		);
};

#endif