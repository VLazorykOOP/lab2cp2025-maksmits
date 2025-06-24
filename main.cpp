#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
const int WIDTH = 800;  // ширина області сим
const int HEIGHT = 600; // висота області сим
const float SPEED = 2.0f; // швидкість руху
struct Point {
    float x, y;
};
enum PersonType {
    LEGAL,     // юр особа
    PHYSICAL   // фіз особа
};
struct Entity {
    Point position;
    Point destination;
    PersonType type;
    bool moving;
    Entity(Point pos, PersonType t) : position(pos), type(t) {
        if ((type == LEGAL && pos.x <= WIDTH / 2 && pos.y <= HEIGHT / 2) ||
            (type == PHYSICAL && pos.x >= WIDTH / 2 && pos.y >= HEIGHT / 2)) {
            moving = false;
            destination = pos;
        } else {
            moving = true;
            generateDestination();
        }
    }
    void generateDestination() {
        if (type == LEGAL) {
            destination.x = static_cast<float>(rand() % (WIDTH / 2));
            destination.y = static_cast<float>(rand() % (HEIGHT / 2));
        } else {
            destination.x = static_cast<float>(WIDTH / 2 + rand() % (WIDTH / 2));
            destination.y = static_cast<float>(HEIGHT / 2 + rand() % (HEIGHT / 2));
        }
    }
    void move() {
        if (!moving) return;

        float dx = destination.x - position.x;
        float dy = destination.y - position.y;
        float dist = std::sqrt(dx * dx + dy * dy);

        if (dist <= SPEED) {
            position = destination;
            moving = false;
            return;
        }
        float ratio = SPEED / dist;
        position.x += dx * ratio;
        position.y += dy * ratio;
    }
    void print() const {
        std::cout << (type == LEGAL ? "Юр. особа" : "Фiз. особа")
                  << " | Поточна позиція: (" << position.x << ", " << position.y << ")"
                  << " -> Кінцева: (" << destination.x << ", " << destination.y << ")"
                  << " | " << (moving ? "Рухається" : "Стоїть") << std::endl;
    }
};

int main() {
    setlocale(LC_CTYPE, "ukr");
    srand(static_cast<unsigned>(time(0)));
    std::vector<Entity> entities;
    entities.emplace_back(Point{700, 100}, LEGAL);  // повинна рухатись вліво вгору
    entities.emplace_back(Point{200, 100}, LEGAL);  // вже в цілі
    entities.emplace_back(Point{100, 500}, PHYSICAL); // повинна рухатись вправо вниз
    entities.emplace_back(Point{600, 550}, PHYSICAL); // вже в цілі
    for (int step = 0; step < 10; ++step) {
        std::cout << "--- Крок симуляції: " << step + 1 << " ---\n";
        for (auto& entity : entities) {
            entity.move();
            entity.print();
        }
        std::cout << "\n";
        bool allStopped = true;
        for (const auto& e : entities)
            if (e.moving)
                allStopped = false;
        if (allStopped) break;
    }
    return 0;
}