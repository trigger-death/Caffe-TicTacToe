#include "tictactoe.h"

// Hashing
unsigned int TicTacToe::hashPosition(int board[9]) {
	unsigned int hash = 0;
	for (int i = 0; i < 9; i++) {
		hash |= (board[i] + 1) << (2 * i);
	}
	return hash;
}

// ID
int TicTacToe::getIDFromPosition(int board[9]) {
	return hashMap[hashPosition(board)];
}
void TicTacToe::getPositionFromID(int id, int board[9]) {
	for (int i = 0; i < 9; i++) {
		board[i] = (int)data[id * 9 + i];
	}
}

// Information
bool TicTacToe::isWin(int board[9], int side) {
	return ((board[0] == side && board[1] == side && board[2] == side) ||
		(board[3] == side && board[4] == side && board[5] == side) ||
		(board[6] == side && board[7] == side && board[8] == side) ||
		(board[0] == side && board[3] == side && board[6] == side) ||
		(board[1] == side && board[4] == side && board[7] == side) ||
		(board[2] == side && board[5] == side && board[8] == side) ||
		(board[0] == side && board[4] == side && board[8] == side) ||
		(board[2] == side && board[4] == side && board[6] == side));
}
bool TicTacToe::isTie(int board[9]) {
	return (getOpenSpaces(board) == 0);
}
int TicTacToe::getCurrentSide(int board[9]) {
	int side = 0;
	for (int i = 0; i < 9; i++) {
		side -= board[i];
	}
	if (side == 0)
		return whoGoesFirst;
	else
		return side;
}
int TicTacToe::getOpenSpaces(int board[9]) {
	int open = 0;
	for (int i = 0; i < 9; i++) {
		if (board[i] == 0)
			open++;
	}
	return open;
}

// Testing
int TicTacToe::getBestPosition(int board[9], int side, std::vector<float>& rewards) {
	return getBestPosition(board, side, 0, 0, rewards);
}
int TicTacToe::getBestPosition(int board[9], int side, float rewardThreshold, bool isThresholdFixed, std::vector<float>& rewards) {
	// Flip the board positionss to match the rewards and positions
	if (whoGoesFirst != DEFAULT_WHO_GOES_FIRST) {
		side = SWITCH_SIDE(side);
		for (int i = 0; i < 9; i++) {
			board[i] = SWITCH_SIDE(board[i]);
		}
	}
	
	bool noBest = true;
	float bestReward = (float)-side*2;
	int bestPosition = -1;
	std::vector<std::pair<int, float>> bests; // <Position, Reward>
	
	// Find all open positions
	for (int i = 0; i < 9; i++) {
		if (board[i] == OPEN_SPACE) {
			board[i] = side;
			
			int id = getIDFromPosition(board);
			float reward = rewards[id];
			float tReward = label[id];
			if (log) {
				std::cout << std::fixed;
				std::cout << "ID: "<< std::setw(4) << std::left << id <<
					", Reward: " << std::setw(7) << std::right <<
						std::setprecision(5) << reward <<
					", True: " << std::setw(7) << std::right <<
						std::setprecision(5) << tReward <<
					std::endl;
			}
			if ((side == 1 && reward > bestReward) || (side == -1 && reward < bestReward) || noBest) {
				bestReward = reward;
				bestPosition = i;
				noBest = false;
			}
			bests.push_back(std::pair<int, float>(i, reward));
			
			board[i] = OPEN_SPACE;
		}
	}
	
	// Flip the board positions back
	if (whoGoesFirst != DEFAULT_WHO_GOES_FIRST) {
		for (int i = 0; i < 9; i++) {
			board[i] = SWITCH_SIDE(board[i]);
		}
	}
	
	if (side == -1) {
		bestReward = -bestReward;
		for (int i = 0; i < bests.size(); i++) {
			bests[i].second = -bests[i].second;
		}
	}
	// Filter out best positions outside of the threshold
	for (int i = 0; i < bests.size(); i++) {
		if (isThresholdFixed && bests[i].second < bestReward - rewardThreshold) {
			bests.erase(bests.begin() + i);
			i--;
		}
		else if (!isThresholdFixed && (bests[i].second + 1.0f) < (bestReward + 1.0f) * (1.0f - rewardThreshold)) {
			bests.erase(bests.begin() + i);
			i--;
		}
	}
	
	// Get best position
	if (!bests.empty())
		bestPosition = bests[rand() % bests.size()].first;
	if (log)
		std::cout << "Best Position: " << bestPosition << std::endl;
	return bestPosition;
}

// Display
void TicTacToe::displayPosition(int board[9], int padding) {
	displayPosition(std::cout, board, padding);
}
void TicTacToe::displayPosition(std::ostream& out, int board[9], int padding) {
	for (int y = 0; y < 3; y++) {
		out << std::string(padding, ' ');
		for (int x = 0; x < 3; x++) {
			int side = board[y * 3 + x];
			out << GET_SIDE_CHAR(side);
			if (x < 2)
				out << "|";
			else
				out << std::endl;
		}
		if (y < 2)
			out << std::string(padding, ' ') << "-+-+-" << std::endl;
	}
}
