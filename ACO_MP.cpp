#include "ACO_MP.h"

ACO_MP::ACO_MP(vector<pair<int, int>> edges, int numberOfVertices, float wyparowywanie, int maxNumberOfCycle, int maxNumberOfAnt, int maxTimeOfCycleInMs, int maxTimeProgramInMs, float alfa, float beta) {
	this->wyparowywanie = wyparowywanie;
	this->minNumberOfColor = numberOfVertices;
	this->numberOfCycle = maxNumberOfCycle;
	this->numberOfAnt = maxNumberOfAnt;
	clock_t timeProgram = clock();

	this->edges = edges;

	this->numberOfVertices = numberOfVertices;

	this->createListaSasiedztwa();

	this->createListaNieSasiedztwa();
	this->inicjalizationArrayQuality(numberOfVertices);

	this->setQualityNieSasiadow();
	this->getVertexWithTheMostNeighbors();

	for (int cycle = 0; cycle < this->numberOfCycle && timeProgram + maxTimeProgramInMs > clock(); cycle++) {
		clock_t timeCycle = clock();
		this->setQualityDeltaNieSasiadow();
		int ant, sumUsedColor = 0;
		vector<int> vector_numberOfColors(this->numberOfAnt);
		vector<vector<vector<int>>> vector_subsetsOfVertices(this->numberOfAnt);
		vector<vector<vector<float>>> vector_coloringQualityDelta(this->numberOfAnt, vector<vector<float>>(this->numberOfVertices, vector<float>(this->numberOfVertices)));
		
		#pragma omp parallel for
		for (int ant = 0; ant < (int)this->numberOfAnt; ant++) {
			AntSLF slf(this->listaSasiedztwa, this->numberOfVertices, this->coloringQuality, this->verticewWithTheMostNeighbors, alfa, beta);
			
			vector_numberOfColors[ant] = slf.numberOfColors;
			vector_subsetsOfVertices[ant] = slf.subsetsOfVertices;
			

			for (int i = 0; i < this->numberOfVertices; i++) {
				int j = 0;
				for (j = 0; j < slf.numberOfColors; j++) {
					bool inSubsets = false;
					for (auto vertice : slf.subsetsOfVertices[j]) {
						if (vertice == i) {
							inSubsets = true;
							break;
						}
					}
					if (inSubsets) break;
				}
				for (int vertice : slf.subsetsOfVertices[j]) {
					for (int nieSasiad : this->listaNieSasiedztwa[i]) {
						if (vertice == nieSasiad) {
							vector_coloringQualityDelta[ant][i][nieSasiad] = (1 / (float)slf.numberOfColors);
						}
						else if (vertice < nieSasiad) break;
					}
				}
			}
		}

		ant = this->numberOfAnt;
		for (int i = 0; i < ant; i++) {
			int numberOfColors = vector_numberOfColors[i];
			sumUsedColor += numberOfColors;
			
			if (numberOfColors < this->minNumberOfColor) {
				// std::cout << "\t\t\tREKORD "<< numberOfColors << endl;
				this->bestSubsetsOfVertices = vector_subsetsOfVertices[i];
				this->minNumberOfColor = numberOfColors;
			}
		}
		// std::cout << "\t Avg color: " << (float)sumUsedColor / ant << " czas: " << clock() - timeCycle << " ant: " << ant << endl;
		for (int i = 0; i < this->numberOfVertices; i++) {
			for (int vertice : this->listaNieSasiedztwa[i]) {
				for (int a = 0; a < ant; a++) {
					this->coloringQualityDelta[i][vertice] += vector_coloringQualityDelta[a][i][vertice];
				}
				this->coloringQuality[i][vertice] = this->wyparowywanie * this->coloringQuality[i][vertice] + this->coloringQualityDelta[i][vertice];
			}
		}
	}
}

void ACO_MP::inicjalizationArrayQuality(int numberOfVertices) {
	vector<float> tempFloat(numberOfVertices);
	for (int i = 0; i < numberOfVertices; i++) {
		this->coloringQuality.push_back(tempFloat);
		this->coloringQualityDelta.push_back(tempFloat);
	}
}

void ACO_MP::createListaSasiedztwa() {
	vector<vector<int>> listaSasiedztwa(this->numberOfVertices);
	this->listaSasiedztwa = listaSasiedztwa;
	for (auto edge : this->edges) {
		this->listaSasiedztwa[edge.first].push_back(edge.second);
		this->listaSasiedztwa[edge.second].push_back(edge.first);
	}
}

void ACO_MP::createListaNieSasiedztwa() {
	vector<vector<int>> listaNieSasiedztwa(this->numberOfVertices);
	this->listaNieSasiedztwa = listaNieSasiedztwa;
	for (int i = 0; i < this->numberOfVertices; i++) {
		for (int j = i + 1; j < this->numberOfVertices; j++) {
			bool isSasiad = false;
			for (int sasiad : this->listaSasiedztwa[i]) {
				if (sasiad == j) {
					isSasiad = true;
					break;
				}
			}
			if (!isSasiad) this->listaNieSasiedztwa[i].push_back(j);
		}
	}
}

void ACO_MP::setQualityNieSasiadow() {
	for (int i = 0; i < this->numberOfVertices; i++)
		for (int nieSasiad : this->listaNieSasiedztwa[i])
			this->coloringQuality[i][nieSasiad] = 1;
}

void ACO_MP::setQualityDeltaNieSasiadow() {
	for (int i = 0; i < this->numberOfVertices; i++)
		for (int nieSasiad : this->listaNieSasiedztwa[i])
			this->coloringQualityDelta[i][nieSasiad] = 0;
}

void ACO_MP::getVertexWithTheMostNeighbors() {
	int ver = 0;
	int stopien = 0;
	for (int vertice = 0; vertice < this->numberOfVertices; vertice++) {
		if (this->listaSasiedztwa[vertice].size() > stopien) {
			ver = vertice;
			stopien = this->listaSasiedztwa[vertice].size();
		}
	}
	this->verticewWithTheMostNeighbors = ver;
}
