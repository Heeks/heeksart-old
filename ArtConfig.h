// ArtConfig.h
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.
#include <wx/config.h>
#include <wx/confbase.h>
#include <wx/fileconf.h>

class ArtConfig: public wxConfig
{
public:
	ArtConfig():wxConfig(_T("HeeksArt")){}
	~ArtConfig(){}
};
