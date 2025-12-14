#ifndef HANGMAN_HPP
#define HANGMAN_HPP

#include <SFML/Graphics.hpp>
#include <ctime>
#include <cstdlib>
#include <string>
#include <vector>

#define MAX_TRIES 6

// Word and hint pairs
std::vector<std::pair<std::string, std::string>> wordHintList = {
{"blizzard", "A storm that erases the world in white"},
{"eclipse", "When light hides behind a perfect shadow"},
{"avalanche", "A mountain’s sudden downward roar"},
{"tornado", "A spinning column with no mercy"},
{"mirage", "A lie made of heat and distance"},
{"horizon", "Where the sky pretends to touch the earth"},
{"nebula", "A cosmic cloud where stars are born"},
{"tempest", "A storm with a furious personality"},

{"chameleon", "A creature that refuses to choose one color"},
{"raven", "A dark bird with a darker reputation"},
{"lynx", "A wild cat with pointed ears of mystery"},
{"panther", "A silent shadow in the jungle"},
{"vulture", "A patient bird waiting for the last heartbeat"},
{"scorpion", "A tail that strikes before you notice"},
{"leviathan", "A legendary monster from the deep"},

{"phoenix", "A creature that dies only to rise again"},
{"chimera", "A beast made of impossible parts"},
{"griffin", "A guardian with a lion's body and eagle wings"},
{"minotaur", "A beast forever trapped in a maze"},
{"cyclops", "A giant with a single all-seeing eye"},

{"paradox", "A truth that contradicts itself"},
{"epiphany", "A sudden flash of understanding"},
{"enigma", "A mystery wrapped in confusion"},
{"serendipity", "A fortunate discovery by accident"},
{"labyrinth", "A place where the exit hides from you"},
{"obsidian", "Dark volcanic glass, sharp as betrayal"},
{"eloquence", "Speech that flows like poetry"},
{"empathy", "Feeling what another heart feels"},
{"zenith", "The highest point in the sky"},
{"harbinger", "A sign of things to come"},
{"nebulous", "Cloudy, unclear, hard to define"},

{"algorithm", "A sequence of steps computers obey"},
{"quantum", "The strange world of the very small"},
{"hologram", "Light pretending to be real"},
{"circuitry", "The nervous system of machines"},
{"fractal", "A pattern that repeats forever"},

{"silhouette", "A story told only in shadow"},
{"reverie", "A daydream that steals your attention"},
{"phantom", "Seen but never truly there"},
{"ascend", "To rise toward the impossible"},
{"eternal", "Something without beginning or end"},
{"solitude", "Alone, yet not always lonely"},
{"illusion", "When your eyes lie to your mind"}
};

void DrawHangman(sf::RenderWindow& window, int triesLeft) {
    sf::RectangleShape line(sf::Vector2f(100, 5));
    line.setFillColor(sf::Color::Black);

    // Base
    line.setPosition(50, 400);
    window.draw(line);

    // Pole
    line.setSize(sf::Vector2f(5, 300));
    line.setPosition(100, 100);
    window.draw(line);

    // Beam
    line.setSize(sf::Vector2f(150, 5));
    line.setPosition(100, 100);
    window.draw(line);

    // Rope
    line.setSize(sf::Vector2f(5, 50));
    line.setPosition(250, 100);
    window.draw(line);

    int parts = MAX_TRIES - triesLeft;

    if (parts >= 1) {
        sf::CircleShape head(30);
        head.setFillColor(sf::Color::Transparent);
        head.setOutlineColor(sf::Color::Black);
        head.setOutlineThickness(3);
        head.setPosition(220, 150);
        window.draw(head);
    }
    if (parts >= 2) {
        line.setSize(sf::Vector2f(5, 100));
        line.setRotation(0);
        line.setPosition(250, 210); // body
        window.draw(line);
    }
    if (parts >= 3) {
        line.setSize(sf::Vector2f(40, 5));
        line.setRotation(45);
        line.setPosition(250, 230); // left arm
        window.draw(line);
        line.setRotation(0);
    }
    if (parts >= 4) {
        line.setSize(sf::Vector2f(40, 5));
        line.setRotation(135);
        line.setPosition(254, 234); // right arm
        window.draw(line);
        line.setRotation(0);
    }
    if (parts >= 5) {
        line.setSize(sf::Vector2f(40, 5));
        line.setRotation(135); // left leg
        line.setPosition(255, 310);
        window.draw(line);
        line.setRotation(0);
    }
    if (parts >= 6) {
        line.setSize(sf::Vector2f(40, 5));
        line.setRotation(45); // right leg
        line.setPosition(255, 310);
        window.draw(line);
        line.setRotation(0);
    }
}

void playHangman() {
    sf::Font font;
    if (!font.loadFromFile("Arial.ttf")) {
        return; // Ensure "Arial.ttf" is in working directory
    }

    bool playAgain = true;

    while (playAgain) {
        srand(static_cast<unsigned>(time(0)));

        // Choose random word and hint
        auto [word, hint] = wordHintList[rand() % wordHintList.size()];
        int wordLength = static_cast<int>(word.length());
        std::string dispWord(wordLength * 2 - 1, ' '); // for spaces between _
        for (int i = 0; i < wordLength; ++i) {
            dispWord[i * 2] = '_';
        }

        std::string guessedLetters;
        int found = 0;
        int tries = MAX_TRIES;

        sf::RenderWindow window(sf::VideoMode(700, 500), "Hangman Game - SFML", sf::Style::Default);

        sf::Text wordText;
        wordText.setFont(font);
        wordText.setCharacterSize(36);
        wordText.setPosition(45, 45);
        wordText.setFillColor(sf::Color::Blue);

        sf::Text guessedText;
        guessedText.setFont(font);
        guessedText.setCharacterSize(24);
        guessedText.setPosition(330, 160);
        guessedText.setFillColor(sf::Color::Black);

        sf::Text messageText;
        messageText.setFont(font);
        messageText.setCharacterSize(30);
        messageText.setPosition(180, 400);
        messageText.setFillColor(sf::Color::Red);

        sf::Text hintText;
        hintText.setFont(font);
        hintText.setCharacterSize(24);
        hintText.setPosition(300, 120);
        hintText.setFillColor(sf::Color::Magenta);
        hintText.setString("Hint: " + hint);

        bool gameOver = false;

        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window.close();

                if (!gameOver && event.type == sf::Event::TextEntered && isalpha(event.text.unicode) && found < wordLength && tries > 0) {
                    char guess = static_cast<char>(tolower(event.text.unicode));
                    if (guessedLetters.find(guess) == std::string::npos) {
                        guessedLetters += guess;
                        bool correct = false;
                        for (int i = 0; i < wordLength; ++i) {
                            if (word[i] == guess && dispWord[i * 2] == '_') {
                                dispWord[i * 2] = guess;
                                found++;
                                correct = true;
                            }
                        }
                        if (!correct) {
                            tries--;
                        }
                    }
                }
                else if (gameOver && event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::Enter) {
                        // Restart the game properly
                        window.close();     // close current game window
                        playAgain = true;   // allow the outer loop to start a new game
                        break;              // exit the current window loop
                    }
                    else if (event.key.code == sf::Keyboard::Escape) {
                        window.close();
                        playAgain = false;
                    }
                }

            }

            wordText.setString("Word: " + dispWord);
            guessedText.setString("Guessed: " + guessedLetters);

            if (found == wordLength) {
                messageText.setString("You Win! Press Enter to Restart \nor ESC to Quit");
                gameOver = true;
            }
            else if (tries == 0) {
                messageText.setString("You Lose! Word was: " + word + "\nPress Enter to Restart or ESC to Quit");
                gameOver = true;
            }
            else {
                messageText.setString("");
            }

            window.clear(sf::Color(255, 255, 180));
            DrawHangman(window, tries);
            window.draw(wordText);
            window.draw(guessedText);
            window.draw(hintText);
            window.draw(messageText);
            window.display();

            if (!window.isOpen()) break;
        }

        // If the window closed during gameplay (not by R or ESC)
        if (!gameOver) {
            playAgain = false;  // Exit if closed prematurely
        }
    }
}


#endif // HANGMAN_HPP
