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
    vector<vector<int>> antPaths;
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
        antPaths = vector<vector<int>>(NUM_ANTS, vector<int>(numCities));
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

        if (total > 0) {
            for (int i = 0; i < numCities; i++) {
                probabilities[i] /= total;
            }
        }

        double r = dis(gen);
        double sum = 0.0;
        for (int i = 0; i < numCities; i++) {
            sum += probabilities[i];
            if (r <= sum && !visited[i]) {
                return i;
            }
        }

        for (int i = 0; i < numCities; i++) {
            if (!visited[i]) return i;
        }

        return -1;
    }

    double calculatePathLength(const vector<int>& path) {
        double length = 0.0;
        for (int i = 0; i < numCities - 1; i++) {
            length += distances[path[i]][path[i + 1]];
        }
        length += distances[path[numCities - 1]][path[0]];
        return length;
    }

    void updatePheromones() {
        for (int i = 0; i < numCities; i++) {
            for (int j = 0; j < numCities; j++) {
                pheromones[i][j] *= (1.0 - RHO);
            }
        }

        for (int ant = 0; ant < NUM_ANTS; ant++) {
            double contribution = 1.0 / antPathsLength[ant];
            for (int i = 0; i < numCities - 1; i++) {
                int city1 = antPaths[ant][i];
                int city2 = antPaths[ant][i+1];
                pheromones[city1][city2] += contribution;
                pheromones[city2][city1] += contribution;
            }
            int last = antPaths[ant][numCities - 1];
            int first = antPaths[ant][0];
            pheromones[last][first] += contribution;
            pheromones[first][last] += contribution;
        }
    }

};