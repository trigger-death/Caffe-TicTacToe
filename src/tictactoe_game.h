#ifndef TICTACTOE_GAME_H
#define TICTACTOE_GAME_H

#include "tictactoe.h"
#include <ncurses.h>

#define SLEEP(milliseconds) usleep((milliseconds) * 1000)

#define WIN_X (1)
#define WIN_Y (1)
#define WIN_W (60)
#define WIN_H (14)

#define SIDE_COLOR_X (1)
#define SIDE_COLOR_O (2)
#define SIDE_COLOR_NONE (0)
#define GET_SIDE_COLOR(side) (side == SIDE_X ? SIDE_COLOR_X : (side == SIDE_O ? SIDE_COLOR_O : SIDE_COLOR_NONE))

enum class GameModes {
	PLAYER_VS_PLAYER = 0,
	PLAYER_VS_AI = 1,
	AI_VS_AI = 2
};
enum class Menus {
	MAIN_MENU,
	SETTINGS_MENU,
	PLAY_GAME
};
enum class PlayerTypes {
	PLAYER,
	AI
};

// UI
int flush_getch();
int nodelay_getch();
int timeout_getch(int milliseconds);

class MainMenu;
class SettingsMenu;

class TicTacToeGame {
private:
	// Members
	TicTacToe ttt;
	bool running;
	bool autoContinue;
	
	// Menus
	Menus currentMenu;
	std::shared_ptr<MainMenu> mainMenu;
	std::shared_ptr<SettingsMenu> settingsMenu;
	
	// AI X
	std::vector<float> aiRewardsX;
	bool useSnapshotX;
	std::string snapshotPathX;
	int iterationsX;
	float rewardThresholdX;
	bool isThresholdFixedX;
	float rewardDecayX;
	
	// AI Y
	std::vector<float> aiRewardsO;
	bool useSnapshotO;
	std::string snapshotPathO;
	int iterationsO;
	float rewardThresholdO;
	bool isThresholdFixedO;
	float rewardDecayO;
	
	// Gameplay
	GameModes gameMode;
	int playerSide;
	int board[9];
	int currentPosition;
	int currentSide;
	std::string status;
	std::string status2;
	int statusColor;
	int winsX;
	int winsO;
	int ties;
	
public:
	// Constructors
	TicTacToeGame();
	~TicTacToeGame();

	// Running
	void run();
	
	// Helpers
	std::vector<float>& getAIRewards(int side);
	bool& getUseSnapshot(int side);
	std::string& getSnapshotPath(int side);
	int& getIterations(int side);
	float& getRewardThreshold(int side);
	bool& getIsThresholdFixed(int side);
	float& getRewardDecay(int side);
	
private:
	// Gameplay
	bool playGame();
	void startGame();
	void endGame(int winner);
	
	// Turns
	void nextTurn();
	int playerTakeTurn();
	int aiTakeTurn();
	
	// Helpers
	PlayerTypes getPlayerType(int side);
	void getWinSpaces(int board[9], bool winSpaces[9]);
	
	// Drawing
	void drawBoard();
	void drawWindow();
	
	// AI
	bool loadRewards(int side);
	
	// Friends
	friend class MainMenu;
	friend class SettingsMenu;
};

#endif

