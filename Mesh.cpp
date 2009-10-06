// Mesh.cpp: implementation of the CMesh class.
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.

#include "stdafx.h"
#include "Mesh.h"
#include "MeshFace.h"
#include "MeshEdge.h"
#include "Matrix.h"
#include "interface/Tool.h"
#include "interface/MarkedObject.h"
#include "tinyxml/tinyxml.h"

// static member variables
bool CMesh::m_display_vertices = false;
bool CMesh::m_display_edges = true;
bool CMesh::m_display_faces = true;

void CMesh::set_initial(){
}

CMesh::CMesh()
{
	set_initial();
}

CMesh::CMesh(const CMesh &m){
	operator=(m);
}

CMesh::~CMesh()
{
	DeleteVertices();
	DeleteEdges();
	DeleteFaces();
}

const CMesh& CMesh::operator=(const CMesh &m){
	set_initial();

	return *this;
}

void CMesh::DeleteVertices(){
	std::map<CMeshPosition, CMeshVertex*>::iterator It;
	for(It = m_vertices.begin(); It != m_vertices.end(); It++){
		CMeshVertex* v = It->second;
		delete v;
	}

	m_vertices.clear();
}

void CMesh::DeleteEdges(){
	std::set<CMeshEdge*>::iterator It;
	for(It = m_edges.begin(); It != m_edges.end(); It++){
		CMeshEdge* edge = *It;
		delete edge;
	}

	m_edges.clear();
}

void CMesh::DeleteFaces(){
	std::set<CMeshFace*>::iterator It;
	for(It = m_faces.begin(); It != m_faces.end(); It++){
		CMeshFace* face = *It;
		face->SetOwner(NULL);
		delete face;
	}

	m_faces.clear();
}

void CMesh::KillGLLists(void)
{
	DestroyChildDisplayLists();
}

void CMesh::DestroyChildDisplayLists(){
	{
		std::set<CMeshEdge*>::iterator It;
		for(It = m_edges.begin(); It != m_edges.end(); It++){
			CMeshEdge* edge = *It;
			edge->KillGLLists();
		}
	}

	{
		std::set<CMeshFace*>::iterator It;
		for(It = m_faces.begin(); It != m_faces.end(); It++){
			CMeshFace* face = *It;
			face->KillGLLists();
		}
	}

	{
		std::map<CMeshPosition, CMeshVertex*>::iterator It;
		for(It = m_vertices.begin(); It != m_vertices.end(); It++){
			CMeshVertex* v = It->second;
			v->KillGLLists();
		}
	}
}

void CMesh::render_elements(bool select, bool selected, bool no_colour){
	if(m_display_faces){
		if(!no_colour){
			HeeksColor(64, 220, 64).glMaterial(1);
			glEnable(GL_LIGHTING);
		}

		for(std::set<CMeshFace*>::iterator It = m_faces.begin(); It != m_faces.end(); It++){
			CMeshFace* face = *It;
			if(select)glPushName((unsigned int)face);
			if(face->m_gl_list)glCallList(face->m_gl_list);
			if(select)glPopName();
		}

		if(!no_colour){
			glDisable(GL_LIGHTING);
		}

		for(std::set<CMeshFace*>::iterator It = m_faces.begin(); It != m_faces.end(); It++){
			CMeshFace* face = *It;
			if(heeksCAD->ObjectMarked(face))face->DrawGripperSelectItems();
		}

	}

	if(m_display_edges && !no_colour){
		std::set<CMeshEdge*>::iterator It;
		for(It = m_edges.begin(); It != m_edges.end(); It++){
			CMeshEdge* edge = *It;
			if(select)glPushName((unsigned int)edge);
			if(edge->m_gl_list)glCallList(edge->m_gl_list);
			if(heeksCAD->ObjectMarked(edge))edge->DrawGripperSelectItems();
			if(select)glPopName();
		}
	}

	if(m_display_vertices || select && !no_colour){
		std::map<CMeshPosition, CMeshVertex*>::iterator It;
		for(It = m_vertices.begin(); It != m_vertices.end(); It++){
			CMeshVertex* v = It->second;
			if(select)glPushName((unsigned int)v);
			if(v->m_gl_list)glCallList(v->m_gl_list);
			if(heeksCAD->ObjectMarked(v))v->DrawGripperSelectItems();
			if(select)glPopName();
		}	
	}

}

void CMesh::MakeSureDisplayListsExists(){
	{
		std::set<CMeshEdge*>::iterator It;
		for(It = m_edges.begin(); It != m_edges.end(); It++){
			CMeshEdge* edge = *It;
			edge->MakeSureDisplayListExists();
		}
	}

	{
		std::set<CMeshFace*>::iterator It;
		for(It = m_faces.begin(); It != m_faces.end(); It++){
			CMeshFace* face = *It;
			face->MakeSureDisplayListExists();
		}
	}

	{
		std::map<CMeshPosition, CMeshVertex*>::iterator It;
		for(It = m_vertices.begin(); It != m_vertices.end(); It++){
			CMeshVertex* v = It->second;
			v->MakeSureDisplayListExists();
		}
	}
}

void CMesh::glCommands(bool select, bool marked, bool no_color){
	MakeSureDisplayListsExists();

	render_elements(true, marked, no_color);
}

void CMesh::GetBox(CBox &box)
{
	if(m_vertices.size() == 0)return;

	int i = 0;
	std::map<CMeshPosition, CMeshVertex*>::iterator It;
	for(It = m_vertices.begin(); It != m_vertices.end(); It++){
		CMeshVertex* v = It->second;
		box.Insert(v->vertex().GetPtr());
		i++;
	}
}

HeeksObj *CMesh::MakeACopy(void)const
{
	return new CMesh(*this);
}

bool CMesh::ModifyByMatrix(const double *mat){
	std::list<CMeshVertex*> vertex_list;

	Matrix tmat(mat);

	{
		std::map<CMeshPosition, CMeshVertex*>::iterator It;
		for(It = m_vertices.begin(); It != m_vertices.end(); It++){
			CMeshVertex* v = It->second;
			v->set_vertex(tmat * v->vertex());
			vertex_list.push_back(v);
		}
	}

	m_vertices.clear();

	std::list<CMeshVertex*>::iterator VIt;
	for(VIt = vertex_list.begin(); VIt != vertex_list.end(); VIt++){
		CMeshVertex* v = *VIt;
		m_vertices.insert(std::pair<CMeshPosition, CMeshVertex*>(v->m_position, v));
	}

	{
		std::set<CMeshEdge*>::iterator It;
		for(It = m_edges.begin(); It != m_edges.end(); It++){
			CMeshEdge* edge = *It;
			for(int i = 0; i<2; i++){
				edge->m_c[i].set_vertex(tmat * edge->m_c[i].vertex());
			}
		}
	}

	{
		std::set<CMeshFace*>::iterator It;
		for(It = m_faces.begin(); It != m_faces.end(); It++){
			CMeshFace* face = *It;
			face->m_centre.set_vertex(tmat * face->m_centre.vertex());
		}
	}

	DestroyChildDisplayLists();

	return false;
}

CMeshFace* CMesh::AddTriangle(const Point& a, const Point& b, const Point& c, const Point& ab, const Point& ba, const Point& bc, const Point& cb, const Point& ca, const Point& ac, const Point& centre){
	// add face
	CMeshFace* face = new CMeshFace;
	CMeshVertex* v0 = AddVertex(a);
	face->m_v[0] = v0;
	v0->m_faces.insert(face);
	CMeshVertex* v1 = AddVertex(b);
	face->m_v[1] = v1;
	v1->m_faces.insert(face);
	CMeshVertex* v2 = AddVertex(c);
	face->m_v[2] = v2;
	v2->m_faces.insert(face);
	face->m_centre = CMeshPosition(centre);

	Add(face, NULL);
	m_faces.insert(face);

	// add the edges
	{
		face->m_e[0] = AddEdge(face->m_v[0], face->m_v[1], CMeshPosition(ab), CMeshPosition(ba), face, &(face->m_edge_dir[0]));
		face->m_e[1] = AddEdge(face->m_v[1], face->m_v[2], CMeshPosition(bc), CMeshPosition(cb), face, &(face->m_edge_dir[1]));
		face->m_e[2] = AddEdge(face->m_v[2], face->m_v[0], CMeshPosition(ca), CMeshPosition(ac), face, &(face->m_edge_dir[2]));
	}

	DestroyChildDisplayLists();

	return face;
}

CMeshVertex* CMesh::AddVertex(const Point &v){
	CMeshVertex* mv = FindVertex(v);
	if(mv)return mv;

	mv = new CMeshVertex(v);
	m_vertices.insert(std::pair<CMeshPosition, CMeshVertex*>(mv->m_position, mv));

	// set vertex's owner to this
	Add(mv, NULL);

	return mv;
}

CMeshVertex* CMesh::FindVertex(const Point &v){
	if(m_vertices.size() == 0)return NULL;

	std::map<CMeshPosition, CMeshVertex*>::iterator It;
	CMeshPosition position(v);
	It = m_vertices.find(position);
	if(It == m_vertices.end())return NULL;
	return It->second;
}

CMeshEdge* CMesh::FindEdge(CMeshVertex* v1, CMeshVertex* v2, EDGE_MAP_IT* FindIt1, EDGE_MAP_IT* FindIt2){
	bool delete_it1 = false;
	bool delete_it2 = false;

	if(FindIt1 == NULL){
		FindIt1 = new EDGE_MAP_IT;
		delete_it1 = true;
	}
	if(FindIt2 == NULL){
		FindIt2 = new EDGE_MAP_IT;
		delete_it2 = true;
	}

	CMeshEdge* edge = NULL;

	// look for v1 - v2
	(*FindIt1) = m_edge_map.find(v1);
	if((*FindIt1) != m_edge_map.end()){
		std::map<const CMeshVertex*, CMeshEdge*>::iterator FindIt;
		FindIt = (*FindIt1)->second.find(v2);
		if(FindIt != (*FindIt1)->second.end()){
			// edge found  v1 - v2
			edge = FindIt->second;
		}
	}

	// if not found
	if(edge == NULL){
		// look for v2 - v1
		(*FindIt2) = m_edge_map.find(v2);
		if((*FindIt2) != m_edge_map.end()){
			std::map<const CMeshVertex*, CMeshEdge*>::iterator FindIt;
			FindIt = (*FindIt2)->second.find(v1);
			if(FindIt != (*FindIt2)->second.end()){
				// edge found v2 - v1
				edge = FindIt->second;
			}
		}
	}

	if(delete_it1)delete FindIt1;
	if(delete_it2)delete FindIt2;

	return edge;
}

CMeshEdge* CMesh::AddEdge(CMeshVertex* v1, CMeshVertex* v2, const CMeshPosition& c1, const CMeshPosition& c2, CMeshFace* face, bool *second_face){
	// find edge, or add new edge
	// v1 - v2 is the same as v2 - v1

	std::map< const CMeshVertex*, std::map<const CMeshVertex*, CMeshEdge*> >::iterator FindIt1, FindIt2;

	CMeshEdge* edge = FindEdge(v1, v2, &FindIt1, &FindIt2);

	// if not found
	if(edge == NULL){
		// make a new edge..

		// if v1 in the first map
		if(FindIt1 != m_edge_map.end()){
			// add using v1
			edge = new CMeshEdge(v1, v2, c1, c2, this);
			FindIt1->second.insert(std::pair<CMeshVertex*, CMeshEdge*>(v2, edge));
		}
		// else if v2 in the first map
		else if(FindIt2 != m_edge_map.end()){
			// add using v1
			edge = new CMeshEdge(v1, v2, c1, c2, this);
			FindIt2->second.insert(std::pair<CMeshVertex*, CMeshEdge*>(v1, edge));
		}
		// else if neither v1 nor v2 is in the first map
		else{
			// add using v1 and v2
			edge = new CMeshEdge(v1, v2, c1, c2, this);
			std::map< const CMeshVertex*, CMeshEdge* > temp_map;
			temp_map.insert( std::pair< const CMeshVertex*, CMeshEdge* >(v2, edge));

			m_edge_map.insert(std::pair< const CMeshVertex*, std::map< const CMeshVertex*, CMeshEdge* > >( v1, temp_map ));
		}

		// insert into m_edges
		m_edges.insert(edge);
	}

	if(second_face)*second_face = false;
	if(edge->m_f[0] == NULL)edge->m_f[0] = face;
	else if(edge->m_f[1] == NULL)
	{
		edge->m_f[1] = face;
		if(second_face)*second_face = true;
	}
	else{
		wxMessageBox(_("Edge cannot have more than 2 faces!"));
	}

	return edge;
}

static CMesh* mesh_for_tools = NULL;

class ConvertMeshToTriangles:public Tool{
public:
	// Tool's virtual functions
	const wxChar* GetTitle(){return _("Convert Mesh To Triangles");}
	void Run(){
		HeeksObj* new_object = mesh_for_tools->ConvertToTriangles();
		heeksCAD->CreateUndoPoint();
		heeksCAD->Add(new_object, NULL);
		heeksCAD->Remove(mesh_for_tools);
		heeksCAD->Changed();
	}
	wxString BitmapPath(){ return _T("meshtotri");}
};

static ConvertMeshToTriangles convert_mesh_to_triangles;

class NormalizeAllVs:public Tool{
public:
	// Tool's virtual functions
	const wxChar* GetTitle(){return _("Normalize All Vertices");}
	void Run(){
		mesh_for_tools->NormalizeAllVertices();
	}
	wxString BitmapPath(){ return _T("meshnormv");}
};

static NormalizeAllVs normalize_all_vertices;

class SetAllVNormals:public Tool{
public:
	// Tool's virtual functions
	const wxChar* GetTitle(){return _("Set All Vertex Normals");}
	void Run(){
		if(!mesh_for_tools->SetAllVertexNormals()){
			wxMessageBox(_("At least one vertex could not be normalized!"));
		}
	}
};

static SetAllVNormals set_all_vnormals;

class fnRecalcCentres:public Tool{
public:
	// Tool's virtual functions
	const wxChar* GetTitle(){return _("Recalculate All Face Centres");}
	void Run(){
		mesh_for_tools->RecalculateFaceCentres();
	}
};

static fnRecalcCentres recalc_centres;

class MirrorMesh:public Tool{
public:
	// Tool's virtual functions
	const wxChar* GetTitle(){return _("Mirror all x<1");}
	void Run(){
		double tolerance = 0.4;
		//if(heeksCAD->InputDouble(_("Enter Tolerance"), _("Tolerance"), tolerance))
		//{
			mesh_for_tools->Mirror(tolerance);
		//}
	}
};

static MirrorMesh mirror_mesh;

void CMesh::GetTools(std::list<Tool*>* t_list, const wxPoint* p)
{
	mesh_for_tools = this;
	t_list->push_back(&convert_mesh_to_triangles);
	t_list->push_back(&normalize_all_vertices);
	t_list->push_back(&set_all_vnormals);
	t_list->push_back(&recalc_centres);
	t_list->push_back(&mirror_mesh);

	HeeksObj::GetTools(t_list, p);
}

bool CMesh::Stretch(const double *p, const double* shift, void* data)
{
	return false;
}

bool CMesh::RemoveGivenEdge(CMeshEdge* edge){
	for(int i = 0; i<2; i++){
		edge->m_v[i]->m_edges.erase(edge);
	}

	if(edge->m_f[0])edge->m_f[0]->RemoveGivenEdge(edge);
	if(edge->m_f[1])edge->m_f[1]->RemoveGivenEdge(edge);

	m_edges.erase(edge);

	// remove edge from m_edge_map
	std::map< const CMeshVertex*, std::map<const CMeshVertex*, CMeshEdge*> >::iterator FindIt1, FindIt2;
	bool edge_removed = false;

	// look for v1 - v2
	FindIt1 = m_edge_map.find(edge->m_v[0]);
	if(FindIt1 != m_edge_map.end()){
		std::map<const CMeshVertex*, CMeshEdge*>::iterator FindIt;
		FindIt = FindIt1->second.find(edge->m_v[1]);
		if(FindIt != FindIt1->second.end() && FindIt->second == edge){
			// edge found  v1 - v2
			FindIt1->second.erase(FindIt);
			edge_removed = true;
			if(FindIt1->second.size() == 0)m_edge_map.erase(FindIt1);
		}
	}
				
	// if not found
	if(!edge_removed){
		// look for v2 - v1
		FindIt2 = m_edge_map.find(edge->m_v[1]);
		if(FindIt2 != m_edge_map.end()){
			std::map<const CMeshVertex*, CMeshEdge*>::iterator FindIt;
			FindIt = FindIt2->second.find(edge->m_v[0]);
			if(FindIt != FindIt2->second.end() && FindIt->second == edge){
				// edge found  v1 - v2
				std::map<const CMeshVertex*, CMeshEdge*>& map = FindIt2->second;
				map.erase(FindIt);
				edge_removed = true;
				if(map.size() == 0)
					m_edge_map.erase(FindIt2);
			}
		}
	}

	return edge_removed;
}

void CMesh::Remove(HeeksObj* object){
	switch(object->GetType()){
		case MeshFaceType:
			{
				std::list<CMeshEdge*> edge_delete_list;

				CMeshFace* face = (CMeshFace*)object;
				face->SetOwner(NULL);
				{
					for(int i = 0; i<3; i++){
						for(int j = 0; j<2; j++){
							if(face->m_e[i]->m_f[j] == face){
								face->m_e[i]->m_f[j] = NULL;
								break;
							}
						}

						if(face->m_e[i]->m_f[0] == NULL && face->m_e[i]->m_f[1] == NULL){
							edge_delete_list.push_back(face->m_e[i]);
						}
					}
				}
				m_faces.erase(face);

				{
					for(int i = 0; i<3; i++){
						face->m_v[i]->m_faces.erase(face);
					}
				}

				{
					std::list<CMeshEdge*>::iterator It;
					for(It = edge_delete_list.begin(); It != edge_delete_list.end(); It++){
						CMeshEdge* edge = *It;
						Remove(edge);
						delete edge;
					}
				}
			}
			break;

		case MeshEdgeType:
			{
				CMeshEdge* edge = (CMeshEdge*)object;
				edge->SetOwner(NULL);
				RemoveGivenEdge(edge);
			}
			break;
	}

	object->SetOwner(NULL);
}

void CMesh::ChangeVertex(CMeshVertex* v, const Point& vt){
	if(FindVertex(vt)){
		return;// can't move it to existing vertex position
	}

	CMeshPosition old_position = v->m_position;

	v->set_vertex(vt);
	v->KillGLLists();

	{
		std::set<CMeshEdge*>::iterator It;
		for(It = v->m_edges.begin(); It != v->m_edges.end(); It++){
			(*It)->KillGLLists();
		}
	}

	{
		std::set<CMeshFace*>::iterator It;
		for(It = v->m_faces.begin(); It != v->m_faces.end(); It++){
			(*It)->KillGLLists();
		}
	}

	CMeshPosition new_position = v->m_position;

	m_vertices.erase(old_position);
	m_vertices.insert(std::pair<CMeshPosition, CMeshVertex*>(new_position, v));
}

void CMesh::KillGLListss(){
	{
		std::set<CMeshEdge*>::iterator It;
		for(It = m_edges.begin(); It != m_edges.end(); It++){
			CMeshEdge* edge = *It;
			edge->KillGLLists();
		}
	}

	{
		std::set<CMeshFace*>::iterator It;
		for(It = m_faces.begin(); It != m_faces.end(); It++){
			CMeshFace* face = *It;
			face->KillGLLists();
		}
	}

	{
		std::map<CMeshPosition, CMeshVertex*>::iterator It;
		for(It = m_vertices.begin(); It != m_vertices.end(); It++){
			CMeshVertex* v = It->second;
			v->KillGLLists();
		}
	}
}

static HeeksObj* object_to_add_triangles_to = NULL;

static void add_triangle_to_object(const Point& vt0, const Point& vt1, const Point& vt2, const Point& n0, const Point& n1, const Point& n2){
	if(object_to_add_triangles_to == NULL)return;

	// make a triangle
	float t[3][3] = {{vt0.x, vt0.y, vt0.z}, {vt1.x, vt1.y, vt1.z}, {vt2.x, vt2.y, vt2.z}};
	heeksCAD->STLSolidAddTriangle(object_to_add_triangles_to, t[0]);
}

HeeksObj* CMesh::ConvertToTriangles(){
	if(m_faces.size() == 0)return NULL;
	
	HeeksObj* new_object = heeksCAD->NewSTLSolid();
	
	std::set<CMeshFace*>::iterator It;
	for(It = m_faces.begin(); It != m_faces.end(); It++){
		CMeshFace* face = *It;

		// make lots of small triangles
		object_to_add_triangles_to = new_object;
		face->GetSmoothTriangles(add_triangle_to_object);
	}

	return new_object;
}

void CMesh::SetClickMarkPoint(MarkedObject* marked_object, const double* ray_start, const double* ray_direction){
	{
		HeeksObj* object = marked_object->GetFirstOfEverything();
		while(object){
			if(object->GetType() == MeshVertexType){
				heeksCAD->Mark(object);
				return;
			}

			object = marked_object->Increment();
		}
	}

	{
		HeeksObj* object = marked_object->GetFirstOfEverything();
		while(object){
			if(object->GetType() == MeshEdgeType){
				heeksCAD->Mark(object);
				return;
			}

			object = marked_object->Increment();
		}
	}

	{
		HeeksObj* object = marked_object->GetFirstOfEverything();
		while(object){
			if(object->GetType() == MeshFaceType){
				heeksCAD->Mark(object);
				return;
			}

			object = marked_object->Increment();
		}
	}
}

void CMesh::NormalizeAllVertices(){
	std::map<CMeshPosition, CMeshVertex*>::iterator It;
	for(It = m_vertices.begin(); It != m_vertices.end(); It++){
		CMeshVertex* v = It->second;
		v->NormalizeAllEdgeDirections();
	}
}

bool CMesh::SetAllVertexNormals(){
	bool success = true;
	{
		std::map<CMeshPosition, CMeshVertex*>::iterator It;
		for(It = m_vertices.begin(); It != m_vertices.end(); It++){
			CMeshVertex* v = It->second;
			Point vc = v->get_best_normal_from_edges();
			if(vc.magn() < 0.1){
				success = false;
			}		
			else{
				v->m_norm = vc;
			}
		}
	}

	return success;
}

void CMesh::RecalculateFaceCentres(){
	std::set<CMeshFace*>::iterator It;
	for(It = m_faces.begin(); It != m_faces.end(); It++){
		CMeshFace* face = *It;
		face->RecalculateAGoodCentre();
	}
}

void CMesh::WriteXML(TiXmlNode *root)
{
	TiXmlElement * element = new TiXmlElement( "mesh" );
	root->LinkEndChild( element ); 

	Point p;

	TiXmlElement * faces_element = new TiXmlElement( "faces" );
	element->LinkEndChild( faces_element ); 

	for(std::set<CMeshFace*>::iterator It = m_faces.begin(); It != m_faces.end(); It++)
	{
		CMeshFace* face = *It;
		TiXmlElement * e;
		e = new TiXmlElement( "face" );
		faces_element->LinkEndChild( e ); 

		p = face->m_v[0]->m_position.vertex();
		e->SetDoubleAttribute("ax", p.x);
		e->SetDoubleAttribute("ay", p.y);
		e->SetDoubleAttribute("az", p.z);
		p = face->m_v[1]->m_position.vertex();
		e->SetDoubleAttribute("bx", p.x);
		e->SetDoubleAttribute("by", p.y);
		e->SetDoubleAttribute("bz", p.z);
		p = face->m_v[2]->m_position.vertex();
		e->SetDoubleAttribute("cx", p.x);
		e->SetDoubleAttribute("cy", p.y);
		e->SetDoubleAttribute("cz", p.z);
		p = face->m_edge_dir[0] ? face->m_e[0]->m_c[1].vertex() : face->m_e[0]->m_c[0].vertex();
		e->SetDoubleAttribute("abx", p.x);
		e->SetDoubleAttribute("aby", p.y);
		e->SetDoubleAttribute("abz", p.z);
		p = face->m_edge_dir[0] ? face->m_e[0]->m_c[0].vertex() : face->m_e[0]->m_c[1].vertex();
		e->SetDoubleAttribute("bax", p.x);
		e->SetDoubleAttribute("bay", p.y);
		e->SetDoubleAttribute("baz", p.z);
		p = face->m_edge_dir[1] ? face->m_e[1]->m_c[1].vertex() : face->m_e[1]->m_c[0].vertex();
		e->SetDoubleAttribute("bcx", p.x);
		e->SetDoubleAttribute("bcy", p.y);
		e->SetDoubleAttribute("bcz", p.z);
		p = face->m_edge_dir[1] ? face->m_e[1]->m_c[0].vertex() : face->m_e[1]->m_c[1].vertex();
		e->SetDoubleAttribute("cbx", p.x);
		e->SetDoubleAttribute("cby", p.y);
		e->SetDoubleAttribute("cbz", p.z);
		p = face->m_edge_dir[2] ? face->m_e[2]->m_c[1].vertex() : face->m_e[2]->m_c[0].vertex();
		e->SetDoubleAttribute("cax", p.x);
		e->SetDoubleAttribute("cay", p.y);
		e->SetDoubleAttribute("caz", p.z);
		p = face->m_edge_dir[2] ? face->m_e[2]->m_c[0].vertex() : face->m_e[2]->m_c[1].vertex();
		e->SetDoubleAttribute("acx", p.x);
		e->SetDoubleAttribute("acy", p.y);
		e->SetDoubleAttribute("acz", p.z);
		p = face->m_centre.vertex();
		e->SetDoubleAttribute("mx", p.x);
		e->SetDoubleAttribute("my", p.y);
		e->SetDoubleAttribute("mz", p.z);
	}

	TiXmlElement * vertices_element = new TiXmlElement( "vertices" );
	element->LinkEndChild( vertices_element ); 

	for(std::map<CMeshPosition, CMeshVertex*>::iterator It = m_vertices.begin(); It != m_vertices.end(); It++)
	{
		CMeshVertex* v = It->second;
		TiXmlElement * e;
		e = new TiXmlElement( "v" );
		vertices_element->LinkEndChild( e ); 
		p = v->m_position.vertex();
		e->SetDoubleAttribute("x", p.x);
		e->SetDoubleAttribute("y", p.y);
		e->SetDoubleAttribute("z", p.z);
		p = v->m_norm;
		e->SetDoubleAttribute("dx", p.x);
		e->SetDoubleAttribute("dy", p.y);
		e->SetDoubleAttribute("dz", p.z);
	}

}

// static
HeeksObj* CMesh::ReadFromXMLElement(TiXmlElement* element)
{
	CMesh* new_object = new CMesh();
	Point a, b, c, ab, ba, bc, cb, ca, ac, m;

	TiXmlElement* faces_element = element->FirstChildElement("faces");
	if(faces_element){
		// loop through all the objects
		for(TiXmlElement* pElem = TiXmlHandle(faces_element).FirstChildElement().Element(); pElem; pElem = pElem->NextSiblingElement())
		{
			// they are all faces
			pElem->Attribute("ax", &a.x);
			pElem->Attribute("ay", &a.y);
			pElem->Attribute("az", &a.z);
			pElem->Attribute("bx", &b.x);
			pElem->Attribute("by", &b.y);
			pElem->Attribute("bz", &b.z);
			pElem->Attribute("cx", &c.x);
			pElem->Attribute("cy", &c.y);
			pElem->Attribute("cz", &c.z);
			pElem->Attribute("abx", &ab.x);
			pElem->Attribute("aby", &ab.y);
			pElem->Attribute("abz", &ab.z);
			pElem->Attribute("bax", &ba.x);
			pElem->Attribute("bay", &ba.y);
			pElem->Attribute("baz", &ba.z);
			pElem->Attribute("bcx", &bc.x);
			pElem->Attribute("bcy", &bc.y);
			pElem->Attribute("bcz", &bc.z);
			pElem->Attribute("cbx", &cb.x);
			pElem->Attribute("cby", &cb.y);
			pElem->Attribute("cbz", &cb.z);
			pElem->Attribute("cax", &ca.x);
			pElem->Attribute("cay", &ca.y);
			pElem->Attribute("caz", &ca.z);
			pElem->Attribute("acx", &ac.x);
			pElem->Attribute("acy", &ac.y);
			pElem->Attribute("acz", &ac.z);
			pElem->Attribute("mx", &m.x);
			pElem->Attribute("my", &m.y);
			pElem->Attribute("mz", &m.z);

			new_object->AddTriangle(a, b, c, ab, ba, bc, cb, ca, ac, m);
		}
	}

	Point p;

	TiXmlElement* vertices_element = element->FirstChildElement("vertices");
	if(vertices_element){
		// loop through all the objects
		for(TiXmlElement* pElem = TiXmlHandle(vertices_element).FirstChildElement().Element(); pElem; pElem = pElem->NextSiblingElement())
		{

			// they are all faces
			pElem->Attribute("x", &p.x);
			pElem->Attribute("y", &p.y);
			pElem->Attribute("z", &p.z);
			CMeshVertex* v = new_object->FindVertex(p);
			if(v){
				pElem->Attribute("dx", &v->m_norm.x);
				pElem->Attribute("dy", &v->m_norm.y);
				pElem->Attribute("dz", &v->m_norm.z);
			}
		}
	}

	return new_object;
}

void CMesh::Mirror(double tolerance)
{
	std::set<CMeshFace*> faces_to_delete;
	std::list<CMeshVertex*> vertices_to_move;

	// loop through all the vertices
	for(std::map<CMeshPosition, CMeshVertex*>::iterator It = m_vertices.begin(); It != m_vertices.end(); It++)
	{
		CMeshVertex* v = It->second;
		Point p = v->vertex();
		if(fabs(p.x) < tolerance)
		{
			vertices_to_move.push_back(v);
		}
		else if(p.x < 0)
		{
			// delete all these faces
			for(std::set<CMeshFace*>::iterator ItF = v->m_faces.begin(); ItF != v->m_faces.end(); ItF++)
			{
				CMeshFace* face = *ItF;
				faces_to_delete.insert(face);
			}
		}
	}

	// move the vertices
	for(std::list<CMeshVertex*>::iterator It = vertices_to_move.begin(); It != vertices_to_move.end(); It++)
	{
		CMeshVertex* v = *It;
		Point p = v->vertex();
		p.x = 0.0;
		ChangeVertex(v, p);
	}

	// delete the faces
	for(std::set<CMeshFace*>::iterator It = faces_to_delete.begin(); It != faces_to_delete.end(); It++)
	{
		CMeshFace* face = *It;
		Remove(face);
		delete face;
	}

	// copy and mirror all the existing faces
	std::set<CMeshFace*> face_set = m_faces;
	for(std::set<CMeshFace*>::iterator It = face_set.begin(); It != face_set.end(); It++)
	{
		CMeshFace* face = *It;
		Point a = face->m_v[0]->vertex();
		Point b = face->m_v[1]->vertex();
		Point c = face->m_v[2]->vertex();
		Point ab = FindEdge(face->m_v[0], face->m_v[1])->GetControlPointNearVertex(face->m_v[0]).vertex();
		Point ba = FindEdge(face->m_v[1], face->m_v[0])->GetControlPointNearVertex(face->m_v[1]).vertex();
		Point bc = FindEdge(face->m_v[1], face->m_v[2])->GetControlPointNearVertex(face->m_v[1]).vertex();
		Point cb = FindEdge(face->m_v[2], face->m_v[1])->GetControlPointNearVertex(face->m_v[2]).vertex();
		Point ca = FindEdge(face->m_v[2], face->m_v[0])->GetControlPointNearVertex(face->m_v[2]).vertex();
		Point ac = FindEdge(face->m_v[0], face->m_v[2])->GetControlPointNearVertex(face->m_v[0]).vertex();
		Point centre = face->m_centre.vertex();
		a.x = -a.x;
		b.x = -b.x;
		c.x = -c.x;
		ab.x = -ab.x;
		ba.x = -ba.x;
		bc.x = -bc.x;
		cb.x = -cb.x;
		ca.x = -ca.x;
		ac.x = -ac.x;
		centre.x = -centre.x;
		AddTriangle(a, c, b, ac, ca, cb, bc, ba, ab, centre);
	}
}