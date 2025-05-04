#pragma once

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