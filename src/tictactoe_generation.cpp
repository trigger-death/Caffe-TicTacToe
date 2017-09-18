#include "tictactoe.h"

// Constructor
TicTacToe::TicTacToe(bool log) {
	srand(time(nullptr));
	
	this->snapshotPath = "./snapshots";
	this->rewardDecay = DEFAULT_REWARD_DECAY;
	this->whoGoesFirst = DEFAULT_WHO_GOES_FIRST;
	this->log = log;
	
	// Read settings
	std::ifstream fin("./settings.txt", std::ifstream::in);
	if (fin.is_open()) {
		std::string name;
		fin >> std::boolalpha;
		while (!fin.eof()) {
			fin >> name;
			if (fin.eof())
				break;
			else if (name == "snapshotPath") {
				getline(fin, snapshotPath);
				// Trim whitespace
				while (!snapshotPath.empty()) {
					if (snapshotPath[0] == ' ' || snapshotPath[0] == '\t')
						snapshotPath = snapshotPath.substr(1, snapshotPath.length() - 1);
					else
						break;
				}
				while (!snapshotPath.empty()) {
					if (snapshotPath[snapshotPath.length()-1] == ' ' || snapshotPath[snapshotPath.length()-1] == '\t')
						snapshotPath = snapshotPath.substr(0, snapshotPath.length() - 1);
					else
						break;
				}
				// Remove quotes
				if (snapshotPath.length() >= 2 && snapshotPath[0] == '"' &&
					snapshotPath[snapshotPath.length()-1] == '"') {
					snapshotPath = snapshotPath.substr(1, snapshotPath.length() - 2);
				}
			}
			else if (name == "rewardDecay")
				fin >> rewardDecay;
			else if (name == "log")
				fin >> log;
		}
		fin.close();
	}
	
	int board[9] = {OPEN_SPACE};
	int id = 0;
	hashMap[0] = id;
	for (int i = 0; i < 9; i++) {
		data.push_back(0.0f);
	}
	label.push_back(-2.0f); // Unset reward
	id++;
	// Yes, use DEFAULT_WHO_GOES_FIRST and not whoGoesFirst
	generatePermutations(id, board, DEFAULT_WHO_GOES_FIRST);
	generateRewards(board, DEFAULT_WHO_GOES_FIRST);
	
	if (log) {
		std::cout << "================================" << std::endl;
		std::cout << " Tic-Tac-Toe" << std::endl;
		std::cout << "--------------------------------" << std::endl;
		std::cout << "Reward Decay: " << rewardDecay << std::endl;
		
		std::cout << std::endl;
		
		std::cout << "Board Permutations: " << label.size() << std::endl;
		int unsetRewards = 0;
		for (int i = 0; i < label.size(); i++) {
			if (label[i] == -2.0f) {
				std::cout << "Unset Reward ID: " << i << std::endl;
				getPositionFromID(i, board);
				displayPosition(std::cout, board);
				unsetRewards++;
			}
		}
		std::cout << "Unset Rewards: " << unsetRewards << std::endl;
		std::cout << "--------------------------------" << std::endl;
	}
}

// Setup Permutations
void TicTacToe::generatePermutations(int &id, int board[9], int side) {
	// travel through all 9 locations
    for(int i = 0; i < 9; i++)  {
        // if this location is empty
        if (board[i] == OPEN_SPACE) {
            // occupy the current location with the current side.
            board[i] = side;

            // generate hash key for the current board
            unsigned int key = hashPosition(board);

            // if the current location has not seen before.
            if (hashMap.find(key) == hashMap.end()) {
                // save the current board in the hash map for training.
                hashMap[key] = id;
                for (int j = 0; j < 9; j++) {
					data.push_back((float)board[j]);
				}
                label.push_back(-2.0f); // Unset reward
                id++;
                
				// if no one has won, recursively generate the next board.
				if (!isWin(board,  side)) {
					generatePermutations(id, board, -side);
				}
            }

            // recover the current location to "empty"
            board[i] = OPEN_SPACE;
        }
    }
}

// Setup Rewards
float TicTacToe::generateRewards(int board[9], int side) {
	int currentID = getIDFromPosition(board);
	int count = 0;
	
	// Check if the reward has already been generated
	float reward = label[currentID];
	if (reward != -2.0f) {
		return reward;
	}
	reward = 0.0f;
	
	if (isWin(board, SWITCH_SIDE(side))) {
		label[currentID] = (float)-side;
	}
	else if (isTie(board)) {
		label[currentID] = 0.0f;
	}
	else {
		for(int i = 0; i < 9; i++) {
			// if this location is empty
			if (board[i] == OPEN_SPACE) {
				count++;
				board[i] = side;
				
				reward += generateRewards(board, -side);
				
				board[i] = OPEN_SPACE;
			}
		}
		label[currentID] = reward / (float)count * rewardDecay;
	}
	return label[currentID];
}

// Reward Decay
void TicTacToe::loadRewardDecay(float decay, std::vector<float>& rewards) {
	//rewards = label;
	label.clear();
	for (int i = 0; i < data.size() / 9; i++) {
		label.push_back(-2.0f);
	}
	rewardDecay = decay;
	int board[9] = {OPEN_SPACE};
	generateRewards(board, DEFAULT_WHO_GOES_FIRST);
	rewards.clear();
	rewards.insert(rewards.begin(), label.begin(), label.end());
}
