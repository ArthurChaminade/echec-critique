#pragma once
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

class Network {
private:


	
	vector<vector<float>> nodes;
	vector<int> nodesDimensions; //nodesDimensions[layer] = nodes[layer].size()
public:
	int fitness = 0;
	vector<vector<float>> weights; //weight[layer][elementOflayer*nodesDimensions[layer+1] + elementOf(Layer+1)] = weight of link elementOfLayer -> elementOf(Layer+1)
	vector<vector<float>> weightsSum;

	Network(vector<int> nodesDimensions);

	Network(char* fileName);

	void saveToFile(char*);

	void input(vector<float>);

	void randomWeights();

	float calculateOutput();

	Network reproduction(Network);
};