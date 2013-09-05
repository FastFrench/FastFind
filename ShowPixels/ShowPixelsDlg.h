
// ShowPixelsDlg.h : header file
//

#pragma once

#define NB_COLOR 7
#include <afxcview.h>
#include "resource.h"		// main symbols
#include "afxcolorbutton.h"


// CShowPixelsDlg dialog
class CShowPixelsDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CShowPixelsDlg);
	friend class CShowPixelsDlgAutoProxy;

// Construction
public:
	CShowPixelsDlg(CWnd* pParent = NULL);	// standard constructor
	virtual ~CShowPixelsDlg();

// Dialog Data
	enum { IDD = IDD_SHOWPIXELS_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
	void RecomputeTree();
	BOOL CanExit();

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnClose();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnCapture();
	afx_msg void OnAdd();
	DECLARE_MESSAGE_MAP()
public:
	CString m_Pseudo;
	afx_msg void OnBnClickedButtonGo();
	BOOL m_bCheckClientArea;
	CString m_Categorie;
	CString m_Couleur;
	CTreeCtrl m_Tree;

	struct CategoryStruct {
		CString    sCategory;		// Valeur de la catégorie (chaine)
		HTREEITEM  hTreeItem;		// handle HTREEITEM dans l'arbre
		bool       bDeleted;
		INT64 m_CumulCount;
		int   m_NbScan;
		int   m_CurrentScanCount;
		CategoryStruct() {bPositiveLast = true; m_CumulBadCount = 0; sCategory = "Unknown"; hTreeItem = 0; bDeleted = false; m_CumulCount = 0; m_NbScan = 0; m_CurrentScanCount = 0;}

		int   m_CumulBadCount;
		bool  bPositiveLast;

		void DeleteMeIfEmpty(CTreeCtrl &Tree);

	};
	CArray<CategoryStruct, CategoryStruct &> m_TabCategory;
	
	struct ColorStruct {
		int        iColor;		// Couleur (entier)
		HTREEITEM  hTreeItem;   // handle HTREEITEM dans l'arbre
		int        iCategory;   // Référence dans le tableau des catégories
		
		int   m_CumulBadCount;
		bool  bPositiveLast;
		// stats
		INT64 m_CumulCount;
		int   m_NbScan;
		int   m_CurrentScanCount;
		static int CheckColorString(CString sNewColor);
		bool SetColor(CString sNewColor, CShowPixelsDlg *pParentDlg);
		ColorStruct() {bPositiveLast = true; m_CumulBadCount = 0; iColor = -1; hTreeItem = 0; iCategory = -1; m_CumulCount = 0; m_NbScan = 0; m_CurrentScanCount = 0; }
	};
	CArray<ColorStruct, ColorStruct &> m_TabColor;

	ColorStruct *FindColor(int iColor);
	
	int FindColorIndex(int iColor);
	
	int FindOrAddCategory(CString sCategory, bool bOnlyCheck=false);
	
	// Ajoute une couleur dans les structures et dans l'arbre. Si la couleur existe déjà, propose de remplacer, effacer ou ignorer
	bool AddColor(CString sColor, CString sCategorie);
	int iCurrentCount;
	INT64 i64CumulCount;
	CStringArray m_tabCategories; // Cat : Nom de la catégorie
	CStringArray m_tabCouleur; // Clr : valeur de la couleur
	CWordArray m_TabClrCat; // Clr : Indice de la catégorie pour chaque couleur
	CPtrArray m_tabHTreeCategorie; // Cat : Entrée dans l'arbre pour chaque catégorie
	CPtrArray m_tabHTreeCouleur; // Clr : Entrée dans l'arbre pour chaque couleur
	void CShowPixelsDlg::OnSave(bool bAskPath=false, bool bAutoIt=false);
	void CShowPixelsDlg::OnLoad(bool bAskPath=false);
	void CShowPixelsDlg::SaveAsAU3(LPCTSTR sFilePath);
	void CShowPixelsDlg::SaveAsINI(LPCTSTR sFilePath);
	void CShowPixelsDlg::SaveAsCPP(LPCTSTR sFilePathCPP, LPCTSTR sFilePathH);


private:
	UINT_PTR TimerId;
	afx_msg void OnTreeNotification(LPNMHDR pnmhdr, LRESULT* pResult);
	HWND m_hWnd;
	void OnProcess(bool bDisplay, bool bCount, bool bSingleColor=false);
	int m_LastIndexMenuEntry;

public:
	// Durée de l'affichage des pixels, en secondes
	int m_ShowDuration;
	afx_msg void OnBnClickedButtonShow();
	afx_msg void OnBnClickedButtonRaz();
	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnBnClickedButtonLoad();
	afx_msg void OnBnClickedButtonAu3();
	afx_msg void OnBnClickedButtonX1();
	afx_msg void OnBnClickedButtonX2();
	afx_msg void OnBnClickedButtonX3();
	void OnBnClickedButtonXnStart(int n);
	int m_n; 
	void OnBnClickedButtonXnEnd(POINT *pPt);
	COLORREF m_ColorRefBtn;
	CMFCColorButton m_ColorCtrl;
	afx_msg void OnNMClickTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeEditCouleur();
	afx_msg void OnNMDblclkTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonFindSingleColor();
	afx_msg void OnNMRClickTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCategoryReportalllastscanhitsasfalsehits();
	afx_msg void OnCategoryShow();
	afx_msg void OnCategoryCheckall();
	afx_msg void OnCategoryUncheckall();
	afx_msg void OnUpdateCategoryReportalllastscanhitsasfalsehits(CCmdUI *pCmdUI);
	afx_msg void OnCategoryUndoreportasbadhits();
	afx_msg void OnUpdateCategoryUndoreportasbadhits(CCmdUI *pCmdUI);
	afx_msg void OnColorShowallpixelsfromthatcoloronthecapture();
	afx_msg void OnUpdateColorShowallpixelsfromthatcoloronthecapture(CCmdUI *pCmdUI);
	afx_msg void OnCategoryExportbitmapwithallpixelfromtahtcategory();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnGlobalSavethelastcaptureonfile();
};
