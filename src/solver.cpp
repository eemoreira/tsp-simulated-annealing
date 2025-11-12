#include <iostream>
#include <vector>
#include <fstream>
#include "point.hpp"
#include <sstream>
#include <numeric>
#include <random>
#include <algorithm>
#include <chrono>
#include <thread>
#include <mutex>
#include "cooler.hpp"

const double EPS = 1e-9;
const double PI = std::acos(-1);

static std::mutex results_mutex;

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
    std::vector<cooler> cooling_schedules;
    double max_dist = 0.0;

    TSPSolver(const std::vector<std::vector<double>>& distMatrix, int _ITER_PER_TEMP = 1)
        : dist(distMatrix), N(dist.size()), ITER_PER_TEMP(_ITER_PER_TEMP) {
            rng = std::mt19937((uint32_t)std::chrono::steady_clock::now().time_since_epoch().count());

            cooling_schedules.emplace_back(
                "Cooling-0",
                [&](double T0, double TN, int current_iter, int max_iter) {
                    return T0 - (T0 - TN) * (current_iter / double(max_iter));
                }
            );
            cooling_schedules.emplace_back(
                "Cooling-1",
                [&](double T0, double TN, int current_iter, int max_iter) {
                    double alpha = std::pow(TN / T0, 1.0 / max_iter);
                    return T0 * std::pow(alpha, current_iter);
                }
            );

            cooling_schedules.emplace_back(
                "Cooling-5",
                [&](double T0, double TN, int current_iter, int max_iter) {
                    return (T0 - TN) / 2 * (1 + std::cos((current_iter / (double)max_iter) * PI)) + TN;
                }
            );

            for (int i = 0; i < N; i++) {
                for (int j = 0; j < N; j++) {
                    max_dist = std::max(max_dist, dist[i][j]);
                }
            }

            for (int i = 0; i < N; i++) {
                for (int j = 0; j < N; j++) {
                    dist[i][j] /= max_dist;
                }
            }

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

    bool shouldAccept(const std::vector<int>& oldTour, const std::vector<int>& newTour, double temperature) {
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

            bool reverse = uniform(0, 1);
            if (i > j) std::swap(i, j);
            if (reverse) {
                std::reverse(tour.begin() + i, tour.begin() + j + 1);
            } else {
                std::vector<int> segment(tour.begin() + i, tour.begin() + j + 1);
                tour.erase(tour.begin() + i, tour.begin() + j + 1);
                int k = uniform(0, int(tour.size()));
                tour.insert(tour.begin() + k, segment.begin(), segment.end());
            }
        }
    }

    void solve(int run_id) {
        for (const auto& cooler : cooling_schedules) {
            work(cooler, run_id);
        }
    }

    void work(const cooler& cooler, int run_id) {

        std::vector<int> tour(N);
        std::iota(tour.begin(), tour.end(), 0);
        shuffle(tour.begin(), tour.end(), rng);
        std::ofstream fout("res/" + cooler.name + "_run_" + std::to_string(run_id) + ".txt");
        if (!fout.is_open()) {
            throw std::runtime_error("Could not open file for writing iterations");
        }

        double T0 = 0.5;
        double TN = 1e-10;
        int n = 1500000.0;
        int it = 0;

        while (it < n) {
            double T = cooler(T0, TN, it, n);
            if (T - TN < EPS) {
                break;
            }

            for (int i = 0; i < ITER_PER_TEMP; i++) {
                std::vector<int> new_tour = tour;
                applyPermutationNoise(new_tour);
                if (shouldAccept(tour, new_tour, T)) {
                    swap(tour, new_tour);
                }
            }
#ifdef LOCAL_DEBUG
            std::cout << "Iteration: " << it << ", T = " << T << ", tourCost: " << tourCost(tour) << '\n';
#else
            fout << it << ' ' << max_dist * tourCost(tour) << '\n';
#endif

            it += 1;
        }

#ifndef LOCAL_DEBUG
         {
            std::lock_guard<std::mutex> lock(results_mutex);
            std::ofstream out;
            out.open("res/results.txt", std::ios_base::app);
            if (!out.is_open()) {
                // não lança exceção dentro do lock por segurança; escreve no cerr
                std::cerr << "Could not open res/results.txt for appending\n";
            } else {
                out << cooler.name << ' ' << max_dist * tourCost(tour) << '\n';
            }
            // out é fechado ao sair do escopo
        }
#endif
    }

};

signed main() {

#ifdef FIFTY_ONE
    auto dist = readMatrixDistanceFromFile("res/tsp-51.txt");
#else
    auto dist = readMatrixDistanceFromFile("res/tsp-100.txt");
#endif

    try {
        const int N_RUNS = 10;
        const int ITER_PER_TEMP = 8;

        std::vector<std::thread> threads;
        threads.reserve(N_RUNS);

        for (int run = 0; run < N_RUNS; ++run) {
            threads.emplace_back([&, run]() {
                TSPSolver solver(dist, ITER_PER_TEMP);
                solver.solve(run);
            });
        }

        for (auto &t : threads) {
            if (t.joinable()) t.join();
        }

    } catch (const std::exception& ex) {
        std::cerr << "Fatal error: " << ex.what() << '\n';
        return 1;
    }

    return 0;
}
