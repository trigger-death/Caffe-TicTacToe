#ifndef TICTACTOE_MENUS_H
#define TICTACTOE_MENUS_H

#include "tictactoe_game.h"

class MainMenu {
private:
	// Members
	TicTacToeGame* game;
	int selection;
	
public:
	// Constructors
	MainMenu(TicTacToeGame* game);
	
	// Running
	void run();
	
private:
	// Drawing
	void draw();
};

class SettingsMenu {
private:
	// Members
	TicTacToeGame* game;
	int selection;
	int aiSide;
	std::string editLine;
	std::string message;
	
public:
	// Constructors
	SettingsMenu(TicTacToeGame* game);
	
	// Running
	void run();
	
private:
	// Drawing
	void draw();
};

#endif
