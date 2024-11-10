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
            if (!visited[city]) {
                double pheromone = pow(pheromones[currentCity][city], ALPHA);
                double visibility = pow(1.0 / distances[currentCity][city], BETA);
                probabilities[city] = pheromone * visibility;
                total += probabilities[city];
            }
        }

        if (total > 0) {
            for (int i = 0; i < numCities; i++) {
                probabilities[i] /= total;
            }
        }

        double r = dis(gen);
        double sum = 0.0;
        for (int i = 0; i < numCities; i++) {
            if (!visited[i]) {
                sum += probabilities[i];
                if (r <= sum) {
                    return i;
                }
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

    vector<int> solve() {
        vector<int> bestPath;
        double bestLength = numeric_limits<double>::max();

        for (int iteration = 0; iteration < MAX_ITERATIONS; iteration++) {
            for (int ant = 0; ant < NUM_ANTS; ant++) {
                vector<bool> visited(numCities, false);

                int startCity = rand() % numCities;
                antPaths[ant][0] = startCity;
                visited[startCity] = true;

                for (int i = 1; i < numCities; i++) {
                    int currentCity = antPaths[ant][i-1];
                    int nextCity = selectNextCity(ant, currentCity, visited);
                    antPaths[ant][i] = nextCity;
                    visited[nextCity] = true;
                }

                antPathsLength[ant] = calculatePathLength(antPaths[ant]);

                if (antPathsLength[ant] < bestLength) {
                    bestLength = antPathsLength[ant];
                    bestPath = antPaths[ant];
                }
            }

            updatePheromones();
        }
        return bestPath;
    }
};

int main() {
    vector<vector<double>> distances = {
        {0.0, 12.0, 7.0, 15.0, 10.0, 6.0, 14.0, 9.0, 3.0, 18.0},   // Kota 0
        {12.0, 0.0, 8.0, 5.0, 11.0, 16.0, 13.0, 7.0, 4.0, 10.0},   // Kota 1
        {7.0, 8.0, 0.0, 9.0, 17.0, 2.0, 6.0, 14.0, 10.0, 12.0},    // Kota 2
        {15.0, 5.0, 9.0, 0.0, 3.0, 11.0, 8.0, 6.0, 16.0, 13.0},    // Kota 3
        {10.0, 11.0, 17.0, 3.0, 0.0, 4.0, 5.0, 19.0, 7.0, 15.0},   // Kota 4
        {6.0, 16.0, 2.0, 11.0, 4.0, 0.0, 12.0, 8.0, 14.0, 9.0},    // Kota 5
        {14.0, 13.0, 6.0, 8.0, 5.0, 12.0, 0.0, 3.0, 11.0, 7.0},    // Kota 6
        {9.0, 7.0, 14.0, 6.0, 19.0, 8.0, 3.0, 0.0, 15.0, 4.0},     // Kota 7
        {3.0, 4.0, 10.0, 16.0, 7.0, 14.0, 11.0, 15.0, 0.0, 6.0},   // Kota 8
        {18.0, 10.0, 12.0, 13.0, 15.0, 9.0, 7.0, 4.0, 6.0, 0.0}    // Kota 9
    };


    ACO aco(distances);
    vector<int> bestPath = aco.solve();

    cout << "Rute terbaik: ";
    for (int city : bestPath) {
        cout << city << " -> ";
    }
    cout << bestPath[0] << endl;

    return 0;
}