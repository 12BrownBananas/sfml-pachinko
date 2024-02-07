#include <SFML/Graphics.hpp>
namespace ViewUtils {
    sf::Vector2f constrainToWindowDimensions(sf::Window &window, float x, float y);
    sf::Vector2f constrainToWindowDimensions(sf::Window &window, sf::Vector2f pos);
}