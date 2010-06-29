// HeeksArt.cpp
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.
//

#include "stdafx.h"

#include <wx/stdpaths.h>
#include <wx/dynlib.h>
#include <wx/aui/aui.h>
#include "interface/PropertyString.h"
#include "interface/PropertyList.h"
#include "interface/PropertyInt.h"
#include "interface/Observer.h"
#include "interface/ToolImage.h"
#include "ArtConfig.h"
#include "Mesh.h"

CHeeksCADInterface* heeksCAD = NULL;

CHeeksArtApp theApp;

CHeeksArtApp::CHeeksArtApp(){
	m_meshBar = NULL;
	m_num_facets_on_edge = 8;
}

CHeeksArtApp::~CHeeksArtApp(){
}

void CHeeksArtApp::OnInitDLL()
{
}

void CHeeksArtApp::OnDestroyDLL()
{
#if !defined WXUSINGDLL
	wxUninitialize();
#endif

	heeksCAD = NULL;
}

void OnMeshBar( wxCommandEvent& event )
{
	wxAuiManager* aui_manager = heeksCAD->GetAuiManager();
	wxAuiPaneInfo& pane_info = aui_manager->GetPane(theApp.m_meshBar);
	if(pane_info.IsOk()){
		pane_info.Show(event.IsChecked());
		aui_manager->Update();
	}
}

void OnUpdateMeshBar( wxUpdateUIEvent& event )
{
	wxAuiManager* aui_manager = heeksCAD->GetAuiManager();
	event.Check(aui_manager->GetPane(theApp.m_meshBar).IsShown());
}

#define MESH_CENTRE_VAL 9
#define MESH_CONTROL_VAL 6

static void OnSimpleMesh(wxCommandEvent& event)
{
	CMesh* mesh = new CMesh;
	mesh->AddTriangle(Point(0, 0, -10), Point(0, 10, 0), Point(10, 0, 0), Point(0, MESH_CONTROL_VAL, -10), Point(0, 10, -MESH_CONTROL_VAL), Point(MESH_CONTROL_VAL, 10, 0), Point(10, MESH_CONTROL_VAL, 0), Point(10, 0, -MESH_CONTROL_VAL), Point(MESH_CONTROL_VAL, 0, -10), Point(MESH_CENTRE_VAL, MESH_CENTRE_VAL, -MESH_CENTRE_VAL));
	mesh->AddTriangle(Point(0, 0, -10), Point(-10, 0, 0), Point(0, 10, 0), Point(-MESH_CONTROL_VAL, 0, -10), Point(-10, 0, -MESH_CONTROL_VAL), Point(-10, MESH_CONTROL_VAL, 0), Point(-MESH_CONTROL_VAL, 10, 0), Point(0, 10, -MESH_CONTROL_VAL), Point(0, MESH_CONTROL_VAL, -10), Point(-MESH_CENTRE_VAL, MESH_CENTRE_VAL, -MESH_CENTRE_VAL));
	mesh->AddTriangle(Point(0, 0, -10), Point(0, -10, 0), Point(-10, 0, 0), Point(0, -MESH_CONTROL_VAL, -10), Point(0, -10, -MESH_CONTROL_VAL), Point(-MESH_CONTROL_VAL, -10, 0), Point(-10, -MESH_CONTROL_VAL, 0), Point(-10, 0, -MESH_CONTROL_VAL), Point(-MESH_CONTROL_VAL, 0, -10), Point(-MESH_CENTRE_VAL, -MESH_CENTRE_VAL, -MESH_CENTRE_VAL));
	mesh->AddTriangle(Point(0, 0, -10), Point(10, 0, 0), Point(0, -10, 0), Point(MESH_CONTROL_VAL, 0, -10), Point(10, 0, -MESH_CONTROL_VAL), Point(10, -MESH_CONTROL_VAL, 0), Point(MESH_CONTROL_VAL, -10, 0), Point(0, -10, -MESH_CONTROL_VAL), Point(0, -MESH_CONTROL_VAL, -10), Point(MESH_CENTRE_VAL, -MESH_CENTRE_VAL, -MESH_CENTRE_VAL));
	mesh->AddTriangle(Point(0, 0, 10), Point(10, 0, 0), Point(0, 10, 0), Point(MESH_CONTROL_VAL, 0, 10), Point(10, 0, MESH_CONTROL_VAL), Point(10, MESH_CONTROL_VAL, 0), Point(MESH_CONTROL_VAL, 10, 0), Point(0, 10, MESH_CONTROL_VAL), Point(0, MESH_CONTROL_VAL, 10), Point(MESH_CENTRE_VAL, MESH_CENTRE_VAL, MESH_CENTRE_VAL));
	mesh->AddTriangle(Point(0, 0, 10), Point(0, 10, 0), Point(-10, 0, 0), Point(0, MESH_CONTROL_VAL, 10), Point(0, 10, MESH_CONTROL_VAL), Point(-MESH_CONTROL_VAL, 10, 0), Point(-10, MESH_CONTROL_VAL, 0), Point(-10, 0, MESH_CONTROL_VAL), Point(-MESH_CONTROL_VAL, 0, 10), Point(-MESH_CENTRE_VAL, MESH_CENTRE_VAL, MESH_CENTRE_VAL));
	mesh->AddTriangle(Point(0, 0, 10), Point(-10, 0, 0), Point(0, -10, 0), Point(-MESH_CONTROL_VAL, 0, 10), Point(-10, 0, MESH_CONTROL_VAL), Point(-10, -MESH_CONTROL_VAL, 0), Point(-MESH_CONTROL_VAL, -10, 0), Point(0, -10, MESH_CONTROL_VAL), Point(0, -MESH_CONTROL_VAL, 10), Point(-MESH_CENTRE_VAL, -MESH_CENTRE_VAL, MESH_CENTRE_VAL));
	mesh->AddTriangle(Point(0, 0, 10), Point(0, -10, 0), Point(10, 0, 0), Point(0, -MESH_CONTROL_VAL, 10), Point(0, -10, MESH_CONTROL_VAL), Point(MESH_CONTROL_VAL, 0, 0), Point(10, -MESH_CONTROL_VAL, 0), Point(10, 0, MESH_CONTROL_VAL), Point(MESH_CONTROL_VAL, 0, 10), Point(MESH_CENTRE_VAL, -MESH_CENTRE_VAL, MESH_CENTRE_VAL));
	mesh->SetAllVertexNormals();
	heeksCAD->Add(mesh, NULL);
	heeksCAD->Changed();
}

static void AddToolBars()
{
	wxFrame* frame = heeksCAD->GetMainFrame();
	wxAuiManager* aui_manager = heeksCAD->GetAuiManager();
	if(theApp.m_meshBar)delete theApp.m_meshBar;
	theApp.m_meshBar = new wxToolBar(frame, -1, wxDefaultPosition, wxDefaultSize, wxTB_NODIVIDER | wxTB_FLAT);
	theApp.m_meshBar->SetToolBitmapSize(wxSize(ToolImage::GetBitmapSize(), ToolImage::GetBitmapSize()));
	heeksCAD->AddToolBarButton((wxToolBar*)(theApp.m_meshBar), _("Simple Mesh"), ToolImage(_T("mesh")), _T("Add a simple mesh"), OnSimpleMesh);
	theApp.m_meshBar->Realize();
	aui_manager->AddPane(theApp.m_meshBar, wxAuiPaneInfo().Name(_T("MeshBar")).Caption(_T("Mesh tools")).ToolbarPane().Top());
	heeksCAD->RegisterToolBar(theApp.m_meshBar);
}

void CHeeksArtApp::OnStartUp(CHeeksCADInterface* h, const wxString& dll_path)
{
	m_dll_path = dll_path;
	heeksCAD = h;
#if !defined WXUSINGDLL
	wxInitialize();
#endif

	ArtConfig config;

	// About box, stuff
	heeksCAD->AddToAboutBox(wxString(_T("\n\n")) + _("HeeksArt is a free random add-on to HeeksCAD")
		+ _T("\n") + _("          http://code.google.com/p/heeksart/")
		+ _T("\n") + _("Written by Dan Heeks")
		);

	// add menus and toolbars
	wxFrame* frame = heeksCAD->GetMainFrame();

	// tool bars
	heeksCAD->RegisterAddToolBars(AddToolBars);
	AddToolBars();

	// Mesh menu
	wxMenu *menuMesh = new wxMenu;
	heeksCAD->AddMenuItem(menuMesh, _("Add a simple mesh"), ToolImage(_T("mesh")), OnSimpleMesh);
	frame->GetMenuBar()->Append(menuMesh,  _("Mesh"));

	// add tick boxes for them all on the Window menu
	wxMenu* window_menu = heeksCAD->GetWindowMenu();
	heeksCAD->AddMenuItem(window_menu, _T("Mesh"), wxBitmap(), OnMeshBar, OnUpdateMeshBar, NULL, true);
	heeksCAD->RegisterHideableWindow(m_meshBar);

	// add object reading functions
	heeksCAD->RegisterReadXMLfunction("mesh", CMesh::ReadFromXMLElement);
}

void CHeeksArtApp::OnNewOrOpen(bool open, int res)
{
}

static void on_set_num_facets(int value, HeeksObj* object)
{
	theApp.m_num_facets_on_edge = value;
}

void CHeeksArtApp::GetOptions(std::list<Property *> *list){
	PropertyList* mesh_options = new PropertyList(_("mesh options"));

	mesh_options->m_list.push_back ( new PropertyInt ( _("number of splits"), m_num_facets_on_edge, NULL, on_set_num_facets ) );

	list->push_back(mesh_options);
}

void CHeeksArtApp::OnFrameDelete()
{
}

wxString CHeeksArtApp::GetDllFolder()
{
	return m_dll_path;
}

wxString CHeeksArtApp::GetResFolder()
{
#if defined(WIN32) || defined(RUNINPLACE) //compile with 'RUNINPLACE=yes make' then skip 'sudo make install'
	return m_dll_path;
#else
	return (m_dll_path + _T("/../../share/heekscnc"));
#endif
}

class MyApp : public wxApp
{
 
 public:
 
   virtual bool OnInit(void);
 
 };
 
 bool MyApp::OnInit(void)
 
 {
 
   return true;
 
 }

 DECLARE_APP(MyApp)
 
 IMPLEMENT_APP(MyApp)
 
