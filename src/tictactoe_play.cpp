#include "tictactoe.h"
#include "tictactoe_game.h"

int main() {
	// Change working directory to that of program
	char buffer[1024] = { 0 };
	int index = readlink("/proc/self/exe", buffer, 1024);
	buffer[index] = '\0';
	for (int i = index - 1; i >= 0; i--) {
		if (buffer[i] == '/') {
			buffer[i] = '\0';
			break;
		}
	}
	chdir(buffer);
	
	// Run the game
	TicTacToeGame game = TicTacToeGame();
	game.run();
	
	return 0;
}
