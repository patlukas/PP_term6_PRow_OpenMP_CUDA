#pragma once
#include <iostream>
#include <vector>
#include <ctime>

#include "AntSLF.h"

using namespace std;

class ACO{
	float wyparowywanie;
	int numberOfVertices;
	int numberOfCycle;
	int numberOfAnt;
	int verticewWithTheMostNeighbors;
	vector<vector<float>> coloringQuality;
	vector<vector<float>> coloringQualityDelta;
	vector<vector<int>> listaSasiedztwa;
	vector<vector<int>> listaNieSasiedztwa;
	vector<pair<int, int>> edges;

	void inicjalizationArrayQuality(int);
	void createListaSasiedztwa();
	void createListaNieSasiedztwa();
	void setQualityNieSasiadow();
	void setQualityDeltaNieSasiadow();
	void getVertexWithTheMostNeighbors();
	
public:
	ACO(vector<pair<int, int>> edges, int numberOfVertices, float wyparowywanie, int maxNumberOfCycle, int maxNumberOfAnt, int maxTimeOfCycleInMs, int maxTimeProgramInMs, float alfa, float beta);
	int minNumberOfColor;
	vector<vector<int>> bestSubsetsOfVertices;
};

