#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <set>
#include <unordered_set>
#include <queue>
#include <vector>

#define INF 501;
//Point structure=========================================================================================================
struct Point {
    //constructor for a Point 501 as a maximum value
    Point() {
        x = INF;
        y = INF;
        z = INF;
        index = -1;
    }
    //constructor for point with the given set of coordinates
    Point(double x_, double y_, double z_, int index_) : x(x_), y(y_), z(z_), index(index_) {}
    //operators for point
    Point operator-(const Point& newPoint) {
        Point vector;
        vector.x = x - newPoint.x;
        vector.y = y - newPoint.y;
        vector.z = z - newPoint.z;
        return vector;
    }
    
    Point operator+(const Point& newPoint) {
        Point vector;
        vector.x = x + newPoint.x;
        vector.y = y + newPoint.y;
        vector.z = z + newPoint.z;
        return vector;
    }
    
    Point operator*(const double& mult) {
        Point vector;
        vector.x = x * mult;
        vector.y = y * mult;
        vector.z = z * mult;
        return vector;
    }
    
    bool operator==(const Point& newPoint) const{
        return (x == newPoint.x &&
               y == newPoint.y &&
               z == newPoint.z);
    }
    
    double operator,(const Point& newPoint) {
        return (x * newPoint.x + y * newPoint.y + z * newPoint.z);
    }
    
    Point vectorMult(const Point& newPoint) {
        Point mult;
        mult.x = y * newPoint.z - z * newPoint.y;
        mult.y = z * newPoint.x - x * newPoint.z;
        mult.z = x * newPoint.y - y * newPoint.x;
        return mult;
    }
    
    double module() const {
        return sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
    }
    
    double x, y, z;
    int index;
};
//face structure=======================================================================================================
struct Face {
    Face() {}
    
    Face(const Point& p1, const Point& p2, const Point& p3) : i1(p1.index), i2(p2.index), i3(p3.index) {}
    
    Point n;
    int i1, i2, i3;
};
//function for face orientation========================================================================================
void orientation(Face& face) {
    int minVal = std::min(face.i1, std::min(face.i2, face.i3));
    while (face.i1 != minVal) {
        int temp = face.i1;
        face.i1 = face.i2;
        face.i2 = face.i3;
        face.i3 = temp;
    }
}
//hash for unordered set of pairs======================================================================================
struct pairHash {
    inline std::size_t operator() (const std::pair<int, int> & v) const {
        return v.first * 31 + v.second;
    }
};
//comparator for face==================================================================================================
struct comparator {
    inline bool operator() (const Face newFace1, const Face newFace2) const {
        if (newFace1.i1 == newFace2.i1) {
            if (newFace1.i2 == newFace2.i2) {
                return newFace1.i3 < newFace2.i3;
            }
            return newFace1.i2 < newFace2.i2;
        }
        return newFace1.i1 < newFace2.i1;
    }
};
class ConvexHull {
public:
    ConvexHull(int numOfPoints_) : numOfPoints(numOfPoints_) {
        inputPointSet();
        buildConvexHull();
    }
    void print() {
        std::cout << convexHull.size() << std::endl;
        for (auto i : convexHull) {
            printFace(i);
        }
    }
private:
    //input point set==================================================================================================
    void inputPointSet() {
        for (int j = 0; j < numOfPoints; ++j) {
            Point temp;
            std::cin >> temp.x >> temp.y >> temp.z;
            temp.index = j;
            pointSet.push_back(temp);
        }
    }
    //function to find the first point of the first plane==================================================================
    Point findFirst() {
        Point firstPoint;
        for (auto i : pointSet) {
            if (i.z < firstPoint.z) {
                firstPoint = i;
            } else if (i.z == firstPoint.z and i.y < firstPoint.y) {
                firstPoint = i;
            } else if (i.z == firstPoint.z and i.y == firstPoint.y and i.x < firstPoint.x) {
                firstPoint = i;
            }
        }
        return firstPoint;
    }
    //function to find the second point of the first plane=================================================================
    Point findSecond(const Point& firstPoint) {
        Point secondPoint;
        Point normal(0, 0, 1, -1);
        double mult = 1;
        for (auto i : pointSet) {
            if (not (i == firstPoint)) {
                Point temp = i - firstPoint;
                if ((normal, temp) < mult * temp.module()) {
                    secondPoint = i;
                    mult = (normal, temp) / temp.module();
                }
            }
        }
        return secondPoint;
    }
    //function to find the third point of the first plane==================================================================
    Point findThird(Point& firstPoint, Point& secondPoint) {
        Point thirdPoint;
        Point normal(0, 0, 1, -1);
        double mult = 0;
        for (auto i : pointSet) {
            if (not (i == firstPoint or i == secondPoint)) {
                Point temp1 = secondPoint - firstPoint;
                Point temp2 = i - firstPoint;
                Point faceNormal = temp1.vectorMult(temp2);
                if (fabs((faceNormal, normal)) > mult * faceNormal.module()) {
                    thirdPoint = i;
                    mult = fabs((faceNormal, normal) / faceNormal.module());
                }
            }
        }
        return thirdPoint;
    }
    //function to find the first face======================================================================================
    Face firstFace() {
        Point firstPoint = findFirst();
        Point secondPoint = findSecond(firstPoint);
        Point thirdPoint = findThird(firstPoint, secondPoint);
        Point vec1 = secondPoint - firstPoint;
        Point vec2 = thirdPoint - firstPoint;
        if (((Point(0, 0, 1, -1).vectorMult(vec1), vec2)) < 0) {
            edgeQueue.push({firstPoint.index, secondPoint.index});
            edgeQueue.push({secondPoint.index, thirdPoint.index});
            edgeQueue.push({thirdPoint.index, firstPoint.index});
            return Face(firstPoint, secondPoint, thirdPoint);
        } else {
            edgeQueue.push({firstPoint.index, thirdPoint.index});
            edgeQueue.push({thirdPoint.index, secondPoint.index});
            edgeQueue.push({secondPoint.index, firstPoint.index});
            return Face(firstPoint, thirdPoint, secondPoint);
        }
    }
    //build convex Hull================================================================================================
    void buildConvexHull() {
        Face first = firstFace();
        orientation(first);
        convexHull.insert(first);
        
        while (!edgeQueue.empty()) {
            Point temp1 = pointSet[edgeQueue.front().first], temp2 = pointSet[edgeQueue.front().second];
            
            if (check.find({edgeQueue.front().first, edgeQueue.front().second}) != check.end()) {
                edgeQueue.pop();
                continue;
            }
            
            Point temp3 = temp2 - temp1;
            check.insert({edgeQueue.front().first, edgeQueue.front().second});
            edgeQueue.pop();
            
            Point nextPoint = pointSet[0];
            for (int i = 0; i < numOfPoints; i++) {
                Point p = pointSet[i];
                if ((temp3.vectorMult(nextPoint - temp1), (p - temp1)) < 0) {
                    nextPoint = p;
                }
            }
            if (nextPoint == temp1 or nextPoint == temp2) {
                continue;
            }
            std::pair<int, int> secondEdge = {temp1.index, nextPoint.index},
            thirdEdge = {nextPoint.index, temp2.index};
            
            Face face = Face(temp2, temp1, nextPoint);
            orientation(face);
            convexHull.insert(face);
            edgeQueue.push(secondEdge);
            edgeQueue.push(thirdEdge);
        }
    }
    //print face=======================================================================================================
    void printFace(const Face& face) {
        std::cout << "3 "  << face.i1
        << " " << face.i2
        << " " << face.i3 << std::endl;
    }
    //set of all points
    std::vector<Point> pointSet;
    int numOfPoints;
    //convex hull for the pointSet
    std::set<Face, comparator> convexHull;
    std::queue<std::pair<int, int>> edgeQueue;
    std::unordered_set<std::pair<int, int>, pairHash> check;
};

//=====================================================================================================================
int main() {
    int numOfTests = 0;
    std::cin >> numOfTests;
    for (int i = 0; i < numOfTests; ++i) {
        int numOfPoints = 0;
        std::cin >> numOfPoints;
        ConvexHull convexHull(numOfPoints);
        convexHull.print();
    }
    return 0;
}
