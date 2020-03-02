#include "Bubble.h"
#include "Constants.h"

#include <iostream>
#include <cmath>

Bubble::Bubble(sf::Texture& texture, sf::Vector2f pos, sf::Vector2f dir, float sz, float sp)
    : bubble(texture), position(pos), direction(dir), size(sz), speed(sp)
{
    bubble.setScale(size,size);
    bubble.setPosition(pos);
    bubble.setOrigin(bubble.getLocalBounds().width/2.0f,bubble.getLocalBounds().height / 2.0f);
}

Bubble::~Bubble()
{
    //dtor
}

bool Bubble::drift(const Bubble& TheBubble)
{
    // Determine the new position
    sf::Vector2f newPosition = position;

    newPosition.x += speed * direction.x;
    newPosition.y += speed * direction.y;

    // Check the distance to The Bubble
    float deltaX = newPosition.x - TheBubble.position.x;
    float deltaY = newPosition.y - TheBubble.position.y;
    float distanceToTheBubble = std::sqrt(deltaX * deltaX + deltaY * deltaY);

    // Don't move the bubble too close to The Bubble
    if (distanceToTheBubble > 0.5f * width())
    {
        position = newPosition;
        bubble.setPosition(newPosition);
    }

    // Does the bubble drift out of the window?
    if (position.x < 0.0f or position.x > MainWindowSize.x)
        return true;

    // Wrap the top and bottom of the Main window
    if (position.y < width()/2 or position.y > MainWindowSize.y - width()/2)
    {
        direction.y *= -1.0f;
    }

    return false;
}

bool Bubble::move()
{
    // Determine the new position
    sf::Vector2f newPosition = position;

    newPosition.x += speed * direction.x;
    newPosition.y += speed * direction.y;

    position = newPosition;
    bubble.setPosition(newPosition);

    // Does the bubble drift out of the window?
    if (position.x < 0.0f or position.x > MainWindowSize.x or position.y < 0.0f or position.y > MainWindowSize.y)
        return true;
    return false;
}

bool Bubble::collide(const std::list<Bubble*>& bubbles) const
{
    float deltaX, deltaY, distanceToTheBubble;
    for (auto each = bubbles.begin(); each != bubbles.end(); ++each)
    {
        deltaX = position.x - (*each)->getPosition().x;
        deltaY = position.y - (*each)->getPosition().y;

        distanceToTheBubble = std::sqrt(deltaX * deltaX + deltaY * deltaY);
        if (distanceToTheBubble < (width()/2.0f + (*each)->width()/2.0f))
        {
            return true;
        }
    }

    // Collide with a wall
    if (position.x < width()/2 or position.x > MainWindowSize.x - width()/2
        or position.y < width()/2 or position.y > MainWindowSize.y - width()/2)
    {
        return true;
    }
    return false;
}

