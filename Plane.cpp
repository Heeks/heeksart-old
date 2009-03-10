// Plane.cpp
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.

#include "stdafx.h"
#include "Plane.h"

bool Plane::Intersect(const Line& l, Point& p)const
{
	Point v(l.p0, l.p1);
	double den = v * n;
	if(fabs(den) < 0.000001)return false;
	double t = -(n * l.p0 + d) / den;
	p = l.p0 + v * t;
	return true;
}

bool Plane::Intersect(const Plane& pl, Line& l)const
{
	Point D = (n ^ pl.n).norm();
	if(D == Point(0.0, 0.0, 0.0))return false;
	double dp = n * pl.n;
	double den = dp * dp - 1.0;
	double a = (d - pl.d * dp) / den;
	double b = (pl.d - d * dp) / den;
	l.p0 = n * a + pl.n * b;
	l.p1 = l.p0 + D;
	return true;
}
