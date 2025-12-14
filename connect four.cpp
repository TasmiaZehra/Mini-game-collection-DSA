#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <algorithm>

class ConnectFour {
public:
    static const int ROWS = 6;
    static const int COLS = 7;
    static const int CELL_SIZE = 100;
    static const int WIN_COUNT = 4;

    enum Player { NONE = 0, RED, YELLOW };

    ConnectFour() : board(ROWS, std::vector<Player>(COLS, NONE)), currentPlayer(RED), gameOver(false) {
        if (!font.loadFromFile("Arial.ttf")) {
            std::cerr << "Font loading failed.\n";
        }
        statusText.setFont(font);
        statusText.setCharacterSize(24);
        statusText.setFillColor(sf::Color::Black);
        statusText.setPosition(10, ROWS * CELL_SIZE + 10);  // Position below the board
    }

    void play(sf::RenderWindow& window) {
        resetGame();

        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window.close();

                if (!gameOver && event.type == sf::Event::MouseButtonPressed) {
                    int col = event.mouseButton.x / CELL_SIZE;
                    if (col >= 0 && col < COLS && dropToken(col, currentPlayer)) {
                        if (checkWin(currentPlayer)) {
                            gameOver = true;
                            winnerText = (currentPlayer == RED ? "Red wins!" : "Yellow wins!");
                        }
                        else {
                            currentPlayer = (currentPlayer == RED ? YELLOW : RED);
                        }
                    }
                }

                if (gameOver && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::R) {
                    resetGame();
                }

            }

            draw(window);
        }
    }

private:
    std::vector<std::vector<Player>> board;
    Player currentPlayer;
    bool gameOver;
    std::string winnerText;
    sf::Font font;
    sf::Text statusText;

    bool dropToken(int col, Player player) {
        for (int row = ROWS - 1; row >= 0; --row) {
            if (board[row][col] == NONE) {
                board[row][col] = player;
                return true;
            }
        }
        return false;
    }

    bool checkWin(Player player) {
        // Horizontal check
        for (int row = 0; row < ROWS; ++row)
            for (int col = 0; col <= COLS - WIN_COUNT; ++col)
                if (std::all_of(board[row].begin() + col, board[row].begin() + col + WIN_COUNT,
                    [=](Player p) { return p == player; }))
                    return true;

        // Vertical check
        for (int col = 0; col < COLS; ++col)
            for (int row = 0; row <= ROWS - WIN_COUNT; ++row) {
                bool win = true;
                for (int i = 0; i < WIN_COUNT; ++i)
                    if (board[row + i][col] != player)
                        win = false;
                if (win) return true;
            }

        // Diagonal down-left check
        for (int row = WIN_COUNT - 1; row < ROWS; ++row)
            for (int col = 0; col <= COLS - WIN_COUNT; ++col) {
                bool win = true;
                for (int i = 0; i < WIN_COUNT; ++i)
                    if (board[row - i][col + i] != player)
                        win = false;
                if (win) return true;
            }

        // Diagonal down-right check
        for (int row = 0; row <= ROWS - WIN_COUNT; ++row)
            for (int col = 0; col <= COLS - WIN_COUNT; ++col) {
                bool win = true;
                for (int i = 0; i < WIN_COUNT; ++i)
                    if (board[row + i][col + i] != player)
                        win = false;
                if (win) return true;
            }

        return false;
    }

    void resetGame() {
        for (auto& row : board)
            std::fill(row.begin(), row.end(), NONE);
        currentPlayer = RED;
        gameOver = false;
        winnerText = "";
    }

    void draw(sf::RenderWindow& window) {
        window.clear(sf::Color::White);

        // Draw background grid
        sf::RectangleShape background(sf::Vector2f(COLS * CELL_SIZE, ROWS * CELL_SIZE));
        background.setFillColor(sf::Color(0, 102, 204)); // Blue grid
        background.setPosition(0, 0);
        window.draw(background);

        // Draw tokens
        sf::CircleShape token((CELL_SIZE - 10) / 2);
        token.setPointCount(100);

        for (int row = 0; row < ROWS; ++row) {
            for (int col = 0; col < COLS; ++col) {
                float centerX = col * CELL_SIZE + CELL_SIZE / 2;
                float centerY = row * CELL_SIZE + CELL_SIZE / 2;
                token.setPosition(centerX - token.getRadius(), centerY - token.getRadius());

                switch (board[row][col]) {
                case RED:    token.setFillColor(sf::Color::Red); break;
                case YELLOW: token.setFillColor(sf::Color::Yellow); break;
                default:     token.setFillColor(sf::Color(230, 230, 230)); break;
                }

                window.draw(token);
            }
        }
        if (gameOver) {
            // Draw horizontal overlay: full width, fixed height
            float overlayHeight = 200;
            sf::RectangleShape overlay(sf::Vector2f(COLS * CELL_SIZE, overlayHeight));
            overlay.setFillColor(sf::Color(255, 255, 255, 220)); // Semi-transparent white
            overlay.setPosition(0, (ROWS * CELL_SIZE - overlayHeight) / 2); // Vertically centered
            overlay.setOutlineColor(sf::Color::Black);
            overlay.setOutlineThickness(2);
            window.draw(overlay);

            // Winner message
            sf::Text result(winnerText, font, 36);
            result.setFillColor(sf::Color::Red);
            result.setStyle(sf::Text::Bold);
            float resultX = (COLS * CELL_SIZE - result.getLocalBounds().width) / 2;
            float resultY = overlay.getPosition().y + 40;
            result.setPosition(resultX, resultY);
            window.draw(result);

            // Restart instruction
            sf::Text restart("Press 'R' to restart", font, 24);
            restart.setFillColor(sf::Color::Black);
            float restartX = (COLS * CELL_SIZE - restart.getLocalBounds().width) / 2;
            restart.setPosition(restartX, resultY + 50);
            window.draw(restart);
        }

        window.draw(statusText);
        window.display();
    }
};

// Launcher
void playConnectFourGame() {
    sf::RenderWindow window(sf::VideoMode(ConnectFour::COLS * ConnectFour::CELL_SIZE, ConnectFour::ROWS * ConnectFour::CELL_SIZE + 60), "Connect Four");
    ConnectFour game;
    game.play(window);
}
