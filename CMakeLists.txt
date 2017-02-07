cmake_minimum_required(VERSION 3.6)
project(game_of_life)

set(CMAKE_C_STANDARD 11)

set(CURSES_USE_NCURSES TRUE)
find_package(Curses REQUIRED)

include_directories(${CURSES_INCLUDE_DIR})

set(SOURCE_FILES main.c)
add_executable(game_of_life ${SOURCE_FILES})
target_link_libraries(game_of_life ${CURSES_LIBRARIES})
