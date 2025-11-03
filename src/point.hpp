#pragma once
#include <cstdint>
#include <cmath>

struct point {
    int64_t x;
    int64_t y;
    point(int64_t x_coord, int64_t y_coord) : x(x_coord), y(y_coord) {}
    point() : x(0), y(0) {}
    point operator - (const point& other) const {
        return point(x - other.x, y - other.y);
    }
    double dist() const {
        return std::sqrt(x * x + y * y);
    }
};
