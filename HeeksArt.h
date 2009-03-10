// HeeksArt.h
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.

// defines global variables and functions

#include "interface/HeeksCADInterface.h"

extern CHeeksCADInterface* heeksCAD;

class Property;

class CHeeksArtApp{
public:
	wxToolBarBase* m_meshBar;
	int m_num_facets_on_edge;
	wxString m_dll_path;

	CHeeksArtApp();
	~CHeeksArtApp();

	void OnStartUp(CHeeksCADInterface* h, const wxString& dll_path);
	void OnNewOrOpen(bool open);
	void OnInitDLL();
	void OnDestroyDLL();
	void GetOptions(std::list<Property *> *list);
	void OnFrameDelete();
	const wxString& GetDllFolder();
};

extern CHeeksArtApp theApp;

