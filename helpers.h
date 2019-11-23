#ifndef HELPERS_H
#define HELPERS_H
#include <vector>
using namespace std;

struct Point{
    double x, y;
};

stack<Point> convexHull(vector<Point> points, int n);

#endif