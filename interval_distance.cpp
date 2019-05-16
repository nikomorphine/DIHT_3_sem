#include <cmath>
#include <iostream>
#include <iomanip>
#include <vector>
//structure to store infomation about singkle point
//small number to avoid later division by 0
const double SMALL_NUM = 10e-10;

struct Point {
    Point() {
        x = 0;
        y = 0;
        z = 0;
    }
    Point(double x_, double y_, double z_) : x(x_), y(y_), z(z_) {};
    double x, y, z;
    
    Point operator - (const Point& newPoint) {
        Point vector;
        vector.x = this->x - newPoint.x;
        vector.y = this->y - newPoint.y;
        vector.z = this->z - newPoint.z;
        return vector;
    }
    
    Point operator + (const Point& newPoint) {
        Point vector;
        vector.x = this->x + newPoint.x;
        vector.y = this->y + newPoint.y;
        vector.z = this->z + newPoint.z;
        return vector;
    }
    
    double operator * (Point newPoint) {
        double dot = 0;
        dot = this->x * newPoint.x + this->y * newPoint.y + this->z * newPoint.z;
        return dot;
    }
    
    Point operator * (double mul) {
        Point newPoint;
        newPoint.x = this->x * mul;
        newPoint.y = this->y * mul;
        newPoint.z = this->z * mul;
        return newPoint;
    }
    
    double  norm() {
        double norm = 0;
        norm += this->x * this->x;
        norm += this->y * this->y;
        norm += this->z * this->z;
        return sqrt(norm);
    }
};
//structure to store two points, which refer to one segment, and alse to stor distance betwwen those two
//can be used to calculate distance betwwen two point without storing it
struct Segment {
    Segment() {
        distance = 0;
    }
    //calculating distance between two points
    Segment(Point p1_, Point p2_) : p1(p1_), p2(p2_) {
        distance = sqrt(pow((p1_.x - p2_.x), 2) + pow((p1_.y - p2_.y), 2) + pow((p1_.z - p2_.z), 2));
    }
    Point p1, p2;
    double distance;
};
//calculating minimum distance between two segments, as a minimum of parametrised function
double calcMinDist(Segment segment1, Segment segment2) {
    Point u = segment1.p2 - segment1.p1;
    Point v = segment2.p2 - segment2.p1;
    Point w = segment1.p1 - segment2.p1;
    double a = u * u;         // always >= 0
    double b = u * v;
    double c = v * v;         // always >= 0
    double d = u * w;
    double e = v * w;
    double D = a * c - b * b;        // always >= 0
    double sc, sN, sD = D;       // sc = sN / sD, default sD = D >= 0
    double tc, tN, tD = D;       // tc = tN / tD, default tD = D >= 0
    
    // compute the line parameters of the two closest points
    if (D <= SMALL_NUM) { // the lines are almost parallel
        sN = 0.0;         // force using point P0 on segment S1
        sD = 1.0;         // to prevent possible division by 0.0 later
        tN = e;
        tD = c;
    }
    else {                 // get the closest points on the infinite lines
        sN = (b * e - c * d);
        tN = (a * e - b * d);
        if (sN <= 0.0) {        // sc < 0 => the s=0 edge is visible
            sN = 0.0;
            tN = e;
            tD = c;
        }
        else if (sN >= sD) {  // sc > 1  => the s=1 edge is visible
            sN = sD;
            tN = e + b;
            tD = c;
        }
    }
    
    if (tN <= 0.0) {            // tc < 0 => the t=0 edge is visible
        tN = 0.0;
        // recompute sc for this edge
        if (-d <= 0.0)
            sN = 0.0;
        else if (-d >= a)
            sN = sD;
        else {
            sN = -d;
            sD = a;
        }
    }
    else if (tN >= tD) {      // tc > 1  => the t=1 edge is visible
        tN = tD;
        // recompute sc for this edge
        if ((-d + b) < 0.0)
            sN = 0;
        else if ((-d + b) > a)
            sN = sD;
        else {
            sN = (-d +  b);
            sD = a;
        }
    }
    // finally do the division to get sc and tc
    sc = (abs(sN) <= SMALL_NUM ? 0.0 : sN / sD);
    tc = (abs(tN) <= SMALL_NUM ? 0.0 : tN / tD);
    
    // get the difference of the two closest points
    Point dP = w + u * sc - v * tc;  // =  S1(sc) - S2(tc)
    
    return dP.norm();
}

int main() {
    //creating first segment
    double x, y, z;
    std::cin >> x >> y >> z;
    Point p1(x, y, z);
    std::cin >> x >> y >> z;
    Point p2(x, y, z);
    Segment s1(p1, p2);
    //creating second segment
    std::cin >> x >> y >> z;
    Point p3(x, y, z);
    std::cin >> x >> y >> z;
    Point p4(x, y, z);
    Segment s2(p3, p4);
    
    double minDistance;
    minDistance = calcMinDist(s1, s2);
    std::cout << std::setprecision(10) << std::fixed;
    std::cout << minDistance;
    
    return 0;
}
