cmake_minimum_required(VERSION 3.10)

# Назва проєкту
project(SimulationApp)

# Встановлюємо стандарт C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Додаємо виконуваний файл
add_executable(sim main.cpp)
