#include <QApplication>
#include <QKeyEvent>
#include <QWidget>
#include <QPainter>
#include <vector>
#include <queue>
#include <limits>
#include <QMessageBox>
#include <cstdlib>
using namespace std;

struct Cell {
    int x, y;
    bool obstacle;
    bool visited;
    int distance;
    Cell* previous;

    Cell(int x, int y) : x(x), y(y), obstacle(false), visited(false), distance(numeric_limits<int>::max()), previous(nullptr) {}
};

class MazeGame : public QWidget {
public:
    MazeGame(QWidget* parent = nullptr) : QWidget(parent), rows(10), cols(10), cellSize(30), playerX(0), playerY(0), goalX(9), goalY(9) {
        generateMaze();
        calculateShortestPath();
    }

protected:
    void keyPressEvent(QKeyEvent* event) override {
        switch (event->key()) {
        case Qt::Key_Up:
            movePlayer(playerX, playerY - 1);
            break;
        case Qt::Key_Down:
            movePlayer(playerX, playerY + 1);
            break;
        case Qt::Key_Left:
            movePlayer(playerX - 1, playerY);
            break;
        case Qt::Key_Right:
            movePlayer(playerX + 1, playerY);
            break;
        case Qt::Key_Return:
            if (playerX == goalX && playerY == goalY) {
                QMessageBox::information(this, "You Win", "Congratulations, you reached the goal using the shortest path!");
            } else {
                QMessageBox::information(this, "Not Reached", "You have not reached the goal yet.");
            }
            break;
        default:
            QWidget::keyPressEvent(event);
        }
    }

    void paintEvent(QPaintEvent* event) override {
        QPainter painter(this);
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                if (maze[i][j].obstacle) {
                    painter.fillRect(j * cellSize, i * cellSize, cellSize, cellSize, Qt::black);
                } else if (maze[i][j].visited) {
                    painter.fillRect(j * cellSize, i * cellSize, cellSize, cellSize, Qt::gray);
                }
            }
        }

        // Draw player
        painter.setBrush(Qt::blue);
        painter.drawPolygon(playerShape);

        // Draw goal
        painter.setBrush(Qt::green);
        painter.drawRect(goalX * cellSize, goalY * cellSize, cellSize, cellSize);

        // Draw path
        painter.setPen(Qt::red);
        Cell* current = &maze[playerY][playerX];
        while (current->previous != nullptr) {
            painter.drawLine(current->x * cellSize + cellSize / 2, current->y * cellSize + cellSize / 2,
                             current->previous->x * cellSize + cellSize / 2, current->previous->y * cellSize + cellSize / 2);
            current = current->previous;
        }
    }

private:
    int rows;
    int cols;
    int cellSize;
    int playerX;
    int playerY;
    int goalX;
    int goalY;
    vector<vector<Cell>> maze;
    QPolygonF playerShape{{0, 0}, {-10, 20}, {10, 20}};

    void generateMaze() {
        maze.resize(rows, vector<Cell>(cols, Cell(0, 0)));

        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                maze[i][j] = Cell(j, i);
                if (rand() % 3 == 0) { // Increase density of obstacles
                    maze[i][j].obstacle = true;
                }
            }
        }
    }

    void calculateShortestPath() {
        priority_queue<Cell*, vector<Cell*>, Comparator> pq;
        Cell* start = &maze[0][0];
        start->distance = 0;
        pq.push(start);

        while (!pq.empty()) {
            Cell* current = pq.top();
            pq.pop();

            if (current->visited) {
                continue;
            }

            current->visited = true;

            // Explore neighbors
            for (int dx = -1; dx <= 1; ++dx) {
                for (int dy = -1; dy <= 1; ++dy) {
                    if (dx == 0 && dy == 0) {
                        continue;
                    }

                    int newX = current->x + dx;
                    int newY = current->y + dy;

                    if (newX >= 0 && newX < cols && newY >= 0 && newY < rows && !maze[newY][newX].obstacle) {
                        int newDistance = current->distance + 1;
                        if (newDistance < maze[newY][newX].distance) {
                            maze[newY][newX].distance = newDistance;
                            maze[newY][newX].previous = current;
                            pq.push(&maze[newY][newX]);
                        }
                    }
                }
            }
        }
    }

    void movePlayer(int newX, int newY) {
        if (newX >= 0 && newX < cols && newY >= 0 && newY < rows && !maze[newY][newX].obstacle) {
            playerX = newX;
            playerY = newY;
            update();
        }
    }

    struct Comparator {
        bool operator()(const Cell* a, const Cell* b) {
            return a->distance > b->distance;
        }
    };
};

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    MazeGame mazeGame;
    mazeGame.setWindowTitle("2D Maze Game");
    mazeGame.resize(300, 300);
    mazeGame.show();

    return app.exec();
}
