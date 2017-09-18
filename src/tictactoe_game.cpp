#include "tictactoe_menus.h"
#include <caffe/caffe.hpp>
#include <caffe/layers/memory_data_layer.hpp>

#define TITLE_X (WIN_X + 4)
#define BOARD_X (WIN_X + 4)
#define BOARD_Y (WIN_Y + 2)
#define STATUS_X (WIN_X + 2)
#define STATUS_Y (WIN_Y + WIN_H - 3)
#define TIE_GAME (OPEN_SPACE)
#define QUIT_GAME (-2)
#define WINS_X (BOARD_X + 9)
#define WINS_Y (BOARD_Y)

// UI
int flush_getch() {
	timeout(0);
	int ch;
	do {
		ch = getch();
	} while (ch != ERR);
	timeout(-1);
	return getch();
}
int nodelay_getch() {
	timeout(0);
	int ch = getch();
	timeout(-1);
	return ch;
}
int timeout_getch(int milliseconds) {
	timeout(milliseconds);
	int ch = getch();
	timeout(-1);
	return ch;
}

// Constructors
TicTacToeGame::TicTacToeGame() {
	this->running = false;
	this->autoContinue = false;
	
	// Menus
	this->currentMenu = Menus::MAIN_MENU;
	
	// AI X
	this->useSnapshotX = false;
	this->snapshotPathX = "./snapshots";
	this->iterationsX = 0;
	this->rewardThresholdX = 0.05;
	this->isThresholdFixedX = false;
	this->rewardDecayX = DEFAULT_REWARD_DECAY;
	
	// AI Y
	this->useSnapshotO = false;
	this->snapshotPathO = "./snapshots";
	this->iterationsO = 0;
	this->rewardThresholdO = 0.05;
	this->isThresholdFixedO = false;
	this->rewardDecayO = DEFAULT_REWARD_DECAY;
	
	// Gameplay
	this->gameMode = GameModes::PLAYER_VS_PLAYER;
	this->playerSide = SIDE_X;
	this->currentPosition = 0;
	this->currentSide = ttt.whoGoesFirst;
	this->status = "";
	this->status2 = "";
	this->statusColor = SIDE_COLOR_NONE;
	
	// Read settings
	std::ifstream fin("./settings.txt", std::ifstream::in);
	if (fin.is_open()) {
		std::string name;
		fin >> std::boolalpha;
		while (!fin.eof()) {
			fin >> name;
			if (fin.eof())
				break;
			// AI X
			else if (name == "snapshotPathX") {
				getline(fin, snapshotPathX);
				// Trim whitespace
				while (!snapshotPathX.empty()) {
					if (snapshotPathX[0] == ' ' || snapshotPathX[0] == '\t')
						snapshotPathX = snapshotPathX.substr(1, snapshotPathX.length() - 1);
					else
						break;
				}
				while (!snapshotPathX.empty()) {
					if (snapshotPathX[snapshotPathX.length()-1] == ' ' || snapshotPathX[snapshotPathX.length()-1] == '\t')
						snapshotPathX = snapshotPathX.substr(0, snapshotPathX.length() - 1);
					else
						break;
				}
				// Remove quotes
				if (snapshotPathX.length() >= 2 && snapshotPathX[0] == '"' &&
					snapshotPathX[snapshotPathX.length()-1] == '"') {
					snapshotPathX = snapshotPathX.substr(1, snapshotPathX.length() - 2);
				}
			}
			else if (name == "useSnapshotX")
				fin >> useSnapshotX;
			else if (name == "iterationsX")
				fin >> iterationsX;
			else if (name == "rewardThresholdX")
				fin >> rewardThresholdX;
			else if (name == "isThresholdFixedX")
				fin >> isThresholdFixedX;
			else if (name == "rewardDecayX")
				fin >> rewardDecayX;
				
			// AI O
			else if (name == "snapshotPathO") {
				getline(fin, snapshotPathO);
				// Trim whitespace
				while (!snapshotPathO.empty()) {
					if (snapshotPathO[0] == ' ' || snapshotPathO[0] == '\t')
						snapshotPathO = snapshotPathO.substr(1, snapshotPathO.length() - 1);
					else
						break;
				}
				while (!snapshotPathO.empty()) {
					if (snapshotPathO[snapshotPathO.length()-1] == ' ' || snapshotPathO[snapshotPathO.length()-1] == '\t')
						snapshotPathO = snapshotPathO.substr(0, snapshotPathO.length() - 1);
					else
						break;
				}
				// Remove quotes
				if (snapshotPathO.length() >= 2 && snapshotPathO[0] == '"' &&
					snapshotPathO[snapshotPathO.length()-1] == '"') {
					snapshotPathO = snapshotPathO.substr(1, snapshotPathO.length() - 2);
				}
			}
			else if (name == "useSnapshotO")
				fin >> useSnapshotO;
			else if (name == "iterationsO")
				fin >> iterationsO;
			else if (name == "rewardThresholdO")
				fin >> rewardThresholdO;
			else if (name == "isThresholdFixedO")
				fin >> isThresholdFixedO;
			else if (name == "rewardDecayO")
				fin >> rewardDecayO;
			
			// Other
			else if (name == "playerSide") {
				fin >> name;
				if (name == "O")
					playerSide = SIDE_O;
				else if (name == "X")
					playerSide = SIDE_X;
			}
			else if (name == "autoContinue")
				fin >> autoContinue;
		}
		fin.close();
	}
	
	if (!loadRewards(SIDE_X)) {
		this->useSnapshotX = false;
		loadRewards(SIDE_X);
	}
	if (!loadRewards(SIDE_O)) {
		this->useSnapshotO = false;
		loadRewards(SIDE_O);
	}
	
	// NCurses init
	initscr();
	noecho();
	curs_set(false);
	keypad(stdscr, true);
	start_color();
	cbreak();
	ESCDELAY = 1;
	init_pair(SIDE_COLOR_X, COLOR_RED, COLOR_BLACK);
	init_pair(SIDE_COLOR_O, COLOR_GREEN, COLOR_BLACK);
}
TicTacToeGame::~TicTacToeGame() {
	// NCurses uninit
	endwin();
}

// Helpers
std::vector<float>& TicTacToeGame::getAIRewards(int side) {
	if (side == SIDE_X)
		return aiRewardsX;
	return aiRewardsO;
}
bool& TicTacToeGame::getUseSnapshot(int side) {
	if (side == SIDE_X)
		return useSnapshotX;
	return useSnapshotO;
}
std::string& TicTacToeGame::getSnapshotPath(int side) {
	if (side == SIDE_X)
		return snapshotPathX;
	return snapshotPathO;
}
int& TicTacToeGame::getIterations(int side) {
	if (side == SIDE_X)
		return iterationsX;
	return iterationsO;
}
float& TicTacToeGame::getRewardThreshold(int side) {
	if (side == SIDE_X)
		return rewardThresholdX;
	return rewardThresholdO;
}
bool& TicTacToeGame::getIsThresholdFixed(int side) {
	if (side == SIDE_X)
		return isThresholdFixedX;
	return isThresholdFixedO;
}
float& TicTacToeGame::getRewardDecay(int side) {
	if (side == SIDE_X)
		return rewardDecayX;
	return rewardDecayO;
}

// Running
void TicTacToeGame::run() {
	mainMenu = std::make_shared<MainMenu>(this);
	settingsMenu = std::make_shared<SettingsMenu>(this);
	running = true;
	int key;
	
	curs_set(true);
	do {
		if (currentMenu == Menus::MAIN_MENU) {
			mainMenu->run();
		}
		else if (currentMenu == Menus::SETTINGS_MENU) {
			settingsMenu->run();
		}
		else if (currentMenu == Menus::PLAY_GAME) {
			winsX = 0;
			winsO = 0;
			ties = 0;
			curs_set(false);
			// Seed who goes first in the first game (opposite of result)
			ttt.whoGoesFirst = (rand() % 2 == 0 ? SIDE_X : SIDE_O);
			do {
				bool quitGame = playGame();
				if (quitGame)
					break;
				if (autoContinue && gameMode == GameModes::AI_VS_AI) {
					key = timeout_getch(300);
					//SLEEP(300);
					//key = nodelay_getch();
				}
				else
					key = flush_getch();
			} while (key != 27 && key != '\b' && key != KEY_BACKSPACE); // Escape pressed
			currentMenu = Menus::MAIN_MENU;
			curs_set(true);
		}
			
	} while (running);
}

// Gameplay
bool TicTacToeGame::playGame() {
	startGame();
	bool playing = true;
	int key;
	
	while (playing) {
		// Update
		PlayerTypes currentPlayer = getPlayerType(currentSide);
		if (currentPlayer == PlayerTypes::PLAYER) {
			int position = playerTakeTurn();
			if (position == QUIT_GAME)
				return true;
		}
		else {
			int position = aiTakeTurn();
			if (position == QUIT_GAME)
				return true;
		}
	
		drawBoard();
		
		key = nodelay_getch();
		if (key == 27 || key == '\b' || key == KEY_BACKSPACE)
			return true;
		
		// Check for ending
		if (ttt.isWin(board, currentSide)) {
			if (currentSide == SIDE_X)
				winsX++;
			else
				winsO++;
			SLEEP(300);
			endGame(currentSide);
			playing = false;
			drawBoard();
		}
		else if (ttt.isTie(board)) {
			ties++;
			SLEEP(300);
			endGame(TIE_GAME);
			playing = false;
			drawBoard();
		}
		else {
			nextTurn();
			drawBoard();
			SLEEP(300);
		}
		
		key = nodelay_getch();
		if (key == 27 || key == '\b' || key == KEY_BACKSPACE)
			return true;
	}
	return false;
}
void TicTacToeGame::startGame() {
	// Reset the board
	for (int i = 0; i < 9; i++) {
		board[i] = OPEN_SPACE;
	}
	
	// Choose who starts
	currentSide = SWITCH_SIDE(ttt.whoGoesFirst);
	ttt.whoGoesFirst = currentSide;
	
	PlayerTypes currentPlayer = getPlayerType(currentSide);
	if (currentPlayer == PlayerTypes::PLAYER)
		status = "Player ";
	else
		status = "CPU ";
	status += GET_SIDE_STR(currentSide) + " goes first!";
	status2 = "";
	statusColor = GET_SIDE_COLOR(currentSide);
	
	drawBoard();
}
void TicTacToeGame::endGame(int winner) {
	if (winner == TIE_GAME) {
		status = "Tie game!";
	}
	else {
		PlayerTypes winnerType = getPlayerType(winner);
		if (winnerType == PlayerTypes::PLAYER)
			status = "Player ";
		else
			status = "CPU ";
		status += GET_SIDE_STR(winner);
		status += " wins!";
	}
	status += " Press any key to start a new game.";
	status2 = "Press escape or backspace to go back to the menu.";
	statusColor = GET_SIDE_COLOR(winner);
}

// Moves
void TicTacToeGame::nextTurn() {
	currentSide = SWITCH_SIDE(currentSide);
	PlayerTypes currentPlayer = getPlayerType(currentSide);
	if (currentPlayer == PlayerTypes::PLAYER)
		status = "Player ";
	else
		status = "CPU ";
	status += GET_SIDE_STR(currentSide) + "'s turn!";
	status2 = "";
	statusColor = GET_SIDE_COLOR(currentSide);
}
int TicTacToeGame::playerTakeTurn() {
	curs_set(true);
	int key = 0;
	do {
		int x = currentPosition % 3;
		int y = currentPosition / 3;
		move(BOARD_Y + y*2, BOARD_X + x*2);
		
		key = flush_getch();
		if (key == KEY_LEFT && x > 0)
			x--;
		else if (key == KEY_RIGHT && x < 2)
			x++;
		else if (key == KEY_UP && y > 0)
			y--;
		else if (key == KEY_DOWN && y < 2)
			y++;
		else if (key == 27 || key == '\b' || key == KEY_BACKSPACE) // ESCAPE
			return QUIT_GAME;
	
		currentPosition = y * 3 + x;
	} while ((key != 10 && key != ' ') || board[currentPosition] != OPEN_SPACE);
	curs_set(false);
	board[currentPosition] = currentSide;
	return currentPosition;
}
int TicTacToeGame::aiTakeTurn() {
	//SLEEP(600);
	int key = timeout_getch(600);
	if (key == QUIT_GAME)
		return QUIT_GAME;
		
	int position = ttt.getBestPosition(
		board, currentSide,
		getRewardThreshold(currentSide),
		getIsThresholdFixed(currentSide),
		getAIRewards(currentSide)
	);
	board[position] = currentSide;
	return position;
}

// Helpers
PlayerTypes TicTacToeGame::getPlayerType(int side) {
	if (gameMode == GameModes::PLAYER_VS_PLAYER)
		return PlayerTypes::PLAYER;
	else if (gameMode == GameModes::AI_VS_AI)
		return PlayerTypes::AI;
	else if (gameMode == GameModes::PLAYER_VS_AI)
		return (side == playerSide ? PlayerTypes::PLAYER : PlayerTypes::AI);
}
void TicTacToeGame::getWinSpaces(int board[9], bool winSpaces[9]) {
	int side = SIDE_O;
	do {
		side = -side;
		int xSideCounts[3] = {0};
		int ySideCounts[3] = {0};
		int sideCount = 0;
		int sideCount2 = 0;
		for (int j = 0; j < 3; j++) {
			if (board[j * 3 + j] == side)
				sideCount++;
			if (board[j * 3 + 2 - j] == side)
				sideCount2++;
		}
		for (int j = 0; j < 3; j++) {
			if (sideCount == 3)
				winSpaces[j * 3 + j] = true;
			if (sideCount2 == 3)
				winSpaces[j * 3 + 2 - j] = true;
		}
		for (int i = 0; i < 3; i++) {
			sideCount = 0;
			sideCount2 = 0;
			for (int j = 0; j < 3; j++) {
				if (board[i * 3 + j] == side)
					sideCount++;
				if (board[j * 3 + i] == side)
					sideCount2++;
			}
			for (int j = 0; j < 3; j++) {
				if (sideCount == 3)
					winSpaces[i * 3 + j] = true;
				if (sideCount2 == 3)
					winSpaces[j * 3 + i] = true;
			}
		}
		
	} while (side != SIDE_O);
}

// Drawing
void TicTacToeGame::drawBoard() {
	clear();
	bool winSpaces[9] = {false};
	getWinSpaces(board, winSpaces);
	for (int y = 0; y < 3; y++) {
		for (int x = 0; x < 3; x++) {
			int side = board[y*3 + x];
			int color = GET_SIDE_COLOR(side);
			mvprintw(BOARD_Y + y*2, BOARD_X + x*2, GET_SIDE_CHAR(side));
			if (winSpaces[y * 3 + x])
				mvchgat(BOARD_Y + y*2, BOARD_X + x*2, 1, A_BOLD, color, nullptr);
			else
				mvchgat(BOARD_Y + y*2, BOARD_X + x*2, 1, A_NORMAL, color, nullptr);
			if (x < 2)
				mvprintw(BOARD_Y + y*2, BOARD_X + x*2+1, "|");
		}
		if (y < 2)
			mvprintw(BOARD_Y + y*2+1, BOARD_X, "-+-+-");
	}
	mvprintw(STATUS_Y, STATUS_X, status.c_str());
	mvchgat(STATUS_Y, STATUS_X, -1, A_NORMAL, statusColor, nullptr);
	mvprintw(STATUS_Y + 1, STATUS_X, status2.c_str());
	
	
	mvprintw(WINS_Y, WINS_X, "---Wins-------");
	
	std::string line = "Player X: ";
	if (getPlayerType(SIDE_X) == PlayerTypes::AI)
		line = "   CPU X: ";
	line += std::to_string(winsX);
	mvprintw(WINS_Y+1, WINS_X+1, line.c_str());
	
	line = "Player O: ";
	if (getPlayerType(SIDE_O) == PlayerTypes::AI)
		line = "   CPU O: ";
	line += std::to_string(winsO);
	mvprintw(WINS_Y+2, WINS_X+1, line.c_str());
	
	mvprintw(WINS_Y+3, WINS_X+1, (std::string("Tie Game: ") + std::to_string(ties)).c_str());
	mvprintw(WINS_Y+4, WINS_X+1, (std::string("   Total: ") + std::to_string(winsX + winsO + ties)).c_str());
	
	drawWindow();
	
	refresh();
}
void TicTacToeGame::drawWindow() {
	int x = WIN_X;
	int y = WIN_Y;
	int w = WIN_W;
	int h = WIN_H;
	mvhline(y, x, '=', w);
	mvhline(y+h-1, x+1, '-', w-2);
	mvvline(y+1, x, '|', h-2);
	mvvline(y+1, x+w-1, '|', h-2);
	mvaddch(y+h-1, x, '+');
	mvaddch(y+h-1, x+w-1, '+');
	mvprintw(y, TITLE_X, "Tic-Tac-Toe");
}

// AI
bool TicTacToeGame::loadRewards(int side) {
	if (!getUseSnapshot(side)) {
		ttt.loadRewardDecay(getRewardDecay(side), getAIRewards(side));
		return true;
	}
	std::string path = getSnapshotPath(side) + "/TTT_iter_" + std::to_string(getIterations(side)) + ".caffemodel";
	if (!std::ifstream(path))
		return false;
		
	std::shared_ptr<caffe::Net<float>> testnet;
	testnet.reset(new caffe::Net<float>("./model.prototxt", caffe::TEST));
	testnet->CopyTrainedLayersFrom(path.c_str());
	
	caffe::MemoryDataLayer<float> *dataLayer_testnet =
			(caffe::MemoryDataLayer<float> *) (testnet->layer_by_name("test_inputdata").get());
			
	std::vector<float> label;
	for (int i = 0; i < ttt.label.size(); i++) {
		label.push_back((ttt.label[i] + 1.0f) / 2.0f);
	}
	
	dataLayer_testnet->Reset(&ttt.data[0], &label[0], ttt.label.size());
	
	testnet->Forward();
	
	boost::shared_ptr<caffe::Blob<float>> output_layer = testnet->blob_by_name("output");
	const float* begin = output_layer->cpu_data();
	const float* end = begin + ttt.label.size();
	
	getAIRewards(side).clear();
	getAIRewards(side).insert(getAIRewards(side).begin(), begin, end);
	
	for (int i = 0; i < ttt.label.size(); i++) {
		getAIRewards(side)[i] = ((getAIRewards(side)[i] * 2.0f) - 1.0f);
	}
	return true;
}
