#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <vector>
#include <ctime>
#include <fstream>
#include <climits>
#include <string>

namespace Minesweeper {

    constexpr int GRID_SIZE = 9;
    constexpr int TILE_SIZE = 32;
    constexpr int NUM_MINES = 10;
    constexpr int BOTTOM_UI_HEIGHT = 80;

    struct Tile {
        bool revealed = false;
        bool flagged = false;
        bool mine = false;
        int adjacentMines = 0;
    };

    extern std::vector<std::vector<Tile>> grid;
    extern bool gameOver;
    extern bool won;
    extern time_t startTime;
    extern int bestTime;
    extern bool timerRunning;
    extern sf::Font font;

    void loadHighScore();
    void saveHighScore(int time);
    void placeMines();
    void calculateAdjacency();
    void reveal(int r, int c);
    bool checkWin();
    void playMinesweeper();

}

