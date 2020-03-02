#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <list>
#include <string>
#include <sstream>
#include <fstream>
#include <ctime>
#include <cmath>

#include "Constants.h"
#include "Bubble.h"
#include "HighScores.h"

using std::cout;
using std::endl;

sf::Vector2f randomWindowPosition(const Bubble& BigBubble);
sf::Vector2f randomDirection();
float randomSize();
float randomSpeed();
std::string welcome(sf::RenderWindow& window, const HighScores& highScores);
std::string getWelcomeText();
char getKey();

int main()
{
    srand(static_cast<unsigned>(time(nullptr)));
    bool pauseFlag = false;
    sf::RenderWindow window(sf::VideoMode(1024,720), "Bubble Pop", sf::Style::Close);
    sf::Event evnt;
    unsigned numberOfBubbles = 0;
    HighScores highScores;
//   bool joystickConnected = sf::Joystick::isConnected(0);
    sf::Vector2f joystick = sf::Vector2f(0.f,0.f);

    // create bottom game border
    sf::RectangleShape bottom(sf::Vector2f(MainWindowSize.x,10.0f));
    bottom.setPosition(sf::Vector2f(0.0f,MainWindowSize.y));
    bottom.setFillColor(sf::Color::Magenta);

    // Create sprites
    sf::Vector2f BigBubblePosition = {MainWindowSize.x/2.0f,MainWindowSize.y/2.0f};
    sf::Texture bubbleTexture;
    bubbleTexture.loadFromFile(BubbleFile);
    Bubble BigBubble(bubbleTexture,BigBubblePosition, randomDirection(), 1.0f, 0.1f);

    std::list<Bubble*> bubbles;

    //////////// Sound effects /////////////
    sf::SoundBuffer bubbleBuffer;
    bubbleBuffer.loadFromFile(BubbleSoundFile);
    sf::Sound bubbleSound;
    bubbleSound.setBuffer(bubbleBuffer);

    sf::SoundBuffer bigBubbleBuffer;
    bigBubbleBuffer.loadFromFile(BigBubbleSoundFile);
    sf::Sound bigBubbleSound;
    bigBubbleSound.setBuffer(bigBubbleBuffer);
    bigBubbleSound.setVolume(100.0f);

    sf::SoundBuffer endOfGameBuffer;
    endOfGameBuffer.loadFromFile(EndOfGameSoundFile);
    sf::Sound endOfGameSound;
    endOfGameSound.setBuffer(endOfGameBuffer);
    bigBubbleSound.setVolume(30.0f);
    bigBubbleSound.setLoop(true);


    // Music
    sf::Music music;
    if (music.openFromFile(BubbleSong))
    {
        music.setVolume(40);
        music.play();
    }

    // Fonts
    sf::Font font;
    font.loadFromFile(ArialFont);
    // Texts
    sf::Text message("Number of Bubbles " + std::to_string(numberOfBubbles), font,24);
    message.setFillColor(sf::Color::Green);
    message.setPosition(400.0f,660.0f);

    auto loopCounter = 0u;
    std::string name = welcome(window, highScores);
    while (window.isOpen())
    {
        while (window.pollEvent(evnt))
        {
            if (sf::Joystick::isButtonPressed(0,0))
                BigBubble.setSpeed(BigBubble.getSpeed()*1.2f);  // 'A' button pressed (increase speed)
            if (sf::Joystick::isButtonPressed(0,1))
                BigBubble.setSpeed(BigBubble.getSpeed()*0.8f);  // 'B' button pressed (decrease speed)

            switch (evnt.type)
            {
            case sf::Event::Closed:
                window.close();
                break;
            case sf::Event::KeyPressed:
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
                    pauseFlag = !pauseFlag;
                else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
                    window.close();
                else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
                    BigBubble.setDirection(sf::Vector2f(0.0f,-1.0f));
                else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
                    BigBubble.setDirection(sf::Vector2f(0.0f,1.0f));
                else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
                    BigBubble.setDirection(sf::Vector2f(-1.0f,0.0f));
                else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
                    BigBubble.setDirection(sf::Vector2f(1.0f,0.0f));
                else
                    break;
            default:
                ;
            }
            if (evnt.type == evnt.Closed)
            {
                window.close();
            }
        }
        joystick = sf::Vector2f(sf::Joystick::getAxisPosition(0, sf::Joystick::X), sf::Joystick::getAxisPosition(0, sf::Joystick::Y));
        if (fabs(joystick.x) > 15.f || fabs(joystick.y) > 15.f)
        {
            BigBubble.setDirection(sf::Vector2f(joystick.x/100.0f,joystick.y/100.0f));
            //std::cout << joystick.x << ' ' << joystick.y << std::endl;
        }

        loopCounter++;
        if (!(loopCounter%500))
        {
            //std::cout << "*** Executing loop #" << loopCounter << std::endl;
            bubbles.push_back(new Bubble(bubbleTexture,randomWindowPosition(BigBubble),randomDirection(),randomSize(), randomSpeed()));
            numberOfBubbles++;
        }

        if (!pauseFlag)
        {
            BigBubble.move();
            if (BigBubble.collide(bubbles))
            {
                bigBubbleSound.play();
                sf::sleep(sf::Time(sf::seconds(1.0f)));
                bigBubbleSound.stop();
                window.close();
            }
            // move random bubbles
            for (auto each = bubbles.begin(); each != bubbles.end(); ++each)
            {
                if ((*each)->drift(BigBubble))
                {
                    bubbleSound.setVolume(100.0f * (*each)->getSize());
                    if (bubbleSound.getStatus() != sf::SoundSource::Playing)
                        bubbleSound.play();
                    bubbles.erase(each);
                    numberOfBubbles--;
                    break;
                }
            }
        }
        window.clear();
        window.draw(bottom);
        for (auto each = bubbles.begin(); each != bubbles.end(); ++each)
            window.draw(**each);
        window.draw(BigBubble);
        message.setString("Number of Bubbles " + std::to_string(numberOfBubbles));
        window.draw(message);
        window.display();
    }
    endOfGameSound.play();
    sf::sleep(sf::Time(sf::seconds(3.0f)));   // stall

    highScores.updateHighScores(Score(name.c_str(),numberOfBubbles,time(0)));
    highScores.WriteHighScoresFile();

}

sf::Vector2f randomWindowPosition(const Bubble& BigBubble)
{
    float distanceToTheBubble = 0.0f;
    float deltaX;
    float deltaY;
    sf::Vector2f temp;
    do
    {
        temp.x = rand()% static_cast<int>(MainWindowSize.x) *0.9f;
        temp.y = rand()% static_cast<int>(MainWindowSize.y) *0.9f;
        deltaX = temp.x - BigBubble.getPosition().x;
        deltaY = temp.y - BigBubble.getPosition().y;
        distanceToTheBubble = std::sqrt(deltaX * deltaX + deltaY * deltaY);
    }
    while (distanceToTheBubble < BigBubble.width()*BigBubble.getSize());
    return temp;
}


sf::Vector2f randomDirection()
{
    sf::Vector2f temp;
    temp.x = (rand() % 21 -10) / 25.0f;
    temp.y = (rand() % 22 -10) / 25.0f;
    return temp;
}

float randomSpeed()
{
    return 1.0f / (rand() % 19 + 2);
}

float randomSize()
{
    return 1.0f / (rand() % 10 + 2);

}

std::string welcome(sf::RenderWindow& window, const HighScores& highScores)
{
    std::string text, buffer,name;

    // Text
    sf::Font font;
    if (!font.loadFromFile(CourierFont))
    {
        std::cout << "Can't find font" << std::endl;
    }
    text = getWelcomeText();
    sf::Text instructions;
    instructions.setString(text.c_str());
    instructions.setFont(font);
    instructions.setCharacterSize(20); // in pixels, not points!
    instructions.setFillColor(sf::Color::Cyan);
    instructions.setPosition(90.0f,20.0f);

    // Create High Scores text
    sf::Text highScoresText;
    highScoresText.setFont(font);
    highScoresText.setCharacterSize(20); // in pixels, not points!
    highScoresText.setFillColor(sf::Color::Green);
    highScoresText.setPosition(360.0f,300.0f);

    // Write High Scores
    std::ostringstream sout;
    sout << highScores << std::endl;
    highScoresText.setString(sout.str());

    sf::Event evnt;
    char input = ' ';

    while (window.isOpen())
    {
        while (window.pollEvent(evnt))
        {
            switch (evnt.type)
            {
            case sf::Event::Closed:
                window.close();
                break;
            case sf::Event::KeyPressed:
                input = getKey();
                if ((input >= 'a' && input <= 'z')||(input >= 'A' && input <= 'Z')||input == ' ')
                {
                    name += input;
                }
                if (input == '\b') // backspace
                {
                    name = "";
                    text = getWelcomeText();
                }
                break;
            default:
                ;
            }
        }
        window.clear();
        instructions.setString((text+name+'_').c_str());
        window.draw(instructions);
        window.draw(highScoresText);

        window.display();
        if (isspace(name[0]))
            name = name.substr(1);  // remove leading space from name
        name[0] = toupper(name[0]);

        if (input == '\n')
        {
            window.clear();
            return name;
        }
    }
    return "";
}

std::string getWelcomeText()
{
    std::ifstream fin(WelcomeFile);
    std::string text, buffer;
    if (!fin)
    {
        std::cout << "Cannot open welcome file: " << WelcomeFile << std::endl;
        return "";
    }

    while (getline(fin, buffer))
    {
        text += buffer += '\n';
    }

    text += "\n          Please type your name and press Enter ===> ";

    fin.close();
    return text;
}


// Determine what key is pressed
char getKey()
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        return  'a';
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::B))
        return  'b';
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::C))
        return  'c';
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        return  'd';
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
        return  'e';
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::F))
        return  'f';
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::G))
        return  'g';
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::H))
        return  'h';
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::I))
        return  'i';
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::J))
        return  'j';
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::K))
        return  'k';
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::L))
        return  'l';
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::M))
        return  'm';
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::N))
        return  'n';
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::O))
        return  'o';
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::P))
        return  'p';
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
        return  'q';
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::R))
        return  'r';
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        return  's';
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::T))
        return  't';
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::U))
        return  'u';
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::V))
        return  'v';
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        return  'w';
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::X))
        return  'x';
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Y))
        return  'y';
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
        return  'z';
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
        return  ' ';
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))
        return  '\n';
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Backspace))
        return  '\b';
    return ' ';
}
