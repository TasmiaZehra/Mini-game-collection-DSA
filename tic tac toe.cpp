#include "TicTacToe.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <string>

char board[3][3];
char currentPlayer = 'X';
int playerWins = 0, computerWins = 0, draws = 0;

void initializeBoard() {
    char num = '1';
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            board[i][j] = num++;
}

bool isWinner(char symbol) {
    for (int i = 0; i < 3; i++)
        if ((board[i][0] == symbol && board[i][1] == symbol && board[i][2] == symbol) ||
            (board[0][i] == symbol && board[1][i] == symbol && board[2][i] == symbol))
            return true;

    return (board[0][0] == symbol && board[1][1] == symbol && board[2][2] == symbol) ||
        (board[0][2] == symbol && board[1][1] == symbol && board[2][0] == symbol);
}

bool isDraw() {
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            if (board[i][j] != 'X' && board[i][j] != 'O')
                return false;
    return true;
}

void saveScores() {
    std::ofstream file("tictactoe_scores.txt");
    if (file.is_open()) {
        file << "Player Wins: " << playerWins << "\n";
        file << "Computer Wins: " << computerWins << "\n";
        file << "Draws: " << draws << "\n";
        file.close();
    }
}

void loadScores() {
    std::ifstream file("tictactoe_scores.txt");
    std::string line;
    if (file.is_open()) {
        file >> line >> playerWins;
        file >> line >> computerWins;
        file >> line >> draws;
        file.close();
    }
}

sf::Vector2i getBoardPosition(int x, int y) {
    return { x / 200, y / 200 };
}

void drawBoard(sf::RenderWindow& window, sf::Font& font) {
    for (int i = 1; i < 3; ++i) {
        sf::RectangleShape lineH(sf::Vector2f(600, 5));
        lineH.setPosition(0, i * 200);
        lineH.setFillColor(sf::Color::Black);
        window.draw(lineH);

        sf::RectangleShape lineV(sf::Vector2f(5, 600));
        lineV.setPosition(i * 200, 0);
        lineV.setFillColor(sf::Color::Black);
        window.draw(lineV);
    }

    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            if (board[i][j] == 'X' || board[i][j] == 'O') {
                sf::Text mark(board[i][j], font, 120);
                mark.setFillColor(sf::Color::Blue);
                mark.setPosition(j * 200 + 60, i * 200 + 40);
                window.draw(mark);
            }
}

bool handlePlayerClick(int x, int y) {
    sf::Vector2i pos = getBoardPosition(x, y);
    int row = pos.y;
    int col = pos.x;
    if (row < 3 && col < 3 && board[row][col] != 'X' && board[row][col] != 'O') {
        board[row][col] = currentPlayer;
        return true;
    }
    return false;
}

void playTicTacToe() {
    sf::RenderWindow window(sf::VideoMode(600, 700), "Tic-Tac-Toe SFML");
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        std::cerr << "Font loading failed.\n";
        return;
    }

    loadScores();
    initializeBoard();

    bool gameOver = false;
    std::string message;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (!gameOver && event.type == sf::Event::MouseButtonPressed &&
                event.mouseButton.button == sf::Mouse::Left) {
                if (handlePlayerClick(event.mouseButton.x, event.mouseButton.y)) {
                    if (isWinner(currentPlayer)) {
                        message = (currentPlayer == 'X') ? "Player Wins!" : "Computer Wins!";
                        if (currentPlayer == 'X') playerWins++;
                        else computerWins++;
                        saveScores();
                        gameOver = true;
                    }
                    else if (isDraw()) {
                        message = "It's a Draw!";
                        draws++;
                        saveScores();
                        gameOver = true;
                    }
                    else {
                        currentPlayer = (currentPlayer == 'X') ? 'O' : 'X';
                    }
                }
            }

            if (gameOver && event.type == sf::Event::KeyPressed &&
                event.key.code == sf::Keyboard::R) {
                initializeBoard();
                currentPlayer = 'X';
                gameOver = false;
                message.clear();
            }
        }

        window.clear(sf::Color::White);
        drawBoard(window, font);

        if (gameOver) {
            sf::RectangleShape overlay(sf::Vector2f(600, 200));
            overlay.setFillColor(sf::Color(255, 255, 255, 200));
            overlay.setPosition(0, 250);
            window.draw(overlay);

            sf::Text result(message, font, 50);
            result.setFillColor(sf::Color::Red);
            result.setStyle(sf::Text::Bold);
            result.setPosition((600 - result.getLocalBounds().width) / 2, 260);
            window.draw(result);

            sf::Text restart("Press 'R' to Restart", font, 30);
            restart.setFillColor(sf::Color::Black);
            restart.setPosition((600 - restart.getLocalBounds().width) / 2, 330);
            window.draw(restart);
        }

        window.display();
    }
}

void displayTicTacToeScores() {
    sf::RenderWindow window(sf::VideoMode(500, 300), "Tic-Tac-Toe Scores");
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        std::cerr << "Font not found for scores.\n";
        return;
    }

    loadScores();

    sf::Text title("Tic-Tac-Toe Scores", font, 32);
    title.setStyle(sf::Text::Bold);
    title.setFillColor(sf::Color::Blue);
    title.setPosition(100, 20);

    sf::Text scoreText("", font, 24);
    scoreText.setFillColor(sf::Color::Black);
    scoreText.setPosition(60, 100);

    std::string scoreData = "Player Wins: " + std::to_string(playerWins) + "\n";
    scoreData += "Computer Wins: " + std::to_string(computerWins) + "\n";
    scoreData += "Draws: " + std::to_string(draws) + "\n";
    scoreText.setString(scoreData);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear(sf::Color::White);
        window.draw(title);
        window.draw(scoreText);
        window.display();
    }
}
