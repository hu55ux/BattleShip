#pragma once



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