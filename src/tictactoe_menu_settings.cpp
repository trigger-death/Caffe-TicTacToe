#include "tictactoe_menus.h"

#define OPTIONS_X (WIN_X + 3)
#define OPTIONS_Y (WIN_Y + 2)
#define MESSAGE_X (WIN_X + 2)
#define MESSAGE_Y (WIN_Y + WIN_H - 3)
#define MAX_LENGTH (34)

bool parseBool(std::string str, bool& value) {
	if (str == "Yes" || str == "yes" || str == "Y" || str == "y" ||
		str == "True" || str == "true" || str == "1") {
		value = true;
		return true;
	}
	else if (str == "No" || str == "no" || str == "N" || str == "n" ||
		str == "False" || str == "false" || str == "0") {
		value = false;
		return true;
	}
	return false;
}

std::string trimZeros(std::string s) {
	while (s[s.length()-1] == '0') {
		if (s == "0" || s == "0.0")
			return s;
		s = s.substr(0, s.length() - 1);
	}
	return s;
}

SettingsMenu::SettingsMenu(TicTacToeGame* game) {
	this->game = game;
	this->aiSide = SWITCH_SIDE(game->playerSide);
}

#define LEAVE_MENU() {\
		game->currentMenu = Menus::MAIN_MENU; \
		skip = true; \
		open = false; \
		break; \
	}

void SettingsMenu::run() {
	bool open = true;
	selection = 0;
	message = "";
	do {
		editLine = "";
		int index = 0;
		switch (selection) {
		case 0:
			editLine = (game->getUseSnapshot(aiSide) ? "yes" : "no"); break;
		case 1:
			editLine = game->getSnapshotPath(aiSide); break;
		case 2:
			editLine = std::to_string(game->getIterations(aiSide)); break;
		case 3:
			editLine = trimZeros(std::to_string(game->getRewardThreshold(aiSide))); break;
		case 4:
			editLine = (game->getIsThresholdFixed(aiSide) ? "yes" : "no"); break;
		case 5:
			editLine = trimZeros(std::to_string(game->getRewardDecay(aiSide))); break;
		case 6:
			editLine = GET_SIDE_CHAR(game->playerSide); break;
		case 7:
			editLine += (game->autoContinue ? "yes" : "no"); break;
		}
		index = editLine.length();
		draw();
		move(OPTIONS_Y + selection, OPTIONS_X + 21 + editLine.length());
		int ch = flush_getch();
		bool skip = false;
		while (ch != '\n') {
			if ((ch == '\b' || ch == KEY_BACKSPACE)) {
				if (selection == 8 || selection == -1) {
					LEAVE_MENU()
				}
				else if (index > 0) {
					editLine = editLine.erase(index - 1, 1);
					index--;
				}
			}
			else if (ch == 27 || (ch == ' ' && selection == 8)) { // ESCAPE
				LEAVE_MENU()
			}
			else if (ch >= 0x20 && ch < 0x80 && selection != 8 && selection != -1 && editLine.length() < MAX_LENGTH) {
				editLine.insert(index, 1, ch);
				index++;
			}
			else if (ch == KEY_LEFT) {
				if (selection == -1) {
					aiSide = SWITCH_SIDE(aiSide);
					skip = true;
					break;
				}
				else if (index > 0) {
					index--;
				}
			}
			else if (ch == KEY_RIGHT) {
				if (selection == -1) {
					aiSide = SWITCH_SIDE(aiSide);
					skip = true;
					break;
				}
				else if (index < editLine.length()) {
					index++;
				}
			}
			else if (ch == KEY_UP && selection > -1) {
				if (selection == 3 && !game->getUseSnapshot(aiSide))
					selection = 0;
				else if (selection == 6 && game->getUseSnapshot(aiSide))
					selection = 4;
				else
					selection--;
				skip = true;
				break;
			}
			else if (ch == KEY_DOWN && selection < 8) {
				if (selection == 0 && !game->getUseSnapshot(aiSide))
					selection = 3;
				if (selection == 4 && game->getUseSnapshot(aiSide))
					selection = 6;
				else
					selection++;
				skip = true;
				break;
			}
			draw();
			move(OPTIONS_Y + selection, OPTIONS_X + 21 + index);
			ch = getch();
		}
		message = "";
		if (skip)
			continue;
		switch (selection) {
		case 0:
			if (parseBool(editLine, game->getUseSnapshot(aiSide))) {
				if (!game->getUseSnapshot(aiSide)) {
					game->loadRewards(aiSide);
					selection = 3;
				}
				else {
					if (!game->loadRewards(aiSide))
						message = "Snapshot not found";
					selection++;
				}
			}
			else {
				message = "Invalid boolean";
			}
			break;
		case 1:
			game->getSnapshotPath(aiSide) = editLine;
			if (!game->loadRewards(aiSide))
				message = "Snapshot not found";
			selection++;
			break;
		case 2:
			game->getIterations(aiSide) = atoi(editLine.c_str());
			if (game->loadRewards(aiSide))
				selection++;
			else
				message = "Snapshot not found";
			break;
		case 3:
			game->getRewardThreshold(aiSide) = atof(editLine.c_str());
			selection++;
			break;
		case 4:
			if (parseBool(editLine, game->getIsThresholdFixed(aiSide))) {
				selection++;
			}
			else {
				message = "Invalid boolean";
			}
			break;
		case 5:
			game->getRewardDecay(aiSide) = atof(editLine.c_str());
			game->loadRewards(aiSide);
			selection++;
			break;
		case 6:
			if (editLine == "O" || editLine == "o") {
				game->playerSide = SIDE_O;
				selection++;
			}
			else if (editLine == "X" || editLine == "x") {
				game->playerSide = SIDE_X;
				selection++;
			}
			else {
				message = "Invalid side";
			}
			break;
		case 7:
			if (parseBool(editLine, game->autoContinue)) {
				selection++;
			}
			else {
				message = "Invalid boolean";
			}
			break;
		case 8:
			LEAVE_MENU()
		}
		editLine = "";
	} while (open);
}

void SettingsMenu::draw() {
	clear();
		
	//mvprintw(OPTIONS_Y - 1, OPTIONS_X + 1, (std::string("<==   CPU ") + GET_SIDE_STR(aiSide) + "   ==>").c_str());
	
	std::string opts[] = {std::string(
		"              CPU ") + GET_SIDE_CHAR(aiSide) + ": <- arrows ->",
		"       Use Snapshot: ",
		"     Snapshots Path: ",
		"Snapshot Iterations: ",
		"   Choice Threshold: ",
		"    Fixed Threshold: ",
		"       Reward Decay: ",
		"        Player Side: ",
		"      Auto Continue: ",
		"       Back to Menu: (press enter)"
	};
	for (int i = -1; i < 9; i++) {
		if (i == selection && selection != 8 && selection != -1) {
			opts[i+1] += editLine;
		}
		else {
			switch (i) {
			case 0:
				opts[i+1] += (game->getUseSnapshot(aiSide) ? "yes" : "no"); break;
			case 1:
				opts[i+1] += game->getSnapshotPath(aiSide); break;
			case 2:
				opts[i+1] += std::to_string(game->getIterations(aiSide)); break;
			case 3:
				opts[i+1] += trimZeros(std::to_string(game->getRewardThreshold(aiSide))); break;
			case 4:
				opts[i+1] += (game->getIsThresholdFixed(aiSide) ? "yes" : "no"); break;
			case 5:
				opts[i+1] += trimZeros(std::to_string(game->getRewardDecay(aiSide))); break;
			case 6:
				opts[i+1] += GET_SIDE_CHAR(game->playerSide); break;
			case 7:
				opts[i+1] += (game->autoContinue ? "yes" : "no"); break;
			}
		}
		mvprintw(OPTIONS_Y + i, OPTIONS_X, opts[i+1].c_str());
	}
	
	mvprintw(MESSAGE_Y, MESSAGE_X, message.c_str());
	mvchgat(MESSAGE_Y, MESSAGE_X, -1, A_NORMAL, SIDE_COLOR_X, nullptr);
	mvprintw(MESSAGE_Y + 1, MESSAGE_X, "Move between options and CPUS with arrow keys.");
	
	game->drawWindow();
	
	refresh();
}
