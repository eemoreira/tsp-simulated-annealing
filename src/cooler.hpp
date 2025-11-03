#pragma once
#include <string>
#include <functional>

struct cooler {
    std::string name;
    std::function<double(double, double, int, int)> schedule;
    cooler() {}
    cooler(
        const std::string& _name,
        const std::function<double(double, double, int, int)>& _schedule
    ) : name(_name), schedule(_schedule) {}

    double operator()( double T0, double TN, int current_iter, int max_iter) const {
        return schedule(T0, TN, current_iter, max_iter);
    }
};
