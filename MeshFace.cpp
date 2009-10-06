// MeshFace.cpp: implementation of the CMeshFace class.
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.

#include "stdafx.h"
#include "MeshFace.h"
#include "Mesh.h"
#include "interface/Tool.h"
#include "Plane.h"
#include "Line.h"
#include "interface/GripperTypes.h"
#include "interface/GripData.h"

CMeshFace::CMeshFace()
{
}

CMeshFace::~CMeshFace()
{
}

class RecalcGoodCentre:public Tool{
public:
	CMeshFace* m_f;

	// Tool's virtual functions
	const wxChar* GetTitle(){return _("Recalculate a Good Centre Point");}
	void Run(){m_f->RecalculateAGoodCentre();}
	wxString BitmapPath(){ return _T("recalccentre");}
};

static RecalcGoodCentre recalculate_good_centre;

void CMeshFace::GetTools(std::list<Tool*>* t_list, const wxPoint* p)
{
	recalculate_good_centre.m_f = this;
	t_list->push_back(&recalculate_good_centre);

	CMeshChild::GetTools(t_list, p);
}

HeeksObj *CMeshFace::MakeACopy(void)const
{
	return new CMeshFace(*this);
}

#define EVERY_OTHER
void CMeshFace::split_bezier_triangle(int level, int edge_to_split, const Point& vt0, const Point& vt1, const Point& vt2, const Point& c, const Point& vt01, const Point& vt10, const Point& vt12, const Point& vt21, const Point& vt20, const Point& vt02, void(*call_back)(const Point& vt0, const Point& vt1, const Point& vt2, const Point& n0, const Point& n1, const Point& n2))
{
	if(level > 0){
		Point avt0;
		Point avt1;
		Point avt2;
		Point avt01;
		Point avt10;
		Point avt12;
		Point avt21;
		Point avt20;
		Point avt02;

		if(edge_to_split == 0){
			avt0  = vt1;
			avt1  = vt2;
			avt2  = vt0;
			avt01 = vt12;
			avt10 = vt21;
			avt12 = vt20;
			avt21 = vt02;
			avt20 = vt01;
			avt02 = vt10;
		}
		else if(edge_to_split == 1){
			avt0  = vt2;
			avt1  = vt0;
			avt2  = vt1;
			avt01 = vt20;
			avt10 = vt02;
			avt12 = vt01;
			avt21 = vt10;
			avt20 = vt12;
			avt02 = vt21;
		}
		else{
			avt0  = vt0;
			avt1  = vt1;
			avt2  = vt2;
			avt01 = vt01;
			avt10 = vt10;
			avt12 = vt12;
			avt21 = vt21;
			avt20 = vt20;
			avt02 = vt02;
		}

		Point nvt0[2];
		Point nvt1[2];
		Point nvt2[2];
		Point nc[2];
		Point nvt01[2];
		Point nvt10[2];
		Point nvt12[2];
		Point nvt21[2];
		Point nvt20[2];
		Point nvt02[2];

		// split into two sub triangles
		for(int i = 0; i<2; i++){
			nvt0[i] = avt0;
			nvt1[i] = avt1;
			nvt2[i] = avt2;
			nc[i] = c;
			nvt01[i] = avt01;
			nvt10[i] = avt10;
			nvt12[i] = avt12;
			nvt21[i] = avt21;
			nvt20[i] = avt20;
			nvt02[i] = avt02;

			if(i == 1){
				nvt0[i]=(nvt02[i]+nvt0[i])/2;
				nvt02[i]=(nvt20[i]+nvt02[i])/2;
				nvt0[i]=(nvt02[i]+nvt0[i])/2;
				nvt20[i]=(nvt2[i]+nvt20[i])/2;
				nvt02[i]=(nvt20[i]+nvt02[i])/2;
				nvt0[i]=(nvt02[i]+nvt0[i])/2;
				nvt01[i]=(nc[i]+nvt01[i])/2;
				nc[i]=(nvt21[i]+nc[i])/2;
				nvt01[i]=(nc[i]+nvt01[i])/2;
				nvt10[i]=(nvt12[i]+nvt10[i])/2;
			}
			else{
				nvt2[i]=(nvt20[i]+nvt2[i])/2;
				nvt20[i]=(nvt02[i]+nvt20[i])/2;
				nvt2[i]=(nvt20[i]+nvt2[i])/2;
				nvt02[i]=(nvt0[i]+nvt02[i])/2;
				nvt20[i]=(nvt02[i]+nvt20[i])/2;
				nvt2[i]=(nvt20[i]+nvt2[i])/2;
				nvt21[i]=(nc[i]+nvt21[i])/2;
				nc[i]=(nvt01[i]+nc[i])/2;
				nvt21[i]=(nc[i]+nvt21[i])/2;
				nvt12[i]=(nvt10[i]+nvt12[i])/2;
			}

		}

		int new_edge_to_split[2] = {0, 1};

		for(int i = 0; i<2; i++){
			split_bezier_triangle(level - 1, new_edge_to_split[i], nvt0[i], nvt1[i], nvt2[i], nc[i], nvt01[i], nvt10[i], nvt12[i], nvt21[i], nvt20[i], nvt02[i], call_back);
		}
	}
	else{
		// render triangle
		// calculate normal, to do - evaluate norm from surface
		Point v0(vt0, vt1);
		Point v1(vt0, vt2);
		Point norm = (v0 ^ v1).norm();
		(*call_back)(vt0, vt1, vt2, norm, norm, norm);
	}
}

static void render_triangle_vertices2(const Point& vt0, const Point& vt1, const Point& vt2, const Point& n0, const Point& n1, const Point& n2){
	glNormal3dv(n0.GetPtr());
	glVertex3dv(vt0.GetPtr());
	glNormal3dv(n1.GetPtr());
	glVertex3dv(vt1.GetPtr());
	glNormal3dv(n2.GetPtr());
	glVertex3dv(vt2.GetPtr());
}

void CMeshFace::GetSmoothTriangles(void(*call_back)(const Point& vt0, const Point& vt1, const Point& vt2, const Point& n0, const Point& n1, const Point& n2)){
	Point vt01 = m_edge_dir[0] ? m_e[0]->m_c[1].vertex() : m_e[0]->m_c[0].vertex();
	Point vt10 = m_edge_dir[0] ? m_e[0]->m_c[0].vertex() : m_e[0]->m_c[1].vertex();
	Point vt12 = m_edge_dir[1] ? m_e[1]->m_c[1].vertex() : m_e[1]->m_c[0].vertex();
	Point vt21 = m_edge_dir[1] ? m_e[1]->m_c[0].vertex() : m_e[1]->m_c[1].vertex();
	Point vt20 = m_edge_dir[2] ? m_e[2]->m_c[1].vertex() : m_e[2]->m_c[0].vertex();
	Point vt02 = m_edge_dir[2] ? m_e[2]->m_c[0].vertex() : m_e[2]->m_c[1].vertex();

		int edge_to_split;
		// choose longest edge to split
		Point vc0(m_v[0]->vertex(), m_v[1]->vertex());
		Point vc1(m_v[1]->vertex(), m_v[2]->vertex());
		Point vc2(m_v[2]->vertex(), m_v[0]->vertex());

		if(vc1.magn() > vc0.magn()){
			if(vc2.magn() > vc1.magn()){
				edge_to_split = 2;
			}
			else{
				edge_to_split = 1;
			}
		}
		else{
			if(vc2.magn() > vc0.magn()){
				edge_to_split = 2;
			}
			else{
				edge_to_split = 0;
			}
		}

	split_bezier_triangle(theApp.m_num_facets_on_edge, edge_to_split, m_v[0]->vertex(), m_v[1]->vertex(), m_v[2]->vertex(), m_centre.vertex(), vt01, vt10, vt12, vt21, vt20, vt02, call_back);
}

void CMeshFace::MakeSureDisplayListExists(){
	if(m_gl_list)return;

	m_gl_list = glGenLists(1);

	glNewList(m_gl_list, GL_COMPILE);
	
	glShadeModel(GL_SMOOTH);

	glBegin(GL_TRIANGLES);
	GetSmoothTriangles(render_triangle_vertices2);
	glEnd();
	
	glShadeModel(GL_FLAT);
	
	glEndList();
}

void CMeshFace::GetGripperPositions(std::list<GripData> *list, bool just_for_endof){
	{
		for(int i = 0; i<3; i++){
			Point vt = m_v[i]->vertex();
			list->push_back(GripData(GripperTypeStretch2,vt.x,vt.y,vt.z,NULL));
		}
	}

	{
		for(int i = 0; i<3; i++){
			for(int j = 0; j<2; j++){
				Point vt = m_e[i]->m_c[j].vertex();
				list->push_back(GripData(GripperTypeStretch,vt.x,vt.y,vt.z,NULL));
			}
		}
	}

	{
		Point vt = m_centre.vertex();
		list->push_back(GripData(GripperTypeStretch4,vt.x,vt.y,vt.z,NULL));
	}
}

double CMeshFace::angle_at_vertex(const CMeshVertex* v)const{
	std::list<CMeshEdge*>::const_iterator It;

	Point vc[2];
	int i = 0;

	for(int k = 0; k<3; k++){
		CMeshEdge* edge = m_e[k];
		if(edge->m_v[0] == v){
			vc[i] = Point(v->vertex(), edge->m_v[1]->vertex());
			i++;
		}
		else if(edge->m_v[1] == v){
			vc[i] = Point(v->vertex(), edge->m_v[0]->vertex());
			i++;
		}
		if(i==2)break;
	}

	if(i<2)return 0;

	double dotp = vc[0].norm() * vc[1].norm();
	double angle = acos(dotp);

	return angle;
}

Point CMeshFace::direction_at_vertex(const CMeshVertex* v)const{
	std::list<CMeshEdge*>::const_iterator It;

	Point vc[2];
	int i = 0;

	for(int k = 0; k<3; k++){
		CMeshEdge* edge = m_e[k];
		if(edge->m_v[0] == v){
			vc[i] = Point(v->vertex(), edge->m_v[1]->vertex());
			i++;
		}
		else if(edge->m_v[1] == v){
			vc[i] = Point(v->vertex(), edge->m_v[0]->vertex());
			i++;
		}
		if(i==2)break;
	}

	if(i<2)return Point(0, 0 ,0);

	return (vc[0] + vc[1]).norm();
}

bool CMeshFace::Stretch(const double *p, const double* shift, void* data){
	// stretch the vertex at "p" by a vector "shift"
	Point vp(p);

	{
		// Find which vertex it is
		for(int i = 0; i<3; i++){
			Point vt = m_v[i]->vertex();
			if(vp == vt){
				Point new_vertex = vp + Point(shift);
				CMesh* mesh = (CMesh*)Owner();
				mesh->ChangeVertex(m_v[i], new_vertex); 
				return false;
			}
		}
	}

	// check control points
	{
		for(int i = 0; i<3; i++){
			for(int j = 0; j<2; j++){
				Point vt = m_e[i]->m_c[j].vertex();
				if(vp == vt){
					Point new_vertex = vp + Point(shift);
					m_e[i]->m_c[j].set_vertex(new_vertex);
					m_e[i]->KillGLLists();
					m_e[i]->InvalidateBothFacesDisplayLists();
					return false;
				}
			}
		}
	}

	// check centre point
	{
		Point vt = m_centre.vertex();
		if(vp == vt){
			Point new_vertex = vp + Point(shift);
			m_centre.set_vertex(new_vertex);
			KillGLLists();
			return false;
		}
	}

	return false;
}

CMeshVertex* CMeshFace::SplitOnGivenEdge(CMeshEdge* edge)
{
	Point vt0;
	Point vt1;
	Point vt2;
	Point vt01;
	Point vt10;
	Point vt12;
	Point vt21;
	Point vt20;
	Point vt02;
	Point c = m_centre.vertex();
	CMeshVertex* new_vertex = NULL;

	if(edge == m_e[0]){
		vt0 = m_v[1]->vertex();
		vt1 = m_v[2]->vertex();
		vt2 = m_v[0]->vertex();
		vt01 = m_edge_dir[1] ? m_e[1]->m_c[1].vertex() : m_e[1]->m_c[0].vertex();
		vt10 = m_edge_dir[1] ? m_e[1]->m_c[0].vertex() : m_e[1]->m_c[1].vertex();
		vt12 = m_edge_dir[2] ? m_e[2]->m_c[1].vertex() : m_e[2]->m_c[0].vertex();
		vt21 = m_edge_dir[2] ? m_e[2]->m_c[0].vertex() : m_e[2]->m_c[1].vertex();
		vt20 = m_edge_dir[0] ? m_e[0]->m_c[1].vertex() : m_e[0]->m_c[0].vertex();
		vt02 = m_edge_dir[0] ? m_e[0]->m_c[0].vertex() : m_e[0]->m_c[1].vertex();
	}
	else if(edge == m_e[1]){
		vt0 = m_v[2]->vertex();
		vt1 = m_v[0]->vertex();
		vt2 = m_v[1]->vertex();
		vt01 = m_edge_dir[2] ? m_e[2]->m_c[1].vertex() : m_e[2]->m_c[0].vertex();
		vt10 = m_edge_dir[2] ? m_e[2]->m_c[0].vertex() : m_e[2]->m_c[1].vertex();
		vt12 = m_edge_dir[0] ? m_e[0]->m_c[1].vertex() : m_e[0]->m_c[0].vertex();
		vt21 = m_edge_dir[0] ? m_e[0]->m_c[0].vertex() : m_e[0]->m_c[1].vertex();
		vt20 = m_edge_dir[1] ? m_e[1]->m_c[1].vertex() : m_e[1]->m_c[0].vertex();
		vt02 = m_edge_dir[1] ? m_e[1]->m_c[0].vertex() : m_e[1]->m_c[1].vertex();
	}
	else{
		vt0 = m_v[0]->vertex();
		vt1 = m_v[1]->vertex();
		vt2 = m_v[2]->vertex();
		vt01 = m_edge_dir[0] ? m_e[0]->m_c[1].vertex() : m_e[0]->m_c[0].vertex();
		vt10 = m_edge_dir[0] ? m_e[0]->m_c[0].vertex() : m_e[0]->m_c[1].vertex();
		vt12 = m_edge_dir[1] ? m_e[1]->m_c[1].vertex() : m_e[1]->m_c[0].vertex();
		vt21 = m_edge_dir[1] ? m_e[1]->m_c[0].vertex() : m_e[1]->m_c[1].vertex();
		vt20 = m_edge_dir[2] ? m_e[2]->m_c[1].vertex() : m_e[2]->m_c[0].vertex();
		vt02 = m_edge_dir[2] ? m_e[2]->m_c[0].vertex() : m_e[2]->m_c[1].vertex();
	}

	// remove the existing face
	CMesh* mesh = (CMesh*)Owner();
	mesh->Remove(this);
	delete this;

	// split into two sub triangles
	for(int i = 0; i<2; i++){
		Point nvt0 = vt0;
		Point nvt1 = vt1;
		Point nvt2 = vt2;
		Point nc = c;
		Point nvt01 = vt01;
		Point nvt10 = vt10;
		Point nvt12 = vt12;
		Point nvt21 = vt21;
		Point nvt20 = vt20;
		Point nvt02 = vt02;

		if(i == 1){
			// mirror
			Point t12 = nvt12;
			Point t21 = nvt21;
			Point t2 = nvt2;
			Point t20 = nvt20;
			nvt12 = nvt10;
			nvt21 = nvt01;
			nvt2 = nvt0;
			nvt20 = nvt02;
			nvt10 = t12;
			nvt01 = t21;
			nvt0 = t2;
			nvt02 = t20;
		}

		nvt2=(nvt20+nvt2)/2;
		nvt20=(nvt02+nvt20)/2;
		nvt2=(nvt20+nvt2)/2;
		nvt02=(nvt0+nvt02)/2;
		nvt20=(nvt02+nvt20)/2;
		nvt2=(nvt20+nvt2)/2;
		nvt21=(nc+nvt21)/2;
		nc=(nvt01+nc)/2;
		nvt21=(nc+nvt21)/2;
		nvt12=(nvt10+nvt12)/2;

		// add the new triangles
		if(i == 1){
			mesh->AddTriangle(nvt0, nvt2, nvt1, nvt02, nvt20, nvt21, nvt12, nvt10, nvt01, nc);
		}
		else{
			mesh->AddTriangle(nvt0, nvt1, nvt2, nvt01, nvt10, nvt12, nvt21, nvt20, nvt02, nc);
			new_vertex = mesh->FindVertex(nvt2);
		}

	}
	return new_vertex;
}

void CMeshFace::RemoveGivenEdge(CMeshEdge* edge)
{
	for(int i = 0; i<3; i++){
		if(m_e[i] == edge){
			m_e[i] = NULL;
			break;
		}
	}
}

Point CMeshFace::simple_3_pts_normal(){
	Point v0(m_v[0]->vertex(), m_v[1]->vertex());
	Point v1(m_v[1]->vertex(), m_v[2]->vertex());
	Point v2(m_v[2]->vertex(), m_v[0]->vertex());

	Point n0 = v2 ^ v0;
	Point n1 = v0 ^ v1;
	Point n2 = v1 ^ v2;

	if(n0.magn() > n1.magn()){
		if(n0.magn() > n2.magn()){
			return n0.norm();
		}
		return n2.norm();
	}

	if(n1.magn() > n2.magn()){
		return n1.norm();
	}

	return n2.norm();
}

Point CMeshFace::midpoints3normal(){
	Point m0 = m_e[0]->GetMidPoint();
	Point m1 = m_e[1]->GetMidPoint();
	Point m2 = m_e[2]->GetMidPoint();

	Point v0(m0, m1);
	Point v1(m1, m2);
	Point v2(m2, m0);

	Point n0 = v2 ^ v0;
	Point n1 = v0 ^ v1;
	Point n2 = v1 ^ v2;

	if(n0.magn() > n1.magn()){
		if(n0.magn() > n2.magn()){
			return n0.norm();
		}
		return n2.norm();
	}

	if(n1.magn() > n2.magn()){
		return n1.norm();
	}

	return n2.norm();
}

double CMeshFace::longest_length(){
	double max_dist = m_v[0]->vertex().dist(m_v[1]->vertex());
	double dist = m_v[1]->vertex().dist(m_v[2]->vertex());
	if(dist>max_dist)max_dist = dist;
	dist = m_v[2]->vertex().dist(m_v[0]->vertex());
	if(dist>max_dist)max_dist = dist;

	return max_dist;
}

bool CMeshFace::is_connected_to_edge(const CMeshEdge* edge)const{
	for(int i = 0; i<3; i++){
		if(m_e[i] == edge)return true;
	}

	return false;
}

bool CMeshFace::RecalculateAGoodCentre()
{
	Plane pl[3];
	for(int i = 0; i<3; i++)
	{
		CMeshEdge* edge = m_e[i];
		Point dir;
		Point mid_point = edge->evaluate_bezier_curve(0.5, edge->m_v[0]->vertex(), edge->m_c[0].vertex(), edge->m_c[1].vertex(), edge->m_v[1]->vertex(), &dir); 
		dir = dir.norm();
		Point n = edge->get_av_face_pair_normal();
		Point cp = (n ^ dir).norm();
		Point n2 = (dir ^ cp).norm();
		pl[i] = Plane(n2, mid_point);
	}

	bool centre_found = false;

	Line line;
	if(pl[0].Intersect(pl[1], line))
	{
		Point p;
		if(pl[2].Intersect(line, p))
		{
			m_centre.set_vertex(p);
			KillGLLists();
			centre_found = true;

			// move it away some more
			Plane flat_pl(midpoints3normal(), m_e[0]->GetMidPoint());
			Point close_point = flat_pl.Near(m_centre.vertex());
			Point new_centre = m_centre.vertex() + Point(close_point, m_centre.vertex()) * 0.5;
			m_centre.set_vertex(new_centre);
		}
	}

	return centre_found;
}

void CMeshFace::DrawGripperSelectItems()
{
	if(heeksCAD->GetBackgroundColor() == HeeksColor(0, 0, 0))glColor3ub(255, 255, 255);
	else glColor3ub(0, 0, 0);

	glBegin(GL_LINES);
	for(int i = 0; i<3; i++){
		for(int j = 0; j<2; j++){
			Point p0 = m_e[i]->m_v[j]->vertex();
			CMeshPosition& p = m_e[i]->GetControlPointNearVertex(m_e[i]->m_v[j]);
			Point p1 = p.vertex();
			glVertex3d(p0.x, p0.y, p0.z);
			glVertex3d(p1.x, p1.y, p1.z);
		}
	}
	glEnd();
}
