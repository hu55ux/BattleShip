#pragma once

#include "constants.h"
#include "point.h"
#include "ship.h"
#include "board.h"
#include "playerBase.h"
#include "humanPlayer.h"
#include "computerPlayer.h"



class Game {
private:
	Player* _player1;
	Player* _player2;
	bool _isAgainstComputer;
	bool _isComputerVsComputer;

	// Set text color (Windows specific)
	void setColor(int color) {
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
	}

	// Reset to default color
	void resetColor() {
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
	}

	// Typewriter effect with color
	void typeText(const string& text, int color = 7, int delayMs = 7) {
		setColor(color);
		for (char c : text) {
			cout << c << flush;
			Sleep(delayMs);
		}
		resetColor();
	}

	// Handle user input for selecting attack with the cursor
	Point selectAttackWithCursor(Player* currentPlayer) {
		Point cursor(0, 0);
		bool attackConfirmed = false;

		while (!attackConfirmed) {
			displayDualBoards(cursor); // Show the boards with the cursor highlight

			if (_isAgainstComputer && currentPlayer == _player1) {
				cout << "\nUse arrow keys to move, ENTER to attack, ESC to cancel\n";
			}
			else if (!_isAgainstComputer && !_isComputerVsComputer) {
				cout << "\nPLAYER " << (currentPlayer == _player1 ? "1" : "2")
					<< ": Use arrow keys to move, ENTER to attack, ESC to cancel\n";
			}

			int c = _getch();
			switch (c) {
			case KEY_UP:
				if (cursor.getY() > 0) cursor.decrementY();
				break;
			case KEY_DOWN:
				if (cursor.getY() < currentPlayer->getBoard().getBoardSize() - 1) cursor.incrementY();
				break;
			case KEY_LEFT:
				if (cursor.getX() > 0) cursor.decrementX();
				break;
			case KEY_RIGHT:
				if (cursor.getX() < currentPlayer->getBoard().getBoardSize() - 1) cursor.incrementX();
				break;
			case ENTER_KEY:
				if (currentPlayer->getAttackBoard()[cursor.getY()][cursor.getX()] == '#') {
					attackConfirmed = true;
					return cursor;
				}
				else {
					cout << "You already attacked here! Try another position!";
					Sleep(1000);
				}
				break;
			case ESC:
				return Point(-1, -1); // Exit
			default:
				break;
			}
		}
	}

	// Display the boards of both players side by side
	void displayDualBoards(const Point& cursor = Point(-1, -1)) {
		system("cls||clear");

		Board& board1 = _player1->getBoard();
		Board& board2 = _player2->getBoard();
		int size = board1.getBoardSize();

		// Display headers
		setColor(11); // Cyan for "YOUR FLEET"
		cout << "  YOUR FLEET";
		cout << string(size * 3 - 10, ' ');
		setColor(12); // Red for "ENEMY TERRITORY"
		cout << "ENEMY TERRITORY\n";
		resetColor();

		// Display column numbers for both boards
		cout << "   ";
		for (int i = 0; i < size; ++i) cout << setw(2) << i << " ";
		cout << "    ";
		for (int i = 0; i < size; ++i) cout << setw(2) << i << " ";
		cout << endl;

		// Display the boards row by row
		for (int y = 0; y < size; ++y) {
			// Player 1's board (left)
			setColor(11); // Cyan for row numbers
			cout << setw(2) << y << " ";
			resetColor();
			for (int x = 0; x < size; ++x) {
				char cell = board1.getCell(x, y);
				if (cell == 'S') setColor(10);      // Player ships - green
				else if (cell == 'H') setColor(12); // Hits - red
				else if (cell == 'M') setColor(8);  // Misses - gray
				cout << setw(2) << cell << " ";
				resetColor();
			}

			// Space between the two boards
			cout << "    ";

			// Player 2's board (right)
			setColor(12); // Red for row numbers
			cout << setw(2) << y << " ";
			resetColor();
			for (int x = 0; x < size; ++x) {
				char cell = board2.getCell(x, y);

				// Highlight cursor if it's on this cell
				if (cursor.getX() == x && cursor.getY() == y) {
					setColor(14); // Yellow background
					setColor(12); // Red text
					cout << "X ";  // Cursor symbol
					resetColor();
				}
				else {
					if (cell == 'H') setColor(12);      // Hits - red
					else if (cell == 'M') setColor(8);  // Misses - gray
					else {
						setColor(7);                   // Hidden or unexplored cells
						cell = '#';                   // Hide opponent's ships
					}
					cout << setw(2) << cell << " ";
					resetColor();
				}
			}
			cout << endl;
		}

		// Display legend
		setColor(10); cout << "S"; resetColor(); cout << " Your Ship  ";
		setColor(12); cout << "H"; resetColor(); cout << " Hit  ";
		setColor(8); cout << "M"; resetColor(); cout << " Miss  ";
		setColor(14); cout << "X"; resetColor(); cout << " Cursor\n";
	}

	// Get the game mode from the user
	int getGameMode() {
		int choice = 0;
		while (true) {
			system("cls||clear");
			cout << "Select game mode: " << COLOR_RESET << endl;
			cout << (choice == 0 ? COLOR_MAGENTA : COLOR_RESET) << "1. Player vs Player." << COLOR_RESET << endl;
			cout << (choice == 1 ? COLOR_MAGENTA : COLOR_RESET) << "2. Player vs Computer." << COLOR_RESET << endl;
			cout << (choice == 2 ? COLOR_MAGENTA : COLOR_RESET) << "3. Computer vs Computer." << COLOR_RESET << endl;
			cout << (choice == 3 ? COLOR_MAGENTA : COLOR_RESET) << "4. Exit game." << COLOR_RESET << endl;
			cout << COLOR_RESET << endl;

			int c = _getch();
			switch (c) {
			case KEY_UP:
				choice = (choice > 0) ? choice - 1 : 2;
				break;
			case KEY_DOWN:
				choice = (choice < 3) ? choice + 1 : 0;
				break;
			case ENTER_KEY:
				return choice;
			default:
				break;
			}
		}
	}

	// Display the welcome message
   // Display the welcome message
	void welcomeMessage() {
		system("cls||clear");
		setColor(11);
		cout << R"(
                                             ___  ___ ______________   __________ _________ 
                                            / _ )/ _ /_  __/_  __/ /  / __/ __/ // /  _/ _ \
                                           / _  / __ |/ /   / / / /__/ _/_\ \/ _  // // ___/
                                          /____/_/ |_/_/   /_/ /____/___/___/_//_/___/_/    
                                                                             
)";
		resetColor();

		typeText("\n\n                        >>> NAVAL COMBAT SIMULATION <<<", 14);
		typeText("\n===========================================================================\n\n", 9);

		typeText("WELCOME, COMMANDER!\n", 10);
		typeText("In this high-stakes naval warfare simulation, you have been assigned\n", 7);
		typeText("the role of Fleet Commander. Your objective is simple but critical:\n", 7);
		typeText("locate and eliminate all enemy warships hidden across the ocean grid.\n\n", 7);

		typeText("GAME OVERVIEW:\n", 10);
		typeText("- Battleship is a turn-based strategy game.\n", 7);
		typeText("- You and your opponent place ships on your respective boards.\n", 7);
		typeText("- On each turn, you will select coordinates to fire missiles.\n", 7);
		typeText("- A hit will damage or destroy an enemy ship. A miss will cost you time.\n", 7);
		typeText("- The first to sink all opposing ships wins the battle.\n\n", 7);

		typeText("TIPS FOR SUCCESS:\n", 10);
		typeText("- Think like a strategist: enemies may try to hide ships near edges.\n", 7);
		typeText("- Pay attention to patterns from previous hits.\n", 7);
		typeText("- Use your memory and deduction skills effectively.\n\n", 7);

		typeText("Are you ready to take command of your fleet and dominate the seas?\n", 11);
		typeText("The fate of your navy lies in your hands, Commander...\n\n", 11);

		typeText("Press any key to launch your mission!\n\n", 14);
		_getch();
	}


public:
	Game(Player* player1, Player* player2)
		: _player1(player1), _player2(player2),
		_isAgainstComputer(dynamic_cast<ComputerPlayer*>(_player2) != nullptr),
		_isComputerVsComputer(dynamic_cast<ComputerPlayer*>(_player1) != nullptr &&
			dynamic_cast<ComputerPlayer*>(_player2) != nullptr) {
	}

	void reset() {
		_player1->reset();
		_player2->reset();
	}

	void start() {
		welcomeMessage();

		int choice = getGameMode();
		if (choice == 3) {
			typeText("\nExiting game. Goodbye!\n", 12);
			return;
		}

		const int boardSize = 10;
		auto board1 = make_shared<Board>(boardSize);
		auto board2 = make_shared<Board>(boardSize);

		switch (choice) {
		case 0: // Player vs Player
			_player1 = new HumanPlayer(*board1);
			_player2 = new HumanPlayer(*board2);
			_isComputerVsComputer = false;
			_isAgainstComputer = false;
			break;
		case 1: // Player vs Computer
			_player1 = new HumanPlayer(*board1);
			_player2 = new ComputerPlayer(*board2);
			_isComputerVsComputer = false;
			_isAgainstComputer = true;
			break;
		case 2: // Computer vs Computer
			_player1 = new ComputerPlayer(*board1);
			_player2 = new ComputerPlayer(*board2);
			_isComputerVsComputer = true;
			_isAgainstComputer = false;
			break;
		default:
			return;
		}

		// Ship placement
		if (_isComputerVsComputer) {
			_player1->placeShips(true);
			_player2->placeShips(true);
		}
		else {
			if (_isAgainstComputer) {
				cout << "Computer is placing ships..." << endl;
				_player2->placeShips(true);
				Sleep(1000);
			}

			bool autoPlace;
			cout << "Player 1, auto-place ships? (1-yes, 0-no): ";
			cin >> autoPlace;
			_player1->placeShips(autoPlace);

			if (!_isAgainstComputer) {
				cout << "Player 2, auto-place ships? (1-yes, 0-no): ";
				cin >> autoPlace;
				_player2->placeShips(autoPlace);
			}
		}

		// Main game loop
		Player* current = _player1;
		Player* opponent = _player2;

		while (true) {
			displayDualBoards();

			// Get attack
			Point attack;
			if (dynamic_cast<HumanPlayer*>(current)) {
				attack = selectAttackWithCursor(current);
				if (attack.getX() == -1) continue;
			}
			else {
				cout << "Computer thinking...";
				Sleep(1500);
				attack = current->selectAttack();
			}

			// Process attack
			bool hit = opponent->getBoard().attack(attack);
			current->processAttackResult(attack, hit);

			// Check win condition
			if (opponent->getBoard().allShipsSunk()) {
				displayDualBoards();
				string winner;
				if (_isComputerVsComputer) {
					winner = "Computer " + string(current == _player1 ? "1" : "2");
				}
				else if (_isAgainstComputer) {
					winner = (current == _player1) ? "You" : "Computer";
				}
				else {
					winner = "Player " + string(current == _player1 ? "1" : "2");
				}
				typeText("\n" + winner + " WIN!\n", 14);
				break;
			}

			// Switch turns if miss
			if (!hit) swap(current, opponent);
		}

		typeText("\nGame Over! Thanks for playing!\n", 14);
		delete _player1;
		delete _player2;
	}
};