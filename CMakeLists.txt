cmake_minimum_required(VERSION 3.10)

project(VimTerminal VERSION 0.10)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Debug)


add_executable(
	VimTerminal
	main.cpp
	TextBox.cpp
	Interact.cpp
	TextCommand.cpp
	CursorRect.cpp
	Ressources.cpp
)

target_link_libraries(
	VimTerminal
	sfml-system
	sfml-window
	sfml-graphics
	pthread
	stdc++fs
)
