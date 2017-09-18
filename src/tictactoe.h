#ifndef TICTACTOE_H
#define TICTACTOE_H

#include <memory>
#include <map>
#include <vector>
#include <termios.h>
#include <unistd.h>
#include <cstdlib>
#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <utility>

#define OPEN_SPACE (0)
#define SIDE_X (1)
#define SIDE_O (-1)
#define SWITCH_SIDE(side) (-(side))
#define GET_SIDE_CHAR(side) ((side) == SIDE_X ? "X" : ((side) == SIDE_O ? "O" : " "))
#define GET_SIDE_STR(side) std::string(GET_SIDE_CHAR(side))
#define DEFAULT_REWARD_DECAY (0.90f)
#define DEFAULT_WHO_GOES_FIRST (SIDE_X)
	
class TicTacToe {
public:
	// Members
	std::string snapshotPath;
	float rewardDecay;
	int whoGoesFirst;
	bool log;
	std::vector<float> data;
    std::vector<float> label;
private:
    std::map<unsigned int, int> hashMap;
    
public:
	// Constructor
    TicTacToe(bool log = true);
	
private:
	// Setup
	void generatePermutations(int &id, int position[9], int side);
	float generateRewards(int position[9], int side);
    
    // Hashing
	unsigned int hashPosition(int board[9]);
	
public:
	// Reward Decay
	void loadRewardDecay(float decay, std::vector<float>& rewards);
	
	// ID
	int getIDFromPosition(int board[9]);
	void getPositionFromID(int id, int board[9]);
	
	// Information
	bool isWin(int board[9], int side);
	bool isTie(int board[9]);
	int getCurrentSide(int board[9]);
	int getOpenSpaces(int board[9]);
	
	// Testing
	int getBestPosition(int board[9], int side, std::vector<float>& rewards);
	int getBestPosition(int board[9], int side, float rewardThreshold, bool isThresholdFixed, std::vector<float>& rewards);

	// Display
	void displayPosition(int board[9], int padding = 0);
	void displayPosition(std::ostream& out, int board[9], int padding = 0);
};


#endif
