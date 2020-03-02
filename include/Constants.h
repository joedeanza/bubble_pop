
#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <SFML/Graphics.hpp>
#include <string>
#ifdef _WIN32
const std::string ResourceDirectory = "c:/Software/Resources/";
#else
const std::string ResourceDirectory = "";
#endif

const sf::Vector2f MainWindowSize{ 1024.f, 640.f };
const std::string WelcomeFile = ResourceDirectory + "bubble_pop/welcome.txt";
const std::string BubbleFile = ResourceDirectory + "bubble.png";
const std::string BubbleSoundFile = ResourceDirectory + "WormPop.wav";
const std::string BigBubbleSoundFile = ResourceDirectory + "mypop.wav";
const std::string EndOfGameSoundFile = ResourceDirectory + "EndOfGame.wav";
const std::string ArialFont = ResourceDirectory + "arial.ttf";
const std::string CourierFont = ResourceDirectory + "CourierNew.ttf";
const std::string BubbleSong = ResourceDirectory + "Lawrence_welk_bubbles_in_the_wine.ogg";



#endif /* CONSTANTS_H */
