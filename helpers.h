#ifndef HELPERS_H
#define HELPERS_H
#include <vector>
using namespace std;

struct Point{
    int index;
    double x, y;
};

std::vector<stack<Point>> convexHull(vector<Point> points, int n);
stack<Point> convexHull_recursive(vector<Point> points, int n);

#endif