#include "Snake.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <ctime>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>

SnakeSegment::SnakeSegment(int x, int y) : x(x), y(y) {}

Snake::Snake() {
    segments.push_back(SnakeSegment(10, 10));
    dir = RIGHT;
}

void Snake::move() {
    for (int i = segments.size() - 1; i > 0; --i)
        segments[i] = segments[i - 1];

    switch (dir) {
    case UP:    segments[0].y -= 1; break;
    case DOWN:  segments[0].y += 1; break;
    case LEFT:  segments[0].x -= 1; break;
    case RIGHT: segments[0].x += 1; break;
    }
}

void Snake::grow() {
    segments.push_back(segments.back());
}

bool Snake::checkCollision() const {
    for (size_t i = 1; i < segments.size(); ++i)
        if (segments[i].x == segments[0].x && segments[i].y == segments[0].y)
            return true;
    return false;
}

void Snake::setDirection(Direction d) {
    if ((dir == UP && d != DOWN) || (dir == DOWN && d != UP) ||
        (dir == LEFT && d != RIGHT) || (dir == RIGHT && d != LEFT))
        dir = d;
}

// Food generator that never appears in walls
sf::Vector2i generateFoodPosition(const Snake& snake) {
    sf::Vector2i pos;
    bool onSnake;
    int maxX = (width / blockSize) - 3;
    int maxY = (height / blockSize) - 4;
    do {
        onSnake = false;
        pos.x = rand() % maxX + 1;
        pos.y = rand() % maxY + 3; // leaving top bar and bottom border
        for (auto& s : snake.segments) {
            if (s.x == pos.x && s.y == pos.y) {
                onSnake = true;
                break;
            }
        }
    } while (onSnake);
    return pos;
}

// Booster generator (similar to food)
sf::Vector2i generateBoosterPosition(const Snake& snake) {
    sf::Vector2i pos;
    bool onSnake;
    int maxX = (width / blockSize) - 3;
    int maxY = (height / blockSize) - 4;
    do {
        onSnake = false;
        pos.x = rand() % maxX + 1;
        pos.y = rand() % maxY + 3;
        for (auto& s : snake.segments) {
            if (s.x == pos.x && s.y == pos.y) {
                onSnake = true;
                break;
            }
        }
    } while (onSnake);
    return pos;
}

void drawBorder(sf::RenderWindow& window, sf::RectangleShape& block) {
    block.setFillColor(sf::Color(139, 69, 19)); // Brick color

    int wBlocks = width / blockSize;
    int hBlocks = height / blockSize;

    for (int x = 0; x < wBlocks; ++x) {
        block.setPosition(x * blockSize, 2 * blockSize);
        window.draw(block);
    }

    for (int x = 0; x < wBlocks; ++x) {
        block.setPosition(x * blockSize, (hBlocks - 1) * blockSize);
        window.draw(block);
    }

    for (int y = 3; y < hBlocks - 1; ++y) {
        block.setPosition(0, y * blockSize);
        window.draw(block);
        block.setPosition((wBlocks - 1) * blockSize, y * blockSize);
        window.draw(block);
    }
}

// Write score to file
void saveScore(int score) {
    std::ofstream outFile("snake_scores.txt", std::ios::app);
    if (outFile.is_open()) {
        outFile <<"Score: "<< score << "\n";
        outFile.close();
    }
}

// Display scores
void displaySnakeScores() {
    sf::RenderWindow window(sf::VideoMode(400, 300), "Snake Scores");
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) return;

    std::ifstream inFile("snake_scores.txt");
    std::string line;
    std::vector<int> scores;
    if (inFile.is_open()) {
        while (std::getline(inFile, line)) {
            try { scores.push_back(std::stoi(line.substr(line.find(":") + 1))); }
            catch (...) {}
        }
        inFile.close();
    }

    std::sort(scores.begin(), scores.end(), std::greater<int>());

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event))
            if (event.type == sf::Event::Closed) window.close();

        window.clear(sf::Color::Black);
        sf::Text title("Snake Scores", font, 30);
        title.setFillColor(sf::Color::White);
        title.setPosition(100, 10);
        window.draw(title);

        int y = 60;
        int count = 0;
        for (int score : scores) {
            sf::Text scoreText(std::to_string(score), font, 24);
            scoreText.setFillColor(sf::Color::Green);
            scoreText.setPosition(150, y);
            window.draw(scoreText);
            y += 30;
            if (++count >= 10) break;
        }

        if (scores.empty()) {
            sf::Text noScores("No scores recorded yet.", font, 20);
            noScores.setFillColor(sf::Color::Red);
            noScores.setPosition(50, 150);
            window.draw(noScores);
        }
        window.display();
    }
}

void playSnake() {
    sf::RenderWindow window(sf::VideoMode(width, height), "Snake Game");
    window.setFramerateLimit(60);
    srand(static_cast<unsigned>(time(0)));

    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) return;

    // Sounds
    sf::SoundBuffer foodBuffer, gameOverBuffer;
    sf::Sound foodSound, gameOverSound;
    foodBuffer.loadFromFile("food.mp3");
    gameOverBuffer.loadFromFile("gameover.mp3");
    foodSound.setBuffer(foodBuffer);
    gameOverSound.setBuffer(gameOverBuffer);

    bool gameOver = false;
    bool gameOverSoundPlayed = false;

    Snake snake;
    sf::Vector2i food = generateFoodPosition(snake);

    // Booster handling
    sf::Vector2i booster(-1, -1);
    bool speedBoosterActive = false;
    bool pointBoosterActive = false;
    bool useSpeedBoosterNext = true; // alternate boosters

    sf::RectangleShape block(sf::Vector2f(blockSize - 1, blockSize - 1));
    int score = 0;
    float baseSpeed = 0.2f;
    int speedLevel = 0;
    const int speedIncreaseInterval = 3;
    const int maxSpeedLevel = 5;
    float moveTimer = 0.f;
    sf::Clock clock;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (gameOver) {
                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::Enter) {
                        // Restart
                        saveScore(score);
                        playSnake();
                        return;
                    } else if (event.key.code == sf::Keyboard::Escape) {
                        saveScore(score);
                        window.close();
                    }
                }
            } else {
                if (event.type == sf::Event::KeyPressed) {
                    switch (event.key.code) {
                    case sf::Keyboard::W: case sf::Keyboard::Up:    snake.setDirection(UP); break;
                    case sf::Keyboard::S: case sf::Keyboard::Down:  snake.setDirection(DOWN); break;
                    case sf::Keyboard::A: case sf::Keyboard::Left:  snake.setDirection(LEFT); break;
                    case sf::Keyboard::D: case sf::Keyboard::Right: snake.setDirection(RIGHT); break;
                    default: break;
                    }
                }
            }
        }

        float currentSpeed = std::max(0.05f, baseSpeed - 0.02f * speedLevel);
        float deltaTime = clock.restart().asSeconds();
        moveTimer += deltaTime;

        if (!gameOver && moveTimer >= currentSpeed) {
            moveTimer = 0.f;
            snake.move();

            if (snake.segments[0].x < 2 || snake.segments[0].x >= (width / blockSize) - 2 ||
                snake.segments[0].y < 2 || snake.segments[0].y >= (height / blockSize) - 2 ||
                snake.checkCollision()) {
                if (!gameOverSoundPlayed) {
                    gameOverSound.play();
                    gameOverSoundPlayed = true;
                }
                saveScore(score);
                gameOver = true;
            }

            // Food collision
            if (!gameOver && snake.segments[0].x == food.x && snake.segments[0].y == food.y) {
                snake.grow();
                score++;
                foodSound.play();
                food = generateFoodPosition(snake);

                // Check for booster spawn
                if (score % 10 == 0) {
                    booster = generateBoosterPosition(snake);
                    speedBoosterActive = useSpeedBoosterNext;
                    pointBoosterActive = !useSpeedBoosterNext;
                    useSpeedBoosterNext = !useSpeedBoosterNext;
                }

                if (score % speedIncreaseInterval == 0 && speedLevel < maxSpeedLevel)
                    speedLevel++;
            }

            // Booster collision
            if (booster.x == snake.segments[0].x && booster.y == snake.segments[0].y) {
                if (speedBoosterActive) {
                    if (speedLevel < maxSpeedLevel) speedLevel++;
                }
                if (pointBoosterActive) score += 5;
                booster = sf::Vector2i(-1, -1);
                speedBoosterActive = pointBoosterActive = false;
            }
        }

        window.clear(sf::Color::Black);

        // Score bar
        sf::RectangleShape scoreBar(sf::Vector2f(width, blockSize));
        scoreBar.setFillColor(sf::Color(50, 50, 50));
        scoreBar.setPosition(0, 0);
        window.draw(scoreBar);

        sf::Text scoreText;
        scoreText.setFont(font);
        scoreText.setString("Score: " + std::to_string(score));
        scoreText.setCharacterSize(24);
        scoreText.setFillColor(sf::Color::White);
        scoreText.setPosition(10, 2);
        window.draw(scoreText);

        drawBorder(window, block);

        // Draw snake
        block.setFillColor(sf::Color::Green);
        for (auto& s : snake.segments) {
            block.setPosition(s.x * blockSize, s.y * blockSize);
            window.draw(block);
        }

        // Draw food
        block.setFillColor(sf::Color::Red);
        block.setPosition(food.x * blockSize, food.y * blockSize);
        window.draw(block);

        // Draw booster
        if (booster.x != -1) {
            block.setFillColor(speedBoosterActive ? sf::Color::Blue : sf::Color::Yellow);
            block.setPosition(booster.x * blockSize, booster.y * blockSize);
            window.draw(block);
        }

        if (gameOver) {
            sf::RectangleShape overlay(sf::Vector2f(width, 200));
            overlay.setFillColor(sf::Color(255, 255, 255, 200));
            overlay.setPosition(0, 200);
            window.draw(overlay);

            sf::Text msg("Game Over", font, 50);
            msg.setFillColor(sf::Color::Red);
            msg.setStyle(sf::Text::Bold);
            msg.setPosition((width - msg.getLocalBounds().width) / 2, 210);
            window.draw(msg);

            sf::Text finalScore("Final Score: " + std::to_string(score), font, 30);
            finalScore.setFillColor(sf::Color::Black);
            finalScore.setPosition((width - finalScore.getLocalBounds().width) / 2, 270);
            window.draw(finalScore);

            sf::Text restart("Press 'Enter' to Restart or Esc to Exit", font, 25);
            restart.setFillColor(sf::Color::Black);
            restart.setPosition((width - restart.getLocalBounds().width) / 2, 320);
            window.draw(restart);
        }

        window.display();
    }
}
