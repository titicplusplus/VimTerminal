cmake_minimum_required(VERSION 3.10)

project(VimTerminal VERSION 0.10)

add_executable(
	VimTerminal
	main.cpp
	Interact.cpp
	TextCommand.cpp
	CursorRect.cpp
)

target_link_libraries(
	VimTerminal
	sfml-system
	sfml-window
	sfml-graphics
	pthread
)
