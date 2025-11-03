#include <iostream>
#include <vector>
#include <fstream>
#include "point.hpp"
#include <sstream>

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

    TSPSolver(const std::vector<std::vector<double>>& distMatrix)
        : dist(distMatrix) {}

    std::vector<point> solve() {
        // Placeholder for TSP solving logic
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
