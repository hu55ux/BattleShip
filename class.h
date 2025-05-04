#pragma once


#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_RIGHT 77
#define KEY_LEFT 75
#define ESC 27
#define ENTER_KEY 13
#define ROTATE 32 

#define COLOR_RESET "\033[0m"
#define COLOR_MAGENTA "\033[35m"





class Point {
private:
	int _x, _y;
public:

	Point() : _x(0), _y(0) {}
	Point(int x, int y) : _x(x), _y(y) {}

	bool operator==(const Point& other) const {
		return this->_x == other._x && this->_y == other._y;
	}

	// Prefix increment X (++x)
	Point& incrementX() {
		++_x;
		return *this;
	}

	// Postfix increment X (x++)
	Point incrementX(int) {
		Point temp = *this;
		++_x;
		return temp;
	}

	// Prefix increment Y (++y)
	Point& incrementY() {
		++_y;
		return *this;
	}

	// Postfix increment Y (y++)
	Point incrementY(int) {
		Point temp = *this;
		++_y;
		return temp;
	}

	// Prefix decrement X (--x)
	Point& decrementX() {
		--_x;
		return *this;
	}

	// Postfix decrement X (x--)
	Point decrementX(int) {
		Point temp = *this;
		--_x;
		return temp;
	}

	// Prefix decrement Y (--y)
	Point& decrementY() {
		--_y;
		return *this;
	}

	// Postfix decrement Y (y--)
	Point decrementY(int) {
		Point temp = *this;
		--_y;
		return temp;
	}

	void setX(int x) { _x = x; }
	void setY(int y) { _y = y; }
	int getX() const { return _x; }
	int getY() const { return _y; }

	void display() const {
		cout << "Coordinates: (" << _x << "," << _y << ")" << endl;
	}
};

class Ship {
private:
	Point _start;
	bool _horizontal;
	int _length;
	vector<Point> _hitPoints;

public:
	Ship(Point start, bool horizontal, int length)
		: _start(start), _horizontal(horizontal), _length(length) {
	}

	bool isHit(const Point& p) const {
		vector<Point> occupied = getOccupiedPoints();
		for (const auto& pos : occupied) {
			if (pos == p) {
				return true;
			}
		}
		return false;
	}

	void registerHit(const Point& p) {
		if (isHit(p)) {
			for (const auto& hit : _hitPoints) {
				if (hit == p) {
					return;
				}
			}
			_hitPoints.push_back(p);
		}
	}

	bool isSunk() const {
		return _hitPoints.size() == _length;
	}

	// Gəminin tutduğu coordinatları qaytarır
	vector<Point> getOccupiedPoints() const {
		vector<Point> points;
		for (int i = 0; i < _length; ++i) {
			if (_horizontal) {
				points.push_back(Point(_start.getX() + i, _start.getY()));
			}
			else {
				points.push_back(Point(_start.getX(), _start.getY() + i));
			}
		}
		return points;
	}

	// Gəminin tutduğu coordinatları təyin edir
	void setOccupiedPoints(const vector<Point>& points) {
		if (points.size() == _length) {
			// Adjust the starting point and orientation based on the points
			_start = points.front();
			_horizontal = (points.size() > 1 && points[0].getY() == points[1].getY());
		}
	}
};

class Board {
private:
	int _size;
	vector<vector<char>> _board;
	vector<Ship> _ships;

public:
	Board(int size = 6) : _size(size), _board(_size, vector<char>(size, '#')) {}

	int getBoardSize() const {
		return _size;
	}

	char getCell(int x, int y) const {
		if (x >= 0 && x < getBoardSize() && y >= 0 && y < getBoardSize()) {
			return _board[y][x];
		}
		return ' ';
	}

	void reset() {
		_board = vector<vector<char>>(_size, vector<char>(_size, '#'));
		_ships.clear();
	}

	bool isValid(const Point& p) const {
		return p.getX() >= 0 && p.getX() < getBoardSize() && p.getY() >= 0 && p.getY() < getBoardSize();
	}

	bool canPlaceShip(const Ship& ship) const {
		for (const Point& p : ship.getOccupiedPoints()) {
			if (!isValid(p)) return false;
			if (_board[p.getX()][p.getY()] != '#') return false;

			// Check surrounding coordinates
			for (int dx = -1; dx <= 1; ++dx) {
				for (int dy = -1; dy <= 1; ++dy) {
					int nx = p.getX() + dx;
					int ny = p.getY() + dy;
					if (isValid(Point(nx, ny)) && _board[nx][ny] != '#') {
						return false; // Check if there is another ship around
					}
				}
			}
		}
		return true;
	}

	bool canPlaceShip(Point start, int length, bool horizontal) const {
		for (int i = 0; i < length; ++i) {
			int x = start.getX() + (horizontal ? i : 0);
			int y = start.getY() + (horizontal ? 0 : i);
			Point p(x, y);

			if (!isValid(p)) return false;
			if (_board[y][x] != '#') return false;

			// Check adjacent cells
			for (int dx = -1; dx <= 1; ++dx) {
				for (int dy = -1; dy <= 1; ++dy) {
					int nx = x + dx;
					int ny = y + dy;
					if (isValid(Point(nx, ny)) && _board[ny][nx] != '#') {
						return false;
					}
				}
			}
		}
		return true;
	}

	bool placeShip(const Ship& ship) {
		if (!canPlaceShip(ship)) return false;
		for (const Point& p : ship.getOccupiedPoints()) {
			_board[p.getX()][p.getY()] = 'S'; // Mark the ship's coordinates
		}
		_ships.push_back(ship);
		return true;
	}

	bool placeShip(Point start, int length, bool horizontal) {
		// Validate if the ship can be placed
		if (!canPlaceShip(start, length, horizontal)) {
			return false;
		}

		// Place the ship on the board
		vector<Point> occupiedPoints;
		for (int i = 0; i < length; ++i) {
			int nx = start.getX() + (horizontal ? i : 0);
			int ny = start.getY() + (horizontal ? 0 : i);
			_board[nx][ny] = 'S'; // Mark the ship's coordinates
			occupiedPoints.emplace_back(nx, ny);
		}

		// Add the ship to the list of ships
		Ship ship(start, horizontal, length);
		ship.setOccupiedPoints(occupiedPoints);
		_ships.push_back(ship);

		return true;
	}

	bool attack(const Point& point) {
		int x = point.getX();
		int y = point.getY();

		if (x < 0 || x >= _size || y < 0 || y >= _size) {
			cout << "Invalid attack coordinates!" << endl;
			return false;
		}

		if (_board[y][x] == 'H' || _board[y][x] == 'M') {
			cout << "Already attacked here!" << endl;
			return false;
		}

		for (auto& ship : _ships) {
			if (ship.isHit(point)) {
				ship.registerHit(point);
				_board[y][x] = 'H';
				cout << "Hit!" << endl;
				return true;
			}
		}
		_board[y][x] = 'M';
		cout << "Miss!" << endl;
		return false;
	}

	void display(bool hideShips = true, bool hideMisses = false) const {
		cout << "  ";
		for (int col = 0; col < _size; ++col) {
			cout << col << " ";
		}
		cout << endl;

		for (int row = 0; row < _size; ++row) {
			cout << row << " ";
			for (int col = 0; col < _size; ++col) {
				char cell = _board[row][col];
				if (cell == 'S' && hideShips) {
					cout << ";";// Gəmiləri gizlə
				}
				else if (cell == 'M' && hideMisses) {
					cout << "# "; // Uğursuz hücumları gizlə
				}
				else {
					cout << cell << " "; // Digər hallarda normal göstər
				}
			}
			cout << endl;
		}
	}

	bool allShipsSunk() const {
		for (const auto& ship : _ships) {
			if (!ship.isSunk()) {
				return false;
			}
		}
		return true;
	}
};

class Player {
protected:
	Board board;
	vector<vector<char>> _attackBoard;


public:
	Player(Board& b) : board(b) {
		_attackBoard.resize(board.getBoardSize(),
			vector<char>(board.getBoardSize(), '#'));
	}

	virtual void placeShips(bool autoPlace = false) = 0;
	virtual Point selectAttack() {
		// Random attack for base implementation
		int size = board.getBoardSize();
		return Point(rand() % size, rand() % size);
	}
	virtual void takeTurn() = 0;
	virtual void processAttackResult(const Point& p, bool hit) {
		// Update attack board
		if (p.getX() >= 0 && p.getX() < board.getBoardSize() &&
			p.getY() >= 0 && p.getY() < board.getBoardSize()) {
			_attackBoard[p.getY()][p.getX()] = hit ? 'H' : 'M';
		}
	}
	virtual void reset() {
		// Reset the main board
		board.reset();

		// Reset attack board
		for (auto& row : _attackBoard) {
			fill(row.begin(), row.end(), '#');
		}
	}

	virtual ~Player() = default;

	Board& getBoard() { return board; }
	const vector<vector<char>>& getAttackBoard() const { return _attackBoard; }
	vector<vector<char>>& getAttackBoard() { return _attackBoard; }
};

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

class ComputerPlayer : public Player {
private:
	map<int, int> _shipsLeft;
	vector<Point> _targetQueue;
	vector<vector<bool>> _attacked;

	void addSurroundingPoints(const Point& p) {
		static const int dx[] = { 0, 0, -1, 1 };
		static const int dy[] = { -1, 1, 0, 0 };

		for (size_t dir = 0; dir < 4; ++dir) {
			int nx = p.getX() + dx[dir];
			int ny = p.getY() + dy[dir];

			if (nx >= 0 && nx < board.getBoardSize() && ny >= 0 && ny < board.getBoardSize()) {
				if (!_attacked[ny][nx]) {
					_targetQueue.push_back(Point(nx, ny));
				}
			}
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

	bool tryPlaceShip(int length) {
		const int maxAttempt = 50;
		int attempts = 0;

		while (attempts < maxAttempt)
		{
			int x = rand() % board.getBoardSize();
			int y = rand() % board.getBoardSize();
			bool horizontal = rand() % 2;

			Ship ship(Point(x, y), horizontal, length);

			if (board.canPlaceShip(ship)) {
				board.placeShip(ship);
				_shipsLeft[length]--;
				return true;
			}
			attempts++;
		}
		return false;
	}

public:
	ComputerPlayer(Board& board) : Player(board) {
		_shipsLeft[4] = 1;
		_shipsLeft[3] = 2;
		_shipsLeft[2] = 3;
		_shipsLeft[1] = 4;

		_attacked.resize(board.getBoardSize(), vector<bool>(board.getBoardSize(), false));
	}

	void reset() override {
		board.reset();
		_shipsLeft.clear();
		_shipsLeft[4] = 1;
		_shipsLeft[3] = 2;
		_shipsLeft[2] = 3;
		_shipsLeft[1] = 4;
		_targetQueue.clear();
		_attacked = vector<vector<bool>>(board.getBoardSize(), vector<bool>(board.getBoardSize(), false));

	}

	void placeShips(bool autoPlace = false) override {

		cout << "Computer is placing ships..." << endl;

		vector<int> shipLengths = { 4,3,3,2,2,2,1,1,1,1 };

		for (int length : shipLengths) {
			if (_shipsLeft[length] <= 0)continue;

			if (!tryPlaceShip(length)) {
				cout << "Warning: Could not place ship of length " << length << endl;
			}
		}

		cout << "Computer's ships placed!" << endl;
	}

	void takeTurn() override {
		Point attackPoint = selectAttack();

		if (board.attack(attackPoint)) {
			cout << "Computer hit at ( " << attackPoint.getX() << ", " << attackPoint.getY() << " )" << endl;
			addSurroundingPoints(attackPoint);
		}
		else {
			cout << "Computer missed at ( " << attackPoint.getX() << ". " << attackPoint.getY() << " )" << endl;
		}
	}

	Point selectAttack() override {
		if (!_targetQueue.empty()) {
			Point target = _targetQueue.back();
			_targetQueue.pop_back();
			_attacked[target.getY()][target.getX()] = true;
			return target;
		}

		int x, y;
		do {
			x = rand() % board.getBoardSize();
			y = rand() % board.getBoardSize();
		} while (_attacked[y][x]);

		_attacked[y][x] = true;
		return Point(x, y);
	}

	void processAttackResult(const Point& p, bool hit) override {
		if (hit) {
			addSurroundingPoints(p);
		}
	}
};


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

	Point selectAttackWithCursor(Player* currentPlayer) {
		Point cursor(0, 0);
		bool attackConfirmed = false;

		while (!attackConfirmed) {
			displayDualBoards(cursor); // Kursoru göndərək

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
				return Point(-1, -1);
			default:
				break;
			}
		}
	}

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




	int getGameMode() {
		int choice = 0;
		while (true)
		{
			system("cls||clear");
			cout << "Select game mode: " << COLOR_RESET << endl;
			cout << (choice == 0 ? COLOR_MAGENTA : COLOR_RESET) << "1. Player vs Player." << COLOR_RESET << endl;
			cout << (choice == 1 ? COLOR_MAGENTA : COLOR_RESET) << "2. Player vs Computer." << COLOR_RESET << endl;
			cout << (choice == 2 ? COLOR_MAGENTA : COLOR_RESET) << "3. Computer vs Computer." << COLOR_RESET << endl;
			cout << (choice == 3 ? COLOR_MAGENTA : COLOR_RESET) << "4. Exit game." << COLOR_RESET << endl;
			cout << COLOR_RESET << endl;

			int c = _getch();
			switch (c)
			{
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

		unique_ptr<Player> player1;
		unique_ptr<Player> player2;

		// create players for choice 
		switch (choice)
		{
		case 0: // Player vs Player
			_player1 = new HumanPlayer(*board1);
			_player2 = new HumanPlayer(*board2);
			_isAgainstComputer = false;
			_isComputerVsComputer = false;
			break;
		case 1: // Player vs Computer
			_player1 = new HumanPlayer(*board1);
			_player2 = new ComputerPlayer(*board2);
			_isAgainstComputer = true;
			_isComputerVsComputer = false;
			break;
			break;
		case 2:  // Computer vs Computer
			_player1 = new ComputerPlayer(*board1);
			_player2 = new ComputerPlayer(*board2);
			_isAgainstComputer = true;
			_isComputerVsComputer = true;
			break;
		default:
			break;
		}

		// Ship placement
		if (_isComputerVsComputer) {
			_player1->placeShips(true);
			_player2->placeShips(true);
		}
		else {
			if (_isAgainstComputer) {
				cout << "Computer is placing ships...";
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

		while (true)
		{
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

		typeText("\n\n                  NAVAL COMBAT SIMULATION", 14);
		typeText("\n=============================================================\n\n", 9);

		typeText("GAME INTRODUCTION:\n", 10);
		typeText("Battleship is a classic strategy game where two opponents attempt\n", 7);
		typeText("to locate and destroy each other's fleet of ships on a grid-based\n", 7);
		typeText("ocean. Your mission is to sink all enemy ships before they sink yours!\n\n", 7);

		typeText("Press any key to continue...\n\n\n", 14);
		_getch();
	}
};


