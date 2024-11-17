#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <random>
#include <iomanip>

using namespace std;

// Konstanta untuk parameter ACO
const int NUM_ANTS = 2;                   // Jumlah semut
const int MAX_ITERATIONS = 3;            // Jumlah iterasi
const double ALPHA = 1.0;                 // Parameter pentingnya feromon
const double BETA = 1.0;                  // Parameter pentingnya visibilitas
const double RHO = 0.15;                  // Tingkat evaporasi feromon
const double INITIAL_PHEROMONE = 1.0;     // Feromon awal pada tiap jalur

class ACO {
private:
    int numCities;                            // Jumlah kota
    vector<vector<double>> distances;         // Matriks jarak antar kota
    vector<vector<double>> pheromones;        // Matriks feromon untuk setiap jalur
    vector<vector<int>> antPaths;             // Rute yang ditempuh tiap semut
    vector<double> antPathsLength;            // Panjang rute yang ditempuh tiap semut
    vector<string> cityNames;

    // Generator angka acak
    random_device rd;
    mt19937 gen;
    uniform_real_distribution<> dis;

public:
    ACO(const vector<vector<double>>& distanceMatrix, const vector<string>& cityNamesInput) :
        gen(rd()),
        dis(0.0, 1.0),
        cityNames(cityNamesInput) {
        numCities = distanceMatrix.size();
        distances = distanceMatrix;
        pheromones = vector<vector<double>>(numCities, vector<double>(numCities, INITIAL_PHEROMONE));
        antPaths = vector<vector<int>>(NUM_ANTS, vector<int>(numCities));
        antPathsLength = vector<double>(NUM_ANTS);
    }

    // Fungsi lainnya tetap sama...

    // Fungsi untuk memilih kota berikutnya berdasarkan probabilitas feromon dan visibilitas
    int selectNextCity(int ant, int currentCity, vector<bool>& visited) {
        vector<double> probabilities(numCities, 0.0);
        double total = 0.0;

        // Hitung probabilitas untuk kota yang belum dikunjungi
        for (int city = 0; city < numCities; city++) {
            if (!visited[city]) {
                double pheromone = pow(pheromones[currentCity][city], ALPHA);
                double visibility = pow(1.0 / distances[currentCity][city], BETA);
                probabilities[city] = pheromone * visibility;
                total += probabilities[city];
            }
        }

        // Normalisasi probabilitas
        if (total > 0) {
            for (int i = 0; i < numCities; i++) {
                probabilities[i] /= total;
            }
        }

        // Tampilkan probabilitas
        cout << "    Dari kota " << cityNames[currentCity] << " ke:\n";
        for (int i = 0; i < numCities; i++) {
            if (!visited[i]) {
                cout << "        Kota " << cityNames[i] << " memiliki probabilitas " << probabilities[i] << endl;
            }
        }

        // Pemilihan kota berdasarkan probabilitas
        double r = dis(gen); // Angka acak antara 0 dan 1
        double sum = 0.0;
        cout << "\n    Nilai random (r): " << r << endl;

        for (int i = 0; i < numCities; i++) {
            if (!visited[i]) {
                sum += probabilities[i];
                if (r <= sum) {
                    cout << "        Kota " << cityNames[i] << " dipilih (r <= kumulatif).\n\n";
                    return i;
                }
            }
        }
    }

    // Fungsi untuk menghitung panjang total dari suatu rute
    double calculatePathLength(const vector<int>& path) {
        double length = 0.0;
        for (int i = 0; i < numCities - 1; i++) {
            length += distances[path[i]][path[i + 1]];
        }
        length += distances[path[numCities - 1]][path[0]]; // Kembali ke kota awal
        return length;
    }

    void updatePheromones() {
        // Evaporasi feromon
        for (int i = 0; i < numCities; i++) {
            for (int j = 0; j < numCities; j++) {
                pheromones[i][j] *= (1.0 - RHO);
            }
        }

        // Tambahkan feromon berdasarkan panjang rute
        for (int ant = 0; ant < NUM_ANTS; ant++) {
            double contribution = 1.0 / antPathsLength[ant];
            for (int i = 0; i < numCities - 1; i++) {
                int city1 = antPaths[ant][i];
                int city2 = antPaths[ant][i + 1];
                pheromones[city1][city2] += contribution;
                pheromones[city2][city1] += contribution;
            }
            // Kembali ke kota awal
            int last = antPaths[ant][numCities - 1];
            int first = antPaths[ant][0];
            pheromones[last][first] += contribution;
            pheromones[first][last] += contribution;
        }
    }

    // Fungsi untuk menampilkan matriks feromon
    void drawFeromone() {
        cout << "\n===== Matriks Feromon =====\n";
        for (int i = 0; i < numCities; i++) {
            for (int j = 0; j < numCities; j++) {
                cout << setw(8) << fixed << setprecision(4) << pheromones[i][j] << " ";
            }
            cout << endl;
        }
        cout << "===========================\n";
    }

    void drawPaths() {
        cout << "\n===== Rute yang Ditempuh oleh Semut =====\n";
        for (int ant = 0; ant < NUM_ANTS; ant++) {
            cout << "Semut ke-" << (ant + 1) << ": ";
            for (int i = 0; i < numCities; i++) {
                cout << cityNames[antPaths[ant][i]];
                if (i < numCities - 1) cout << " -> ";
            }
            cout << " -> " << cityNames[antPaths[ant][0]]; // Kembali ke kota awal
            cout << " | Panjang rute: " << antPathsLength[ant] << endl;
        }
        cout << "=========================================\n";
    }


    void solve(int startCity) {
        drawFeromone();
        vector<int> bestPath;
        double bestLength = numeric_limits<double>::max();

        for (int iteration = 0; iteration < MAX_ITERATIONS; iteration++) {
            cout << "==================== Iterasi " << (iteration + 1) << " ====================" << endl;
            for (int ant = 0; ant < NUM_ANTS; ant++) {
                vector<bool> visited(numCities, false);
                antPaths[ant][0] = startCity;
                visited[startCity] = true;

                for (int i = 1; i < numCities; i++) {
                    int currentCity = antPaths[ant][i - 1];
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
            drawFeromone();
            drawPaths();
        }

        cout << "\nRute terbaik yang berhasil ditempuh: ";
        for (int city : bestPath) {
            cout << cityNames[city] << " -> ";
        }
        cout << cityNames[bestPath[0]] << endl;
    }
};

int main() {
    vector<vector<double>> distances = {
        {0, 2790, 790, 1627, 2145, 96, 1781, 2777, 1374, 2565},
        {2790, 0, 2015, 1374, 373, 2713, 1202, 120, 1435, 340},
        {790, 2015, 0, 1085, 1753, 668, 1240, 2067, 967, 1833},
        {1627, 1374, 1085, 0, 1015, 1543, 197, 1304, 239, 1632},
        {2145, 373, 1753, 1015, 0, 2085, 982, 355, 888, 654},
        {96, 2713, 668, 1543, 2085, 0, 1570, 2697, 1300, 2483},
        {1781, 1202, 1240, 197, 982, 1570, 0, 1180, 274, 1465},
        {2777, 120, 2067, 1304, 355, 2697, 1180, 0, 1360, 415},
        {1374, 1435, 967, 239, 888, 1300, 274, 1360, 0, 1433},
        {2565, 340, 1833, 1632, 654, 2483, 1465, 415, 1433, 0}
    };

    vector<string> cityNames = {
        "New York", "Los Angeles", "Chicago", "Houston", "Phoenix",
        "Philadelphia", "San Antonio", "San Diego", "Dallas", "San Jose"
    };

    int startCity;

    while (startCity != 10) {
        for (int i = 0; i < cityNames.size(); i++) {
            cout << i << ". " << cityNames[i] << endl;
        }

        cout << "Masukkan indeks kota awal (0 - 9): ";

        cin >> startCity;

        if (startCity < 0 || startCity >= cityNames.size()) {
            cerr << "Kota awal tidak valid!" << endl;
            return 1;

        }
        ACO aco(distances, cityNames);
        aco.solve(startCity);
        cout << endl;
    }



    return 0;
}
