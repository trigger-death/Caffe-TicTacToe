#ifndef TICTACTOE_EXPERIMENT_H
#define TICTACTOE_EXPERIMENT_H

// Testing
void experiment(TicTacToe& ttt, std::vector<float>& results);
void experimentLayer(TicTacToe& ttt, std::vector<float>& results, int board[9], int expected[9], std::ofstream& fout, int& rightCount, int& allCount);
void experimentLayer2(TicTacToe& ttt, std::vector<float>& results, int board[9], int expected1[9], int expected2[9], std::ofstream& fout, int& rightCount, int& allCount);

#endif
