#include <SFML/Graphics.hpp>
#include <iostream>
#include <src/util/InputManager.cpp>
#include <src/ui/Circle.cpp>
#include <algorithm>
#include <random>

std::shared_ptr<ScreenspacePositionConstraint> createWindowConstraint(float radius, float width, float height) {
    return std::shared_ptr<ScreenspacePositionConstraint>(new ScreenspacePositionConstraint{-radius*4.f, -radius*4.f, width+radius*4.f, height+radius*4.f});
}
int main()
{
    auto wwidth = 540.f;
    auto wheight = 960.f;
    auto pegrad = 10.f;
    sf::RenderWindow window(sf::VideoMode(wwidth, wheight), "Pachinko");
    window.setFramerateLimit(60);

    /* row 1 */
    auto shape1 = std::shared_ptr<Circle>(new Circle{pegrad, 80.f, 100.f});
    auto shape2 = std::shared_ptr<Circle>(new Circle{pegrad, 180.f, 100.f});
    auto shape3 = std::shared_ptr<Circle>(new Circle{pegrad, 280.f, 100.f});
    auto shape4 = std::shared_ptr<Circle>(new Circle{pegrad, 380.f, 100.f});
    auto shape5 = std::shared_ptr<Circle>(new Circle{pegrad, 480.f, 100.f});
    
    /* row 2 */
    auto shape6 = std::shared_ptr<Circle>(new Circle{pegrad, 40.f, 300.f});
    auto shape7 = std::shared_ptr<Circle>(new Circle{pegrad, 140.f, 300.f});
    auto shape8 = std::shared_ptr<Circle>(new Circle{pegrad, 240.f, 300.f});
    auto shape9 = std::shared_ptr<Circle>(new Circle{pegrad, 340.f, 300.f});
    auto shape10 = std::shared_ptr<Circle>(new Circle{pegrad, 440.f, 300.f});
    auto shape11 = std::shared_ptr<Circle>(new Circle{pegrad, 540.f, 300.f});

    /* row 3 */
    auto shape12 = std::shared_ptr<Circle>(new Circle{pegrad, 80.f, 500.f});
    auto shape13 = std::shared_ptr<Circle>(new Circle{pegrad, 180.f, 500.f});
    auto shape14 = std::shared_ptr<Circle>(new Circle{pegrad, 280.f, 500.f});
    auto shape15 = std::shared_ptr<Circle>(new Circle{pegrad, 380.f, 500.f});
    auto shape16 = std::shared_ptr<Circle>(new Circle{pegrad, 480.f, 500.f});

    /* row 4 */
    auto shape17 = std::shared_ptr<Circle>(new Circle{pegrad, 40.f, 700.f});
    auto shape18 = std::shared_ptr<Circle>(new Circle{pegrad, 140.f, 700.f});
    auto shape19 = std::shared_ptr<Circle>(new Circle{pegrad, 240.f, 700.f});
    auto shape20 = std::shared_ptr<Circle>(new Circle{pegrad, 340.f, 700.f});
    auto shape21 = std::shared_ptr<Circle>(new Circle{pegrad, 440.f, 700.f});
    auto shape22 = std::shared_ptr<Circle>(new Circle{pegrad, 540.f, 700.f});

    /* row 5 */
    auto shape23 = std::shared_ptr<Circle>(new Circle{pegrad, 80.f, 900.f});
    auto shape24 = std::shared_ptr<Circle>(new Circle{pegrad, 180.f, 900.f});
    auto shape25 = std::shared_ptr<Circle>(new Circle{pegrad, 280.f, 900.f});
    auto shape26 = std::shared_ptr<Circle>(new Circle{pegrad, 380.f, 900.f});
    auto shape27 = std::shared_ptr<Circle>(new Circle{pegrad, 480.f, 900.f});

    auto radius = 20.f;

    auto border1 = std::shared_ptr<Rectangle>(new Rectangle(1.f, wheight+(2.f*radius), -radius, wheight/2.f));
    auto border2 = std::shared_ptr<Rectangle>(new Rectangle(1.f, wheight+(2.f*radius), wwidth+radius, wheight/2.f));
    auto border3 = std::shared_ptr<Rectangle>(new Rectangle(wwidth+(2.f*radius), 1.f, wwidth/2.f, -radius*4.f));
    auto border4 = std::shared_ptr<Rectangle>(new Rectangle(wwidth+(2.f*radius), 1.f, wwidth/2.f, wheight+radius*4.f));
    
    std::vector<std::shared_ptr<CollidableUIObj>> otherShapes{
        shape1, shape2, shape3, shape4, shape5,
        shape6, shape7, shape8, shape9, shape10, shape11,
        shape12, shape13, shape14, shape15, shape16,
        shape17, shape18, shape19, shape20, shape21, shape22,
        shape23, shape24, shape25, shape26, shape27,
        border1, border2, border3, border4};
    std::vector<std::shared_ptr<Circle>> renderCircles{
        shape1, shape2, shape3, shape4, shape5,
        shape6, shape7, shape8, shape9, shape10, shape11,
        shape12, shape13, shape14, shape15, shape16,
        shape17, shape18, shape19, shape20, shape21, shape22,
        shape23, shape24, shape25, shape26, shape27
    };
    std::vector<std::shared_ptr<CollidableUIObj>> borders{border1, border2, border3, border4};
    std::vector<std::shared_ptr<PachinkoBall>> pachinkoBalls;
    std::vector<std::shared_ptr<Circle>> shapesToRemove;

    std::mt19937 rng(time(NULL));
    std::uniform_int_distribution<int> gen(1.0f, wwidth);

    sf::Clock spawnClock;
    spawnClock.restart();
    float spawnTime = 0.1f;
    int spawnCount = 0;
    int spawnMax = 20;

    int score = 0;
    
    std::shared_ptr<ScreenspacePositionConstraint> windowConstraint = createWindowConstraint(radius, wwidth, wheight);
    windowConstraint.get()->updateConstrainingObjects(otherShapes);

    //create sectors by evenly subdividing screen 
    CollisionSectors collisionSectors{3, 2, wwidth, wheight, windowConstraint};

    InputManager manager{0, &window};
    manager.registerInput(sf::Keyboard::A, InputSemantics::accept, InputType::keyboard);
    manager.registerInput(sf::Keyboard::B, InputSemantics::accept, InputType::keyboard);

    manager.registerInput(sf::Mouse::Button::Left, InputSemantics::accept, InputType::mouse);
    while (window.isOpen())
    {
        manager.processInput();
        if (spawnClock.getElapsedTime().asSeconds() > spawnTime) {
            spawnClock.restart();
            if (spawnCount < spawnMax) {
                
                /* First, figure out WHERE we can spawn */
                bool canSpawn = true;
                sf::Vector2f prospectiveSpawn{static_cast<float>(gen(rng)), -radius};
                do {
                    canSpawn = true;
                    prospectiveSpawn.x = gen(rng);
                    for (auto i : pachinkoBalls) {
                        auto bpos = i.get()->getPosition();
                        if (MathUtils::point_distance(prospectiveSpawn.x, prospectiveSpawn.y, bpos.x, bpos.y) <= radius*2.f) {
                            canSpawn = false;
                            break;
                        }
                    }
                }
                while (canSpawn == false);

                /* Now spawn there */
                auto newShape = std::shared_ptr<PachinkoBall>(new PachinkoBall{radius, prospectiveSpawn.x, prospectiveSpawn.y});
                otherShapes.push_back(newShape);
                newShape.get()->setPositionConstraint(createWindowConstraint(radius, wwidth, wheight));
                renderCircles.push_back(newShape);
                pachinkoBalls.push_back(newShape);
                windowConstraint.get()->updateConstrainingObjects(otherShapes);
                ++spawnCount;

                collisionSectors.recalculateSectors();

                /* NOTE: Very bad. Code duplication. Reflection. */
                if (dynamic_cast<ScreenspacePositionConstraint*>(newShape.get()->getPositionConstraint())) {
                    ScreenspacePositionConstraint* pos = dynamic_cast<ScreenspacePositionConstraint*>(newShape.get()->getPositionConstraint());
                    auto sameSectorObj = collisionSectors.getObjectsInSameSectors(newShape);
                    for (auto b : borders) {
                        sameSectorObj.push_back(b);
                    }
                    pos->updateConstrainingObjects(sameSectorObj);
                }
            }
        }
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        window.clear();
        for (auto i : pachinkoBalls) {
            i.get()->update();
            collisionSectors.recalculateSectors();
            /* NOTE: Very bad. Code duplication. Reflection. */
            if (dynamic_cast<ScreenspacePositionConstraint*>(i.get()->getPositionConstraint())) {
                ScreenspacePositionConstraint* pos = dynamic_cast<ScreenspacePositionConstraint*>(i.get()->getPositionConstraint());
                auto sameSectorObj = collisionSectors.getObjectsInSameSectors(i);
                for (auto b : borders) {
                    sameSectorObj.push_back(b);
                }
                pos->updateConstrainingObjects(sameSectorObj);
            }
            if (i->getPosition().y > wheight+radius*2.f) {
                shapesToRemove.push_back(i);
            }
        }
        for (auto i : renderCircles) {
            window.draw(i.get()->getShape());
        }
        for (auto i : shapesToRemove) {
            renderCircles.erase(std::remove(renderCircles.begin(), renderCircles.end(), i), renderCircles.end()); //erase-remove idiom
            otherShapes.erase(std::remove(otherShapes.begin(), otherShapes.end(), i), otherShapes.end());
            pachinkoBalls.erase(std::remove(pachinkoBalls.begin(), pachinkoBalls.end(), i), pachinkoBalls.end());
        }
        if (shapesToRemove.size() > 0) {
            ++score;
            std::cout<< "Score Up! " << score <<std::endl;
            spawnCount-=shapesToRemove.size();
            shapesToRemove.clear();
            windowConstraint.get()->updateConstrainingObjects(otherShapes);
        }
        window.display();
    }

    return 0;
}