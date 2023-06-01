#include "ACO_MP.h"
#include "ACO.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <ctime>
#include <cstdlib>
#include <math.h>
#include <chrono>
#include <omp.h>

using namespace std::chrono;
using namespace std;

pair<vector<pair<int, int>>, int> getData() {
    int numberOfVertice;
    fstream file;
    file.open("gc500.txt", ios::in); 
    string k;
    getline(file, k);
    numberOfVertice = stoi(k);
    
    vector<pair<int, int>> v;
    while (getline(file, k)) {
        int l1 = stoi(k.substr(0, k.find(" ")))-1, l2 = stoi(k.substr(k.find(" ") + 1))-1;
        if(l1 < l2) v.push_back(make_pair(l1, l2));
    }
    return make_pair(v, numberOfVertice);
}

int main() {
    #ifdef _OPENMP
        std::cout << "OpenMP version: " << _OPENMP << std::endl;
    #else
        std::cout << "OpenMP is not supported." << std::endl;
    #endif
    srand(time(NULL));

    pair<vector<pair<int, int>>, int> r = getData();
    vector<pair<int, int>> edges = r.first;
    int numverOfVertive = r.second;

    int v[8] = { 250, 500, 750, 1000, 1250, 1500, 1750, 2000 };

    for (int i = 0; i < 8; i++) {
        auto start = high_resolution_clock::now();
        ACO_MP k(edges, numverOfVertive, 0.4, 1, v[i], 10*60*1000, 10 * 60 * 1000, 2.25, 1);
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        
        auto start2 = high_resolution_clock::now();
        ACO k2(edges, numverOfVertive, 0.4, 1, v[i], 10 * 60 * 1000*1000, 10 * 60 * 1000, 2.25, 1);
        auto stop2 = high_resolution_clock::now();
        auto duration2 = duration_cast<microseconds>(stop2 - start2);
        cout << v[i] << " " << duration.count() << " " << duration2.count() << endl;
    }
    return 1;
}
