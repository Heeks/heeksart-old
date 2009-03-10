#if !defined(AFX_MESHDIALOG_H__2C6BFD98_1911_47EA_A185_39851675E8DF__INCLUDED_)
#define AFX_MESHDIALOG_H__2C6BFD98_1911_47EA_A185_39851675E8DF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MeshDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMeshDialog dialog

class CMesh;

class CMeshDialog : public CDialog
{
// Construction
public:
	CMeshDialog(CWnd* pParent = NULL);   // standard constructor

	CMesh* m_mesh;

// Dialog Data
	//{{AFX_DATA(CMeshDialog)
	enum { IDD = IDD_DIALOG4 };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMeshDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMeshDialog)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MESHDIALOG_H__2C6BFD98_1911_47EA_A185_39851675E8DF__INCLUDED_)
