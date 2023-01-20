#include "Network.h"
using namespace std;


Network::Network(vector<int> dimensions) {
	nodesDimensions = dimensions;
	if (nodesDimensions[0] != 285) {
		cout << "WARNING : inputSize differ from 285";
	}
	if (nodesDimensions.back() != 1) {
		cout << "WARNING : outputSize differ from 1";
	}
	nodes = *(new vector<vector<float>>(dimensions.size()));
	weights = *(new vector<vector<float>>(dimensions.size() - 1));
	for (int i = 0; i < dimensions.size(); i++) {
		nodes[i] =  * new vector<float>(dimensions[i],0.f);
		if (i >0) {
			weights[i-1] = *new vector<float>(dimensions[i-1] * dimensions[i]);
		}
	}
}
Network::Network(char* fileName)
{
	
	
	ifstream nnFile(fileName);
	if (nnFile.is_open()) {
		string line;
		if (!getline(nnFile, line)) {
			cout << "ERROR, FILE EMPTY";
			return;
		};
		int nnSize = stoi(line);
		nodesDimensions = *(new vector<int>(nnSize));
		for (int i = 0; i < nnSize; i++) {
			if (!getline(nnFile, line)) {
				cout << "ERROR, FILE IS NOT CONFORM";
				return;
			};
			nodesDimensions[i] = stoi(line);
		}



		nodes = *(new vector<vector<float>>(nnSize));
		weights = *(new vector<vector<float>>(nnSize - 1));
		for (int i = 0; i < nnSize; i++) {
			nodes[i] = *new vector<float>(nodesDimensions[i], 0.f);
			if (i > 0) {
				weights[i - 1] = *new vector<float>(nodesDimensions[i - 1] * nodesDimensions[i]);
				for (int j = 0; j < weights[i - 1].size(); j++) {
					if (!getline(nnFile, line)) {
						cout << "ERROR, FILE EMPTY";
					};
					weights[i - 1][j] = stof(line);
				}
			}
		}
		nnFile.close();
	}

	
}
void Network::saveToFile(char* fileName)
{
	ofstream nnFile(fileName);
	
	if (nnFile.is_open()) {
		nnFile << nodesDimensions.size() << '\n'; //Number of layers on line 1
		for (int i = 0; i < nodesDimensions.size(); i++) {
			nnFile << nodesDimensions[i] << '\n';
		}
		for (int i = 0; i < weights.size(); i++) {
			for (int j = 0; j < weights[i].size(); j++) {
				nnFile << weights[i][j] << '\n';
			}
		}

		nnFile.close();
	}
	else cout << "COULD NOT OPEN FILE";

}
void Network::input(vector<float> input)
{
	nodes[0] = input;
}

float Network::calculateOutput()
{
	for (int layer = 1; layer < nodes.size(); layer++) {
		for (int layerSize = 0; layerSize < nodesDimensions[layer]; layerSize++) {
			nodes[layer][layerSize] = 0.f;
		}
	}

	for (int layer = 1; layer < nodes.size(); layer++) {
		int sublayerSize = nodesDimensions[layer - 1];
		int layerSize = nodesDimensions[layer];
		int compteur = 0;
		vector<float>* w = &weights[layer - 1];
		vector<float>* layerV = &nodes[layer];
		vector<float>* sublayerV = &nodes[layer - 1];

		for (int layerElement = 0; layerElement < layerSize; layerElement++) {
			//pour chaque element de la couche
			auto* element = &((*layerV)[layerElement]);
			for (int sublayerElement = 0; sublayerElement < sublayerSize; sublayerElement++) {
				//On le calcul à partir des poids et de la couche d'avant

				//nodes[layer][layerElement] += nodes[layer - 1][sublayerElement] * weights[layer - 1][compteur];
				*element += (*sublayerV)[sublayerElement] * (*w)[compteur];
				compteur++;

			}

		}

	}
	return nodes.back().back();
}

Network Network::reproduction(Network mother)
{

	Network baby = Network(nodesDimensions);
	for (int i = 0; i < weights.size(); i++) {
		for (int j = 0; j < weights[i].size(); j++) {
			baby.weights[i][j] = max(0.f, min(((weights[i][j] + mother.weights[i][j]) / 2.f) + (0.1f * (float)rand() / RAND_MAX - 0.05f), 2.f));
			
		}
	}
	return baby;
}


void Network::randomWeights() {
	for (int i = 0; i < weights.size(); i++) {
		for (int j = 0; j < weights[i].size(); j++) {
				weights[i][j] = (float) rand()/RAND_MAX;
		}
	}
}
