#include "AntSLF.h"

AntSLF::AntSLF(vector<vector<int>> listaSasiedztwa, int numberOfVertices, vector<vector<float>> coloringQuality, int verticewWithTheMostNeighbors, float alfa, float beta) {
	this->numberOfColors = 1;
	
	this->coloringQuality = coloringQuality;
	this->listaSasiedztwa = listaSasiedztwa;
	this->numberOfVertices = numberOfVertices;
	this->alfa = alfa;
	this->beta = beta;

	this->arraysInicjalization(numberOfVertices);

	int color = 0;
	int verticesToColoring = verticewWithTheMostNeighbors;
	this->coloringVertice(verticesToColoring, color);
	//cout << "\tChoosed vertice: " << verticesToColoring << " color: " << color << endl;
	while(this->verticesWithoutColor.size() > 1) {
		this->updateCminAndDsat(verticesToColoring, color);
		/*for (int v : this->verticesWithoutColor) {
			cout << "\t\t\t\t"<<v<<" cmin: " << this->c_min[v] << " dsat : " << this->dsat[v] << endl;
		}*/
		this->deleteColoredVerticeFromListVerticesWithoutColor(verticesToColoring);
		this->updateSumColoringQuality(verticesToColoring, color);
		verticesToColoring = this->chooseVertice();//wybranie wierzchołka v 1. strategią
		color = this->c_min[verticesToColoring];//wybranie koloru 1. strategią
		//cout << "\tChoosed vertice: " << verticesToColoring << " color: " << color << endl;
		this->coloringVertice(verticesToColoring, color);
		if (color == this->numberOfColors) this->numberOfColors += 1;
	}
}

int AntSLF::chooseVertice() {
	vector<float> p(this->verticesWithoutColor.size());
	float mianownik = 0;
	
	for (int i = 0; i < this->verticesWithoutColor.size(); i++) {
		p[i] = pow(this->chooseVertice_calculateT1(this->verticesWithoutColor[i]), this->alfa);
		p[i] *= pow(this->chooseVertice_calculateN1(this->verticesWithoutColor[i]), this->beta);
		//cout << "\t\t" << this->verticesWithoutColor[i] << " " << p[i] <<"("<< this->verticesSumColoringQuality[this->verticesWithoutColor[i]]<<" ("<< this->chooseVertice_calculateT1(this->verticesWithoutColor[i]) << "))" << endl;;
		//cout << "\t\t\tp=" << this->chooseVertice_calculateT1(this->verticesWithoutColor[i]) << "^" << alfa << "*" << this->chooseVertice_calculateN1(this->verticesWithoutColor[i]) << "^" << beta << "=" << pow(this->chooseVertice_calculateT1(this->verticesWithoutColor[i]), alfa) << "*" << pow(this->chooseVertice_calculateN1(this->verticesWithoutColor[i]), beta) << endl;
		
		//if (p[i] < 1 / 10000) p[i] = 0;
		mianownik += p[i];
	}
	int randomInt = rand() % 10000;
	float random = (float)randomInt / 10000;
	//cout << "\t\trandom: " << random << endl;
	float sum = 0;
	for (int i = 0; i < p.size(); i++) {
		sum += p[i] / mianownik;
		if (sum >= random) return this->verticesWithoutColor[i];
	}
	return this->verticesWithoutColor[0];
}

int AntSLF::chooseVertice_calculateN1(int vertice) {
	return this->dsat[vertice];
}

float AntSLF::chooseVertice_calculateT1(int vertice) {
	return this->chooseVertice_calculateT2(vertice, this->c_min[vertice]);
}

float AntSLF::getSumColoringQuality(int vertice, int color) {
	float sum = 0;
	for (int ver : this->subsetsOfVertices[color]) {
		if (vertice < ver) sum += this->coloringQuality[vertice][ver];
		else sum += this->coloringQuality[ver][vertice];
	}
	return sum;
}

void AntSLF::updateSumColoringQuality(int vertice, int color) {
	for (int verticeWithoutColor : this->verticesWithoutColor) {
		if (c_min[verticeWithoutColor] == color) {
			float quality;
			if (vertice > verticeWithoutColor) quality = this->coloringQuality[verticeWithoutColor][vertice];
			else quality = this->coloringQuality[vertice][verticeWithoutColor];
			verticesSumColoringQuality[verticeWithoutColor] += quality;
		}
	}
}

void AntSLF::updateCminAndDsat(int verticeToColoring, int color) {
	for (int indexVertice : this->listaSasiedztwa[verticeToColoring]) {
		if (listSetColorVertices[indexVertice] != -1) continue; // czy wierzchołek jest już pokolorowany
		if (this->listUsedColorByNeigbours[indexVertice][color]) continue; // czy ten kolor jest już użyty przez sąsiada wierzchołka, jeżeli tak to nie zmieni się c_min i dsat
		this->listUsedColorByNeigbours[indexVertice][color] = true;
		this->dsat[indexVertice] += 1;
		if (this->c_min[indexVertice] == color) { //czy kolor użyty w sąsiednim wierzchołku był minimalny dla tego wierzchołka
			for (int i = color+1; i < this->listUsedColorByNeigbours[indexVertice].size(); i++) { //szukanie nowego c_min
				if (this->listUsedColorByNeigbours[indexVertice][i] == false) {
					this->c_min[indexVertice] = i;
					this->verticesSumColoringQuality[indexVertice] = this->getSumColoringQuality(indexVertice, i);
					break;
				}
			}
		}
	}
}

float AntSLF::chooseVertice_calculateT2(int vertice, int color) {
	if (this->subsetsOfVertices[color].size() == 0) return 1;
	else return this->verticesSumColoringQuality[vertice] / this->subsetsOfVertices[color].size();
}

void AntSLF::arraysInicjalization(int numberOfVertices) {
	vector<bool> tempBool(numberOfVertices);
	vector<int> tempInt(numberOfVertices);
	vector<float> tempFloat(numberOfVertices);
	vector<vector<int>> tempVectorInt(numberOfVertices);
	
	this->c_min = tempInt;
	this->dsat = tempInt;
	this->listSetColorVertices = tempInt;
	this->verticesSumColoringQuality = tempFloat;
	this->subsetsOfVertices = tempVectorInt;

	for (int i = 0; i < numberOfVertices; i++) {
		this->listSetColorVertices[i] = -1;
		this->verticesWithoutColor.push_back(i);
		this->listUsedColorByNeigbours.push_back(tempBool);
	}
}

void AntSLF::coloringVertice(int indexVertice, int color) {
	this->subsetsOfVertices[color].push_back(indexVertice);
	this->listSetColorVertices[indexVertice] = color;
}

void AntSLF::deleteColoredVerticeFromListVerticesWithoutColor(int verticeToDelete) {
	for (int j = 0; j < this->verticesWithoutColor.size(); j++) {
		if (this->verticesWithoutColor[j] == verticeToDelete) {
			this->verticesWithoutColor.erase(this->verticesWithoutColor.begin() + j);
			return;
		}
	}
}
