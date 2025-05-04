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