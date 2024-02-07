#include <SFML/Graphics.hpp>
#include "src/util/ViewUtils.hpp"
#include <algorithm>

namespace ViewUtils {
    sf::Vector2f constrainToWindowDimensions(sf::Window &window, float x, float y) {
        auto sz = window.getSize();
        return sf::Vector2f{std::max(0.f, std::min(static_cast<float>(sz.x), x)), std::max(0.f, std::min(static_cast<float>(sz.y), y))};
    }
    sf::Vector2f constrainToWindowDimensions(sf::Window &window, sf::Vector2f pos) {
        auto sz = window.getSize();
        return sf::Vector2f{std::max(0.f, std::min(static_cast<float>(sz.x), pos.x)), std::max(0.f, std::min(static_cast<float>(sz.y), pos.y))};
    }
}