#ifndef SNAKE_HPP
#define SNAKE_HPP

#include <SFML/Graphics.hpp>
#include <vector>

const int blockSize = 20;
const int width = 800;
const int height = 600;

enum Direction { UP, DOWN, LEFT, RIGHT };

struct SnakeSegment {
    int x, y;
    SnakeSegment(int x, int y);
};

class Snake {
public:
    Snake();
    void move();
    void grow();
    bool checkCollision() const;
    void setDirection(Direction d);

    std::vector<SnakeSegment> segments;
    Direction dir;
};

sf::Vector2i generateFoodPosition(const Snake& snake);
void playSnake();
void displaySnakeScores();

#endif // SNAKE_HPP

