#pragma once


class HumanPlayer : public Player {
private:
	map<int, int> _shipsLeft;
	vector<int> _shipLengths;
	vector<vector<char>> _attackBoard;

	int getValidInput(int min, int max) {
		int input;
		while (true) {
			cin >> input;
			if (cin.fail() || input < min || input > max) {
				cin.clear();
				cin.ignore();
				cout << "Invalid input! Please enter between "
					<< min << " and " << max << ": ";
			}
			else {
				return input;
			}
		}
	}

	void drawShipPreview(const Point& cursor, int length, bool horizontal) {
		system("cls");
		cout << "Placing ship (Length: " << length << ")\n";
		cout << "Position: (" << cursor.getX() << "," << cursor.getY() << ")\n";
		cout << "Orientation: " << (horizontal ? "Horizontal" : "Vertical") << "\n\n";

		const int size = board.getBoardSize();
		bool isValidPlacement = board.canPlaceShip(cursor, length, horizontal);

		for (int y = 0; y < size; y++) {
			for (int x = 0; x < size; x++) {
				bool isPreviewPart = false;
				bool conflictsWithShip = false;

				// Preview hissəsi olub-olmadığını yoxla
				for (int i = 0; i < length; i++) {
					int px = cursor.getX() + (horizontal ? i : 0);
					int py = cursor.getY() + (horizontal ? 0 : i);

					if (x == px && y == py) {
						isPreviewPart = true;
						conflictsWithShip = (board.getCell(x, y) == 'S');
						break;
					}
				}

				if (isPreviewPart) {
					if (conflictsWithShip || !isValidPlacement) {
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12); // Qırmızı
						cout << "X ";
					}
					else {
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10); // Yaşıl
						cout << "O ";
					}
				}
				else {
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7); // Ağ
					cout << (board.getCell(x, y) == 'S' ? "S " : "# ");
				}
			}
			cout << endl;
		}
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7); // Reset

		cout << "\nControls: Arrow keys-Move, SHIFT-Rotate, ENTER-Place\n";
		cout << "Legend: S=Your ship, O=Preview, X=Invalid position\n";
	}


public:
	HumanPlayer(Board& board) : Player(board) {
		_shipsLeft[4] = 1;
		_shipsLeft[3] = 2;
		_shipsLeft[2] = 3;
		_shipsLeft[1] = 4;
		_shipLengths = { 4, 3, 3, 2, 2, 2, 1, 1, 1, 1 };
		_attackBoard = vector<vector<char>>(board.getBoardSize(), vector<char>(board.getBoardSize(), '#'));
	}

	void reset() override {
		board.reset();
		_shipsLeft.clear();
		_shipsLeft[4] = 1;
		_shipsLeft[3] = 2;
		_shipsLeft[2] = 3;
		_shipsLeft[1] = 4;
		_shipLengths = { 4, 3, 3, 2, 2, 2, 1, 1, 1, 1 };
		_attackBoard = vector<vector<char>>(board.getBoardSize(),
			vector<char>(board.getBoardSize(), '#'));
	}

	void placeShips(bool autoPlace = false) override {
		if (autoPlace) {
			cout << "Auto placement selected.\n";
			for (int length : _shipLengths) {
				int attempts = 0;
				const int maxAttempts = 100;

				while (attempts < maxAttempts) {
					int x = rand() % board.getBoardSize();
					int y = rand() % board.getBoardSize();
					bool horizontal = rand() % 2;

					if (board.placeShip(Point(x, y), length, horizontal)) {
						_shipsLeft[length]--;
						break;
					}
					attempts++;
				}

			}
			cout << "All ships placed automatically!\n";
		}
		else {
			system("cls");
			cout << "Manual placement selected. Use arrows to move, SHIFT to rotate, ENTER to place.\n";
			Sleep(1000);

			for (int length : _shipLengths) {
				bool placed = false;
				Point cursor(0, 0);
				bool horizontal = true;  // Default üfüqi orientasiya

				while (!placed) {
					drawShipPreview(cursor, length, horizontal);

					int key = _getch();
					switch (key) {
					case KEY_UP:
						if (cursor.getY() > 0) cursor.decrementY();
						break;
					case KEY_DOWN: {
						int newY = cursor.getY() + 1;
						if (horizontal && newY < board.getBoardSize()) {
							cursor.incrementY();
						}
						else if (!horizontal && (newY + length - 1) < board.getBoardSize()) {
							cursor.incrementY();
						}
						break;
					}
					case KEY_LEFT:
						if (cursor.getX() > 0) cursor.decrementX();
						break;
					case KEY_RIGHT: {
						int newX = cursor.getX() + 1;
						if (!horizontal && newX < board.getBoardSize()) {
							cursor.incrementX();
						}
						else if (horizontal && (newX + length - 1) < board.getBoardSize()) {
							cursor.incrementX();
						}
						break;
					}
					case ROTATE: {
						bool newHorizontal = !horizontal;
						if ((newHorizontal && (cursor.getX() + length <= board.getBoardSize())) ||
							(!newHorizontal && (cursor.getY() + length <= board.getBoardSize()))) {
							horizontal = newHorizontal;
						}
						break;
					}
					case ENTER_KEY:
						if (board.canPlaceShip(cursor, length, horizontal)) {
							board.placeShip(cursor, length, horizontal);
							_shipsLeft[length]--;
							placed = true;
							cout << "Ship placed successfully!\n";
							Sleep(500);
						}
						else {
							cout << "Cannot place ship here! Try another position!\n";
							Sleep(1000);
						}
						break;
					default:
						break;
					}
				}
			}
		}
	}

	void takeTurn() override {
		int x, y;
		cout << "Enter coordinates to attack (x y): ";
		x = getValidInput(0, board.getBoardSize() - 1);
		y = getValidInput(0, board.getBoardSize() - 1);

		if (board.attack(Point(x, y))) {
			cout << "Hit!\n";
		}
		else {
			cout << "Miss!\n";
		}
	}

	void showRemainingShips() {
		for (auto& ship : _shipsLeft) {
			cout << "Length " << ship.first << ": " << ship.second << " ship remaining." << endl;
		}
	}

	int getRandomShipLength() {
		vector<int> availableLengths;
		for (auto& ship : _shipsLeft) {
			if (ship.second > 0) {
				availableLengths.push_back(ship.first);
			}
		}

		if (availableLengths.empty()) return -1;

		int index = rand() % availableLengths.size();
		return availableLengths[index];
	}


	void displayBoards() const {
		cout << "\nYour Board:\n";
		board.display();

		cout << "\nAttack Board:\n";
		for (int y = 0; y < board.getBoardSize(); ++y) {
			for (int x = 0; x < board.getBoardSize(); ++x) {
				cout << _attackBoard[y][x] << " ";
			}
			cout << endl;
		}
	}

	void processAttackResult(const Point& p, bool hit) override {
		_attackBoard[p.getY()][p.getX()] = hit ? 'H ' : 'M ';
	}
};