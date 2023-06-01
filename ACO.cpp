#include "ACO.h"

ACO::ACO(vector<pair<int, int>> edges, int numberOfVertices, float wyparowywanie, int maxNumberOfCycle, int maxNumberOfAnt, int maxTimeOfCycleInMs, int maxTimeProgramInMs, float alfa, float beta) {
	
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

	for (int cycle = 0; cycle < this->numberOfCycle  && timeProgram + maxTimeProgramInMs > clock(); cycle++) {
		clock_t timeCycle = clock();
		// cout <<"Cykl "<< cycle << endl;
		this->setQualityDeltaNieSasiadow();
		int ant, sumUsedColor = 0;
		for (ant = 0; ant < this->numberOfAnt && timeCycle + maxTimeOfCycleInMs > clock(); ant++) {
			//cout << "\tAnt " << ant << endl;
			AntSLF slf(this->listaSasiedztwa, this->numberOfVertices, this->coloringQuality, this->verticewWithTheMostNeighbors, alfa, beta);
			sumUsedColor += slf.numberOfColors;
			//cout << "Wynik ant: " << slf.numberOfColors << endl;
			if (slf.numberOfColors < this->minNumberOfColor) {
				// cout << "\t\t\tREKORD " << slf.numberOfColors<<endl;
				this->bestSubsetsOfVertices = slf.subsetsOfVertices;
				this->minNumberOfColor = slf.numberOfColors;
			}
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
						if (vertice == nieSasiad) this->coloringQualityDelta[i][nieSasiad] += (1 / (float)slf.numberOfColors);
						else if (vertice < nieSasiad) break;
					}
				}
			}
		}
		// cout << "\t Avg color: " << (float)sumUsedColor / ant << " czas: " << clock() - timeCycle << " ant: " << ant << endl;
		for (int i = 0; i < this->numberOfVertices; i++) {
			for (int vertice : this->listaNieSasiedztwa[i]) {
				this->coloringQuality[i][vertice] = this->wyparowywanie * this->coloringQuality[i][vertice] + this->coloringQualityDelta[i][vertice];
				//cout << i<<" "<<vertice<<": "<<this->coloringQuality[i][vertice] << endl;
			}
		}
	}
}

void ACO::inicjalizationArrayQuality(int numberOfVertices) {
	vector<float> tempFloat(numberOfVertices);
	for (int i = 0; i < numberOfVertices; i++) {
		this->coloringQuality.push_back(tempFloat);
		this->coloringQualityDelta.push_back(tempFloat);
	}
}

void ACO::createListaSasiedztwa() {
	vector<vector<int>> listaSasiedztwa(this->numberOfVertices);
	this->listaSasiedztwa = listaSasiedztwa;
	for (auto edge : this->edges) {
		this->listaSasiedztwa[edge.first].push_back(edge.second);
		this->listaSasiedztwa[edge.second].push_back(edge.first);
	}
}

void ACO::createListaNieSasiedztwa() {
	vector<vector<int>> listaNieSasiedztwa(this->numberOfVertices);
	this->listaNieSasiedztwa = listaNieSasiedztwa;
	for (int i = 0; i < this->numberOfVertices; i++) {
		for (int j = i+1; j < this->numberOfVertices; j++) {
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

void ACO::setQualityNieSasiadow() {
	for (int i = 0; i < this->numberOfVertices; i++)
		for (int nieSasiad : this->listaNieSasiedztwa[i])
			this->coloringQuality[i][nieSasiad] = 1;
}

void ACO::setQualityDeltaNieSasiadow() {
	for (int i = 0; i < this->numberOfVertices; i++)
		for (int nieSasiad : this->listaNieSasiedztwa[i])
			this->coloringQualityDelta[i][nieSasiad] = 0;
}

void ACO::getVertexWithTheMostNeighbors() {
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
