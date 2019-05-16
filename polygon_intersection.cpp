#include <cmath>
#include <iostream>
#include <math.h>
#include <vector>

#define EPS 10e-7
//Point structure is equivalent to 2D vector with the same binary operations
struct Point {
    Point(double x_ = 0, double y_ = 0) : x(x_), y(y_) {}
    
    Point operator-(const Point& newPoint) {
        Point vector;
        vector.x = this->x - newPoint.x;
        vector.y = this->y - newPoint.y;
        return vector;
    }
    
    Point operator+(const Point& newPoint) {
        Point vector;
        vector.x = this->x + newPoint.x;
        vector.y = this->y + newPoint.y;
        return vector;
    }
    
    bool operator==(const Point& newPoint) {
        if (x == newPoint.x && y == newPoint.y) {
            return true;
        }
        return false;
    }
    double x, y;
};
//======================================================================================
double angle(const Point& point);
struct Polygon {
    Polygon() {
        numOfVert = 0;
    }
    Polygon(Point p1, Point p2, Point p3) {
        numOfVert = 3;
        vertices.push_back(p1);
        vertices.push_back(p2);
        vertices.push_back(p3);
        calcArea();
    }
    //creating new polygon with std::cin
    Polygon(const int& n, bool inverted) : numOfVert(n) {
        if (not inverted) { //flag inverted states for
            //inverted polygon relative to (0, 0)
            Point temp;
            std::cin >> temp.x >> temp.y;
            Point minY = temp;
            for (int i = 0; i < n - 1 ; ++i) {
                if (temp.y < minY.y or (temp.y == minY.y and temp.x <= minY.x)) {
                    minY = temp;
                    begin = i;
                }
                this->addPoint(temp);
                std::cin >> temp.x >> temp.y;
            }
            if (temp.y < minY.y or (temp.y == minY.y and temp.x <= minY.x)) {
                begin = n - 1;
            }
            this->addPoint(temp);
            reverse();
        } else {
            Point temp;
            std::cin >> temp.x >> temp.y;
            Point maxY = temp;
            for (int i = 0; i < n - 1 ; ++i) {
                if (temp.y > maxY.y or (temp.y == maxY.y and temp.x >= maxY.x)) {
                    maxY = temp;
                    begin = i;
                }
                this->addPoint(Point() - temp);
                std::cin >> temp.x >> temp.y;
            }
            if (temp.y > maxY.y or (temp.y == maxY.y and temp.x >= maxY.x)) {
                begin = n - 1;
            }
            this->addPoint(Point() - temp);
            reverse();
        }
        calcArea();
    }
    //creating polygon with given vector of points
    Polygon(std::vector<Point> points) {
        vertices = points;
        numOfVert = (int) points.size();
        calcArea();
    }
    
    void addPoint(Point newPoint) {
        vertices.push_back(newPoint);
        this->numOfVert = (int) vertices.size();
    }
    
    void calcArea() {
        int j = numOfVert - 1;
        for (int i = 0; i < numOfVert; i++) {
            area += (this->vertices[j].x + this->vertices[i].x) *
            (this->vertices[j].y - this->vertices[i].y);
            j = i;
        }
        area = abs(area / 2.0);
        calcAngles();
    }
    
    void calcAngles() {
        for (int i = 0; i < numOfVert; ++i) {
            angles.push_back(angle(vertices[(i + 1) % numOfVert] - vertices[i]));
        }
    }
    
    void reverse() {
        std::vector<Point> newVertices;
        for (int i = 0; i < numOfVert; ++i) {
            newVertices.push_back(vertices[(i + begin) % numOfVert]);
        }
        vertices = newVertices;
        begin = 0;
    }
    
    bool operator==(Polygon newPolygon) {
        if (numOfVert != newPolygon.numOfVert) {
            return false;
        } else {
            for (int i = 0; i < numOfVert; ++i) {
                if (not (vertices[i] == newPolygon.vertices[i])) {
                    return false;
                }
            }
        }
        return true;
    }
    
    std::vector<Point> vertices;
    std::vector<double> angles;
    int numOfVert;
    double area = 0;
    int begin = 0;
};
//======================================================================================
//function of angle calculation
double angle(const Point& point) {
    if (point.x == 0) {
        if (point.y > 0) {
            return 90;
        } else {
            return -90;
        }
    }
    double angle = point.y / point.x;
    angle = atan(angle) * 180 / M_PI;
    if (point.x < 0) {
        if (point.y > 0) {
            angle += 180;
        } else if (point.y <= 0) {
            angle -= 180;
        }
    }
    return angle;
}
//function to calculate Minkowski sum of two polygons
Polygon minSum(Polygon p1, Polygon p2) {
    Polygon minkowskiSum;
    int i = 0, j = 0;
    while (i < p1.numOfVert and j < p2.numOfVert) {
        minkowskiSum.addPoint(p1.vertices[i % p1.numOfVert] + p2.vertices[j % p2.numOfVert]);
        if (p1.angles[i % p1.numOfVert] > p2.angles[j % p2.numOfVert]) {
            ++i;
        } else if (p1.angles[i % p1.numOfVert] < p2.angles[j % p2.numOfVert]) {
            ++j;
        } else {
            ++i;
            ++j;
        }
    }
    //to proceed remaining vertices
    if (j == p2.numOfVert) {
        while (i <= p1.numOfVert) {
            minkowskiSum.addPoint(p1.vertices[i % p1.numOfVert] + p2.vertices[j % p2.numOfVert]);
            i++;
        }
    } else {
        while (j <= p2.numOfVert) {
            minkowskiSum.addPoint(p1.vertices[i % p1.numOfVert] + p2.vertices[j % p2.numOfVert]);
            j++;
        }
    }
    return minkowskiSum;
}

void checkZeroZero(const Polygon& polygon) {
    double area = 0;
    Point zero(0, 0);
    for (int i = 0; i < polygon.numOfVert; ++i) {
        Polygon temp(zero, polygon.vertices[i], polygon.vertices[(i + 1) % polygon.numOfVert]);
        area += temp.area;
    }
    if (abs(area - polygon.area) < EPS) {
        std::cout << "YES" << std::endl;
    } else {
        std::cout << "NO" << std::endl;
    }
};

int main() {
    int n, m;
    std::cin >> n;
    Polygon p1(n, false);
    std::cin >> m;
    Polygon p2(m, true);
    Polygon minkowskiSum = minSum(p1, p2);
    minkowskiSum.calcArea();
    checkZeroZero(minkowskiSum);
    
    return 0;
}
