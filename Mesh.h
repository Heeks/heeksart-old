// Mesh.h: interface for the CMesh class.
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.

#if !defined MESH_HEADER_INCLUDED
#define MESH_HEADER_INCLUDED

#pragma once

#include "interface/HeeksColor.h"
#include "interface/HeeksObj.h"
#include "HeeksArtTypes.h"
#include "MeshVertex.h"
#include "MeshEdge.h"
#include "MeshFace.h"

#define EDGE_MAP_IT std::map< const CMeshVertex*, std::map<const CMeshVertex*, CMeshEdge*> >::iterator

class CMesh : public HeeksObj  
{
private:
	std::map<CMeshPosition, CMeshVertex*> m_vertices;
	std::set<CMeshEdge*> m_edges;// edges live here
	std::set<CMeshFace*> m_faces;
	std::map< const CMeshVertex*, std::map<const CMeshVertex*, CMeshEdge*> > m_edge_map;// for finding edges

	CMeshVertex* AddVertex(const Point &v);
	CMeshEdge* AddEdge(CMeshVertex* v1, CMeshVertex* v2, const CMeshPosition& c1, const CMeshPosition& c2, CMeshFace* face, bool *second_face = NULL);
	void DeleteVertices();
	void DeleteEdges();
	void DeleteFaces();
	void render_elements(bool select, bool selected, bool no_colour);
	void set_initial();
	bool RemoveGivenEdge(CMeshEdge* edge);

public:
	static bool m_display_vertices;
	static bool m_display_edges;
	static bool m_display_faces;

	CMesh();
	CMesh(const CMesh &m);
	virtual ~CMesh();

	const CMesh& operator=(const CMesh &m);

	// HeeksObj's virtual functions
	int GetType()const{return MeshType;}
	void glCommands(bool select, bool marked, bool no_color);
	void GetBox(CBox &box);
	const wxChar* GetTypeString(void)const{return _("Mesh");}
	long GetMarkingMask()const{return MARKING_FILTER_UNKNOWN;} // can be selected
	HeeksObj *MakeACopy(void)const;
	wxString GetIcon(){return theApp.GetDllFolder() + _T("/icons/mesh");}
	bool ModifyByMatrix(const double *mat);
	void GetTools(std::list<Tool*>* t_list, const wxPoint* p);
	bool Stretch(const double *p, const double* shift, void* data);
	void Remove(HeeksObj* object);
	void KillGLLists(void);
	void WriteXML(TiXmlNode *root);
	void SetClickMarkPoint(MarkedObject* marked_object, const double* ray_start, const double* ray_direction);

	static HeeksObj* ReadFromXMLElement(TiXmlElement* pElem);

	// member functions
	CMeshFace* AddTriangle(const Point& a, const Point& b, const Point& c, const Point& ab, const Point& ba, const Point& bc, const Point& cb, const Point& ca, const Point& ac, const Point& centre);
	void DestroyChildDisplayLists();
	void KillGLListss();
	void MakeSureDisplayListsExists();
	void GetPropertiesString(char *buffer);
	void ChangeVertex(CMeshVertex* v, const Point& vt);
	HeeksObj* ConvertToTriangles();
	CMeshVertex* FindVertex(const Point &v);
	CMeshEdge* FindEdge(CMeshVertex* v1, CMeshVertex* v2, EDGE_MAP_IT* FindIt1 = NULL, EDGE_MAP_IT* FindIt2 = NULL);
	void NormalizeAllVertices();
	bool SetAllVertexNormals();
	void RecalculateFaceCentres();
	void Mirror(double tolerance);
};

#endif
