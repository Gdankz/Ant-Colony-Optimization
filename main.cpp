#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <random>

using namespace std;

// Konstanta untuk parameter ACO
const int NUM_ANTS = 4;                   // Jumlah semut
const int MAX_ITERATIONS = 10;            // Jumlah iterasi
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

    // Generator angka acak
    random_device rd;
    mt19937 gen;
    uniform_real_distribution<> dis;

public:
    // Konstruktor untuk inisialisasi data dan feromon awal
    ACO(const vector<vector<double>>& distanceMatrix) :
    gen(rd()),
    dis(0.0, 1.0) {
        numCities = distanceMatrix.size();
        distances = distanceMatrix;

        // Inisialisasi matriks feromon dengan nilai awal
        pheromones = vector<vector<double>>(numCities, vector<double>(numCities, INITIAL_PHEROMONE));
        antPaths = vector<vector<int>>(NUM_ANTS, vector<int>(numCities));
        antPathsLength = vector<double>(NUM_ANTS);
    }

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

        // Pemilihan kota berdasarkan probabilitas
        double r = dis(gen); // Angka acak antara 0 dan 1
        double sum = 0.0;
        for (int i = 0; i < numCities; i++) {
            if (!visited[i]) {
                sum += probabilities[i];
                if (r <= sum) {
                    return i;
                }
            }
        }

        // Jika semua kota sudah dikunjungi
        for (int i = 0; i < numCities; i++) {
            if (!visited[i]) return i;
        }

        return -1;
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

    // Fungsi untuk memperbarui feromon di setiap jalur
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
                int city2 = antPaths[ant][i+1];
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

    // Fungsi utama untuk menjalankan algoritma ACO
    vector<int> solve() {
        vector<int> bestPath;                  // Rute terbaik ditemukan
        double bestLength = numeric_limits<double>::max();

        // Iterasi utama algoritma ACO
        for (int iteration = 0; iteration < MAX_ITERATIONS; iteration++) {
            for (int ant = 0; ant < NUM_ANTS; ant++) {
                vector<bool> visited(numCities, false);

                // Pilih kota awal secara acak untuk setiap semut
                int startCity = rand() % numCities;
                antPaths[ant][0] = startCity;
                visited[startCity] = true;

                // Bangun rute untuk semut
                for (int i = 1; i < numCities; i++) {
                    int currentCity = antPaths[ant][i-1];
                    int nextCity = selectNextCity(ant, currentCity, visited);
                    antPaths[ant][i] = nextCity;
                    visited[nextCity] = true;
                }

                // Hitung panjang rute yang diambil semut
                antPathsLength[ant] = calculatePathLength(antPaths[ant]);

                // Periksa apakah rute ini adalah rute terbaik
                if (antPathsLength[ant] < bestLength) {
                    bestLength = antPathsLength[ant];
                    bestPath = antPaths[ant];
                }
            }

            // Perbarui feromon setelah semua semut selesai
            updatePheromones();
        }
        return bestPath;
    }
};

int main() {
    // Matriks jarak antar kota (contoh untuk 10 kota)
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

    // Buat objek ACO dan temukan rute terbaik
    ACO aco(distances);
    vector<int> bestPath = aco.solve();

    // Tampilkan rute terbaik
    cout << "Rute terbaik: ";
    for (int city : bestPath) {
        cout << city << " -> ";
    }
    cout << bestPath[0] << endl; // Kembali ke kota awal

    return 0;
}
