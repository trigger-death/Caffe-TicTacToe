#include "tictactoe_menus.h"

#define OPTIONS_X (WIN_X + 3)
#define OPTIONS_Y (WIN_Y + 2)

MainMenu::MainMenu(TicTacToeGame* game) {
	this->game = game;
	this->selection = 0;
}

void MainMenu::run() {
	bool open = true;
	do {
		draw();
		move(OPTIONS_Y + selection, OPTIONS_X);
		int key = flush_getch();
		int choice = -1;
		if (key >= '1' && key <= '9')
			choice = key - '1';
		else if (key == '\n' || key == ' ')
			choice = selection;
		else if (key == KEY_UP && selection > 0)
			selection--;
		else if (key == KEY_DOWN && selection < 4)
			selection++;
		else if (key == 27 || key == '\b' || key == KEY_BACKSPACE) // ESCAPE
			choice = 4;
		
		if (choice >= 0 && choice <= 2) {
			game->gameMode = (GameModes)choice;
			game->currentMenu = Menus::PLAY_GAME;
			open = false;
		}
		else if (choice == 3) {
			game->currentMenu = Menus::SETTINGS_MENU;
			open = false;
		}
		else if (choice == 4) {
			game->running = false;
			open = false;
		}
	} while (open);
}

void MainMenu::draw() {
	clear();
	
	int y = OPTIONS_Y;
	mvprintw(y, OPTIONS_X, "1) Player vs. Player"); y++;
	mvprintw(y, OPTIONS_X, "2) Player vs. CPU"); y++;
	mvprintw(y, OPTIONS_X, "3) CPU vs. CPU"); y++;
	mvprintw(y, OPTIONS_X, "4) Change Settings"); y++;
	mvprintw(y, OPTIONS_X, "5) Exit"); y++;
	
	mvchgat(OPTIONS_Y + selection, OPTIONS_X, -1, A_BOLD, 0, nullptr);
	
	game->drawWindow();
	
	refresh();
}
