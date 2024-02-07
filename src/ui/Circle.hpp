#include <SFML/Graphics.hpp>
#include <deque>
#include <unordered_map>

enum CollisionMaskType {
    unknown,
    rectangle,
    circle
};

class CollisionMask {
    public: 
        CollisionMask(CollisionMaskType typeVar);
        virtual bool getPointCollided(float pointX, float pointY) = 0;
        void setPosition(sf::Vector2f newPos);
        virtual sf::Vector2f getCollisionNormal(CollisionMask &other);
        float x;
        float y;
        const CollisionMaskType type = CollisionMaskType::unknown;
};

//forward declarations
class RectangularCollisionMask; 
class CircularCollisionMask;
class CollidableUIObj;

class RectangularCollisionMask : public CollisionMask {
    public:
        RectangularCollisionMask(float width, float length);
        bool getPointCollided(float pointX, float pointY) override;
        bool getCircleCollided(CircularCollisionMask &other);
        bool getRectangleCollided(RectangularCollisionMask &other);
        virtual sf::Vector2f getCollisionNormal(CircularCollisionMask &other);
        virtual sf::Vector2f getCollisionNormal(RectangularCollisionMask &other);
        float getTop();
        float getLeft();
        float getRight();
        float getBottom();
        float width;
        float height;
        const CollisionMaskType type = CollisionMaskType::rectangle;
};

class CircularCollisionMask : public CollisionMask {
    public:
        CircularCollisionMask(float radius);
        bool getPointCollided(float pointX, float pointY) override;
        bool getCircleCollided(CircularCollisionMask &other);
        bool getRectangleCollided(RectangularCollisionMask &other);
        virtual sf::Vector2f getCollisionNormal(CircularCollisionMask &other);
        virtual sf::Vector2f getCollisionNormal(RectangularCollisionMask &other);
        float radius;
        const CollisionMaskType type = CollisionMaskType::circle;
    protected:
        sf::Vector2f _getProspectiveRectangularCollisionPoint(RectangularCollisionMask &other);
    private:
        sf::Vector2f _getCircularCollisionNormal(CollisionMask &mask);
        sf::Vector2f _getRectangularCollisionNormal(CollisionMask &mask);
};


class PositionConstraint {
    public:
        virtual bool checkWithinConstraints(CollisionMask &mask) = 0;
        virtual std::vector<std::shared_ptr<CollidableUIObj>> getCollisionList(CollisionMask &mask);
};

class RectangularPositionConstraint: public PositionConstraint {
    public:
        RectangularPositionConstraint(float minX, float minY, float maxX, float maxY);
        virtual bool checkWithinConstraints(CollisionMask &mask) override;
    protected:
        float _minX;
        float _minY;
        float _maxX;
        float _maxY;
};

class ConstrainableUIObj {
    public:
        void setPositionConstraint(std::shared_ptr<PositionConstraint> constraint);
        virtual sf::Vector2f getPosition() const = 0;
    protected:
        std::shared_ptr<PositionConstraint> _constraint;
        sf::Vector2f _lastViablePosition;
};

class CollidableUIObj: public ConstrainableUIObj {
    public:
        virtual bool getCollided(CollisionMask &mask) = 0;
        virtual sf::Vector2f getCollisionNormal(CollisionMask &mask) = 0;
        virtual CollisionMaskType getCollisionType();
        virtual std::vector<std::shared_ptr<CollidableUIObj>> getCollisionList() = 0;
        virtual void setPosition(float newX, float newY) = 0;
        virtual void applyForce(float force, float direction);
    protected:
        std::shared_ptr<CollisionMask> _collisionMask;
};

class ScreenspacePositionConstraint: public RectangularPositionConstraint {
    public:
        ScreenspacePositionConstraint(float minX, float minY, float maxX, float maxY);
        void updateConstrainingObjects(std::vector<std::shared_ptr<CollidableUIObj>> objects);
        virtual bool checkWithinConstraints(CollisionMask &mask) override;

        virtual std::vector<std::shared_ptr<CollidableUIObj>> getCollisionList(CollisionMask &mask) override;
    protected:
        std::vector<std::shared_ptr<CollidableUIObj>> _constrainingObjects;
};


class Circle : public CollidableUIObj {
    public:
        Circle(float rad);
        Circle(float rad, float x, float y);
        bool getCollided(CollisionMask &mask) override;
        virtual void update();
        virtual sf::Vector2f getCollisionNormal(CollisionMask &mask) override;
        void recordPosition();
        virtual void setPosition(float newX, float newY) override;
        sf::CircleShape getShape();
        sf::Vector2f getPosition() const override;
        virtual std::vector<std::shared_ptr<CollidableUIObj>> getCollisionList() override;
    protected:
        std::shared_ptr<CircularCollisionMask> _collisionMask;
        sf::CircleShape _shape;
        sf::Vector2f _intendedPosition;
        void _constrainPosition();
        sf::Vector2f _getLastPos();
        sf::Vector2f _getIntendedPosition() const;
        virtual float _getPosX(float x);
        virtual float _getPosY(float y);
        std::deque<sf::Vector2f> _posHistory;
        const int _posHistorySize = 1;
    private:
        const sf::Color _defaultColor = sf::Color::Red;
};

class Rectangle: public CollidableUIObj {
    public:
        Rectangle(float width, float height);
        Rectangle(float width, float height, float x, float y);
        virtual void setPosition(float newX, float newY) override;
        virtual sf::Vector2f getCollisionNormal(CollisionMask &mask) override;
        bool getCollided(CollisionMask &mask) override;
        sf::Vector2f getPosition() const override;
        virtual std::vector<std::shared_ptr<CollidableUIObj>> getCollisionList() override;
    protected:
        std::shared_ptr<RectangularCollisionMask> _collisionMask;
        sf::RectangleShape _shape;
};

class MovableCircle : public Circle {
    public:
        MovableCircle(float rad);
        MovableCircle(float rad, float x, float y);
        virtual void tryGrab(float grabX, float grabY);
        virtual void release();
        virtual void update() override;
        bool isGrabbed();
    protected:
        void _grab(float grabbedX, float grabbedY);
        float _getPosX(float x) override;
        float _getPosY(float y) override;
        float _grabXOffset;
        float _grabYOffset;
        bool _grabbed;
        const sf::Color _restingColor = sf::Color::Green;
        const sf::Color _grabbedColor = sf::Color::Yellow;
};

class ThrowableMovableCircle : public MovableCircle {
    public:
        ThrowableMovableCircle(float rad);
        ThrowableMovableCircle(float rad, float x, float y);
        virtual void update() override;
        void release() override;
        void setPosition(float newX, float newY) override;
    protected:
        void _bounce();
        void _checkBounce();
        float _gravity = 0;
        float _velocity;
        float _direction; //in radians
        const float _bounceConstant = 0.95f;
    private:
        const float _linearDampingConstant = 0.00001f;
        const float _maxVelocity = 5.0f;
        const float _moveInterval = 0.01f;
        const float _frictionConstant = 0.999f;
        const float _velocityAccelerationConstant = 0.5f;
        const float _velocityDeccelerationConstant = 0.1f;
        const float _velocityNormalizationFactor = 0.1f;
};

class PachinkoBall: public ThrowableMovableCircle {
    public:
        PachinkoBall(float rad);
        PachinkoBall(float rad, float x, float y);
        virtual void update() override;
        void tryGrab(float grabX, float grabY) override;
        void applyForce(float force, float direction) override;
        PositionConstraint* getPositionConstraint();
    private:
        const float _gravity = 0.1f;
};

class CollisionSectors {
    public:
        CollisionSectors(int rows, int columns, float totalWidth, float totalHeight, std::shared_ptr<ScreenspacePositionConstraint> world);
        std::vector<std::shared_ptr<CollidableUIObj>> getObjectsInSameSectors(std::shared_ptr<CollidableUIObj> object);
        void recalculateSectors();
    protected:
        std::vector<std::shared_ptr<Rectangle>> _sectors;
        std::unordered_map<std::shared_ptr<CollidableUIObj>, std::vector<Rectangle*>> _objToSector;
        std::map<Rectangle*, std::vector<std::shared_ptr<CollidableUIObj>>> _sectorToObj;
};