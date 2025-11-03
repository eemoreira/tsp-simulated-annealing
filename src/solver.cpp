#include <iostream>
#include <vector>
#include <fstream>
#include "point.hpp"
#include <sstream>
#include <numeric>
#include <random>
#include <algorithm>
#include <chrono>

std::vector<std::vector<double>> readMatrixDistanceFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file");
    }

    std::string line;
    std::vector<point> points;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string id_str, x_str, y_str;

        if (!(iss >> id_str >> x_str >> y_str)) { 
            throw std::runtime_error("Invalid line format");
        }

        int64_t x = std::stoll(x_str);
        int64_t y = std::stoll(y_str);
        points.emplace_back(x, y);
    }

    int n = int(points.size());
    std::vector dist(n, std::vector<double>(n, 0.0));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < i; j++) {
            dist[i][j] = dist[j][i] = (points[i] - points[j]).dist();
        }
    }
    return dist;
}

struct TSPSolver {
    std::vector<std::vector<double>> dist;
    std::mt19937 rng;
    int N;
    int NUM_ITER;

    TSPSolver(const std::vector<std::vector<double>>& distMatrix, int _NUM_ITER = 10000)
        : dist(distMatrix), N(dist.size()), NUM_ITER(_NUM_ITER) {
            rng = std::mt19937((uint32_t)std::chrono::steady_clock::now().time_since_epoch().count());
        }

    int uniform(int l, int r) {
        return std::uniform_int_distribution(l, r)(rng);
    }

    double tourCost(const std::vector<int>& tour) {
        double cost = 0.0;
        for (int i = 0; i < N; i++) {
            cost += dist[tour[i]][tour[(i + 1) % N]];
        }
        return cost;
    }

    void applyPermutationNoise(std::vector<int>& tour) {
        int number_swaps = uniform(1, 5);
        while (number_swaps--) {
            int i = uniform(0, N - 1);
            int j = uniform(0, N - 1);
            std::swap(tour[i], tour[j]);
        }
    }

    std::vector<point> solve() {

        std::vector<int> tour(N);
        std::iota(tour.begin(), tour.end(), 0);
        shuffle(tour.begin(), tour.end(), rng);

        return {};
    }

};

signed main() {

    auto dist = readMatrixDistanceFromFile("res/tsp-51.txt");
    for (int i = 0; i < int(dist.size()); i++) {
        for (int j = 0; j < int(dist.size()); j++) {
            std::cout << dist[i][j] << ' ';
        }
        std::cout << '\n';
    }

    return 0;
}
