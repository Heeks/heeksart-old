// MeshVertex.cpp
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.

#include "stdafx.h"
#include "MeshVertex.h"
#include "Mesh.h"
#include "MeshEdge.h"
#include "interface/Tool.h"
#include "interface/GripperTypes.h"

CMeshVertex::CMeshVertex(const Point& v){
	m_position = CMeshPosition(v);
}

CMeshVertex::CMeshVertex(const CMeshVertex& v){
	operator=(v);
}

CMeshVertex::~CMeshVertex(){
}

const CMeshVertex& CMeshVertex::operator=(const CMeshVertex& v){
	m_position = v.m_position;
	m_edges = v.m_edges;

	return *this;
}

HeeksObj *CMeshVertex::MakeACopy(void)const
{
	return new CMeshVertex(*this);
}

class NormalizeAllEdgeDirections:public Tool{
public:
	CMeshVertex* m_vertex;

	// Tool's virtual functions
	const wxChar* GetTitle(){return _("Normalize All Edge Directions");}
	void Run(){m_vertex->NormalizeAllEdgeDirections();}
	wxString BitmapPath(){ return _T("normedge");}
};

static NormalizeAllEdgeDirections normalize_all_edge_directions;

class SetNormalFromEdges:public Tool{
public:
	CMeshVertex* m_vertex;

	// Tool's virtual functions
	const wxChar* GetTitle(){return _("Set Normal From Edges");}
	void Run(){m_vertex->m_norm = m_vertex->get_best_normal_from_edges();}
	wxString BitmapPath(){ return _T("setnorme");}
};

static SetNormalFromEdges set_normal_from_edges;

void CMeshVertex::GetTools(std::list<Tool*>* t_list, const wxPoint* p)
{
	normalize_all_edge_directions.m_vertex = this;
	set_normal_from_edges.m_vertex = this;
	t_list->push_back(&normalize_all_edge_directions);

	CMeshChild::GetTools(t_list, p);
}

Point CMeshVertex::get_best_normal_from_edges()const{
	std::set<CMeshEdge*>::const_iterator It, It2, It3;

	bool best_valid = false;
	double best_dp;
	Point bestv[3];

	for(It = m_edges.begin(); It != m_edges.end(); It++){
		CMeshEdge* edge1 = *It;
		Point v1 = edge1->get_direction_at_vertex(this);
		for(It2 = m_edges.begin(); It2 != m_edges.end(); It2++){
			CMeshEdge* edge2 = *It2;
			if(edge2 == edge1)continue;
			Point v2 = edge2->get_direction_at_vertex(this);
			double dp1 = fabs(v1 * v2);
			for(It3 = m_edges.begin(); It3 != m_edges.end(); It3++){
				CMeshEdge* edge3 = *It3;
				if(edge3 == edge2 || edge3 == edge1)continue;
				Point v3 = edge3->get_direction_at_vertex(this);
				double dp2 = fabs(v1 * v3);
				double dp3 = fabs(v2 * v3);
				double worst_dp = dp1;
				if(dp2 < worst_dp)worst_dp = dp2;
				if(dp3 < worst_dp)worst_dp = dp3;

				if(!best_valid || worst_dp > best_dp){
					best_dp = worst_dp;
					bestv[0] = v1;
					bestv[1] = v2;
					bestv[2] = v3;
					best_valid = true;
				}
			}
		}
	}

	if(best_valid){
		Point norm[3];
		norm[0] = (bestv[0] ^ bestv[1]).norm();
		norm[1] = (bestv[1] ^ bestv[2]).norm();
		norm[2] = (bestv[2] ^ bestv[0]).norm();
		Point total_norm = (norm[0] + norm[1] + norm[2]).norm();
		return total_norm;
	}

	return Point(0, 0, 0);
}

void CMeshVertex::MakeSureDisplayListExists(){
	if(m_gl_list)return;

	m_gl_list = glGenLists(1);

	glNewList(m_gl_list, GL_COMPILE);
	
	HeeksColor(255, 0, 128).glColor();
	glRasterPos3dv(vertex().GetPtr());

	glEndList();
}

Point CMeshVertex::get_average_direction(const std::set<CMeshEdge*> &edges_to_use)const{
	std::set<CMeshEdge*>::const_iterator It;

	Point total_direction(0, 0, 0);
	for(It = edges_to_use.begin(); It != edges_to_use.end(); It++){
		CMeshEdge* edge = *It;
		Point direction = edge->get_direction_at_vertex(this);
		total_direction = total_direction + direction;
	}

	return total_direction.norm();
}

void CMeshVertex::NormalizeAllEdgeDirections(){
	std::set<CMeshEdge*>::iterator It;
	for(It = m_edges.begin(); It != m_edges.end(); It++){
		CMeshEdge* edge = *It;
		Point v = edge->get_direction_at_vertex(this);
		Point cp = (v^m_norm).norm();
		Point new_v = ((m_norm^cp).norm()) * v.magn();
		edge->set_direction_at_vertex(this, new_v);
	}
}

void CMeshVertex::GetGripperPositions(std::list<double> *list, bool just_for_endof){
	// vertex
	Point vt = vertex();
	list->push_back(GripperTypeStretch2);
	list->push_back(vt.x);
	list->push_back(vt.y);
	list->push_back(vt.z);

	for(std::set<CMeshEdge*>::iterator It = m_edges.begin(); It != m_edges.end(); It++)
	{
		CMeshEdge* edge = *It;
		CMeshPosition& p = edge->GetControlPointNearVertex(this);
		vt = p.vertex();
		list->push_back(GripperTypeStretch);
		list->push_back(vt.x);
		list->push_back(vt.y);
		list->push_back(vt.z);
	}
}

bool CMeshVertex::Stretch(const double *p, const double* shift){
	// stretch the vertex at "p" by a vector "shift"
	Point vp(p);
	
	Point vt = vertex();
	if(vp == vt){
		Point vshift(shift);
		Point new_vertex = vp + vshift;
		CMesh* mesh = (CMesh*)(m_owner);
		if(mesh)mesh->ChangeVertex(this, new_vertex); 

		// stretch all the edges nearest control points
		{
			std::set<CMeshEdge*>::iterator It;
			for(It = m_edges.begin(); It != m_edges.end(); It++){
				CMeshEdge* edge = *It;
				CMeshPosition& c = edge->GetControlPointNearVertex(this);
				c.set_vertex(c.vertex() + vshift);
			}
		}

		// stretch the faces centre points
		{
			Point partial_vshift = vshift * 0.5;
			std::set<CMeshFace*>::iterator It;
			for(It = m_faces.begin(); It != m_faces.end(); It++){
				CMeshFace* face = *It;
				face->m_centre.set_vertex(face->m_centre.vertex() + partial_vshift);
			}
		}

		return false;
	}

	for(std::set<CMeshEdge*>::iterator It = m_edges.begin(); It != m_edges.end(); It++)
	{
		CMeshEdge* edge = *It;
		CMeshPosition& p = edge->GetControlPointNearVertex(this);
		vt = p.vertex();
		if(vp == vt){
			Point new_vertex = vp + Point(shift);
			p.set_vertex(new_vertex);
			//edge->KillGLLists();
			//edge->InvalidateBothFacesDisplayLists();
			Point dir = new_vertex - vertex();
			Point side_vector = dir ^ m_norm;
			m_norm = (side_vector ^ dir).norm();
			NormalizeAllEdgeDirections();
			return false;
		}
	}

	return false;
}

void CMeshVertex::DrawGripperSelectItems()
{
	if(heeksCAD->GetBackgroundColor() == HeeksColor(0, 0, 0))glColor3ub(255, 255, 255);
	else glColor3ub(0, 0, 0);

	Point p0 = vertex();

	glBegin(GL_LINES);
	for(std::set<CMeshEdge*>::iterator It = m_edges.begin(); It != m_edges.end(); It++)
	{
		CMeshEdge* edge = *It;
		CMeshPosition& p = edge->GetControlPointNearVertex(this);
		Point p1 = p.vertex();
		glVertex3d(p0.x, p0.y, p0.z);
		glVertex3d(p1.x, p1.y, p1.z);
	}
	glEnd();
}