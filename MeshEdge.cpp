// MeshEdge.cpp: implementation of the CMeshEdge class.
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.

#include "stdafx.h"
#include "MeshEdge.h"
#include "MeshVertex.h"
#include "MeshFace.h"
#include "Mesh.h"
#include "interface/Tool.h"
#include "interface/GripperTypes.h"
#include "interface/GripData.h"

CMeshEdge::CMeshEdge(CMeshVertex* v0, CMeshVertex* v1, const CMeshPosition& c0, const CMeshPosition& c1, CMesh* owner)
{
	m_v[0] = v0;
	m_v[1] = v1;
	m_v[0]->m_edges.insert(this);
	m_v[1]->m_edges.insert(this);
	m_c[0] = c0;
	m_c[1] = c1;
	m_f[0] = NULL;
	m_f[1] = NULL;
	SetOwner(owner);
}

CMeshEdge::~CMeshEdge()
{
}

class SplitEdge:public Tool{
public:
	CMeshEdge* m_e;

	// Tool's virtual functions
	const wxChar* GetTitle(){return _("Split Edge");}
	void Run(){m_e->Split();}
	wxString BitmapPath(){ return _T("splitedge");}
};

static SplitEdge split_edge;

void CMeshEdge::GetTools(std::list<Tool*>* t_list, const wxPoint* p)
{
	split_edge.m_e = this;
	t_list->push_back(&split_edge);

	CMeshChild::GetTools(t_list, p);
}

void CMeshEdge::split_bezier_line(int level, const Point& vt0, const Point& vt2, const Point& vt02, const Point& vt20, void(*call_back)(const Point& vt0, const Point& vt1))
{
	if(level > 0)
	{
		// split into two sub lines
		for(int i = 0; i<2; i++){
			Point nvt0 = vt0;
			Point nvt2 = vt2;
			Point nvt20 = vt20;
			Point nvt02 = vt02;

			if(i == 1){
				// mirror
				Point t2 = nvt2;
				Point t20 = nvt20;
				nvt2 = nvt0;
				nvt20 = nvt02;
				nvt0 = t2;
				nvt02 = t20;
			}

			nvt2=(nvt20+nvt2)/2;
			nvt20=(nvt02+nvt20)/2;
			nvt2=(nvt20+nvt2)/2;
			nvt02=(nvt0+nvt02)/2;
			nvt20=(nvt02+nvt20)/2;
			nvt2=(nvt20+nvt2)/2;

			// recursive
			split_bezier_line(level - 1, nvt0, nvt2, nvt02, nvt20, call_back);
		}
		
	}
	else{
		// render line
		(*call_back)(vt0, vt2);
	}
}

HeeksObj *CMeshEdge::MakeACopy(void)const
{
	return new CMeshEdge(*this);
}

static void render_line_vertices2(const Point& vt0, const Point& vt1){
	glVertex3dv(vt0.GetPtr());
	glVertex3dv(vt1.GetPtr());
}

void CMeshEdge::GetSmoothLines(void(*call_back)(const Point& vt0, const Point& vt1)){
	split_bezier_line(theApp.m_num_facets_on_edge, m_v[0]->vertex(), m_v[1]->vertex(), m_c[0].vertex(), m_c[1].vertex(), call_back);
}

void CMeshEdge::MakeSureDisplayListExists(){
	if(m_gl_list)return;

	CMesh* mesh = (CMesh*)(Owner());
	if(mesh == NULL)return;

	m_gl_list = glGenLists(1);

	glNewList(m_gl_list, GL_COMPILE);
		
	Point xa = m_v[0]->vertex();
	Point xb = m_v[1]->vertex();
	Point ca = m_c[0].vertex();
	Point cb = m_c[1].vertex();
	
	HeeksColor(162, 179, 206).glColor();

	glBegin(GL_LINES);

	GetSmoothLines(render_line_vertices2);

	glEnd();

	glEndList();
}

void CMeshEdge::InvalidateBothFacesDisplayLists(){
	if(m_f[0])m_f[0]->KillGLLists();
	if(m_f[1])m_f[1]->KillGLLists();
}

void CMeshEdge::GetGripperPositions(std::list<GripData> *list, bool just_for_endof)
{
	// add the vertices for each end of the edge
	{
		for(int i = 0; i<2; i++){
			Point vt =  m_v[i]->vertex();
			list->push_back(GripData(GripperTypeStretch2,vt.x,vt.y,vt.z,NULL));
		}
	}

	// add the control points
	{
		for(int i = 0; i<2; i++){
			Point vt = m_c[i].vertex();
			list->push_back(GripData(GripperTypeStretch,vt.x,vt.y,vt.z,NULL));
		}
	}

	// add a mid point cross for splitting
	{
		Point vt = GetMidPoint();
		list->push_back(GripData(GripperTypeStretch3,vt.x,vt.y,vt.z,NULL));
	}
}

bool CMeshEdge::Stretch(const double *p, const double* shift, void* data){
	// stretch the vertex at "p" by a vector "shift"
	Point vp(p);

	// Find which vertex it is
	for(int i = 0; i<2; i++){
		Point vt = m_v[i]->vertex();
		if(vp == vt){
			Point new_vertex = vp + Point(shift);
			CMesh* mesh = (CMesh*)Owner();
			mesh->ChangeVertex(m_v[i], new_vertex); 
			return false;
		}

	}

	// check control points
	{
		for(int i = 0; i<2; i++){
			Point vt = m_c[i].vertex();
			if(vp == vt){
				Point new_vertex = vp + Point(shift);
				m_c[i].set_vertex(new_vertex);
				KillGLLists();
				InvalidateBothFacesDisplayLists();
			return false;
			}
		}
	}

	// check for mid point diamond for splitting
	{
		Point vt = GetMidPoint();
		if(vp == vt){
			heeksCAD->ClearMarkedList();
			Split();
			return false;
		}
	}
	
	return false;
}

bool CMeshEdge::StretchTemporary(const double *p, const double* shift, void* data)
{
	Point vt = GetMidPoint();
	Point vp(p);
	if(vp == vt)return false;
	Stretch(p, shift, data);
	return true;
}

void CMeshEdge::Split(){
	m_f[0]->SplitOnGivenEdge(this);
	CMeshVertex* new_vertex = m_f[1]->SplitOnGivenEdge(this);
	new_vertex->m_norm = new_vertex->get_best_normal_from_edges();
}

CMeshPosition& CMeshEdge::GetControlPointNearVertex(CMeshVertex* v){
	if(v == m_v[1])return m_c[1];
	return m_c[0];
}

CMeshPosition& CMeshEdge::GetControlPointAwayFromVertex(CMeshVertex* v){
	if(v == m_v[1])return m_c[0];
	return m_c[1];
}

static Point vt_for_GetMidPoint;
static void get_midpoint_callback(const Point& vt0, const Point& vt1){
	vt_for_GetMidPoint = vt1;
}

const Point& CMeshEdge::GetMidPoint(){
	split_bezier_line(1, m_v[0]->vertex(), m_v[1]->vertex(), m_c[0].vertex(), m_c[1].vertex(), get_midpoint_callback);
	return vt_for_GetMidPoint;
}

Point CMeshEdge::get_direction_at_vertex(const CMeshVertex* v){
	if(v==m_v[0])return Point(m_v[0]->vertex(), m_c[0].vertex());
	else return Point(m_v[1]->vertex(), m_c[1].vertex());;
}

void CMeshEdge::set_direction_at_vertex(const CMeshVertex* vt, const Point& v){
	if(vt==m_v[0]){
		m_c[0].set_vertex(m_v[0]->vertex() + v);
	}
	else{
		m_c[1].set_vertex(m_v[1]->vertex() + v);
	}

	InvalidateBothFacesDisplayLists();
	KillGLLists();
}

Point CMeshEdge::get_av_face_pair_normal(){
	Point v0 = m_f[0]->midpoints3normal();
	Point v1 = m_f[1]->midpoints3normal();

	return (v0 + v1).norm();
}

double glob1 = 1.0;
double glob2 = 1.0;

Point CMeshEdge::get_point_on_smooth_edge(double param, const Point& xa, const Point& xb, const Point& va_in, const Point& vb_in, double vfactor, Point* dir){
	if(param<0)param = 0;
	if(param>1)param = 1;

	Point va = va_in * vfactor * glob1;
	Point vb = vb_in * vfactor * glob1;

	Point vab(xa, xb);

	double tim = vab.magn() * glob2;
	
	Point vi = vab.norm() * 2 - (va + vb) * 0.5;
	
	Point xi = xa + (va * tim * 0.5 ) + (vi - va) * tim * 0.25;
	
	Point vai(xa, xi);
	Point vib(xi, xb);
	
	Point p;

	if(param<0.5)
	{
		p = xa + va * (param*tim);
		p = p + (vi - va) * (param *(param*tim));
		if(dir)*dir = (vi - va) * param + va/2;
	}
	else{
		p = xi + vi * ((param - 0.5)*tim);
		p = p + (vb - vi) * (((param - 0.5)) *((param - 0.5)*tim));
		if(dir)*dir = (vb - vi) * (param - 0.5) + vi/2;
	}
	
	return p;
}

Point CMeshEdge::evaluate_bezier_curve(double t, const Point& p0, const Point& p1, const Point& p2, const Point& p3, Point* dir){
	Point b;
	b = p0 * ((1-t)*(1-t)*(1-t));
	b = b + p1 * ( 3 * t * (1 - t)* (1-t) );
	b = b + p2 * ( 3 * t * t * (1-t) );
	b = b + p3 * ( t * t * t );

	if(dir){
		// differentiate to get the direction
		Point vt = p0 * ( -3 + 6*t - 3*t*t );
		vt = vt + p1 * ( 3 -12*t + 9*t*t );
		vt = vt + p2 * ( 6*t - 9*t*t );
		vt = vt + p3 * ( 3*t*t );
		*dir = Point(vt.GetPtr());
    }

	return b;
}

void CMeshEdge::DrawGripperSelectItems()
{
	if(heeksCAD->GetBackgroundColor() == HeeksColor(0, 0, 0))glColor3ub(255, 255, 255);
	else glColor3ub(0, 0, 0);

	glBegin(GL_LINES);
	for(int i = 0; i<2; i++){
		Point p0 = m_v[i]->vertex();
		CMeshPosition& p = GetControlPointNearVertex(m_v[i]);
		Point p1 = p.vertex();
		glVertex3d(p0.x, p0.y, p0.z);
		glVertex3d(p1.x, p1.y, p1.z);
	}
	glEnd();
}
