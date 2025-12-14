#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include "ConnectFour.hpp"
#include "TicTacToe.hpp"
#include "MemoryMatch.hpp"
#include "Hangman.hpp"
#include "Snake.hpp"
#include "minesweeper.hpp"

struct Button {
    sf::RectangleShape shape;
    sf::Text text;

    void setPosition(float x, float y) {
        shape.setPosition(x, y);
        sf::FloatRect textBounds = text.getLocalBounds();
        text.setOrigin(textBounds.left + textBounds.width / 2.f,
            textBounds.top + textBounds.height / 2.f);
        text.setPosition(x + shape.getSize().x / 2.f, y + shape.getSize().y / 2.f);
    }

    bool isMouseOver(sf::RenderWindow& window) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        return shape.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
    }
};

void showScoreMenu(sf::RenderWindow& window, sf::Font& font) {
    const int buttonWidth = 270;
    const int buttonHeight = 48;
    const int buttonSpacing = 15;

    std::vector<Button> scoreButtons(7);
    std::string scoreLabels[] = {
        "Tic Tac Toe Scores",
        "Connect Four Scores",
        "Memory Match Scores",
        "Hangman Scores",
        "Snake Scores",
        "Mine Sweeper Scores",
        "Back to Main Menu"
    };

    for (int i = 0; i < 7; ++i) {
        scoreButtons[i].shape.setSize(sf::Vector2f(buttonWidth, buttonHeight));
        scoreButtons[i].shape.setFillColor(sf::Color(180, 180, 20));
        scoreButtons[i].text.setFont(font);
        scoreButtons[i].text.setString(scoreLabels[i]);
        scoreButtons[i].text.setCharacterSize(24);
        scoreButtons[i].text.setFillColor(sf::Color::Black);

        float x = (window.getSize().x - buttonWidth) / 2.f;
        float y = 160 + i * (buttonHeight + buttonSpacing);
        scoreButtons[i].setPosition(x, y);
    }

    sf::Text heading;
    heading.setFont(font);
    heading.setString("GAME SCORES");
    heading.setCharacterSize(30);
    heading.setFillColor(sf::Color::White);
    heading.setStyle(sf::Text::Bold);
    sf::FloatRect headingBounds = heading.getLocalBounds();
    heading.setOrigin(headingBounds.left + headingBounds.width / 2.f,
        headingBounds.top + headingBounds.height / 2.f);
    heading.setPosition(window.getSize().x / 2.f, 60.f);

    bool inScoreMenu = true;
    while (inScoreMenu && window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                for (int i = 0; i < 7; ++i) {
                    if (scoreButtons[i].isMouseOver(window)) {
                        if (i == 6) return; // Back to main menu
                        std::string filenames[] = {
                            "highscores.txt", "connectfour_scores.txt", "memorymatch_scores.txt",
                            "hangman_scores.txt", "snake_scores.txt", "minesweeper_scores.txt"
                        };

                        std::ifstream file(filenames[i]);
                        std::string line, content;
                        if (file.is_open()) {
                            while (getline(file, line)) content += line + "\n";
                            file.close();
                        }
                        else {
                            content = "No scores available.";
                        }

                        sf::Text scoreText(content, font, 22);
                        scoreText.setFillColor(sf::Color::White);
                        scoreText.setPosition(30, 100);

                        sf::RenderWindow scoreWindow(sf::VideoMode(500, 400), scoreLabels[i]);
                        while (scoreWindow.isOpen()) {
                            sf::Event e;
                            while (scoreWindow.pollEvent(e)) {
                                if (e.type == sf::Event::Closed ||
                                    (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::Escape)) {
                                    scoreWindow.close();
                                }
                            }
                            scoreWindow.clear(sf::Color(0, 0, 60)); // Score window background color
                            scoreWindow.draw(scoreText);
                            scoreWindow.display();
                        }
                    }
                }
            }
        }

        for (auto& btn : scoreButtons) {
            if (btn.isMouseOver(window)) {
                btn.shape.setFillColor(sf::Color(100, 100, 250));
                btn.text.setFillColor(sf::Color::White);
            }
            else {
                btn.shape.setFillColor(sf::Color(180, 180, 180));
                btn.text.setFillColor(sf::Color::Black);
            }
        }

        window.clear(sf::Color(0, 60, 0)); // Different background for score menu
        window.draw(heading);
        for (auto& btn : scoreButtons) {
            window.draw(btn.shape);
            window.draw(btn.text);
        }
        window.display();
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode(400, 700), "Mini Games Collection");
    sf::Font font;
    if (!font.loadFromFile("Arial.ttf")) {
        std::cerr << "Failed to load font\n";
        return 1;
    }

    sf::Text welcomeText;
    welcomeText.setFont(font);
    welcomeText.setString("    WELCOME  \n TO MINI GAME \n  COLLECTION");
    welcomeText.setCharacterSize(35);
    welcomeText.setFillColor(sf::Color::White);
    welcomeText.setStyle(sf::Text::Bold);
    sf::FloatRect welcomeBounds = welcomeText.getLocalBounds();
    welcomeText.setOrigin(welcomeBounds.left + welcomeBounds.width / 2.f,
        welcomeBounds.top + welcomeBounds.height / 2.f);
    welcomeText.setPosition(window.getSize().x / 2.f, 60.f);

    const int buttonWidth = 270;
    const int buttonHeight = 48;
    const int buttonSpacing = 15;

    std::vector<Button> buttons(8);
    std::string labels[] = {
        "1. Connect Four", "2. Tic Tac Toe", "3. Memory Match", "4. Hangman",
        "5. Snake", "6. Mine Sweeper", "View Scores", "Exit"
    };

    for (int i = 0; i < 8; ++i) {
        buttons[i].shape.setSize(sf::Vector2f(buttonWidth, buttonHeight));
        buttons[i].shape.setFillColor(sf::Color(180, 180, 180));
        buttons[i].text.setFont(font);
        buttons[i].text.setString(labels[i]);
        buttons[i].text.setCharacterSize(24);
        buttons[i].text.setFillColor(sf::Color::Black);

        float x = (window.getSize().x - buttonWidth) / 2.f;
        float y = 140 + i * (buttonHeight + buttonSpacing);
        buttons[i].setPosition(x, y);
    }

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                for (int i = 0; i < 8; ++i) {
                    if (buttons[i].isMouseOver(window)) {
                        switch (i) {
                        case 0: playConnectFourGame(); break;
                        case 1: playTicTacToe(); break;
                        case 2: playMemoryMatch(); break;
                        case 3: playHangman(); break;
                        case 4: playSnake(); break;
                        case 5: Minesweeper::playMinesweeper(); break;
                        case 6: showScoreMenu(window, font); break;
                        case 7: window.close(); break;
                        }
                    }
                }
            }
        }

        for (auto& btn : buttons) {
            if (btn.isMouseOver(window)) {
                btn.shape.setFillColor(sf::Color(100, 100, 250));
                btn.text.setFillColor(sf::Color::White);
            }
            else {
                btn.shape.setFillColor(sf::Color(180, 180, 180));
                btn.text.setFillColor(sf::Color::Black);
            }
        }

        window.clear(sf::Color(128, 0, 128));
        window.draw(welcomeText);
        for (auto& btn : buttons) {
            window.draw(btn.shape);
            window.draw(btn.text);
        }
        window.display();
    }

    return 0;
}
