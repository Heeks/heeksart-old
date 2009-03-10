// Plane.h
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.
#if !defined PLANE_HEADER_INCLUDED
#define PLANE_HEADER_INCLUDED

#include "Point.h"
#include "Line.h"

class Plane {
	public:
		double d;
		Point n;

		Plane():d(0.0){}
		Plane(double D, const Point& N):d(D), n(N){}
		Plane(const Point& N, const Point& p){n = N.norm(); d = -(n * p);}

		bool Intersect(const Line& l, Point& p)const; 
		bool Intersect(const Plane& pl, Line& l)const;
		Point Near(const Point& p)const{return -n * ((n * p) + d) + p;}
};

#endif

