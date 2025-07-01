#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>

const int WIDTH = 800;
const int HEIGHT = 600;
const float SPEED = 2.0f;

std::mutex coutMutex;
std::mutex entityMutex;

struct Point {
    float x, y;
};

enum PersonType {
    LEGAL,
    PHYSICAL
};

class Entity {
    Point position;
    Point destination;
    PersonType type;
    std::atomic<bool> moving;
    std::thread worker;

public:
    Entity(Point pos, PersonType t) : position(pos), type(t) {
        if ((type == LEGAL && pos.x <= WIDTH / 2 && pos.y <= HEIGHT / 2) ||
            (type == PHYSICAL && pos.x >= WIDTH / 2 && pos.y >= HEIGHT / 2)) {
            moving = false;
            destination = pos;
        }
        else {
            moving = true;
            generateDestination();
        }
    }

    ~Entity() {
        if (worker.joinable()) {
            worker.join();
        }
    }

    void start() {
        worker = std::thread(&Entity::run, this);
    }

    void generateDestination() {
        if (type == LEGAL) {
            destination.x = static_cast<float>(rand() % (WIDTH / 2));
            destination.y = static_cast<float>(rand() % (HEIGHT / 2));
        }
        else {
            destination.x = static_cast<float>(WIDTH / 2 + rand() % (WIDTH / 2));
            destination.y = static_cast<float>(HEIGHT / 2 + rand() % (HEIGHT / 2));
        }
    }

    void run() {
        while (moving) {
            move();
            print();
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
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

    void print() {
        std::lock_guard<std::mutex> lock(coutMutex);
        std::cout << (type == LEGAL ? "Юр. особа" : "Фiз. особа")
            << " | Поточна позиція: (" << position.x << ", " << position.y << ")"
            << " -> Кінцева: (" << destination.x << ", " << destination.y << ")"
            << " | " << (moving ? "Рухається" : "Стоїть") << std::endl;
    }

    void join() {
        if (worker.joinable()) {
            worker.join();
        }
    }
};

// Паралельне створення нових Entity
void entityGenerator(std::vector<Entity*>& entities, std::atomic<bool>& running) {
    while (running) {
        std::this_thread::sleep_for(std::chrono::seconds(3)); // новий Entity кожні 3 секунди
        PersonType type = rand() % 2 == 0 ? LEGAL : PHYSICAL;
        Point pos;
        if (type == LEGAL) {
            pos = { static_cast<float>(WIDTH / 2 + rand() % (WIDTH / 2)),
                    static_cast<float>(HEIGHT / 2 + rand() % (HEIGHT / 2)) };
        }
        else {
            pos = { static_cast<float>(rand() % (WIDTH / 2)),
                    static_cast<float>(rand() % (HEIGHT / 2)) };
        }

        Entity* newEntity = new Entity(pos, type);
        {
            std::lock_guard<std::mutex> lock(entityMutex);
            entities.push_back(newEntity);
        }
        newEntity->start();

        {
            std::lock_guard<std::mutex> lock(coutMutex);
            std::cout << "[Додано нову " << (type == LEGAL ? "Юр. особу" : "Фіз. особу") << "]\n";
        }
    }
}

int main() {
    setlocale(LC_CTYPE, "ukr");
    srand(static_cast<unsigned>(time(0)));

    std::vector<Entity*> entities;
    std::atomic<bool> generatorRunning = true;

    // Початкові Entity
    entities.push_back(new Entity({ 700, 100 }, LEGAL));
    entities.push_back(new Entity({ 200, 100 }, LEGAL));
    entities.push_back(new Entity({ 100, 500 }, PHYSICAL));
    entities.push_back(new Entity({ 600, 550 }, PHYSICAL));

    // Запуск потоків руху
    for (auto& e : entities) {
        e->start();
    }

    // Запускаємо генератор нових об'єктів
    std::thread generatorThread(entityGenerator, std::ref(entities), std::ref(generatorRunning));

    // Головний цикл — чекаємо кілька секунд, потім завершуємо
    std::this_thread::sleep_for(std::chrono::seconds(15));
    generatorRunning = false;

    if (generatorThread.joinable())
        generatorThread.join();

    // Чекаємо завершення руху всіх Entity
    {
        std::lock_guard<std::mutex> lock(entityMutex);
        for (auto& e : entities) {
            e->join();
            delete e;
        }
    }

    std::cout << "\nСимуляцію завершено.\n";
    return 0;
}