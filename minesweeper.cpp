#include "Minesweeper.hpp"
#include <cstdlib>
#include <ctime>

namespace Minesweeper {

    std::vector<std::vector<Tile>> grid(GRID_SIZE, std::vector<Tile>(GRID_SIZE));
    bool gameOver = false;
    bool won = false;
    time_t startTime;
    int bestTime = INT_MAX;
    bool timerRunning = false;
    int finalTime = 0;
    sf::Font font;

    void loadHighScore() {
        std::ifstream file("minesweeper_besttime.txt");
        if (file.is_open()) {
            file >> bestTime;
            if (file.fail()) bestTime = INT_MAX;
            file.close();
        }
    }

    void saveHighScore(int time) {
        std::ofstream file("minesweeper_besttime.txt");
        if (file.is_open()) {
            file << time;
            file.close();
        }
    }

    void placeMines() {
        srand(static_cast<unsigned>(time(nullptr)));
        int placed = 0;
        while (placed < NUM_MINES) {
            int r = rand() % GRID_SIZE;
            int c = rand() % GRID_SIZE;
            if (!grid[r][c].mine) {
                grid[r][c].mine = true;
                placed++;
            }
        }
    }

    void calculateAdjacency() {
        for (int r = 0; r < GRID_SIZE; ++r) {
            for (int c = 0; c < GRID_SIZE; ++c) {
                if (grid[r][c].mine) continue;
                int count = 0;
                for (int dr = -1; dr <= 1; ++dr) {
                    for (int dc = -1; dc <= 1; ++dc) {
                        int nr = r + dr, nc = c + dc;
                        if (nr >= 0 && nr < GRID_SIZE && nc >= 0 && nc < GRID_SIZE && grid[nr][nc].mine)
                            count++;
                    }
                }
                grid[r][c].adjacentMines = count;
            }
        }
    }

    void reveal(int r, int c) {
        if (r < 0 || r >= GRID_SIZE || c < 0 || c >= GRID_SIZE || grid[r][c].revealed || grid[r][c].flagged)
            return;
        grid[r][c].revealed = true;
        if (grid[r][c].adjacentMines == 0 && !grid[r][c].mine) {
            for (int dr = -1; dr <= 1; ++dr)
                for (int dc = -1; dc <= 1; ++dc)
                    reveal(r + dr, c + dc);
        }
    }

    bool checkWin() {
        for (int r = 0; r < GRID_SIZE; ++r)
            for (int c = 0; c < GRID_SIZE; ++c)
                if (!grid[r][c].mine && !grid[r][c].revealed)
                    return false;
        return true;
    }

    void playMinesweeper() {
        sf::RenderWindow window(sf::VideoMode(GRID_SIZE * TILE_SIZE, GRID_SIZE * TILE_SIZE + BOTTOM_UI_HEIGHT), "Minesweeper 9x9");
        sf::RectangleShape tileShape(sf::Vector2f(TILE_SIZE - 2, TILE_SIZE - 2));

        if (!font.loadFromFile("arial.ttf")) return;

        auto resetGame = [&]() {
            for (auto& row : grid)
                for (auto& tile : row)
                    tile = Tile{};

            placeMines();
            calculateAdjacency();

            gameOver = false;
            won = false;
            startTime = time(nullptr);
            timerRunning = true;
            finalTime = 0;      // <-- RESET SAVED TIME
            };

        loadHighScore();
        resetGame();

        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window.close();

                if (!gameOver && event.type == sf::Event::MouseButtonPressed) {
                    int x = event.mouseButton.x / TILE_SIZE;
                    int y = event.mouseButton.y / TILE_SIZE;
                    if (x < GRID_SIZE && y < GRID_SIZE) {

                        // LEFT CLICK
                        if (event.mouseButton.button == sf::Mouse::Left && !grid[y][x].flagged) {
                            if (grid[y][x].mine) {

                                // ---- LOSS ----
                                gameOver = true;
                                timerRunning = false;
                                finalTime = static_cast<int>(time(nullptr) - startTime);   // <-- FREEZE TIME
                                grid[y][x].revealed = true;

                                for (auto& row : grid)
                                    for (auto& tile : row)
                                        if (tile.mine)
                                            tile.revealed = true;
                            }
                            else {
                                reveal(y, x);

                                if (checkWin()) {

                                    // ---- WIN ----
                                    gameOver = true;
                                    won = true;
                                    timerRunning = false;
                                    finalTime = static_cast<int>(time(nullptr) - startTime);  // <-- FREEZE TIME

                                    if (finalTime < bestTime) {
                                        bestTime = finalTime;
                                        saveHighScore(finalTime);
                                    }
                                }
                            }
                        }
                        // RIGHT CLICK
                        else if (event.mouseButton.button == sf::Mouse::Right) {
                            grid[y][x].flagged = !grid[y][x].flagged;
                        }
                    }
                }
                else if (gameOver && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
                    resetGame();
                }
            }

            window.clear(sf::Color::White);

            // DRAW GRID
            for (int r = 0; r < GRID_SIZE; ++r) {
                for (int c = 0; c < GRID_SIZE; ++c) {
                    tileShape.setPosition(c * TILE_SIZE + 1, r * TILE_SIZE + 1);
                    tileShape.setFillColor(grid[r][c].revealed ? sf::Color(180, 180, 180) : sf::Color(200, 200, 200));
                    window.draw(tileShape);

                    if (grid[r][c].flagged && !grid[r][c].revealed) {
                        sf::Text flag("F", font, 20);
                        flag.setFillColor(sf::Color::Red);
                        flag.setPosition(c * TILE_SIZE + 8, r * TILE_SIZE + 4);
                        window.draw(flag);
                    }
                    else if (grid[r][c].revealed && grid[r][c].mine) {
                        sf::CircleShape mineShape(10);
                        mineShape.setFillColor(sf::Color::Black);
                        mineShape.setPosition(c * TILE_SIZE + 6, r * TILE_SIZE + 6);
                        window.draw(mineShape);
                    }
                    else if (grid[r][c].revealed && grid[r][c].adjacentMines > 0) {
                        sf::Text number(std::to_string(grid[r][c].adjacentMines), font, 20);
                        number.setFillColor(sf::Color::Blue);
                        number.setPosition(c * TILE_SIZE + 10, r * TILE_SIZE + 4);
                        window.draw(number);
                    }
                }
            }

            // ---- FIXED TIMER DISPLAY ----
            int elapsedTime;

            if (timerRunning)
                elapsedTime = static_cast<int>(time(nullptr) - startTime);
            else
                elapsedTime = finalTime;   // <-- SHOW FROZEN TIME

            // BEST TIME UI
            std::string bestTimeStr = (bestTime == INT_MAX) ? "N/A" : std::to_string(bestTime);
            sf::Text bestTimeText("Best Time: " + bestTimeStr + "s", font, 20);
            bestTimeText.setFillColor(sf::Color::Black);
            bestTimeText.setPosition(5, GRID_SIZE * TILE_SIZE + 5);
            window.draw(bestTimeText);

            // CURRENT TIME UI
            sf::Text elapsedTimeText("Time: " + std::to_string(elapsedTime) + "s", font, 20);
            elapsedTimeText.setFillColor(sf::Color::Black);
            elapsedTimeText.setPosition(5, GRID_SIZE * TILE_SIZE + 30);
            window.draw(elapsedTimeText);

            // GAME OVER OVERLAY
            if (gameOver) {
                sf::RectangleShape overlay(sf::Vector2f(GRID_SIZE * TILE_SIZE, 60));
                overlay.setFillColor(sf::Color(255, 255, 255, 220));
                overlay.setPosition(0, GRID_SIZE * TILE_SIZE / 2.f - 30);
                window.draw(overlay);

                sf::Text msg(won ? "You Win!" : "Game Over", font, 25);
                msg.setFillColor(won ? sf::Color::Green : sf::Color::Red);
                msg.setStyle(sf::Text::Bold);
                msg.setPosition(GRID_SIZE * TILE_SIZE / 2.f - msg.getLocalBounds().width / 2.f, GRID_SIZE * TILE_SIZE / 2.f - 20);
                window.draw(msg);

                sf::Text restartMsg("Press Enter to Restart", font, 20);
                restartMsg.setFillColor(sf::Color::Black);
                restartMsg.setPosition(GRID_SIZE * TILE_SIZE / 2.f - restartMsg.getLocalBounds().width / 2.f, GRID_SIZE * TILE_SIZE / 2.f + 10);
                window.draw(restartMsg);
            }

            window.display();
        }
    }
}
