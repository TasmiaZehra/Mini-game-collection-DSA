#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <vector>
#include <random>
#include <ctime>
#include <algorithm>

using namespace std;
using namespace sf;

const int gridSize = 4;
const int totalCards = gridSize * gridSize;
const float cardSize = 100.f;
const float spacing = 10.f;
const float offset = 20.f;

Font font;

// ✅ New Overlay: Center Strip for End Message
void drawOverlay(RenderWindow& window, const string& message) {
    float stripHeight = 120.f;
    float stripY = (window.getSize().y - stripHeight) / 2.f;

    RectangleShape strip(Vector2f(window.getSize().x - 2 * offset, stripHeight));
    strip.setPosition(offset, stripY);
    strip.setFillColor(Color(255, 255, 255, 220));  // light opaque strip
    strip.setOutlineThickness(0);  // No border
    window.draw(strip);

    Text winText(message, font, 40);
    winText.setFillColor(Color::Red);
    FloatRect bounds = winText.getLocalBounds();
    winText.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
    winText.setPosition(window.getSize().x / 2.f, stripY + stripHeight / 2.f - 20);
    window.draw(winText);

    Text restart("Press 'R' to restart", font, 24);
    restart.setFillColor(Color::Black);
    FloatRect bounds2 = restart.getLocalBounds();
    restart.setOrigin(bounds2.width / 2.f, bounds2.height / 2.f);
    restart.setPosition(window.getSize().x / 2.f, stripY + stripHeight / 2.f + 25);
    window.draw(restart);
}

void playMemoryMatch() {
    RenderWindow window(VideoMode(500, 500), "Memory Game - 4x4");

    if (!font.loadFromFile("arial.ttf")) {
        cout << "Failed to load font!\n";
        return;
    }

    vector<int> sequence(totalCards);
    for (int i = 0; i < totalCards; i += 2) {
        sequence[i] = sequence[i + 1] = i / 2;
    }

    shuffle(sequence.begin(), sequence.end(), default_random_engine(static_cast<unsigned>(time(0))));
    vector<bool> revealed(totalCards, false);
    vector<bool> matched(totalCards, false);
    vector<int> currentChoice;

    int score = 0;
    Clock clock;
    bool isPaused = false;
    bool gameOver = false;
    Time pauseStart;

    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed)
                window.close();
            else if (event.type == Event::KeyPressed && event.key.code == Keyboard::R && gameOver) {
                // Reset game
                shuffle(sequence.begin(), sequence.end(), default_random_engine(static_cast<unsigned>(time(0))));
                fill(revealed.begin(), revealed.end(), false);
                fill(matched.begin(), matched.end(), false);
                currentChoice.clear();
                score = 0;
                isPaused = false;
                gameOver = false;
            }
            else if (!gameOver && event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
                int x = event.mouseButton.x;
                int y = event.mouseButton.y;

                for (int i = 0; i < totalCards; ++i) {
                    float posX = (i % gridSize) * (cardSize + spacing) + offset;
                    float posY = (i / gridSize) * (cardSize + spacing) + offset;

                    FloatRect bounds(posX, posY, cardSize, cardSize);
                    if (bounds.contains(static_cast<float>(x), static_cast<float>(y)) && !revealed[i] && !matched[i]) {
                        if (currentChoice.size() < 2) {
                            revealed[i] = true;
                            currentChoice.push_back(i);
                        }
                    }
                }
            }
        }

        if (!isPaused && currentChoice.size() == 2) {
            isPaused = true;
            pauseStart = clock.getElapsedTime();
        }

        if (isPaused && clock.getElapsedTime() - pauseStart > seconds(1)) {
            if (sequence[currentChoice[0]] == sequence[currentChoice[1]]) {
                matched[currentChoice[0]] = matched[currentChoice[1]] = true;
                score++;
            }
            else {
                revealed[currentChoice[0]] = revealed[currentChoice[1]] = false;
            }
            currentChoice.clear();
            isPaused = false;
        }

        if (!gameOver && all_of(matched.begin(), matched.end(), [](bool m) { return m; })) {
            gameOver = true;
        }

        window.clear(Color(240, 240, 240)); // Light background

        // Draw cards
        for (int i = 0; i < totalCards; ++i) {
            float posX = (i % gridSize) * (cardSize + spacing) + offset;
            float posY = (i / gridSize) * (cardSize + spacing) + offset;

            RectangleShape card(Vector2f(cardSize, cardSize));
            card.setPosition(posX, posY);
            card.setOutlineThickness(2);
            card.setOutlineColor(Color::Black);

            if (revealed[i] || matched[i]) {
                card.setFillColor(Color(215, 245, 227));  // revealed
                window.draw(card);

                Text number(to_string(sequence[i]), font, 36);
                number.setFillColor(Color(22, 101, 52)); // Deep green
                number.setStyle(Text::Bold);

                FloatRect textRect = number.getLocalBounds();
                number.setOrigin(textRect.left + textRect.width / 2.f, textRect.top + textRect.height / 2.f);
                number.setPosition(posX + cardSize / 2.f, posY + cardSize / 2.f);
                window.draw(number);
            }
            else {
                card.setFillColor(Color(74, 222, 128)); // light green back
                window.draw(card);

                Text star("*", font, 36);
                star.setFillColor(Color(22, 101, 52)); // deep green
                star.setStyle(Text::Bold);

                FloatRect textRect = star.getLocalBounds();
                star.setOrigin(textRect.left + textRect.width / 2.f, textRect.top + textRect.height / 2.f);
                star.setPosition(posX + cardSize / 2.f, posY + cardSize / 2.f);
                window.draw(star);
            }
        }

        if (gameOver) {
            drawOverlay(window, "You Win!");
        }

        window.display();
    }
}
