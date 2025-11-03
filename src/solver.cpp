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
    int ITER_PER_TEMP;

    TSPSolver(const std::vector<std::vector<double>>& distMatrix, int _ITER_PER_TEMP = 3)
        : dist(distMatrix), N(dist.size()), ITER_PER_TEMP(_ITER_PER_TEMP) {
            rng = std::mt19937((uint32_t)std::chrono::steady_clock::now().time_since_epoch().count());
        }

    int uniform(int l, int r) {
        return std::uniform_int_distribution(l, r)(rng);
    }

    double uniform(double l, double r) {
        return std::uniform_real_distribution(l, r)(rng);
    }

    double tourCost(const std::vector<int>& tour) {
        double cost = 0.0;
        for (int i = 0; i < N; i++) {
            cost += dist[tour[i]][tour[(i + 1) % N]];
        }
        return cost;
    }

    bool shouldAccept(const std::vector<int> oldTour, const std::vector<int> newTour, double temperature) {
        double oldCost = tourCost(oldTour);
        double newCost = tourCost(newTour);
        if (newCost < oldCost) {
            return true;
        }
        double acceptanceProb = std::exp((oldCost - newCost) / temperature);
        double randomProb = uniform(0.0, 1.0);
        return randomProb < acceptanceProb;
    }

    void applyPermutationNoise(std::vector<int>& tour) {
        int number_swaps = uniform(1, 5);
        while (number_swaps--) {
            int i = uniform(0, N - 1);
            int j = uniform(0, N - 1);
            std::swap(tour[i], tour[j]);
        }
    }

    std::vector<int> solve() {

        std::vector<int> tour(N);
        std::iota(tour.begin(), tour.end(), 0);
        shuffle(tour.begin(), tour.end(), rng);

        double T = 1000.0;
        double T_min = 1e-8;
        double decay = 0.998;

        std::cout << "Current temperature: " << T << ", Current tour cost: " << tourCost(tour) << '\n';
        while (true) {
            for (int i = 0; i < ITER_PER_TEMP; i++) {
                std::vector<int> new_tour = tour;
                applyPermutationNoise(new_tour);
                if (shouldAccept(tour, new_tour, T)) {
                    swap(tour, new_tour);
                }
            }
            T *= decay;
            if (T < T_min) {
                break;
            }
        }
        std::cout << "Current temperature: " << T << ", Current tour cost: " << tourCost(tour) << '\n';

        return tour;
    }

};

signed main() {

    auto dist = readMatrixDistanceFromFile("res/tsp-51.txt");
    TSPSolver solver(dist, 10);
    auto best_tour = solver.solve();

    return 0;
}
