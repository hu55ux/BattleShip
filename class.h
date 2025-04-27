#pragma once


class Point {
private:
	int _x, _y;

public:
	Point() : _x(0), _y(0) {}
	Point(int x, int y) : _x(x), _y(y) {}

	bool operator==(const Point& other) const {
		return this->_x == other._x && this->_y == other._y;
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

	bool placeShip(const Ship& ship) {
		if (!canPlaceShip(ship)) return false;
		for (const Point& p : ship.getOccupiedPoints()) {
			_board[p.getX()][p.getY()] = 'S'; // Mark the ship's coordinates
		}
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

	void display(bool hideShips = true) const {
		cout << "  ";
		for (int col = 0; col < _size; ++col) {
			cout << col << " ";
		}
		cout << endl;

		for (int row = 0; row < _size; ++row) {
			cout << row << " ";
			for (int col = 0; col < _size; ++col) {
				char cell = _board[row][col];
				if (hideShips && cell == 'S') {
					cout << "# ";
				}
				else {
					cout << cell << " ";
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

	Board& getBoard() { return board; }
};

class HumanPlayer : public Player {
private:
	map<int, int> _shipsLeft;

public:
	HumanPlayer(Board& board) : Player(board) {
		_shipsLeft[4] = 1;
		_shipsLeft[3] = 2;
		_shipsLeft[2] = 3;
		_shipsLeft[1] = 4;
	}

	void placeShips(bool autoPlace = false) override {
		if (autoPlace) {
			cout << "Auto placement selected." << endl;
			while (!_shipsLeft.empty()) {
				int length = getRandomShipLength();
				if (_shipsLeft.find(length) == _shipsLeft.end() || _shipsLeft[length] == 0) {
					continue;
				}
				int x = rand() % 10;
				int y = rand() % 10;
				bool horizontal = rand() % 2;
				Ship ship(Point(x, y), horizontal, length);

				if (board.placeShip(ship)) {
					_shipsLeft[length]--;
					board.display();
				}
			}
			cout << "Your ships are placed automatically!" << endl;
		}
		else {
			while (!_shipsLeft.empty()) {
				int length = getRandomShipLength();
				if (_shipsLeft.find(length) == _shipsLeft.end() || _shipsLeft[length] == 0) {
					continue;
				}

				cout << "Enter the coordinates for a ship of size " << length << endl;
				cout << "Enter starting X (0-9): ";
				int x;
				cin >> x;
				cout << "Enter starting Y (0-9): ";
				int y;
				cin >> y;

				cout << "Enter direction (0 for horizontal, 1 for vertical): ";
				int direction;
				cin >> direction;
				bool horizontal = (direction == 0);

				Ship ship(Point(x, y), horizontal, length);
				if (board.placeShip(ship)) {
					_shipsLeft[length]--;
					board.display();
				}
				else {
					cout << "Invalid position! Please try again." << endl;
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
		int x, y;
		cout << "Enter attack coordinates (x y): ";
		cin >> x >> y;
		return Point(x, y);
	}

	void processAttackResult(const Point& p, bool hit) override {}
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

public:
	ComputerPlayer(Board& board) : Player(board) {
		_shipsLeft[4] = 1;
		_shipsLeft[3] = 2;
		_shipsLeft[2] = 3;
		_shipsLeft[1] = 4;

		_attacked.resize(board.getBoardSize(), vector<bool>(board.getBoardSize(), false));
	}

	void placeShips(bool autoPlace = false) override {
		cout << "Computer is placing ships automatically..." << endl;
		while (!_shipsLeft.empty()) {
			int length = getRandomShipLength();
			if (_shipsLeft.find(length) == _shipsLeft.end() || _shipsLeft[length] == 0) {
				continue;
			}

			int x = rand() % board.getBoardSize();
			int y = rand() % board.getBoardSize();
			bool horizontal = rand() % 2;

			Ship ship(Point(x, y), horizontal, length);

			if (board.placeShip(ship)) {
				_shipsLeft[length]--;
				board.display();
			}
		}

		cout << "Computer's ships placed!" << endl;
	}

	void takeTurn() override {
		Point attackPoint = selectAttack();

		if (board.attack(attackPoint)) {
			cout << "Computer hit at (" << attackPoint.getX() << ", " << attackPoint.getY() << ")" << endl;
		}
		else {
			cout << "Computer missed at (" << attackPoint.getX() << ", " << attackPoint.getY() << ")" << endl;
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
