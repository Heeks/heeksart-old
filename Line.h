// Line.h

#if !defined LINE_HEADER_INCLUDED
#define LINE_HEADER_INCLUDED

#include "Point.h"

class Line
{
public:
	Point p0;
	Point p1;

	Line(){}
	Line(const Point& P0, const Point& P1):p0(P0), p1(P1){}
};

#endif