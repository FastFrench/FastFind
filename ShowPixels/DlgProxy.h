
// DlgProxy.h: header file
//

#pragma once

class CShowPixelsDlg;


// CShowPixelsDlgAutoProxy command target

class CShowPixelsDlgAutoProxy : public CCmdTarget
{
	DECLARE_DYNCREATE(CShowPixelsDlgAutoProxy)

	CShowPixelsDlgAutoProxy();           // protected constructor used by dynamic creation

// Attributes
public:
	CShowPixelsDlg* m_pDialog;

// Operations
public:

// Overrides
	public:
	virtual void OnFinalRelease();

// Implementation
protected:
	virtual ~CShowPixelsDlgAutoProxy();

	// Generated message map functions

	DECLARE_MESSAGE_MAP()
	DECLARE_OLECREATE(CShowPixelsDlgAutoProxy)

	// Generated OLE dispatch map functions

	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
};

