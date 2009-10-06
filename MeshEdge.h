// MeshEdge.h: interface for the CMeshEdge class.
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.

#if !defined MESH_EDGE_HEADER_INCLUDED
#define MESH_EDGE_HEADER_INCLUDED

#pragma once

#include "MeshChild.h"
#include "MeshPosition.h"

class CMeshVertex;
class CMeshFace;
class CMesh;

class CMeshEdge:public CMeshChild
{
private:
	void split_bezier_line(int level, const Point& vt0, const Point& vt2, const Point& vt02, const Point& vt20, void(*call_back)(const Point& vt0, const Point& vt1));

public:
	CMeshVertex* m_v[2];
	CMeshPosition m_c[2]; // control points
	CMeshFace* m_f[2];

	CMeshEdge(CMeshVertex* v0, CMeshVertex* v1, const CMeshPosition& c0, const CMeshPosition& c1, CMesh* owner);
	virtual ~CMeshEdge();

	// HeeksObj's virtual functions
	int GetType()const{return MeshEdgeType;}
	const wxChar* GetTypeString(void)const{return _("Mesh Edge");}
	HeeksObj *MakeACopy(void)const;
	wxString GetIcon(){return _T("meshedge");}
	void GetTools(std::list<Tool*>* t_list, const wxPoint* p);
	void GetGripperPositions(std::list<GripData> *list, bool just_for_endof);
	bool Stretch(const double *p, const double* shift, void* data);
	bool StretchTemporary(const double *p, const double* shift, void* data);

	// member functions
	void MakeSureDisplayListExists();
	void GetSmoothLines(void(*call_back)(const Point& vt0, const Point& vt1));
	void InvalidateBothFacesDisplayLists();
	void Split();
	CMeshPosition& GetControlPointNearVertex(CMeshVertex* v);
	CMeshPosition& GetControlPointAwayFromVertex(CMeshVertex* v);
	const Point& GetMidPoint();
	Point get_direction_at_vertex(const CMeshVertex* v);
	void set_direction_at_vertex(const CMeshVertex* vt, const Point& v);
	Point get_av_face_pair_normal();
	void DrawGripperSelectItems();

	// old function using old smoothing code
	static Point get_point_on_smooth_edge(double param, const Point& xa, const Point& xb, const Point& va, const Point& vb, double vfactor, Point* dir);

	static Point evaluate_bezier_curve(double param, const Point& p0, const Point& p1, const Point& p2, const Point& p3, Point* dir);
};


#endif
