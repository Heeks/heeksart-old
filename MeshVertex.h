// MeshVertex.h: interface for the CMeshVertex class.
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.

#if !defined MESH_VERTEX_HEADER_INCLUDED
#define MESH_VERTEX_HEADER_INCLUDED

#pragma once

#include "MeshChild.h"
#include "MeshPosition.h"

class CMeshEdge;
class CMeshFace;

class CMeshVertex:public CMeshChild
{
private:

public:
	CMeshPosition m_position;
	Point m_norm;

	// edge direction showing stuff
	void MakeSureDisplayListExists();

	std::set<CMeshEdge*> m_edges;
	std::set<CMeshFace*> m_faces;

	CMeshVertex(const Point& v);
	CMeshVertex(const CMeshVertex& v);
	virtual ~CMeshVertex();

	const CMeshVertex& operator=(const CMeshVertex& v);

	// HeeksObj's virtual functions
	int GetType()const{return MeshVertexType;}
	const wxChar* GetTypeString(void)const{return _("Mesh Vertex");}
	HeeksObj *MakeACopy(void)const;
	const wxBitmap &GetIcon();
	void GetTools(std::list<Tool*>* t_list, const wxPoint* p);
	void GetGripperPositions(std::list<GripData> *list, bool just_for_endof);
	bool Stretch(const double *p, const double* shift, void* data);
	void DrawGripperSelectItems();

	// member functions
	Point vertex()const{return m_position.vertex();}
	void set_vertex(const Point& v){m_position.set_vertex(v);}
	Point get_average_direction(const std::set<CMeshEdge*> &edges_to_use)const;
	Point get_best_normal_from_edges()const;
	void NormalizeAllEdgeDirections();
};

#endif
