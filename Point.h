// Point.h
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.

#pragma once

#include <cmath>

// combines 3D vertex and vector

class Point{
public:
	double x, y, z;

	Point():x(0.0), y(0.0){}
	Point(double X, double Y, double Z):x(X), y(Y), z(Z){}
	Point(const Point& p){operator=(p);}
	Point(const Point& p1, const Point& p2){operator=(p2 - p1);}
	Point(const double* p):x(p[0]), y(p[1]), z(p[2]){}

	const Point operator+(const Point& p)const{return Point(x + p.x, y + p.y, z + p.z);}
	const Point operator-(const Point& p)const{return Point(x - p.x, y - p.y, z - p.z);}
	const Point operator*(double d)const{return Point(x * d, y * d, z * d);}
	const Point operator/(double d)const{return Point(x / d, y / d, z / d);}
	const Point operator^(const Point &p)const{return Point(y * p.z - z * p.y, z * p.x - x * p.z, x * p.y - y * p.x);}// cross product
	const Point operator-()const{return Point(-x, -y, -z);}
	const double operator*(const Point &p)const{return (x * p.x + y * p.y + z * p.z);}// dot product
	const Point& operator=(const Point& p){x = p.x; y = p.y; z = p.z; return *this;}
	bool operator==(const Point& p)const{return dist(p)<0.001;}
	double dist(const Point &p)const{double dx = p.x - x; double dy = p.y - y; double dz = p.z - z; return sqrt(dx*dx + dy*dy + dz*dz);}
	void extract(double* e)const{e[0] = x; e[1] = y; e[2] = z;}
	const double* GetPtr()const{return &x;}
	double magn()const{return sqrt(x*x + y*y + z*z);}
	const Point norm()const{double m = magn(); if(m < 0.0000000000001)return Point(0, 0, 0); return Point(*this)/m;}
};

