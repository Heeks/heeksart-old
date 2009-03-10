// MeshDialog.cpp : implementation file
//

#include "stdafx.h"
#include "solids.h"
#include "MeshDialog.h"
#include "Mesh.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMeshDialog dialog


CMeshDialog::CMeshDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CMeshDialog::IDD, pParent)
{
	m_mesh = NULL;
	//{{AFX_DATA_INIT(CMeshDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CMeshDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMeshDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMeshDialog, CDialog)
	//{{AFX_MSG_MAP(CMeshDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMeshDialog message handlers

BOOL CMeshDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CheckDlgButton(IDC_CHECK1, m_mesh->m_display_edges ? TRUE:FALSE);
	CheckDlgButton(IDC_CHECK2, m_mesh->m_display_faces ? TRUE:FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CMeshDialog::OnOK() 
{
	m_mesh->m_display_edges = IsDlgButtonChecked(IDC_CHECK1) != 0;
	m_mesh->m_display_faces = IsDlgButtonChecked(IDC_CHECK2) != 0;

	CDialog::OnOK();
}

