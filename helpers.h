#ifndef HELPERS_H
#define HELPERS_H
#include <vector>
using namespace std;

struct Point{
    int index;
    double x, y;
};

std::vector<stack<Point>> convexHull(vector<Point> points, int n);
void convexHull_recursive(vector<stack<Point>> &result,vector<Point> &points, int n);

#endif