#define _USE_MATH_DEFINES

#include "src/util/MathUtils.hpp"
#include <cmath>
#include <math.h>

namespace MathUtils {
    float point_distance(float x1, float y1, float x2, float y2) {
        return std::sqrt(std::pow(x2-x1, 2)+std::pow(y2-y1, 2)*1.0);
    }

    sf::Vector2f vectorize_velocity(float velocity, float dirInRad) {
        return sf::Vector2f{velocity*std::cos(dirInRad), velocity*std::sin(dirInRad)};
    }

    float degtorad(float degrees) {
        return degrees*3.14159265f/180;
    }

    float radtodeg(float radians) {
        return radians*180/3.14159265f;
    }

    float get_direction(sf::Vector2f velocityVector) {
        return std::atan2(velocityVector.y, velocityVector.x);
    }

    template <typename T> int sign(T val) {
        return (T(0) < val) - (val < T(0));
    }

    int range_rand(int min, int max) {
        int n = max - min + 1;
        int remainder = RAND_MAX % n;
        int x;
        do{
            x = rand();
        }while (x >= RAND_MAX - remainder);
        return min + x % n;
    }
}