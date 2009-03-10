// MeshPosition.h: interface for the CMeshPosition class.
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.

#if !defined MESH_POSITION_HEADER_INCLUDED
#define MESH_POSITION_HEADER_INCLUDED

#pragma once

#define MESH_POSITION_UNIT 0.000001
#define ONE_OVER_MESH_POSITION_UNIT 1000000

class CMeshPosition{
private:
	// multiples of MESH_POSITION_UNIT mm
	int m_x[3];

public:
	CMeshPosition(){m_x[0] = 0; m_x[1] = 0; m_x[2] = 0;}
	CMeshPosition(const CMeshPosition& p);
	CMeshPosition(const Point& v);
	CMeshPosition(int x, int y, int z);

	const CMeshPosition& operator=(const CMeshPosition& p);

	bool operator<(const CMeshPosition& p)const;// for inserting in sets
	bool operator>(const CMeshPosition& p)const;// for inserting in sets
	bool operator==(const CMeshPosition& p)const;// for inserting in sets

	Point vertex()const;
	void set_vertex(const Point& v);
	const int* GetPtr()const{return m_x;}
};

#endif
