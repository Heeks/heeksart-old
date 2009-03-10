// MeshPosition.cpp
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.

#include "stdafx.h"
#include "MeshPosition.h"

CMeshPosition::CMeshPosition(const CMeshPosition& p){
	operator=(p);
}

CMeshPosition::CMeshPosition(const Point& v){
	set_vertex(v);
}

CMeshPosition::CMeshPosition(int x, int y, int z){
	m_x[0] = x;
	m_x[1] = y;
	m_x[2] = z;
}

const CMeshPosition& CMeshPosition::operator=(const CMeshPosition& p){
	for(int i = 0; i<3; i++){
		m_x[i] = p.m_x[i];
	}

	return *this;
}

bool CMeshPosition::operator<(const CMeshPosition& p)const{
	for(int i = 0; i<3; i++){
		if(m_x[i]<p.m_x[i])return true;
		if(m_x[i]>p.m_x[i])return false;
	}

	return false;
}

bool CMeshPosition::operator>(const CMeshPosition& p)const{
	for(int i = 0; i<3; i++){
		if(m_x[i]>p.m_x[i])return true;
		if(m_x[i]<p.m_x[i])return false;
	}

	return false;
}

bool CMeshPosition::operator==(const CMeshPosition& p)const{
	for(int i = 0; i<3; i++){
		if(m_x[i]!=p.m_x[i])return false;
	}

	return true;
}

Point CMeshPosition::vertex()const{
	return Point(((double)m_x[0]) * MESH_POSITION_UNIT,
		((double)m_x[1]) * MESH_POSITION_UNIT,
		((double)m_x[2]) * MESH_POSITION_UNIT);
}

void CMeshPosition::set_vertex(const Point& v){
	for(int i = 0; i<3; i++){
		double x = v.GetPtr()[i] * ONE_OVER_MESH_POSITION_UNIT;
		if(x<0)x -= 0.5;
		else x += 0.5;
		m_x[i] = (int)x;
	}
}

