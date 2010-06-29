// MeshFace.h: interface for the CMeshFace class.
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.

#if !defined MESH_FACE_HEADER_INCLUDED
#define MESH_FACE_HEADER_INCLUDED

#pragma once

#include "MeshChild.h"
#include "MeshPosition.h"

class CMeshVertex;
class CMeshEdge;
class CMesh;

class CMeshFace:public CMeshChild
{
private:
	void split_bezier_triangle(int level, int edge_to_split, const Point& vt0, const Point& vt1, const Point& vt2, const Point& c, const Point& vt01, const Point& vt10, const Point& vt12, const Point& vt21, const Point& vt20, const Point& vt02, void(*call_back)(const Point& vt0, const Point& vt1, const Point& vt2, const Point& n0, const Point& n1, const Point& n2));

public:
	CMeshVertex* m_v[3];
	CMeshEdge* m_e[3];
	bool m_edge_dir[3];
	CMeshPosition m_centre;

	CMeshFace();
	virtual ~CMeshFace();

	// HeeksObj's virtual functions
	int GetType()const{return MeshFaceType;}
	const wxChar* GetTypeString(void)const{return _("Mesh Face");}
	HeeksObj *MakeACopy(void)const;
	const wxBitmap &GetIcon();
	void GetGripperPositions(std::list<GripData> *list, bool just_for_endof);
	void GetTools(std::list<Tool*>* t_list, const wxPoint* p);
	bool Stretch(const double *p, const double* shift, void* data);
	void DrawGripperSelectItems();

	// member functions
	void MakeSureDisplayListExists();
	void GetSmoothTriangles(void(*call_back)(const Point& vt0, const Point& vt1, const Point& vt2, const Point& n0, const Point& n1, const Point& n2));
	CMeshVertex* SplitOnGivenEdge(CMeshEdge* edge);
	void RemoveGivenEdge(CMeshEdge* edge);
	//Point normal();
	Point simple_3_pts_normal();
	Point midpoints3normal();
	double angle_at_vertex(const CMeshVertex* v)const;
	Point direction_at_vertex(const CMeshVertex* v)const;
	double longest_length();
	bool is_connected_to_edge(const CMeshEdge* edge)const;
	bool RecalculateAGoodCentre();
};

#endif
