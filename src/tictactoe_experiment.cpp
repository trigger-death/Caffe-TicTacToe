#include "tictactoe.h"

using namespace std;

#define EXPERIMENT_LAYER(brd, expect) { \
		int position[9] = brd; \
		int expected[9] = expect; \
		experimentLayer(ttt, results, position, expected, fout, rightCount, allCount); \
	} 
#define EXPERIMENT_LAYER_2(brd, expect1, expect2) { \
		int board[9] = brd; \
		int expected1[9] = expect1; \
		int expected2[9] = expect2; \
		experimentLayer2(ttt, results, board, expected1, expected2, fout, rightCount, allCount); \
	}
// Comma
#define C ,

// Testing
void experimentLayer2(TicTacToe& ttt, std::vector<float>& results, int board[9], int expected1[9], int expected2[9], ofstream& fout, int& rightCount, int& allCount) {
	int side = ttt.getCurrentSide(board);
	int best = ttt.getBestPosition(board, side, results);
	
	cout << "--- Test ---:" << endl;
	fout << "--- Test ---:" << endl;
	ttt.displayPosition(cout, board);
	ttt.displayPosition(fout, board);
	cout << endl;
	fout << endl;
	board[best] = side;
	ttt.displayPosition(cout, board);
	ttt.displayPosition(fout, board);

	bool result1 = true;
	bool result2 = true;
	for(int i = 0;i < 9 && (result1 || result2); i++) {
		if (board[i] != expected1[i]) {
			result1 = false;
		}
		if (board[i] != expected2[i]) {
			result2 = false;
		}
	}
	
	cout << endl << "--- ---- ---:" << ((result1 || result2) ? "right" : "wrong") << endl << endl;
	fout << endl << "--- ---- ---:" << ((result1 || result2) ? "right" : "wrong") << endl << endl;
	if (result1 || result2) {
		rightCount++;
	}
	allCount++;
}
void experimentLayer(TicTacToe& ttt, std::vector<float>& results, int board[9], int expected[9], ofstream& fout, int& rightCount, int& allCount) {
	int side = ttt.getCurrentSide(board);
	int best = ttt.getBestPosition(board, side, results);
	
	cout << "--- Test ---:" << endl;
	fout << "--- Test ---:" << endl;
	ttt.displayPosition(cout, board);
	ttt.displayPosition(fout, board);
	cout << endl;
	fout << endl;
	board[best] = side;
	ttt.displayPosition(cout, board);
	ttt.displayPosition(fout, board);

	bool result = true;
	for(int i = 0;i < 9; i++) {
		if (board[i] != expected[i]) {
			result = false;
			break;
		}
	}
	
	cout << endl << "--- ---- ---:" << (result ? "right" : "wrong") << endl << endl;
	fout << endl << "--- ---- ---:" << (result ? "right" : "wrong") << endl << endl;
	if (result) {
		rightCount++;
	}
	allCount++;
}
void experiment(TicTacToe& ttt, std::vector<float>& results) {
	ofstream fout("test.txt", std::ofstream::out);
	int rightCount = 0;
	int allCount = 0;
	EXPERIMENT_LAYER(
		{0},
		
		{0 C 0 C 0 C
		0 C 1 C 0 C
		0 C 0 C 0}
	)

	EXPERIMENT_LAYER(
		{0 C 0 C 0 C
		1 C 1 C -1 C
		0 C 0 C -1},
		
		{0 C 0 C 1 C
		1 C 1 C  -1 C
		0 C 0 C -1}
	)
	
	EXPERIMENT_LAYER(
		{0 C 0 C -1 C
		0 C 1 C 0 C
		1 C 0 C -1},
		
		{0 C 0 C -1 C
		0 C 1 C  1 C
		1 C 0 C -1}
	)

	EXPERIMENT_LAYER(
		{0 C 0 C -1 C
		0 C 0 C 0 C
		1 C 1 C -1},
		
		{0 C 0 C -1 C
		0 C 0 C  1 C
		1 C 1 C -1}
	)

	EXPERIMENT_LAYER(
		{0 C 0 C -1 C
		0 C 1 C 0 C
		0 C 1 C -1},
		
		{0 C 1 C -1 C
		0 C 1 C  0 C
		0 C 1 C -1}
	)


	EXPERIMENT_LAYER(
		{-1 C -1 C 0 C
		0 C 1 C 0 C
		0 C 1 C 0},
		
		{-1 C -1 C 1 C
		0 C 1 C  0 C
		0 C 1 C 0}
	)
	
	EXPERIMENT_LAYER_2(
		{0 C 0 C -1 C
		0 C 1 C 1 C
		-1 C 1 C -1},
		
		{0 C 0 C -1 C
		1 C 1 C  1 C
		-1 C 1 C -1},
		
		{0 C 1 C -1 C
		0 C 1 C  1 C
		-1 C 1 C -1}
	)

	EXPERIMENT_LAYER(
		{1 C -1 C -1 C
		0 C 1 C  0 C
		-1 C 1 C 0},
		
		{1 C -1 C -1 C
		0 C 1 C  0 C
		-1 C 1 C 1}
	)

	cout << "\nOverall results: " << rightCount << " / " << allCount << endl;
	fout << "\nOverall results: " << rightCount << " / " << allCount << endl;
	fout.close();
	
}
