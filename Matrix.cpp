// Matrix.cpp
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.

#include "stdafx.h"
#include "Matrix.h"

Matrix::Matrix()
{
	x[0] = 1.0;
	x[1] = 0.0;
	x[2] = 0.0;
	x[3] = 0.0;
	x[4] = 0.0;
	x[5] = 1.0;
	x[6] = 0.0;
	x[7] = 0.0;
	x[8] = 0.0;
	x[9] = 0.0;
	x[10] = 1.0;
	x[11] = 0.0;
	x[12] = 0.0;
	x[13] = 0.0;
	x[14] = 0.0;
	x[15] = 1.0;
}

Matrix::Matrix(const Matrix& m)
{
	memcpy(x, m.x, sizeof(x));
}

Matrix::Matrix(const double *m)
{
	memcpy(x, m, sizeof(x));
}

Matrix Matrix::operator*(const Matrix& m)const
{
	int i, k, l;
	Matrix n;

	for (i = 0; i < 16; i++)
	{
		l = i - (k = (i % 4));
		n.x[i] =  m.x[l] * x[k] + m.x[l+1] * x[k+4] + m.x[l+2] * x[k+8] + m.x[l+3] * x[k+12];
	}

	return n;
}

Point Matrix::operator*(const Point& p)const
{
	Point n;
	n.x = p.x * x[0] + p.y * x[1] + p.z * x[2]  + x[3];
	n.y = p.x * x[4] + p.y * x[5] + p.z * x[6]  + x[7];
	n.z = p.x * x[8] + p.y * x[9] + p.z * x[10] + x[11];
	return n;
}
