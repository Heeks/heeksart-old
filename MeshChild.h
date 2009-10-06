// MeshChild.h:
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.

#if !defined MESH_CHILD_HEADER_INCLUDED
#define MESH_CHILD_HEADER_INCLUDED

#pragma once

#include "interface/HeeksObj.h"
#include "HeeksArtTypes.h"

class CMeshChild : public HeeksObj  
{
public:
	int m_gl_list;
	CMeshChild():m_gl_list(0){}
	virtual ~CMeshChild();

	// HeeksObj's virtual functions
	void KillGLLists(void);
	long GetMarkingMask()const{return MARKING_FILTER_UNKNOWN;} // can be selected
};

#endif
