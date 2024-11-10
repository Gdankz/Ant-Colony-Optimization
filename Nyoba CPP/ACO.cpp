#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <random>

using namespace std;

const int NUM_ANTS = 4;
const int MAX_ITERATIONS = 10;
const double ALPHA = 1.0;
const double BETA = 1.0;
const double RHO = 0.15;
const double INITIAL_PHEROMONE = 1.0;

class ACO {
private:
    int numCities;
    vector<vector<double>> distances;
    vector<vector<double>> pheromones;
    vector<vector<int>> antpaths;
    vector<double> antPathsLength;

    random_device rd;
    mt19937 gen;
    uniform_real_distribution<> dis;

public:
    ACO(const vector<vector<double>>& distanceMatrix) :
    gen(rd()),
    dis(0.0,1.0) {
        numCities = distanceMatrix.size();
        distances = distanceMatrix;

        pheromones = vector<vector<double>>(numCities, vector<double>(numCities, INITIAL_PHEROMONE));
        antpaths = vector<vector<int>>(NUM_ANTS, vector<int>(numCities));
        antPathsLength = vector<double>(NUM_ANTS);
    }

    int selectNextCity(int ant, int currentCity, vector<bool>& visited) {
        vector<double> probabilities(numCities, 0.0);
        double total = 0.0;

        for (int city = 0; city < numCities; city++) {
            double pheromone = pow(pheromones[currentCity][city], ALPHA);
            double visibility = pow(1.0/distances[currentCity][city], BETA);
            probabilities[city] = pheromone * visibility;
            total += probabilities[city];
        }
};