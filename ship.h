#pragma once


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