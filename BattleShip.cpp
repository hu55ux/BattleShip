#include <iostream>
#include <vector>
#include <map>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>
#include <limits>
#include <conio.h>
#include <windows.h> // For color and animation
#include <memory>
#include <iomanip>

using namespace std;

#include "class.h"


int main() {
	srand(static_cast<unsigned>(time(nullptr)));  // Random seed initialization

	try
	{
		// Create game boards
		const int boardSize = 10;
		auto board1 = make_shared<Board>(boardSize);
		auto board2 = make_shared<Board>(boardSize);

		// Create players (will be overridden by game mode selection)
		unique_ptr<Player>player1 = make_unique<HumanPlayer>(*board1);
		unique_ptr<Player>player2 = make_unique<HumanPlayer>(*board2);

		// Initialize and run the game
		Game game(player1.get(), player2.get());
		game.start();
	}
	catch (const exception& e)
	{
		cerr << "Error: " << e.what() << endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;

	return 0;
}