cmake_minimum_required(VERSION 2.8)
project(tictactoe)

set(CMAKE_CXX_STANDARD 14)

find_package(OpenCV REQUIRED)
find_package(Caffe REQUIRED)
file(GLOB SOURCES
	src/tictactoe.h
	src/tictactoe_general.cpp
	src/tictactoe_generation.cpp
)
file(GLOB TRAIN_SOURCES
	src/tictactoe.h
	src/tictactoe_experiment.h
	src/tictactoe_experiment.cpp
	src/tictactoe_train.cpp
)
file(GLOB PLAY_SOURCES
	src/tictactoe.h
	src/tictactoe_game.h
	src/tictactoe_game.cpp
	src/tictactoe_menus.h
	src/tictactoe_menu_main.cpp
	src/tictactoe_menu_settings.cpp
	src/tictactoe_play.cpp
)
find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIRS} ${Caffe_INCLUDE_DIRS})
add_library(tictactoe STATIC ${SOURCES})
add_executable(tictactoe_train ${TRAIN_SOURCES})
add_executable(tictactoe_play ${PLAY_SOURCES})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/..)
set_target_properties(tictactoe PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/..")
set_target_properties(tictactoe_train PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/..")
set_target_properties(tictactoe_play PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/..")


target_link_libraries(tictactoe_train ${OpenCV_LIBS} ${Caffe_LIBRARIES} ${CURSES_LIBRARIES} tictactoe)
target_link_libraries(tictactoe_play ${OpenCV_LIBS} ${Caffe_LIBRARIES} ${CURSES_LIBRARIES} tictactoe)
