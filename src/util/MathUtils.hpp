#include <SFML/Graphics.hpp>
namespace MathUtils {
    float point_distance(float x1, float y1, float x2, float y2);
    sf::Vector2f vectorize_velocity(float velocity, float dirInRad);
    float degtorad(float degrees);
    float radtodeg(float radians);
    float get_direction(sf::Vector2f velocityVector);
    int range_rand(int min, int max);

    template <typename T> int sign(T val);
}