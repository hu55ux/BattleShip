#pragma once

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
			if (_board[p.getY()][p.getX()] != '#') return false;

			// Ətraf hüceyrələri yoxla
			for (int dx = -1; dx <= 1; ++dx) {
				for (int dy = -1; dy <= 1; ++dy) {
					int nx = p.getX() + dx;
					int ny = p.getY() + dy;
					if (isValid(Point(nx, ny)) && _board[ny][nx] != '#') {
						return false;
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
			_board[p.getY()][p.getX()] = 'S';
		}
		_ships.push_back(ship);
		return true;
	}

	bool placeShip(Point start, int length, bool horizontal) {
		if (!canPlaceShip(start, length, horizontal)) return false;

		vector<Point> occupiedPoints;
		for (int i = 0; i < length; ++i) {
			int x = start.getX() + (horizontal ? i : 0);
			int y = start.getY() + (horizontal ? 0 : i);
			_board[y][x] = 'S';
			occupiedPoints.emplace_back(x, y);
		}

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