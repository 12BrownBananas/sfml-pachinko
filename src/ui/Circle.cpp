#include <iostream>
#include "src/ui/Circle.hpp"
#include <SFML/Graphics.hpp>
#include "src/util/MathUtils.cpp"
#include <unordered_map>

void ConstrainableUIObj::setPositionConstraint(std::shared_ptr<PositionConstraint> constraint) {
    _constraint = constraint;
}

Circle::Circle(float rad) {
    _shape = sf::CircleShape{rad};
    _shape.setOrigin(sf::Vector2f{rad, rad});
    _collisionMask = std::shared_ptr<CircularCollisionMask>(new CircularCollisionMask(rad));
}
Circle::Circle(float rad, float x, float y) {
    _shape = sf::CircleShape{rad};
    _shape.setOrigin(sf::Vector2f{rad, rad});
    _shape.setFillColor(_defaultColor);
    _collisionMask = std::shared_ptr<CircularCollisionMask>(new CircularCollisionMask(rad));
    setPosition(x, y);
}
Rectangle::Rectangle(float width, float height) {
    _shape = sf::RectangleShape(sf::Vector2f{width, height});
    _shape.setOrigin(width/2.f, height/2.f);
    _collisionMask = std::shared_ptr<RectangularCollisionMask>(new RectangularCollisionMask(width, height));
    setPosition(0.f, 0.f);
}
Rectangle::Rectangle(float width, float height, float x, float y) {
    _shape = sf::RectangleShape(sf::Vector2f{width, height});
    _shape.setOrigin(width/2.f, height/2.f);
    _collisionMask = std::shared_ptr<RectangularCollisionMask>(new RectangularCollisionMask(width, height));
    setPosition(x, y);
}

CollisionMask::CollisionMask(CollisionMaskType typeVar): type(typeVar) {}

void CollisionMask::setPosition(sf::Vector2f newPos) {
    x = newPos.x;
    y = newPos.y;
}

void Circle::setPosition(float newX, float newY) {
    _shape.setPosition(sf::Vector2f{_getPosX(newX), _getPosY(newY)});
    _intendedPosition = _shape.getPosition();
    _collisionMask.get()->setPosition(getPosition());
    if (_constraint)
        _constrainPosition();
    _collisionMask.get()->setPosition(getPosition());
}

void Rectangle::setPosition(float newX, float newY) {
    _shape.setPosition(newX, newY);
    _collisionMask.get()->setPosition(getPosition());
}
bool Rectangle::getCollided(CollisionMask &mask) {
    if (&mask == _collisionMask.get()) return false;
    if (mask.type == CollisionMaskType::unknown) {
        return false;
    }
    else if (mask.type == CollisionMaskType::circle) {
        auto circularMask = dynamic_cast<CircularCollisionMask*>(&mask);
        return circularMask->getRectangleCollided(*_collisionMask.get());
    }
    else if (mask.type == CollisionMaskType::rectangle) {
        auto rectangularMask = dynamic_cast<RectangularCollisionMask*>(&mask);
        return rectangularMask->getRectangleCollided(*_collisionMask.get());
    }
    return false;
}
sf::Vector2f Rectangle::getCollisionNormal(CollisionMask &mask) {
    auto myMask = *_collisionMask.get();
    if (mask.type == CollisionMaskType::circle) {
        return myMask.getCollisionNormal(*dynamic_cast<CircularCollisionMask*>(&mask));
    }
    return sf::Vector2f{0.f, 0.f};
}

sf::Vector2f Rectangle::getPosition() const {
    auto pos = _shape.getPosition();
    return _shape.getPosition();
}

sf::CircleShape Circle::getShape() {
    return _shape;
}

float Circle::_getPosX(float x) {
    return x;
}
float Circle::_getPosY(float y) {
    return y;
}
sf::Vector2f Circle::_getLastPos() {
    return _posHistory.back();
}

CollisionMaskType CollidableUIObj::getCollisionType() {
    return _collisionMask.get()->type;
}

void CollidableUIObj::applyForce(float force, float direction) {}

sf::Vector2f Circle::getCollisionNormal(CollisionMask &mask) {
    auto myMask = *_collisionMask.get();
    if (mask.type == CollisionMaskType::circle) {
        return myMask.getCollisionNormal(*dynamic_cast<CircularCollisionMask*>(&mask));
    }
    return sf::Vector2f{0.f, 0.f};
}
bool Circle::getCollided(CollisionMask &mask) {
    if (&mask == _collisionMask.get()) return false;
    if (mask.type == CollisionMaskType::unknown) {
        return false;
    }
    else if (mask.type == CollisionMaskType::circle) {
        auto circularMask = dynamic_cast<CircularCollisionMask*>(&mask);
        return circularMask->getCircleCollided(*_collisionMask.get());
    }
    else if (mask.type == CollisionMaskType::rectangle) {
        auto rectangularMask = dynamic_cast<RectangularCollisionMask*>(&mask);
        return rectangularMask->getCircleCollided(*_collisionMask.get());
    }
    return false;
}

void Circle::_constrainPosition() {
    if (_constraint && !_constraint.get()->checkWithinConstraints(*_collisionMask.get())) {
        _shape.setPosition(_lastViablePosition);
    }
    else 
        _lastViablePosition = sf::Vector2f{_collisionMask.get()->x, _collisionMask.get()->y};
}
sf::Vector2f Circle::getPosition() const {
    return _shape.getPosition();
}

sf::Vector2f Circle::_getIntendedPosition() const {
    return _intendedPosition;
}

void Circle::recordPosition() {
    _posHistory.push_back(_shape.getPosition());
    if (_posHistory.size() > _posHistorySize)
        _posHistory.pop_front();
}

CircularCollisionMask::CircularCollisionMask(float radius): CollisionMask(CollisionMaskType::circle), radius(radius) {}
RectangularCollisionMask::RectangularCollisionMask(float width, float height): CollisionMask(CollisionMaskType::rectangle), width(width), height(height) {}

bool CircularCollisionMask::getPointCollided(float pointX, float pointY) {
    return MathUtils::point_distance(x, y, pointX, pointY) <= radius;
}
bool CircularCollisionMask::getCircleCollided(CircularCollisionMask &other) {
    return MathUtils::point_distance(x, y, other.x, other.y) <= (radius+other.radius);
}
bool CircularCollisionMask::getRectangleCollided(RectangularCollisionMask &other) {
    auto test = _getProspectiveRectangularCollisionPoint(other);
    float dist = MathUtils::point_distance(x, y, test.x, test.y);
    if (dist <= radius) return true;
    return false;
}
sf::Vector2f CircularCollisionMask::_getProspectiveRectangularCollisionPoint(RectangularCollisionMask &other) {
    float testX = x;
    float testY = y;
    if (x < other.getLeft()) testX = other.getLeft();
    else if (x > other.getRight()) testX = other.getRight();
    if (y < other.getTop()) testY = other.getTop();
    else if (y > other.getBottom()) testY = other.getBottom();
    return sf::Vector2f{testX, testY};
}

sf::Vector2f CollisionMask::getCollisionNormal(CollisionMask &other) {
    return sf::Vector2f{0.f, 0.f};
}
sf::Vector2f CircularCollisionMask::getCollisionNormal(CircularCollisionMask &other) {
    return MathUtils::vectorize_velocity(1.0, MathUtils::get_direction(sf::Vector2f{(other.x-x), (other.y-y)}));
}
sf::Vector2f CircularCollisionMask::getCollisionNormal(RectangularCollisionMask &other) {
    auto test = _getProspectiveRectangularCollisionPoint(other);
    //assuming test is colliding, we just need to figure out where...
    if (test.x == other.getLeft()) return sf::Vector2f{-1.f, 0.f};
    if (test.x == other.getRight()) return sf::Vector2f{1.f, 0.f};
    if (test.y == other.getTop()) return sf::Vector2f{0.f, -1.f};
    if (test.y == other.getBottom()) return sf::Vector2f{0.f, 1.f};
    return sf::Vector2f{0.f, 0.f};
}

sf::Vector2f RectangularCollisionMask::getCollisionNormal(CircularCollisionMask &other) {
    return other.getCollisionNormal(*this);
}
sf::Vector2f RectangularCollisionMask::getCollisionNormal(RectangularCollisionMask &other) {
    return sf::Vector2f{0.f, 0.f};
}

bool RectangularCollisionMask::getPointCollided(float pointX, float pointY) {
    return pointX >= getLeft() && pointX <= getRight() && pointY >= getTop() && pointY <= getBottom();
}
bool RectangularCollisionMask::getCircleCollided(CircularCollisionMask &other) {
    return other.getRectangleCollided(*this);
}
bool RectangularCollisionMask::getRectangleCollided(RectangularCollisionMask &other) {
    return other.getPointCollided(getLeft(), getTop()) || other.getPointCollided(getRight(), getTop()) || other.getPointCollided(getLeft(), getBottom()) || other.getPointCollided(getRight(), getBottom());
}
float RectangularCollisionMask::getTop() {
    return y-height/2.f;
}
float RectangularCollisionMask::getLeft() {
    return x-width/2.f;
}
float RectangularCollisionMask::getRight() {
    return x+width/2.f;
}
float RectangularCollisionMask::getBottom() {
    return y+height/2.f;
}

MovableCircle::MovableCircle(float rad) : Circle(rad) {
    _shape.setFillColor(_restingColor);
    _grabbed = false;
    _grabXOffset = 0;
    _grabYOffset = 0;
}
MovableCircle::MovableCircle(float rad, float x, float y) : Circle(rad, x, y) {
    _shape.setFillColor(_restingColor);
    _grabbed = false;
    _grabXOffset = 0;
    _grabYOffset = 0;
}

void Circle::update() {}

void MovableCircle::update() {
    if (_grabbed) return;
}

float MovableCircle::_getPosX(float x) {
    return x+_grabXOffset;
}
float MovableCircle::_getPosY(float y) {
    return y+_grabYOffset;
}

bool MovableCircle::isGrabbed() {
    return _grabbed;
}

void MovableCircle::tryGrab(float grabX, float grabY) {
    auto pos = _shape.getPosition();
    auto dist = MathUtils::point_distance(pos.x, pos.y, grabX, grabY);
    if (dist <= _shape.getRadius())
        _grab(pos.x-grabX, pos.y-grabY);
}

void MovableCircle::_grab(float grabbedX, float grabbedY) {
    _grabbed = true;
    _shape.setFillColor(_grabbedColor);
    _grabXOffset = grabbedX;
    _grabYOffset = grabbedY;
}

void MovableCircle::release() {
    _grabbed = false;
    _grabXOffset = 0;
    _grabYOffset = 0;
    _shape.setFillColor(_restingColor);
}

ThrowableMovableCircle::ThrowableMovableCircle(float rad) : MovableCircle(rad), _direction(0), _velocity(0) {}
ThrowableMovableCircle::ThrowableMovableCircle(float rad, float x, float y) : MovableCircle(rad, x, y), _direction(0), _velocity(0) {}
PachinkoBall::PachinkoBall(float rad) : ThrowableMovableCircle(rad) {}
PachinkoBall::PachinkoBall(float rad, float x, float y) : ThrowableMovableCircle(rad, x, y) {}


void ThrowableMovableCircle::update() {
    if (_grabbed) return;
    auto vectorizedVel = MathUtils::vectorize_velocity(_velocity, _direction);
    auto currPos = getPosition();
    auto remainingDistance = _velocity;
    while (remainingDistance > 0) {
        currPos+=MathUtils::vectorize_velocity(_moveInterval, _direction);
        remainingDistance-=_moveInterval;
        _collisionMask.get()->setPosition(currPos);
        _checkBounce();
    }
    setPosition(currPos.x, currPos.y);
    _velocity = std::max(0.f, (_velocity*_frictionConstant)-_linearDampingConstant);
}

void PachinkoBall::update() {
    if (_grabbed) return;
    auto vectorizedVel = MathUtils::vectorize_velocity(_velocity, _direction);
    auto currPos = getPosition();
    vectorizedVel.y+=_gravity;
    _direction = MathUtils::get_direction(vectorizedVel);
    _velocity = MathUtils::point_distance(currPos.x, currPos.y, currPos.x+vectorizedVel.x, currPos.y+vectorizedVel.y);
    ThrowableMovableCircle::update();
}

void ThrowableMovableCircle::_checkBounce() {
    if (_constraint) {
        if (!_constraint.get()->checkWithinConstraints(*_collisionMask.get())) { //collision!
            _bounce();
            _constrainPosition();
        }
    }
}

void ThrowableMovableCircle::_bounce() {
    if (!_constraint) return;
    auto mask = *_collisionMask.get();
    auto bouncePos = sf::Vector2f{mask.x, mask.y};
    _velocity = _velocity*_bounceConstant;
    float collisionBias = 2.f;

    std::vector<std::shared_ptr<CollidableUIObj>> collisionList = _constraint.get()->getCollisionList(mask);
    if (collisionList.size() <= 0) return;
    float xavg;
    float yavg;
    for (auto i : collisionList) {
        auto n = i->getCollisionNormal(mask);
        xavg += n.x;
        yavg += n.y;

        auto forceDir = MathUtils::get_direction(sf::Vector2f{-1.f*n.x, -1.f*n.y});
        i->applyForce(_velocity, forceDir);
    }
    sf::Vector2f collisionNormal = sf::Vector2f{xavg/collisionList.size(), yavg/collisionList.size()};
    sf::Vector2f velocityNormal = MathUtils::vectorize_velocity(1.f, _direction);
    
    sf::Vector2f averageNormal = sf::Vector2f{((collisionNormal.x*collisionBias)+velocityNormal.x)/2.f, ((collisionNormal.y*collisionBias)+velocityNormal.y)/2.f};

    _collisionMask.get()->setPosition(bouncePos);
    _direction = MathUtils::get_direction(averageNormal);
}

void ThrowableMovableCircle::setPosition(float newX, float newY) {
    if (isGrabbed()) {
        auto pos = _getIntendedPosition();
        Circle::setPosition(newX, newY);
        auto newPos = _getIntendedPosition();
        auto targetVel = std::abs(MathUtils::point_distance(newPos.x, newPos.y, pos.x, pos.y))*_velocityNormalizationFactor;
        if (targetVel < _velocity) _velocity+=std::max(targetVel-_velocity, -1.0f*_velocityDeccelerationConstant);
        else if (targetVel > _velocity) _velocity+=std::min(targetVel-_velocity, _velocityAccelerationConstant);
        if (targetVel != 0) _direction = MathUtils::get_direction(sf::Vector2f{newPos.x-pos.x, newPos.y-pos.y});
    }
    else Circle::setPosition(newX, newY);
}

void ThrowableMovableCircle::release() {
    MovableCircle::release();
    _checkBounce();
    _velocity = std::min(_maxVelocity, _velocity);
}

std::vector<std::shared_ptr<CollidableUIObj>> PositionConstraint::getCollisionList(CollisionMask &mask) {
    return std::vector<std::shared_ptr<CollidableUIObj>>{}; //default implementation returns empty list. This may not be good design.
}

std::vector<std::shared_ptr<CollidableUIObj>> Circle::getCollisionList() {
    if (!_constraint) return std::vector<std::shared_ptr<CollidableUIObj>>{}; //return an empty list
    return _constraint.get()->getCollisionList(*_collisionMask.get());
}
std::vector<std::shared_ptr<CollidableUIObj>> Rectangle::getCollisionList() {
    if (!_constraint) return std::vector<std::shared_ptr<CollidableUIObj>>{}; //return an empty list
    return _constraint.get()->getCollisionList(*_collisionMask.get());
}

RectangularPositionConstraint::RectangularPositionConstraint(float minX, float minY, float maxX, float maxY): 
    _minX(minX),
    _minY(minY),
    _maxX(maxX),
    _maxY(maxY) 
    {}

bool RectangularPositionConstraint::checkWithinConstraints(CollisionMask &mask) {
    return mask.x > _minX && mask.x < _maxX && mask.y > _minY && mask.y < _maxY;
}

ScreenspacePositionConstraint::ScreenspacePositionConstraint(float minX, float minY, float maxX, float maxY): RectangularPositionConstraint(minX, minY, maxX, maxY) {}
void ScreenspacePositionConstraint::updateConstrainingObjects(std::vector<std::shared_ptr<CollidableUIObj>> objects) {
    _constrainingObjects = objects;
}
bool ScreenspacePositionConstraint::checkWithinConstraints(CollisionMask &mask) {
    if (!RectangularPositionConstraint::checkWithinConstraints(mask)) return false;
    for (auto other : _constrainingObjects) {
        if (other->getCollided(mask)) {
            return false;
        }
    }
    return true;
}

std::vector<std::shared_ptr<CollidableUIObj>> ScreenspacePositionConstraint::getCollisionList(CollisionMask &mask) {
    std::vector<std::shared_ptr<CollidableUIObj>> collidedObjects;
    for (auto other : _constrainingObjects) {
        if (other.get()->getCollided(mask)) {
            collidedObjects.push_back(other);
        }
    }
    return collidedObjects;
}

void PachinkoBall::tryGrab(float grabX, float grabY) {
    return; //grabbing is disabled on this class
}

void PachinkoBall::applyForce(float force, float direction) {
    auto velVec = MathUtils::vectorize_velocity(force, direction);
    auto currVel = MathUtils::vectorize_velocity(_velocity, _direction);
    auto avgVec = sf::Vector2f((velVec.x+currVel.x)/2.f, (velVec.y+currVel.y)/2.f);
    _direction = MathUtils::get_direction(avgVec);
    _velocity*=_bounceConstant;
}

PositionConstraint* PachinkoBall::getPositionConstraint() {
    return _constraint.get();
}

CollisionSectors::CollisionSectors(int rows, int columns, float totalWidth, float totalHeight, std::shared_ptr<ScreenspacePositionConstraint> world) {
    float sectorWidth = totalWidth/columns;
    float sectorHeight = totalHeight/rows;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            //5.f is an arbitrary pad value to ensure "sufficient overlap" between sectors.
            //There's probably a more certain way to calculate this, but for now, it's a fine "guess" value.
            auto sector = std::shared_ptr<Rectangle>(new Rectangle(sectorWidth+5.f, sectorHeight+5.f, (sectorWidth/2)+(sectorWidth*j), (sectorHeight/2)+(sectorHeight*i)));
            sector.get()->setPositionConstraint(world);
            _sectors.push_back(sector);
        }
    }
}

std::vector<std::shared_ptr<CollidableUIObj>> CollisionSectors::getObjectsInSameSectors(std::shared_ptr<CollidableUIObj> object) {
    std::vector<Rectangle*> sectors;
    if (_objToSector.count(object) > 0) {
        auto objSect = _objToSector[object];
        sectors.insert(sectors.end(), objSect.begin(), objSect.end());
    }
    std::vector<std::shared_ptr<CollidableUIObj>> objects;
    for (auto rect : sectors) {
        auto obj = _sectorToObj[rect];
        for (auto i : obj) {
            if (i != object) objects.push_back(i);
        }
    }
    return objects;
}

void CollisionSectors::recalculateSectors() {
    _objToSector.clear();
    _sectorToObj.clear();
    int sectorId = 0;
    for (auto sect : _sectors) {
        auto cl = sect.get()->getCollisionList(); //Gets all of the objects colliding with this sector. (cl = "collision list")
        auto ls = _sectorToObj[sect.get()]; //Inserts a new element to _sectorToObj
        ls.insert(ls.end(), cl.begin(), cl.end()); //Appends the entire vector "cl" to the end of "ls"
        _sectorToObj[sect.get()] = ls;
        for (auto c : cl) {
            auto sl = _objToSector[c];
            sl.push_back(sect.get());
            _objToSector[c] = sl;
        }
    }
}