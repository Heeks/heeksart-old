// Matrix.h
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.

#if !defined MATRIX_HEADER_INCLUDED
#define MATRIX_HEADER_INCLUDED

class Matrix{
public:
	double x[16];

	Matrix();
	Matrix(const double* m);
	Matrix(const Matrix& m);

	Matrix operator*(const Matrix& m)const;
	Point operator*(const Point& p)const;
};

#endif