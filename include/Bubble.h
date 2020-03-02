#ifndef BUBBLE_H
#define BUBBLE_H
#include <SFML/Graphics.hpp>
#include <list>

class Bubble
{
public:
    enum Direction {Up,Down,Left,Right};
    Bubble(sf::Texture& texture, sf::Vector2f position, sf::Vector2f direction = sf::Vector2f(0.0f,0.0f), float size = 0.75f, float speed = 0.25f);
    virtual ~Bubble();
    operator sf::Sprite() const
    {
        return bubble;
    }
    float getSize() const
    {
        return size;
    }
    sf::Vector2f getPosition() const
    {
        return position;
    }
    sf::Vector2f getDirection()
    {
        return direction;
    }
    float getSpeed() const { return speed; }
    float width() const
    {
        return bubble.getLocalBounds().width * size;
    }
    bool drift(const Bubble& TheBubble);
    bool move();  // This is for the big bubble
    void setDirection(sf::Vector2f dir)
    {
        direction = dir;
    }
    void setSpeed(float sp) { speed = sp; }
    bool collide(const std::list<Bubble*>& bubbles) const;

protected:

private:
    sf::Sprite bubble;
    sf::Vector2f position;
    sf::Vector2f direction;
    float size;
    float speed;
};

#endif // BUBBLE_H
