#pragma once


#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_RIGHT 77
#define KEY_LEFT 75
#define ESC 27
#define ENTER_KEY 13
#define SHIFT 16




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
	Board(int size = 6) : _size(size), _board(_size, vector<char>(size, '.')) {}

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
		_board = vector<vector<char>>(_size, vector<char>(_size, '.'));
		_ships.clear();
	}

	bool isValid(const Point& p) const {
		return p.getX() >= 0 && p.getX() < getBoardSize() && p.getY() >= 0 && p.getY() < getBoardSize();
	}

	bool canPlaceShip(const Ship& ship) const {
		for (const Point& p : ship.getOccupiedPoints()) {
			if (!isValid(p)) return false;
			if (_board[p.getX()][p.getY()] != '.') return false;

			// Check surrounding coordinates
			for (int dx = -1; dx <= 1; ++dx) {
				for (int dy = -1; dy <= 1; ++dy) {
					int nx = p.getX() + dx;
					int ny = p.getY() + dy;
					if (isValid(Point(nx, ny)) && _board[nx][ny] != '.') {
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
			if (_board[y][x] != '.') return false;

			// Check adjacent cells
			for (int dx = -1; dx <= 1; ++dx) {
				for (int dy = -1; dy <= 1; ++dy) {
					int nx = x + dx;
					int ny = y + dy;
					if (isValid(Point(nx, ny)) && _board[ny][nx] != '.') {
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
					cout << ". "; // Gəmiləri gizlə
				}
				else if (cell == 'M' && hideMisses) {
					cout << ". "; // Uğursuz hücumları gizlə
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



public:
	Player(Board& b) : board(b) {}

	virtual void placeShips(bool autoPlace = false) = 0;
	virtual Point selectAttack() = 0;
	virtual void takeTurn() = 0;
	virtual void processAttackResult(const Point& p, bool hit) = 0;
	virtual void reset() = 0;

	Board& getBoard() { return board; }
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

public:
	HumanPlayer(Board& board) : Player(board) {
		_shipsLeft[4] = 1;
		_shipsLeft[3] = 2;
		_shipsLeft[2] = 3;
		_shipsLeft[1] = 4;
		_shipLengths = { 4,3,3,2,2,2,1,1,1,1 };
		_attackBoard = vector<vector<char>>(board.getBoardSize(), vector<char>(board.getBoardSize(), '.'));
	}

	void reset() override {
		board.reset();
		_shipsLeft.clear();
		_shipsLeft[4] = 1;
		_shipsLeft[3] = 2;
		_shipsLeft[2] = 3;
		_shipsLeft[1] = 4;
		_attackBoard = vector<vector<char>>(board.getBoardSize(),
			vector<char>(board.getBoardSize(), '.'));
	}

	void placeShips(bool autoPlace = false) override {
		if (autoPlace) {
			cout << "Auto placement selected." << endl;
			for (int length : _shipLengths)
			{
				int attempts = 0;
				const int maxAttempts = 100;

				while (attempts < maxAttempts)
				{
					int x = rand() % board.getBoardSize();
					int y = rand() % board.getBoardSize();
					bool horizontal = rand() % 2;

					Ship ship(Point(x, y), horizontal, length);

					if (board.placeShip(ship)) {
						_shipsLeft[length]--;
						break;
					}
					attempts++;
				}
				if (attempts >= maxAttempts) {
					cout << "Failed to place ship of length " << length << " ." << endl;
				}
			}
			cout << "Your ships placed autoatically!" << endl;
		}
		else
		{
			system("cls");
			cout << "Manual placement selected. Use arrows to move, SHIFT to rotate, ENTER to place.\n";

			for (int length : _shipLengths) {
				bool placed = false;
				Point cursor(0, 0);
				bool horizontal = false;

				while (!placed) {
					system("cls");
					board.display();

					// Draw ship preview
					for (int i = 0; i < length; ++i) {
						int nx = cursor.getX() + (horizontal ? i : 0);
						int ny = cursor.getY() + (horizontal ? 0 : i);
						if (nx < board.getBoardSize() && ny < board.getBoardSize()) {
							cout << "\033[32mO\033[0m "; // Green 'O'
						}
					}

					cout << "\nShip length: " << length
						<< ", Position: (" << cursor.getX() << "," << cursor.getY() << ")"
						<< ", Orientation: " << (horizontal ? "Horizontal" : "Vertical") << "\n";

					int key = _getch();
					switch (key)
					{
					case KEY_UP:
						if (cursor.getY() > 0) cursor.decrementY();
						break;

					case KEY_DOWN:
						if (cursor.getY() < board.getBoardSize() - 1) cursor.incrementY();
						break;

					case KEY_LEFT:
						if (cursor.getX() > 0) cursor.decrementX();
						break;

					case KEY_RIGHT:
						if (cursor.getX() < board.getBoardSize() - 1) cursor.incrementX();
						break;

					case SHIFT:
						if ((horizontal && (cursor.getY() + length - 1 < board.getBoardSize())) ||
							(!horizontal && (cursor.getX() + length - 1 < board.getBoardSize()))) {
							horizontal = !horizontal;
						}
						break;
					case ENTER_KEY:
						if (board.canPlaceShip(cursor, length, horizontal)) {
							board.placeShip(Point(cursor.getX(), cursor.getY()), length, horizontal);
							placed = true;
							cout << "Ship placed successfully!" << endl;
						}
						else {
							cout << "Cannot place ship here! Try another position!" << endl;
							_getch();
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
		cin >> x >> y;

		if (board.attack(Point(x, y))) {
			cout << "Hit!" << endl;
		}
		else {
			cout << "Miss!" << endl;
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

	Point selectAttack() override {
		Point cursor(0, 0);
		bool attackConfirmed = false;

		while (!attackConfirmed)
		{
			system("cls||clear");
			cout << "Select attack coordinates (Use Arrow keys to move, Enter to attack, Esc to cancel: )" << endl;

			cout << " ";
			for (int i = 0; i < board.getBoardSize(); i++)	cout << i << " ";
			cout << endl;

			for (int y = 0; y < board.getBoardSize(); ++y)
			{
				cout << y << " ";
				for (int x = 0; x < board.getBoardSize(); ++x)
				{
					if (x == cursor.getX() && y == cursor.getY()) {
						cout << "\033[31mX\033[0m "; // red for cursor
					}
					else
					{
						cout << _attackBoard[y][x];
					}
				}
				cout << endl;
			}

			int key = _getch();
			switch (key)
			{
			case KEY_UP:
				if (cursor.getY() > 0) cursor.decrementY();
				break;
			case KEY_DOWN:
				if (cursor.getY() < board.getBoardSize() - 1) cursor.incrementY();
				break;
			case KEY_LEFT:
				if (cursor.getX() > 0) cursor.decrementX();
				break;
			case KEY_RIGHT:
				if (cursor.getX() < board.getBoardSize()) cursor.incrementX();
				break;
			case ENTER_KEY:
				if (_attackBoard[cursor.getY()][cursor.getX()] == '.') {
					attackConfirmed = true;
				}
				else {
					cout << "You already attacked here! Try another position!" << endl;
				}
				break;
			case ESC:
				return Point(-1, -1);
			default:
				break;
			}
		}
		return cursor;


	}

	void displayBoards() const {
		cout << "\nYour Board\t\t  Attack Board\n";
		cout << "  ";
		for (int i = 0; i < board.getBoardSize(); ++i) cout << i << " ";
		cout << "\t  ";
		for (int i = 0; i < board.getBoardSize(); ++i) cout << i << " ";
		cout << endl;

		for (int y = 0; y < board.getBoardSize(); ++y)
		{
			cout << y << " ";
			for (int x = 0; x < board.getBoardSize(); ++x)
			{
				char c = board.getCell(x, y);
				cout << (c == 'S' ? '.' : c) << " "; // Gəmiləri göstərmirik
			}

			cout << '\t' << y << " ";
			for (int x = 0; x < board.getBoardSize(); ++x)
			{
				cout << _attackBoard[y][x];
			}
			cout << endl;
		}

	}

	void processAttackResult(const Point& p, bool hit) override {
		_attackBoard[p.getY()][p.getX()] = hit ? 'H' : 'M';
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

	void displayDualBoards() {
		system("cls");

		Board& board1 = _player1->getBoard();
		Board& board2 = _player2->getBoard();
		int size = board1.getBoardSize();

		// Display headers
		setColor(11);
		cout << "  YOUR FLEET";
		cout << string(size * 2 - 10, ' ');
		setColor(12);
		cout << "ENEMY TERRITORY\n";
		resetColor();

		// Column numbers
		cout << "  ";
		for (int i = 0; i < size; ++i) cout << i << " ";
		cout << "   ";
		for (int i = 0; i < size; ++i) cout << i << " ";
		cout << endl;

		// Display boards
		for (int y = 0; y < size; ++y) {
			// Player 1's board (left)
			setColor(11);
			cout << y << " ";
			resetColor();
			for (int x = 0; x < size; ++x) {
				char cell = board1.getCell(x, y);
				if (cell == 'S') setColor(10);      // Your ships - green
				else if (cell == 'H') setColor(12); // Hits - red
				else if (cell == 'M') setColor(8);  // Misses - gray
				cout << cell << " ";
				resetColor();
			}

			// Player 2's board (right)
			cout << "  ";
			setColor(12);
			cout << y << " ";
			resetColor();
			for (int x = 0; x < size; ++x) {
				char cell = board2.getCell(x, y);
				if (cell == 'H') setColor(12);      // Hits - red
				else if (cell == 'M') setColor(8);  // Misses - gray
				else {
					setColor(7);                   // Unexplored - normal
					cell = '.';                   // Hide opponent's ships
				}
				cout << cell << " ";
				resetColor();
			}
			cout << endl;
		}

		// Legend
		setColor(10); cout << "S"; resetColor(); cout << " Your Ship  ";
		setColor(12); cout << "H"; resetColor(); cout << " Hit  ";
		setColor(8); cout << "M"; resetColor(); cout << " Miss\n";
	}

	void welcomeMessage() {
		system("cls");
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

		typeText("Press any key to continue...", 14);
		_getch();
	}



	void start() {

		welcomeMessage();

		bool autoPlace;

		if (_isComputerVsComputer) {
			_player1->placeShips(true);
			_player2->placeShips(true);
		}
		else if (_isAgainstComputer) {
			cout << "Computer is placing automatically..." << endl;
			_player2->placeShips(true);
			cout << "Player 1, enable auto ship placement? (1-yes, 0-no): ";
			cin >> autoPlace;
			_player1->placeShips(autoPlace);
		}
		else {
			cout << "\nPlayer 1, place your ships:" << endl;
			cout << "Enable auto placement? (1-Yes, 0-No): ";
			cin >> autoPlace;
			_player1->placeShips(autoPlace);

			cout << "\nPlayer 2, place your ships:" << endl;
			cout << "Enable auto placement? (1-Yes, 0-No): ";
			cin >> autoPlace;
			_player2->placeShips(autoPlace);
		}

		Player* current = _player1;
		Player* opponent = _player2;

		while (true)
		{
			displayDualBoards();
			if (_isComputerVsComputer) {
				cout << "Computer " << (current == _player1 ? "1" : "2") << "'S TURN: " << endl;
			}
			else if (_isAgainstComputer) {
				cout << "\n" << (current == _player1 ? "YOUR TURN" : "COMPUTER'S TURN") << "\n";
			}
			else {
				cout << "\nPLAYER " << (current == _player1 ? "1" : "2") << "'S TURN\n";
			}

			Point attack = current->selectAttack();
			bool hit = opponent->getBoard().attack(attack);
			current->processAttackResult(attack, hit);

			if (opponent->getBoard().allShipsSunk()) {
				displayDualBoards();
				if (_isComputerVsComputer) {
					typeText("\nCOMPUTER " + string(current == _player1 ? "1" : "2") + " WINS!\n", 14);
				}
				else if (_isAgainstComputer) {
					typeText(current == _player1 ? "\nYOU WIN!\n" : "\nCOMPUTER WINS!\n",
						current == _player1 ? 10 : 12);
				}
				else {
					typeText("\nPLAYER " + string(current == _player1 ? "1" : "2") + " WINS!\n", 14);
				}
				break;
			}

			if (!hit) {
				swap(current, opponent);
				if ((_isAgainstComputer || _isComputerVsComputer) && current == _player2) {
					cout << "Computer is thinking...";
					Sleep(1500);
				}
			}

		}

		typeText("\nGame Over! Thanks for playing!\n", 14);

	}


};


