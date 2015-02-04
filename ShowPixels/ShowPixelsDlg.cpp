
// ShowPixelsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ShowPixels.h"
#include "ShowPixelsDlg.h"
#include "DlgProxy.h"
#include "afxdialogex.h"
#include "MouseHook.h"

#define TIMER_ID 9844

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CShowPixelsDlg dialog




IMPLEMENT_DYNAMIC(CShowPixelsDlg, CDialogEx);

CShowPixelsDlg::CShowPixelsDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CShowPixelsDlg::IDD, pParent)
	, m_Pseudo(_T(""))
	, m_ShowDuration(5)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_hWnd = NULL;
}

CShowPixelsDlg::~CShowPixelsDlg()
{
	Tracer.Format(DEBUG_STREAM_SYSTEM, L"** Synthesis:\n");
	OnSave();
}

void CShowPixelsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_PSEUDO, m_Pseudo);
	DDX_Check(pDX, IDC_CHECK_CLIENT, m_bCheckClientArea);

	DDX_Text(pDX, IDC_EDIT_CATEGORIE, m_Categorie);
	DDX_Text(pDX, IDC_EDIT_COULEUR, m_Couleur);
	DDX_Control(pDX, IDC_TREE, m_Tree);

	DDX_Text(pDX, IDC_EDIT_DURATION, m_ShowDuration);
	DDV_MinMaxInt(pDX, m_ShowDuration, 1, 3600);
	DDX_Text(pDX, IDC_MFCCOLORBUTTON1, m_ColorRefBtn);
	DDX_Control(pDX, IDC_MFCCOLORBUTTON1, m_ColorCtrl);
}

BEGIN_MESSAGE_MAP(CShowPixelsDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_CLOSE()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_GO,  &CShowPixelsDlg::OnBnClickedButtonGo)
	ON_BN_CLICKED(IDC_BUTTON_ADD, &CShowPixelsDlg::OnAdd)
	ON_BN_CLICKED(IDCAPTURE,	 &CShowPixelsDlg::OnCapture)
	ON_NOTIFY(TVN_BEGINLABELEDIT,	IDC_TREE, OnTreeNotification)
	ON_NOTIFY(TVN_ENDLABELEDIT,		IDC_TREE, OnTreeNotification)
	ON_NOTIFY(TVN_SELCHANGING,		IDC_TREE, OnTreeNotification)
	ON_NOTIFY(TVN_SELCHANGED,		IDC_TREE, OnTreeNotification)
	ON_NOTIFY(TVN_GETDISPINFO,		IDC_TREE, OnTreeNotification)
	ON_NOTIFY(TVN_SETDISPINFO,		IDC_TREE, OnTreeNotification)
	ON_NOTIFY(TVN_ITEMEXPANDING,	IDC_TREE, OnTreeNotification)
	ON_NOTIFY(TVN_ITEMEXPANDED,		IDC_TREE, OnTreeNotification)
	ON_NOTIFY(TVN_BEGINDRAG,		IDC_TREE, OnTreeNotification)
	ON_NOTIFY(TVN_KEYDOWN,			IDC_TREE, OnTreeNotification)
	ON_NOTIFY(TVN_SELCHANGED,		IDC_TREE, OnTreeNotification)
	ON_NOTIFY(TVN_SELCHANGING,		IDC_TREE, OnTreeNotification)
	ON_NOTIFY(TVN_ITEMEXPANDING,	IDC_TREE, OnTreeNotification)
	ON_NOTIFY(TVN_ITEMEXPANDED,		IDC_TREE, OnTreeNotification)
	ON_NOTIFY(TVN_DELETEITEM,		IDC_TREE, OnTreeNotification)
	ON_NOTIFY(TVN_GETINFOTIP,		IDC_TREE, OnTreeNotification)
	ON_BN_CLICKED(IDC_BUTTON_SHOW, &CShowPixelsDlg::OnBnClickedButtonShow)
	ON_BN_CLICKED(IDC_BUTTON_RAZ, &CShowPixelsDlg::OnBnClickedButtonRaz)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &CShowPixelsDlg::OnBnClickedButtonSave)
	ON_BN_CLICKED(IDC_BUTTON_LOAD, &CShowPixelsDlg::OnBnClickedButtonLoad)
	ON_BN_CLICKED(IDC_BUTTON_AU3, &CShowPixelsDlg::OnBnClickedButtonAu3)
	ON_BN_CLICKED(IDC_BUTTON_X1, &CShowPixelsDlg::OnBnClickedButtonX1)
	ON_BN_CLICKED(IDC_BUTTON_X2, &CShowPixelsDlg::OnBnClickedButtonX2)
	ON_BN_CLICKED(IDC_BUTTON_X3, &CShowPixelsDlg::OnBnClickedButtonX3)
	ON_NOTIFY(NM_CLICK, IDC_TREE, &CShowPixelsDlg::OnNMClickTree)
	ON_EN_CHANGE(IDC_EDIT_COULEUR, &CShowPixelsDlg::OnEnChangeEditCouleur)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE, &CShowPixelsDlg::OnNMDblclkTree)
	ON_BN_CLICKED(IDC_BUTTON_FIND_SINGLE_COLOR, &CShowPixelsDlg::OnBnClickedButtonFindSingleColor)
	ON_NOTIFY(NM_RCLICK, IDC_TREE, &CShowPixelsDlg::OnNMRClickTree)
	ON_COMMAND(ID_CATEGORY_REPORTALLLASTSCANHITSASFALSEHITS, &CShowPixelsDlg::OnCategoryReportalllastscanhitsasfalsehits)
	ON_COMMAND(ID_CATEGORY_SHOW, &CShowPixelsDlg::OnCategoryShow)
	ON_COMMAND(ID_CATEGORY_CHECKALL, &CShowPixelsDlg::OnCategoryCheckall)
	ON_COMMAND(ID_CATEGORY_UNCHECKALL, &CShowPixelsDlg::OnCategoryUncheckall)
	ON_UPDATE_COMMAND_UI(ID_CATEGORY_REPORTALLLASTSCANHITSASFALSEHITS, &CShowPixelsDlg::OnUpdateCategoryReportalllastscanhitsasfalsehits)
	ON_COMMAND(ID_CATEGORY_UNDOREPORTASBADHITS, &CShowPixelsDlg::OnCategoryUndoreportasbadhits)
	ON_UPDATE_COMMAND_UI(ID_CATEGORY_UNDOREPORTASBADHITS, &CShowPixelsDlg::OnUpdateCategoryUndoreportasbadhits)
	ON_COMMAND(ID_COLOR_SHOWALLPIXELSFROMTHATCOLORONTHECAPTURE, &CShowPixelsDlg::OnColorShowallpixelsfromthatcoloronthecapture)
	ON_UPDATE_COMMAND_UI(ID_COLOR_SHOWALLPIXELSFROMTHATCOLORONTHECAPTURE, &CShowPixelsDlg::OnUpdateColorShowallpixelsfromthatcoloronthecapture)
	ON_COMMAND(ID_CATEGORY_EXPORTBITMAPWITHALLPIXELFROMTAHTCATEGORY, &CShowPixelsDlg::OnCategoryExportbitmapwithallpixelfromtahtcategory)
	ON_WM_TIMER()
	ON_COMMAND(ID_GLOBAL_SAVETHELASTCAPTUREONFILE, &CShowPixelsDlg::OnGlobalSavethelastcaptureonfile)
END_MESSAGE_MAP()


// CShowPixelsDlg message handlers

BOOL CShowPixelsDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	GetDlgItem(IDC_BUTTON_GO)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_SHOW)->EnableWindow(FALSE);	
	SetDebugMode(0x07);
	OnLoad();
	TimerId = SetTimer(TIMER_ID, 50, NULL);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CShowPixelsDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CShowPixelsDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CShowPixelsDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// Automation servers should not exit when a user closes the UI
//  if a controller still holds on to one of its objects.  These
//  message handlers make sure that if the proxy is still in use,
//  then the UI is hidden but the dialog remains around if it
//  is dismissed.

void CShowPixelsDlg::OnClose()
{
	if (CanExit())
		CDialogEx::OnClose();
}

void CShowPixelsDlg::OnOK()
{
	//if (CanExit())
		//CDialogEx::OnOK();
}

void CShowPixelsDlg::OnCancel()
{
	if (CanExit())
		CDialogEx::OnCancel();
}

BOOL CShowPixelsDlg::CanExit()
{
	UnRegisterHook();	
	KillTimer(TimerId);
	return TRUE;
}



void CShowPixelsDlg::OnProcess(bool bDisplay, bool bCount, bool bSingleColor)
{
	int NoSnapShot = 0;
	UpdateData(1);
	if (!SnapShotData::IsSnapShotValid(NoSnapShot, _T("ShowPixels"))) return ;	
	
	// Remplissage map pour optimisation recherche des couleurs
	CMap<int, int, int, int> myMap;
	int iSingleColor=-1;
	if (bSingleColor)
	{
		iSingleColor = CShowPixelsDlg::ColorStruct::CheckColorString( m_Couleur);
		if (iSingleColor==-1) return;
		myMap.SetAt(iSingleColor, -1); // TODO: afficher le comptage
	}
	else
		for (int i=0; i<m_TabColor.GetSize(); i++) {
				if (bCount || m_Tree.GetCheck(m_TabColor[i].hTreeItem)) 
					myMap.SetAt(m_TabColor[i].iColor, i);
			m_TabColor[i].m_CurrentScanCount = 0;
			m_TabColor[i].bPositiveLast = true;
			}
		
	HWND hPreviousWnd = ::GetForegroundWindow();
	::SetForegroundWindow(m_hWnd);
	Sleep(200);
	theApp.BeginWaitCursor();
	GetDlgItem(IDC_BUTTON_GO)->EnableWindow(0);
	GetDlgItem(IDC_BUTTON_SHOW)->EnableWindow(0);
	int screen_pixel_count;
	LPCOLORREF screen_pixel = (LPCOLORREF)GetRawData(NoSnapShot, screen_pixel_count);
	
	DWORD T0 = GetTickCount(); // Démarrage du chrono
	bool odd = true; // Astuce simple pour restituer l'écran dans son état initial (un nombre pair d'inversion des pixels)
	//HDC hdc = NULL;
	//if (bDisplay)
	//	hdc = GtSnapShotData[NoSnapShot].GetDC();
	bool bFirst = true;
	int iColor=0;
	int iSingleCounter=0;
	DuplicateSnapShot(NoSnapShot, NoSnapShot+1);
	int screen_pixel_countDst;
	LPCOLORREF screen_pixel_Dst = (LPCOLORREF)GetRawData(NoSnapShot+1, screen_pixel_countDst);
	while (1) {// Moins de n secondes ou nombre impaire de passages
		odd = !odd;
		
		for (int iCursor = 0; iCursor  < screen_pixel_count; ++iCursor )
			if (myMap.Lookup((int)(screen_pixel[iCursor ]), iColor)) { // A screen pixel has been found to match one of the exact colors we're looking for.
				if (bDisplay) {
					int OldPixel = screen_pixel_Dst[iCursor ];// GtSnapShotData[NoSnapShot+1].SSGetPixel(x, y);
					screen_pixel_Dst[iCursor] = RGB((GetRValue(OldPixel)+128)&0xFF, (GetGValue(OldPixel)+128)&0xFF,	(GetBValue(OldPixel)+128)&0xFF);
					}
				if (bFirst /* && bCount*/)
					if (bSingleColor || iColor<0)
						iSingleCounter++;
					else
						m_TabColor[iColor].m_CurrentScanCount++;
				}
		DrawSnapShot(NoSnapShot+1);				
		bFirst = false;
		UpdateData(0);
		
		if (!bDisplay) break;
		if (( (GetTickCount()-T0)>(1000*(DWORD)m_ShowDuration)) && odd) break;
		}
	if (!bSingleColor) {
		if (bCount) {
			for (int i=0; i<m_TabCategory.GetSize(); i++) { // R.A.Z. Catégories
				m_TabCategory[i].m_CurrentScanCount = 0;
				m_TabCategory[i].m_CumulCount = 0;
				}
			iCurrentCount = 0;
			i64CumulCount = 0;

			for (int i=0; i<m_TabColor.GetSize(); i++) { // Comptage couleurs / catégories
				if (m_TabColor[i].m_CurrentScanCount>0) {
					m_TabCategory[m_TabColor[i].iCategory].m_CurrentScanCount += m_TabColor[i].m_CurrentScanCount;
					iCurrentCount += m_TabColor[i].m_CurrentScanCount;
					i64CumulCount += m_TabColor[i].m_CumulCount;
					if (!bCount) continue;
					m_TabColor[i].m_CumulCount += m_TabColor[i].m_CurrentScanCount;
					m_TabCategory[m_TabColor[i].iCategory].m_CumulCount += m_TabColor[i].m_CumulCount;
					}			
				}
		
			for (int i=0; i<m_TabCategory.GetSize(); i++) { // Pour toutes les catégories repérées, incrément du nombre de passages des ressources de la catégorie
				m_TabCategory[i].bPositiveLast = true;					
				if (!m_TabCategory[i].bDeleted) 
					if (m_TabCategory[i].m_CurrentScanCount)  {
						m_Tree.SetCheck(m_TabCategory[i].hTreeItem, 1);
						m_Tree.SetItemState(m_TabCategory[i].hTreeItem, TVIS_BOLD, TVIS_BOLD);
						m_Tree.Expand(m_TabCategory[i].hTreeItem, TVE_EXPAND);
					}
					else {
						m_Tree.SetCheck(m_TabCategory[i].hTreeItem, 0);
						m_Tree.SetItemState(m_TabCategory[i].hTreeItem, 0, TVIS_BOLD);
						m_Tree.Expand(m_TabCategory[i].hTreeItem, TVE_COLLAPSE);
					}


				}
			for (int i=0; i<m_TabColor.GetSize(); i++) { // Pour toutes les catégories repérées, incrément du nombre de passages des ressources de la catégorie
				if (m_TabCategory[m_TabColor[i].iCategory].m_CurrentScanCount) {
					m_TabColor[i].m_NbScan++;
					m_Tree.SetCheck(m_TabColor[i].hTreeItem, 1);
					}
				else  {
					m_Tree.SetCheck(m_TabColor[i].hTreeItem, 0);
					}
				m_Tree.SetItemState(m_TabColor[i].hTreeItem, m_TabColor[i].m_CurrentScanCount ? TVIS_BOLD : 0, TVIS_BOLD);					
				}
			}

		CString sRes;
		for (int i=0; i<m_TabCategory.GetSize(); i++)
			if (m_TabCategory[i].m_CurrentScanCount)
				if (m_TabCategory[i].bPositiveLast)
					sRes.AppendFormat(L"%s: %d hits - ", m_TabCategory[i].sCategory, m_TabCategory[i].m_CurrentScanCount);
				else
					sRes.AppendFormat(L"%s: %d BAD hits - ", m_TabCategory[i].sCategory, m_TabCategory[i].m_CurrentScanCount);
		SetDlgItemText(IDC_STATIC_STATUS, sRes);
		SetDlgItemText(IDC_STATIC_CUR_COL, L"");
	}
	else
		{// bSingleColor
		CString sRes;
		sRes.Format(L"%d hits in last Capture",iSingleCounter);
		SetDlgItemText(IDC_STATIC_CUR_COL, sRes);
		SetDlgItemText(IDC_STATIC_STATUS, L"");		
	}
	//if (hdc) GtSnapShotData[NoSnapShot].ReleaseDC(hdc);
	theApp.EndWaitCursor();
	GetDlgItem(IDC_BUTTON_GO)->EnableWindow(1);	
	GetDlgItem(IDC_BUTTON_SHOW)->EnableWindow(1);	
	
	::SetForegroundWindow(hPreviousWnd);
	
	InvalidateRect(NULL);
}

// Capture de l'écran ou la fenêtre (avec attente pour donner l'occasion de choisir la fenêtre à capturer si le nom n'est pas saisi)
void CShowPixelsDlg::OnCapture()
{
	HWND hPreviousWnd = ::GetForegroundWindow();
	
	int NoSnapShot = 0;
	UpdateData(1); // On met à jour les variables membres de CShowPixelsDlg en fonction des contrôles
	m_hWnd = NULL;
	if (!m_Pseudo.IsEmpty()) 
		m_hWnd = ::FindWindow(NULL, m_Pseudo);
	
	if (m_hWnd == NULL) { // Le nom de la fenêtre n'est pas saisi ou cette fenêtre n'existe pas
		SetDlgItemText(IDC_COMMENT, L"You have three seconds to select the window you want to capture");
		Sleep(2000);
		SetDlgItemText(IDC_COMMENT, L"Only one second left...");
		Sleep(1000);
		m_hWnd = ::GetForegroundWindow();
		GetForegroundWindow()->GetWindowText(m_Pseudo);
	}
	::GetWindowText(m_hWnd, m_Pseudo.GetBuffer(128),128);
	SetDlgItemText(IDC_COMMENT, CString(L"Captured Window: ")+m_Pseudo+(m_bCheckClientArea?" (client area)":" (Full Window)"));
	
	::SetForegroundWindow(m_hWnd);
	::SetHWnd(m_hWnd, m_bCheckClientArea==TRUE);
	::Sleep(100);	
	SnapShot(0, 0, 0, 0, NoSnapShot);
	
	if (!SnapShotData::IsSnapShotValid(NoSnapShot, _T("ShowPixels"))) {
		GetDlgItem(IDC_BUTTON_GO)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_SHOW)->EnableWindow(FALSE);	
	}
	else {
		GetDlgItem(IDC_BUTTON_GO)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_SHOW)->EnableWindow(TRUE);	
	}
	UpdateData(0); // On demande aux contrôles de refléter l'état des variables membres de CShowPixelsDlg
	::SetForegroundWindow(hPreviousWnd);
	Tracer.Format(DEBUG_STREAM_SYSTEM, L"New capture\n\n");

	return;	
}



// Gestion des couleurs et catégories
CShowPixelsDlg::ColorStruct *CShowPixelsDlg::FindColor(int iColor) {
	for (int i=0; i<m_TabColor.GetSize(); i++)
		if (m_TabColor[i].iColor==iColor) 
			return &m_TabColor[i]; // Trouvé => en renvoie l'adresse de la structure
	return NULL; // Pas trouvé
	}
	
int CShowPixelsDlg::FindColorIndex(int iColor) {
	for (int i=0; i<m_TabColor.GetSize(); i++)
		if (m_TabColor[i].iColor==iColor) 
			return i; // Trouvé => en renvoie l'adresse de la structure
	return -1; // Pas trouvé
	}
	
int CShowPixelsDlg::FindOrAddCategory(CString sCategory, bool bOnlyCheck/*=false*/) {
	sCategory.Trim(); // On supprime les espaces au début et à la fin
	int i;
	for (i=0; i<m_TabCategory.GetSize(); i++)
		if (m_TabCategory[i].sCategory.CompareNoCase(sCategory)==0) 
			return i; // Trouvé => en renvoie l'adresse de la structure
	if (bOnlyCheck) return -1;
	CategoryStruct locCategoryStruct;
	locCategoryStruct.sCategory = sCategory;
	locCategoryStruct.hTreeItem = m_Tree.InsertItem(sCategory);
	m_Tree.SetItemData(locCategoryStruct.hTreeItem, -(m_TabCategory.Add(locCategoryStruct)+1));
	return i; // Pas trouvé
	}
	
	// Ajoute une couleur dans les structures et dans l'arbre. Si la couleur existe déjà, propose de remplacer, effacer ou ignorer
bool CShowPixelsDlg::AddColor(CString sColor, CString sCategorie) {
	ColorStruct locColorStruct;
	locColorStruct.iColor = locColorStruct.CheckColorString(sColor);
	if (locColorStruct.iColor==-1) return false;
	int IndexColor = FindColorIndex(locColorStruct.iColor);
	if (IndexColor != -1) { // La couleur existe déjà
		int iRes = MessageBox(L"This color allready exists.\n\n\tClick YES to delete it ?\n\tNO to change its category into the new one ?\n\tCancel to change nothing ?", m_TabCategory[m_TabColor[IndexColor].iCategory].sCategory,  MB_YESNOCANCEL);
		if (iRes == IDCANCEL) return false; // Si "Cancel", on quitte
		if (iRes == IDYES || iRes == IDNO) { // Suppression de la couleur
			m_Tree.DeleteItem(m_TabColor[IndexColor].hTreeItem); // On supprime la couleur de l'arbre				
			m_TabCategory[m_TabColor[IndexColor].iCategory].DeleteMeIfEmpty(m_Tree); // Et la catégorie aussi si elle est maintenant vide
			m_TabColor.RemoveAt(IndexColor); // On supprime cette couleur du tableau
			RecomputeTree();
			if (iRes == IDYES) return false; // Si "Oui", on supprime juste l'entrée et on ne fait rien d'autre
			}
		}
		
	// A ce stade, la couleur n'existe pas (ou plus). 
	locColorStruct.iCategory = FindOrAddCategory(sCategorie);	
	sColor.Format(L"0x%06X", locColorStruct.iColor); // On retranscrit proprement la couleur en chaîne (écriture univoque)
	locColorStruct.hTreeItem = m_Tree.InsertItem(sColor, m_TabCategory[locColorStruct.iCategory].hTreeItem);
	m_Tree.SetItemData(locColorStruct.hTreeItem, m_TabColor.Add(locColorStruct));
	RecomputeTree();
	return true;
	}


void CShowPixelsDlg::OnAdd() {
	UpdateData(1);
	AddColor(m_Couleur, m_Categorie);	
}




void CShowPixelsDlg::CategoryStruct::DeleteMeIfEmpty( CTreeCtrl &Tree )
{
	if (bDeleted) return;
	if (Tree.GetChildItem(hTreeItem)==NULL) 
	{
		Tree.DeleteItem(hTreeItem);
		hTreeItem = 0;
		bDeleted = true;
		sCategory= "Deleted";
	}
}

int CShowPixelsDlg::ColorStruct::CheckColorString( CString sNewColor )
{
	int iColor;
	if (swscanf(sNewColor, L"%X", &iColor)!=1) { // On vérifie que la couleur est valide
		AfxMessageBox(L"This color is not valid. The color is 6 hexa digits 0xRRGGBB (24 bits)", MB_ICONSTOP|MB_OK);
		return -1;
	}
	return iColor;
}

bool CShowPixelsDlg::ColorStruct::SetColor( CString sNewColor, CShowPixelsDlg *pParentDlg )
{
	int tmpiColor = CheckColorString(sNewColor);
	if (tmpiColor==-1) return false;
	int IndexColor = pParentDlg->FindColorIndex(tmpiColor);
	if (IndexColor != -1) { // La couleur existe déjà
		AfxMessageBox(L"This color allready exists.", MB_ICONSTOP|MB_OK);
		return false;
	}
	m_CumulCount = 0; m_NbScan = 0; m_CurrentScanCount = 0; 
	iColor = tmpiColor;
	return true;
}

void CShowPixelsDlg::OnBnClickedButtonGo()
{
	OnProcess(false, true);
}

void CShowPixelsDlg::OnBnClickedButtonShow()
{
	OnProcess(true, false);
}


void CShowPixelsDlg::OnBnClickedButtonRaz()
{
	int iRes = AfxMessageBox(L"Please hit \n\tYes to delete all data, colors and Categories from your tree, \n\tNo to reset all counters for colors and categories, \n\tor Cancel to do nothing.", MB_YESNOCANCEL);

	if (iRes==IDYES) {
		m_Tree.DeleteAllItems();
		m_TabCategory.RemoveAll();
		m_TabColor.RemoveAll();
		iCurrentCount = 0;
		i64CumulCount = 0;
	}
	if (iRes==IDNO) {
		for (int i=0; i<m_TabCategory.GetCount(); i++) 
			m_TabCategory[i].m_CumulCount = m_TabCategory[i].m_CumulBadCount = m_TabCategory[i].m_CurrentScanCount = m_TabCategory[i].m_NbScan = 0;
		for (int i=0; i<m_TabColor.GetCount(); i++) 
			m_TabColor[i].m_CumulCount = m_TabColor[i].m_CumulBadCount = m_TabColor[i].m_CurrentScanCount = m_TabColor[i].m_NbScan = 0;		
		iCurrentCount = 0;
		i64CumulCount = 0;
	}
}


// =================
// == Sauvegardes ==
// =================
static TCHAR BASED_CODE szFilter[] = _T("Configuration file (*.ini)|*.ini|AutoIt Script file (*.au3)|*.au3|C++ file (*.cpp)|*.cpp|All files (*.*)|*.*||");
static TCHAR BASED_CODE szFilterAu3[] = _T("AutoIt Script file (*.au3)|*.au3|C++ file (*.cpp)|*.cpp|Configuration file (*.ini)|*.ini|All files (*.*)|*.*||");

BOOL winExec(CString fileName)
{
	return CreateProcess(NULL, fileName.GetBuffer(), NULL, NULL, TRUE, SW_NORMAL, NULL, NULL, NULL, NULL);	
}

// Sauvegarde dans base de registre (sFilePath==NULL) ou dans un fichier .ini
void CShowPixelsDlg::SaveAsINI(LPCTSTR sFilePath)
{
	LPCTSTR szOldProfile = theApp.m_pszProfileName;
	LPCTSTR szOldRegistry = theApp.m_pszRegistryKey;
	if (sFilePath) {
		theApp.m_pszProfileName = _tcsdup(sFilePath);
		theApp.m_pszRegistryKey = NULL;
		}
	else { // Quelques informations ne sont sauvegardées qu'en base de registres
		theApp.WriteProfileString(L"General", L"WindowName", m_Pseudo);
		theApp.WriteProfileInt(L"General", L"Duration", m_ShowDuration);
		theApp.WriteProfileInt(L"General", L"ClientOnly", m_bCheckClientArea);		
	}


	CString key, value;
	theApp.WriteProfileInt(L"Color", L"NB", m_TabColor.GetSize());
	for (int i=0; i<m_TabColor.GetSize(); i++) {
		key.Format(L"Color %d", i);
		value.Format(L"0x%06X", m_TabColor[i].iColor);
		theApp.WriteProfileString(key, L"RGB", value); 
		theApp.WriteProfileInt(key, L"Category", m_TabColor[i].iCategory); 		
		TCHAR sCumul[32];
		wsprintf(sCumul, L"%I64d", m_TabColor[i].m_CumulCount);
		theApp.WriteProfileString(key, L"CumulCount", sCumul); 		
		theApp.WriteProfileInt(key, L"NbScan", m_TabColor[i].m_NbScan); 		
		theApp.WriteProfileInt(key, L"CumulBadCount", m_TabColor[i].m_CumulBadCount); 		
		}
	theApp.WriteProfileInt(L"Category", L"NB", m_TabCategory.GetSize());
	for (int i=0; i<m_TabCategory.GetSize(); i++) {
		key.Format(L"Category %d", i);
		theApp.WriteProfileString(key, L"Name", m_TabCategory[i].sCategory); 
		theApp.WriteProfileInt(key, L"Deleted", m_TabCategory[i].bDeleted); 
		}

	if (sFilePath)
	{
		free((void *)theApp.m_pszProfileName);
		theApp.m_pszProfileName = szOldProfile;
		theApp.m_pszRegistryKey = szOldRegistry;
		if (AfxMessageBox(L"The file is written. Do you want to see it ?",MB_YESNO)==IDYES)
			winExec(CString(L"NotePad ") + CString(sFilePath));

	}
}

// Sauvegarde dans un fichier de script au format .au3 
void CShowPixelsDlg::SaveAsAU3(LPCTSTR sFilePath)
{
	if (sFilePath==NULL || m_TabCategory.GetCount()==0) return;
	try {
	CStdioFile fic(sFilePath, CFile::modeCreate | CFile::modeWrite);
	CString sBuf;
	fic.WriteString(L"#cs\t==================================================================================\n");
	sBuf.Format(L"\tAutoIt Script automatically generated on %s by ShowPixel\n\n;\t\t\t (c) FastFrench 2011\n\n", CTime::GetCurrentTime().Format(L"%c"));
	fic.WriteString(sBuf);
	sBuf.Format(L"\t***  %d colors splitted into %d categories ***\n", m_TabColor.GetSize(), m_TabCategory.GetSize());
 	fic.WriteString(sBuf);
	fic.WriteString(L"#ce\t==================================================================================\n\n");
	int iCat,iCol, iNbCol;
	for (iCat=0; iCat<m_TabCategory.GetSize(); iCat++) 
		if (!m_TabCategory[iCat].bDeleted) {
			iNbCol = 0;
			for (iCol=0; iCol<m_TabColor.GetSize(); iCol++) 
				if (m_TabColor[iCol].iCategory==iCat)
					iNbCol++;
			if (!iNbCol) continue;
			sBuf.Format(L"\n;\t-------------------------------\n;\tCategory : %s (%d colors)\n;\t-------------------------------\n", m_TabCategory[iCat].sCategory, iNbCol);
 			fic.WriteString(sBuf);
			CString Id;
			Id.Format(L"$FFNB_%s_COLORS",m_TabCategory[iCat].sCategory);
			Id.MakeUpper();
			sBuf.Format(L"Global const %s=%d\n", Id, iNbCol);
			fic.WriteString(sBuf);
			sBuf.Format(L"Global const $gFF%sColors[%s]=[", m_TabCategory[iCat].sCategory, Id);
			fic.WriteString(sBuf);
			bool bFirst=true;			
			for (iCol=0; iCol<m_TabColor.GetSize(); iCol++) 
				if (m_TabColor[iCol].iCategory==iCat) {
					if (bFirst) {
						sBuf.Format(L"0x%06X", m_TabColor[iCol].iColor);
 						bFirst=false;	
						}
					else
						sBuf.Format(L", 0x%06X", m_TabColor[iCol].iColor);
					fic.WriteString(sBuf);			
				}
			fic.WriteString(L"]\n");
			}	
		
		fic.WriteString(L"\n\n");
		fic.WriteString(L";\t==================================================================================\n");
		fic.WriteString(L";\tFastFind specific code: you can just comment or remove this part if you don't want to use FastFind.au3\n");
		fic.WriteString(L";\t==================================================================================\n\n");
		fic.WriteString(L"#include \"FastFind.au3\" ; Wrapper to FastFind.dll, for lightning-fast pixel searches.\n\n");
		
		fic.WriteString(L"\n\n");
		fic.WriteString(L"global $gFFPos[2] = [0,0]; Last known position, default start for next search\nglobal  $gFFWin[4] = [0, 0, 0, 0]; Borders of default (or last) Window\n");
		fic.WriteString(L"\n\n");
		
		fic.WriteString(L"Func FFFindNext($SearchPos=$gFFPos)\n");
		fic.WriteString(L"; In a 10x10 spot, we're first looking for the nearest spot with at least 4 pure color pixels. \n");
		fic.WriteString(L"; If none, then we're looking for a spot with as much as possible pure color pixels\n");
		fic.WriteString(L"; If still none found, then we're looking for a spot with as much as possible pixels using ShadeVariation of 2\n");
		fic.WriteString(L"; You can adapt those parameters as needed\n");
		fic.WriteString(L"local $Res = FFBestSpot(10, 1, 4, $SearchPos[0], $SearchPos[1], -1, 2, true, $gFFWin[0], $gFFWin[1], $gFFWin[2], $gFFWin[3])\n");
		fic.WriteString(L"If IsArray($Res) Then\n"); 
		fic.WriteString(L"\t$gFFPos[0] = $Res[0]\n");
		fic.WriteString(L"\t$gFFPos[1] = $Res[1]\n");
		fic.WriteString(L"\tFFAddExcludedArea($Res[0]-5, $Res[1]-5, $Res[0]+5, $Res[1]+5); This will avoid to detect same spot again later\n");
		fic.WriteString(L"\treturn $Res\n");
		fic.WriteString(L"Else\n");
		fic.WriteString(L"\tSetError(1)\n");
		fic.WriteString(L"\treturn False\n");
		fic.WriteString(L"EndIf\n");
		
		
		fic.WriteString(L"EndFunc\n\n");

		fic.WriteString(L"Func FFStart($ColorList, $SearchPos=$gFFPos, $WinLimits=$gFFWin, $hWnd=-1) ; Set all settings to start search on a new list of colors\n");
		fic.WriteString(L"\tFFResetExcludedAreas() ; New color list => we reset the restriction rectangles list\n");
		fic.WriteString(L"\tFFResetColors() ; Start with an empty list of colors\n");
		fic.WriteString(L"\tFFAddColor($ColorList)\n");
		fic.WriteString(L"\treturn FFFindNext()\n");
		fic.WriteString(L"EndFunc\n\n\n");

		fic.WriteString(L"\n\n;	==================================================================================\n");
		fic.WriteString(L";	Example of usage - uncomment as needed \n");
		fic.WriteString(L";	==================================================================================\n\n");
		fic.WriteString(L"#cs\n");
		sBuf.Format(L"global $MyWindowHandle = WinGetHandle ( \"%s\")\n", m_Pseudo);
		fic.WriteString(sBuf);
		fic.WriteString(L"If @Error Then $MyWindowHandle = 0 ; If Window not found, entire screen\n");
		fic.WriteString(L"FFSetWnd($MyWindowHandle, True) ; Restricts FastFind Searches to the client area of this specific Window\n");
		
		sBuf.Format(L"for $FFCat = 0 to %d\n",m_TabCategory.GetSize()-1);
		fic.WriteString(sBuf);
		fic.WriteString(L"\tSwitch $FFCat\n");
		for (iCat=0; iCat<m_TabCategory.GetSize(); iCat++) 
			if (!m_TabCategory[iCat].bDeleted) {
				sBuf.Format(L"\t\tCase %d \n",iCat);
				fic.WriteString(sBuf);
				sBuf.Format(L"\t\t\tlocal $SearchResult = FFStart($gFF%sColors)\n",m_TabCategory[iCat]);
				fic.WriteString(sBuf);

			}
		fic.WriteString(L"\tEndSwitch\n\n");
		
		fic.WriteString(L"\twhile (	@Error = 0)\n\n");
		fic.WriteString(L"\t  	; Do here what you want once you find a good Spot. \n");
		fic.WriteString(L"\t  	; Position (center of spot) is in (x:$SearchResult[0], y:$SearchResult[1])\n");
		fic.WriteString(L"\t  	; $SearchResult[2] gives the number of good pixels found in 10x10 spot (1 min, or closest spot with at least 20)\n\n");
		fic.WriteString(L"\t 	$SearchResult = FFFindNext() ; Looks for the next spot\n");
		fic.WriteString(L"\tWend\n\n");
		fic.WriteString(L"Next\n");
		
		fic.WriteString(L"\n#ce\n");
	}
	catch(...) {
		AfxMessageBox(L"Script generation failed");
		return;
	}

	if (AfxMessageBox(L"The script is written. Do you want to display it ?",MB_YESNO)==IDYES)
		winExec(CString(L"NotePad ")+sFilePath);		
}

// Sauvegarde dans un fichier de script au format .CPP 
void CShowPixelsDlg::SaveAsCPP(LPCTSTR sFilePathCPP, LPCTSTR sFilePathH)
{
	if (sFilePathCPP==NULL || sFilePathH==NULL || m_TabCategory.GetCount()==0) return;
	try {
	CStdioFile ficCPP(sFilePathCPP, CFile::modeCreate | CFile::modeWrite);
	CStdioFile ficH(sFilePathH, CFile::modeCreate | CFile::modeWrite);
	CString sBuf;
	sBuf = "//\t==================================================================================\n";
	ficCPP.WriteString(sBuf);
	ficH.WriteString(sBuf);
	sBuf.Format(L"//\tC/C++ code automatically generated on %s by FFShowPixel \n\n//\t\t\t(c) FastFrench 2011\n\n", CTime::GetCurrentTime().Format(L"%c"));
	ficCPP.WriteString(sBuf);
	ficH.WriteString(sBuf);
	sBuf.Format(L"//\t\t***  %d colors splitted into %d categories ***\n", m_TabColor.GetSize(), m_TabCategory.GetSize());
 	ficCPP.WriteString(sBuf);
	ficH.WriteString(sBuf);
	sBuf = "//\t==================================================================================\n\n\n";
	ficCPP.WriteString(sBuf);
	ficH.WriteString(sBuf);
	sBuf = sFilePathH;
	sBuf.Format(L"#include \"%s\"\n", sBuf.Mid(sBuf.ReverseFind('\\')+1));
	ficCPP.WriteString(sBuf);
	int iCat, iCol, iNbCol;
	for (iCat=0; iCat<m_TabCategory.GetSize(); iCat++) 
		if (!m_TabCategory[iCat].bDeleted) {
			iNbCol = 0;
			for (iCol=0; iCol<m_TabColor.GetSize(); iCol++) 
				if (m_TabColor[iCol].iCategory==iCat)
					iNbCol++;
			if (!iNbCol) continue;
			
			// Détermination ID nombre de couleurs
			CString Id;
			Id.Format(L"NB_%s_COLORS",m_TabCategory[iCat].sCategory);
			Id.MakeUpper();

			// Ecriture code .h
			sBuf.Format(L"\n#define %s %d\n", Id, iNbCol);
 			ficH.WriteString(sBuf);
			sBuf.Format(L"extern const int g%sColors[%s];\n", m_TabCategory[iCat].sCategory, Id);
			ficH.WriteString(sBuf);
			
			// Ecriture code .cpp
			sBuf.Format(L"\n//\t-------------------------------\n//\t  Category : %s (%d colors)\n", m_TabCategory[iCat].sCategory, iNbCol);
 			ficCPP.WriteString(sBuf);
			sBuf.Format(L"const int g%sColors[%s]={", m_TabCategory[iCat].sCategory, Id);
			ficCPP.WriteString(sBuf);
			bool bFirst=true;			
			iNbCol = 0;
			for (iCol=0; iCol<m_TabColor.GetSize(); iCol++) 
				if (m_TabColor[iCol].iCategory==iCat) {
					if (!bFirst)
						ficCPP.WriteString(L",");
					else
						bFirst = false;

					if (++iNbCol % 4 == 0) // 4 couleurs par ligne maxi
						ficCPP.WriteString(L"\n\t\t\t");

					sBuf.Format(L"0x%06X", m_TabColor[iCol].iColor); 					
					ficCPP.WriteString(sBuf);			
				}
			ficCPP.WriteString(L"};\n");
			}	
	}
	catch(...) {
		AfxMessageBox(L"Code generation failed.");
		return;
	}

	if (AfxMessageBox(L"Code is generated. Do you want to edit it ? ",MB_YESNO)==IDYES) {
		winExec(CString(L"NotePad ")+sFilePathH);		
		winExec(CString(L"NotePad ")+sFilePathCPP);		
	}
}

void CShowPixelsDlg::OnSave(bool bAskPath, bool bAutoIt) {
	if (bAskPath) // si bAskPath est true, on choisit le fichier de configuration.
	{			  // Dans le cas contraire (par défaut), on utilise la base de registres
		CFileDialog fd(FALSE, bAutoIt?_T(".au3"):_T(".ini"), bAutoIt?_T("ShowPixel.au3"):_T("ShowPixel.ini"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST, bAutoIt?szFilterAu3:szFilter, this);
		if (fd.DoModal()==IDCANCEL) return;
		if (fd.GetFileExt().CompareNoCase(L"AU3")==0)
			SaveAsAU3(fd.GetPathName());
		else
			if (fd.GetFileExt().CompareNoCase(L"CPP")==0 || fd.GetFileExt().CompareNoCase(L"H")==0)
				SaveAsCPP(fd.GetPathName(), fd.GetPathName().Left(fd.GetPathName().GetLength()-3)+"h");
			else
				SaveAsINI(fd.GetPathName());
	}
	else
		SaveAsINI(NULL);
}

void CShowPixelsDlg::OnLoad(bool bAskPath) {
	LPCTSTR szOldProfile = theApp.m_pszProfileName;
	LPCTSTR szOldRegistry = theApp.m_pszRegistryKey;
	if (bAskPath) { // si bAskPath est true, on choisit le fichier de configuration.
				   // Dans le cas contraire (par défaut), on utilise la base de registres
		CFileDialog fd(TRUE, _T(".ini"), _T("ShowPixel.ini"), OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, szFilter, this);
		if (fd.DoModal()==IDCANCEL) return;
		theApp.m_pszProfileName = _tcsdup(fd.GetPathName());
		theApp.m_pszRegistryKey = NULL;
		}
	else { // Quelques informations ne sont sauvegardées qu'en base de registres
		m_Pseudo = theApp.GetProfileString(L"General", L"WindowName", L"");
		m_ShowDuration = theApp.GetProfileInt(L"General", L"Duration", 5);
		m_bCheckClientArea = theApp.GetProfileInt(L"General", L"ClientOnly", 1)==1;		
		}


	CString key, value;
	int NbColor = theApp.GetProfileInt(L"Color", L"NB", 0);
	int NbCategory = theApp.GetProfileInt(L"Category", L"NB", 0);
	m_TabColor.RemoveAll();
	m_TabCategory.RemoveAll();
	int *iCategConvert = new int[NbCategory];
	for (int i=0; i<NbCategory; i++) {
		key.Format(L"Category %d", i);
		CString sCateg = theApp.GetProfileString(key, L"Name", L"Unknown");
		bool bDeleted = theApp.GetProfileInt(key, L"Deleted", 0)==1; 
		if (bDeleted) continue; // on en profite pour filtrer les catégories supprimées
		iCategConvert[i] = FindOrAddCategory(sCateg); // On conserve le nouvel indice pour cette catégorie
		}
	iCurrentCount = 0;
	i64CumulCount = 0;
	int nbColIgnored = 0;
	for (int i=0; i<NbColor; i++) {
		CShowPixelsDlg::ColorStruct locColor;
		key.Format(L"Color %d", i);
		value = theApp.GetProfileString(key, L"RGB", L"0"); 
		swscanf(value, L"%X", &locColor.iColor);
		if (FindColor(locColor.iColor)) {
			nbColIgnored++;
			continue;
		}
		
		CString sRes = theApp.GetProfileString(key, L"CumulCount", NULL); 		
		swscanf(sRes, L"%I64d", &locColor.m_CumulCount);
		locColor.m_CumulBadCount = theApp.GetProfileInt(key, L"CumulBadCount", 0); 		
		
		locColor.m_NbScan = theApp.GetProfileInt(key, L"NbScan", 0); 		
		locColor.iCategory = iCategConvert[theApp.GetProfileInt(key, L"Category", 0)]; 		
		m_TabCategory[locColor.iCategory].m_CumulCount += locColor.m_CumulCount;
		i64CumulCount += locColor.m_CumulCount;

		value.Format(L"0x%06X", locColor.iColor);
		locColor.hTreeItem = m_Tree.InsertItem(value, m_TabCategory[locColor.iCategory].hTreeItem); // Todo : in proper category
		m_Tree.SetItemData(locColor.hTreeItem, i);
		m_TabColor.Add(locColor);
		}
	delete iCategConvert;
	if (nbColIgnored){
		CString sMsg;
		sMsg.Format(L"%d colors ignored, because already in the tree", nbColIgnored);
		AfxMessageBox(sMsg);
		}
	if (bAskPath)
	{
		free((void*)theApp.m_pszProfileName);
		theApp. m_pszProfileName = szOldProfile;
		theApp.m_pszRegistryKey = szOldRegistry;
	}
	UpdateData(0);
}
void CShowPixelsDlg::OnBnClickedButtonSave()
{
	OnSave(true);
}


void CShowPixelsDlg::OnBnClickedButtonLoad()
{
	OnLoad(true);
}


void CShowPixelsDlg::OnBnClickedButtonAu3()
{
	OnSave(true, true);
}
HWND hPreviousWnd = NULL;
void CShowPixelsDlg::OnBnClickedButtonXnStart(int n)
{
	m_n = n-1;
	hPreviousWnd = ::GetForegroundWindow();
	::SetForegroundWindow(m_hWnd);
	//RegisterHook(this);	

}
void CShowPixelsDlg::OnBnClickedButtonXnEnd(POINT *pPt)
{
	//UnRegisterHook();
	int BestColor = -1;
	int BestCount = 0;
	for (int x=pPt->x-m_n; x<=pPt->x+m_n; x++)
		for (int y=pPt->y-m_n; y<=pPt->y+m_n; y++) {
			int ColorSelected = GetPixelFromScreen(x, y, 0);
			int iCount = ColorCount(ColorSelected, 0, 0);
			TRACE(L"OnBnClickedButtonXnEnd : Color 0x%06X at %d,%d => %d pixels\n", ColorSelected, x, y, iCount);				
			if (iCount>BestCount) {
				BestColor = ColorSelected;
				BestCount = iCount;
			}
		}
	
	//int ColorSelected = GtSnapShotData[0].GetPixelFromScreen(pPt->x, pPt->y);
	TRACE(L"OnBnClickedButtonXnEnd : BestColor 0x%08X => %d pixels\n", BestColor, BestCount);
	CString sMsg;
	sMsg.Format(L"BestColor 0x%08X => %d pixels\n", BestColor, BestCount);
	SetDlgItemText(IDC_STATIC_CUR_COL, sMsg);
	//AfxMessageBox(sMsg);
	if (BestColor==-1) 
		AfxMessageBox(L"Position invalide");
	else
	{
		m_Couleur.Format(L"0x%06X", BestColor);
		m_ColorRefBtn = (BestColor&0xFF0000)>>16 | (BestColor&0x00FF00) | (BestColor&0x00FF)<<16;
		m_ColorCtrl.SetColor(m_ColorRefBtn);
		UpdateData(0);
	}
	if (hPreviousWnd) 
		::SetForegroundWindow(hPreviousWnd);
}

void CShowPixelsDlg::OnBnClickedButtonX1()
{
	OnBnClickedButtonXnStart(1);
}


void CShowPixelsDlg::OnBnClickedButtonX2()
{
	OnBnClickedButtonXnStart(2);
}


void CShowPixelsDlg::OnBnClickedButtonX3()
{
	OnBnClickedButtonXnStart(3);
}




void CShowPixelsDlg::OnEnChangeEditCouleur()
{
	UpdateData(1);
	int ColorSelected = CShowPixelsDlg::ColorStruct::CheckColorString( m_Couleur);	
	m_ColorRefBtn = (ColorSelected&0xFF0000)>>16 | (ColorSelected&0x00FF00) | (ColorSelected&0x00FF)<<16;
	m_ColorCtrl.SetColor(m_ColorRefBtn);
	UpdateData(0);

}





void CShowPixelsDlg::OnBnClickedButtonFindSingleColor()
{
	OnProcess(true, false, true);
}



//
// Tree notifications
// 


void CShowPixelsDlg::OnCategoryReportalllastscanhitsasfalsehits()
{

	CShowPixelsDlg::CategoryStruct &cat = m_TabCategory[m_LastIndexMenuEntry];
	ASSERT(cat.bPositiveLast);
	cat.bPositiveLast = false;

	//  Ajustement stats catégorie
	cat.m_CumulCount -= cat.m_CurrentScanCount;
	cat.m_CumulBadCount += cat.m_CurrentScanCount;
	for (int i=0; i<m_TabColor.GetSize(); i++) {
		CShowPixelsDlg::ColorStruct &coul = m_TabColor[i];
		if (coul.iCategory==m_LastIndexMenuEntry) {
			ASSERT(coul.bPositiveLast == true);
			coul.m_CumulCount -= coul.m_CurrentScanCount;
			coul.m_CumulBadCount += coul.m_CurrentScanCount;	
			coul.bPositiveLast = false;
		}
	}

    //m_Tree.Select(cat.hTreeItem, TVGN_CARET);
	//m_Tree.SetLineColor(0x0000FF);
}


void CShowPixelsDlg::OnCategoryShow()
{
	for (int i=0; i<m_TabColor.GetSize(); i++) 
		m_Tree.SetCheck(m_TabColor[i].hTreeItem, (m_TabColor[i].iCategory==m_LastIndexMenuEntry) || (m_LastIndexMenuEntry == 0x7FFFFFFF));
	if (m_LastIndexMenuEntry>=0)
		OnProcess(true, false, false);
}


void CShowPixelsDlg::OnCategoryCheckall()
{
	for (int i=0; i<m_TabColor.GetSize(); i++) {
		CShowPixelsDlg::ColorStruct &coul = m_TabColor[i];
		if ((coul.iCategory==m_LastIndexMenuEntry) || (m_LastIndexMenuEntry == 0x7FFFFFFF)) {
			if (!m_Tree.GetCheck(coul.hTreeItem))
				m_Tree.SetCheck(coul.hTreeItem, 1);
		}
	}
}


void CShowPixelsDlg::OnCategoryUncheckall()
{
	for (int i=0; i<m_TabColor.GetSize(); i++) {
		CShowPixelsDlg::ColorStruct &coul = m_TabColor[i];
		if ((coul.iCategory==m_LastIndexMenuEntry) || (m_LastIndexMenuEntry == 0x7FFFFFFF)) {
			if (m_Tree.GetCheck(coul.hTreeItem))
				m_Tree.SetCheck(coul.hTreeItem, 0);
		}
	}
}

void CShowPixelsDlg::OnCategoryUndoreportasbadhits()
{
	CShowPixelsDlg::CategoryStruct &cat = m_TabCategory[m_LastIndexMenuEntry];
	ASSERT(!cat.bPositiveLast);
	
	//  Ajustement stats catégorie
	cat.m_CumulCount += cat.m_CurrentScanCount;
	cat.m_CumulBadCount -= cat.m_CurrentScanCount;
	for (int i=0; i<m_TabColor.GetSize(); i++) {
		CShowPixelsDlg::ColorStruct &coul = m_TabColor[i];
		if (coul.iCategory==m_LastIndexMenuEntry) {
			ASSERT(coul.bPositiveLast == false);
			coul.m_CumulCount += coul.m_CurrentScanCount;
			coul.m_CumulBadCount -= coul.m_CurrentScanCount;	
			coul.bPositiveLast = true;
		}
	}

    //m_Tree.Select(m_TabCategory[m_LastIndexMenuEntry].hTreeItem, TVGN_CARET);
	//m_Tree.SetLineColor(0x000000);

	m_TabCategory[m_LastIndexMenuEntry].bPositiveLast = true;
}

void CShowPixelsDlg::OnCategoryExportbitmapwithallpixelfromtahtcategory()
{
	int NoSnapShot = 0;
	if (!SnapShotData::IsSnapShotValid(NoSnapShot, _T("ExportBitmap"))) return ;	
	::ResetColors ();
	for (int i=0; i<m_TabColor.GetSize(); i++) 
		if ( (m_TabColor[i].iCategory==m_LastIndexMenuEntry) || (m_LastIndexMenuEntry == 0x7FFFFFFF))
			::AddColor(m_TabColor[i].iColor);
	::KeepColor(NoSnapShot, -1, 1);
	SaveJPG(NoSnapShot, L"FFColors", 95);
	OnCapture();	
	int iSuffix = GetLastFileSuffix();
	CString FileName;
	if (iSuffix)
		FileName.Format(L"ShowPixel%d.JPG", iSuffix);
	else
		FileName = L"ShowPixel.JPG";
	ShellExecute(NULL, L"open", FileName, NULL, NULL, SW_SHOWDEFAULT);
	//WinExec(FileName, SW_SHOWDEFAULT);

}


void CShowPixelsDlg::OnUpdateCategoryReportalllastscanhitsasfalsehits(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_TabCategory[m_LastIndexMenuEntry].bPositiveLast);
}


// Undo possible si bPositiveLast est false
void CShowPixelsDlg::OnUpdateCategoryUndoreportasbadhits(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(!m_TabCategory[m_LastIndexMenuEntry].bPositiveLast);
}

// Affiche tous les pixels de la couleur désignée
// Equivalent à double-clic sur la couleur + le bouton pour montrer la présence de cette couleur
void CShowPixelsDlg::OnColorShowallpixelsfromthatcoloronthecapture()
{
	int iClr = m_TabColor[m_LastIndexMenuEntry].iColor;
	m_Couleur.Format(L"0x%06X", iClr);
	int iCategory = m_TabColor[m_LastIndexMenuEntry].iCategory;
	m_Categorie = m_TabCategory[iCategory].sCategory;
	UpdateData(0);
	OnEnChangeEditCouleur();
	OnProcess(true, false, true);
}


void CShowPixelsDlg::OnUpdateColorShowallpixelsfromthatcoloronthecapture(CCmdUI *pCmdUI)
{
}

typedef struct notif_tag
{
	UINT    nCode;
	TCHAR   sztCodeName[30];
} notifDescrip;

notifDescrip tabNotif[12] = {
	{TVN_ITEMEXPANDING, _T("TVN_ITEMEXPANDING")},
	{TVN_BEGINLABELEDIT, _T("TVN_BEGINLABELEDIT")},
	{TVN_ENDLABELEDIT, _T("TVN_ENDLABELEDIT")},
	{TVN_SELCHANGING, _T("TVN_SELCHANGING")},
	{TVN_SELCHANGED, _T("TVN_SELCHANGED")},
	{TVN_GETDISPINFO, _T("TVN_GETDISPINFO")},
	{TVN_SETDISPINFO, _T("TVN_SETDISPINFO")},
	{TVN_BEGINDRAG, _T("TVN_BEGINDRAG")},
	{TVN_KEYDOWN, _T("TVN_KEYDOWN")},
	{TVN_DELETEITEM, _T("TVN_DELETEITEM")},
	{TVN_ITEMEXPANDED, _T("TVN_ITEMEXPANDED")},
	{TVN_GETINFOTIP, _T("TVN_GETINFOTIP")}
};

LPCWSTR GetNotifyString(UINT NotId)
{
	for (int i=0; i<sizeof(tabNotif)/sizeof(tabNotif[0]); i++)
		if (tabNotif[i].nCode==NotId)
			return tabNotif[i].sztCodeName;
	return L"Unknown";
}


void CShowPixelsDlg::OnNMRClickTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;
    // recuperation position de la souris au moment du click
	CPoint Point;
    DWORD dwPos;
	dwPos = GetMessagePos(); 
	POINTS ScreenPos = MAKEPOINTS(dwPos);
	Point.x = ScreenPos.x;
	Point.y = ScreenPos.y; 
		// conversion des coordonnées screen pour la zone cliente du TreeView
	m_Tree.ScreenToClient( &Point );
	UINT Flags = TVHT_ONITEMLABEL;
		// recuperation de l'item selectionné.
	HTREEITEM ht = m_Tree.HitTest( Point ,&Flags );
	if (ht == NULL){ // Global Menu
		m_LastIndexMenuEntry = 0x7FFFFFFF;
		CMenu menu;
		CMenu* pPopup;
		// the font popup is stored in a resource
		menu.LoadMenu(IDR_MENU_CATEGORY);
		pPopup = menu.GetSubMenu(2); // Tree menu
		//ClientToScreen(&Point);
		pPopup->TrackPopupMenu( TPM_LEFTALIGN|TPM_RIGHTBUTTON, ScreenPos.x, ScreenPos.y, this);		
		return;
	}
	
	int iIndex = (int)m_Tree.GetItemData(ht);
	int iCategory = 0;
	if (iIndex>=0) {
		//int iClr = m_TabColor[iIndex].iColor;
		//m_Couleur.Format(L"0x%06X", iClr);
		//iCategory = m_TabColor[iIndex].iCategory;
		m_LastIndexMenuEntry = iIndex;
		CMenu menu;
		CMenu* pPopup;
		// the font popup is stored in a resource
		menu.LoadMenu(IDR_MENU_CATEGORY);
		pPopup = menu.GetSubMenu(1);
		//ClientToScreen(&Point);
		pPopup->TrackPopupMenu( TPM_LEFTALIGN|TPM_RIGHTBUTTON, ScreenPos.x, ScreenPos.y, this);
	}
	else
	{
		
		iCategory = -(iIndex + 1);
		m_LastIndexMenuEntry = iCategory;
		CMenu menu;
		CMenu* pPopup;

		// the font popup is stored in a resource
		menu.LoadMenu(IDR_MENU_CATEGORY);
		pPopup = menu.GetSubMenu(0);
		pPopup->EnableMenuItem(ID_CATEGORY_REPORTALLLASTSCANHITSASFALSEHITS, (!m_TabCategory[m_LastIndexMenuEntry].bPositiveLast || m_TabCategory[m_LastIndexMenuEntry].m_CurrentScanCount==0)?MF_GRAYED|MF_BYCOMMAND:MF_ENABLED|MF_BYCOMMAND);
		pPopup->EnableMenuItem(ID_CATEGORY_UNDOREPORTASBADHITS, (m_TabCategory[m_LastIndexMenuEntry].bPositiveLast || m_TabCategory[m_LastIndexMenuEntry].m_CurrentScanCount==0)?MF_GRAYED|MF_BYCOMMAND:MF_ENABLED|MF_BYCOMMAND);
		//ClientToScreen(&Point);
		pPopup->TrackPopupMenu( TPM_LEFTALIGN|TPM_RIGHTBUTTON, ScreenPos.x, ScreenPos.y, this );

	}	
}


void CShowPixelsDlg::OnNMDblclkTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;
    // recuperation position de la souris au moment du click
	CPoint Point;
    DWORD dwPos;
	dwPos = GetMessagePos(); 
	Point.x = LOWORD (dwPos);
	Point.y = HIWORD (dwPos); 
		// conversion des coordonnées screen pour la zone cliente du TreeView
	m_Tree.ScreenToClient( &Point );
	UINT Flags = TVHT_ONITEMLABEL;
		// recuperation de l'item selectionné.
	HTREEITEM ht = m_Tree.HitTest( Point ,&Flags );
	if (ht == NULL) return;
	int iIndex = (int)m_Tree.GetItemData(ht);
	int iCategory = 0;
	if (iIndex>=0) {
		int iClr = m_TabColor[iIndex].iColor;
		m_Couleur.Format(L"0x%06X", iClr);
		iCategory = m_TabColor[iIndex].iCategory;
	}
	else
		iCategory = -(iIndex + 1);
	m_Categorie = m_TabCategory[iCategory].sCategory;
	UpdateData(0);
	OnEnChangeEditCouleur();
}

void CShowPixelsDlg::OnNMClickTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;

    // recuperation position de la souris au moment du click
	CPoint Point;
    DWORD dwPos;
	dwPos = GetMessagePos(); 
	Point.x = LOWORD (dwPos);
	Point.y = HIWORD (dwPos); 
		// conversion des coordonnées screen pour la zone cliente du TreeView
	m_Tree.ScreenToClient( &Point );
	UINT Flags = TVHT_ONITEMLABEL;
		// recuperation de l'item selectionné.
	HTREEITEM ht = m_Tree.HitTest( Point ,&Flags );
	//if (m_Tree.GetParentItem(ht) == TVI_ROOT);
}

void CShowPixelsDlg::RecomputeTree()
{
	for (DWORD_PTR i=0; i<(DWORD_PTR)m_TabColor.GetSize(); i++)
		if (m_Tree.GetItemData(m_TabColor[i].hTreeItem)!=i)
			m_Tree.SetItemData(m_TabColor[i].hTreeItem, i);
	for (DWORD_PTR i=0; i<(DWORD_PTR)m_TabCategory.GetSize(); i++)
		if (m_TabCategory[i].hTreeItem && !m_TabCategory[i].bDeleted)
			if (m_Tree.GetItemData(m_TabCategory[i].hTreeItem)!=-((int)i+1))
				m_Tree.SetItemData(m_TabCategory[i].hTreeItem, -((int)i+1));
}

afx_msg void CShowPixelsDlg::OnTreeNotification(LPNMHDR pnmhdr, LRESULT* pResult)
{
	UINT code = pnmhdr->code;
	CString sAction = GetNotifyString(code);
	//TRACE(L"Tree action: %s\n", sAction);
	
	switch (code) {
	case TVN_GETINFOTIP:
		{
			LPNMTVGETINFOTIP lpGetInfoTip = (LPNMTVGETINFOTIP)pnmhdr;
			int iIndice = m_Tree.GetItemData(lpGetInfoTip->hItem);
			if (iIndice>=0) {
				// Tooltip Couleur
				CShowPixelsDlg::ColorStruct &coul = m_TabColor[iIndice];
				int iCategory = m_TabColor[iIndice].iCategory;
				CShowPixelsDlg::CategoryStruct &cat = m_TabCategory[iCategory];					
				int CurTot = cat.m_CurrentScanCount;
				INT64 Tot64 = cat.m_CumulCount;
				int Tot64Err = cat.m_CumulBadCount;
				double LastPrct = CurTot?(100.0*coul.m_CurrentScanCount)/CurTot:0.0;
				double CumulPrct = CurTot?(100.0*coul.m_CurrentScanCount)/CurTot:0.0;
				double ErrPrct = Tot64Err?(100.0 *coul.m_CumulBadCount)/Tot64Err:0.0;
				CString sState;
				if (coul.m_CurrentScanCount)
					if (coul.bPositiveLast)
						sState.Format(L"Last scan : %d good hits -",coul.m_CurrentScanCount);
					else
						sState.Format(L"Last scan : %d BAD hits -",coul.m_CurrentScanCount);
				else
					sState.Format(L"Last scan : - ");
				if (coul.m_NbScan)
					wsprintf(lpGetInfoTip->pszText, L"%6X => %s (%04.1f%%), Cumul:%I64d (%04.1f%%), mean value:%d - Errors : Cumul = %d (%04.1f%%)", 
						coul.iColor, sState, LastPrct,
						coul.m_CumulCount, CumulPrct,  
						(int)(coul.m_CumulCount/coul.m_NbScan), coul.m_CumulBadCount, 
						ErrPrct);
				else
					wsprintf(lpGetInfoTip->pszText, L"%6X => %s (%04.1f%%)", 
						coul.iColor, sState, LastPrct
						);
					
			}
			else { // ToolTip Catégorie
				iIndice = -(iIndice+1);
				CShowPixelsDlg::CategoryStruct &cat = m_TabCategory[iIndice];	
				CString sState;
				if (cat.m_CurrentScanCount)
					if (cat.bPositiveLast)
						sState.Format(L"Last scan : %d good hits -",cat.m_CurrentScanCount);
					else
						sState.Format(L"Last scan : %d BAD hits -",cat.m_CurrentScanCount);
				else
					sState.Format(L"Last scan : - ");
				wsprintf(lpGetInfoTip->pszText, L"%s => Last scan : %s (%2.1f%%), Cumul:%I64d (%2.1f%%)", 
						cat.sCategory, sState, iCurrentCount?(100.0*cat.m_CurrentScanCount)/iCurrentCount:0.0,
						cat.m_CumulCount,i64CumulCount?(100.0*cat.m_CumulCount)/i64CumulCount:0.0);
			}
			break;
		}
	case TVN_BEGINLABELEDIT:
		{
			LPNMTVDISPINFO ptvdi = (LPNMTVDISPINFO) pnmhdr;
			// todo : recherche si il s'agit d'une catégorie, sinon return TRUE pour cancel (?!)
			*pResult = FALSE;
			break;
		}
	case TVN_ENDLABELEDIT:
		{
			LPNMTVDISPINFO ptvdi = (LPNMTVDISPINFO) pnmhdr;
			if (ptvdi->item.pszText!=NULL)
			{ 
				int iIndice = m_Tree.GetItemData(ptvdi->item.hItem);
				if (iIndice>=0)
					// Remplacement d'une Couleur
					*pResult = m_TabColor[iIndice].SetColor(CString(ptvdi->item.pszText), this);
				else {
					// Renommage d'une Catégorie
					iIndice = -(iIndice+1);
					int ExistingIndice = FindOrAddCategory(ptvdi->item.pszText, true);
					if (ExistingIndice!=-1 && ExistingIndice!=iIndice)
					{
						AfxMessageBox(L"This category already exists");
						*pResult = false;
					}
					if (ExistingIndice==-1)
					{
						*pResult = true;
						m_TabCategory[iIndice].sCategory = ptvdi->item.pszText;
					}
				}

			}
			break;
		}
	case TVN_DELETEITEM:
		{
			LPNMTREEVIEW pnmtv = (LPNMTREEVIEW) pnmhdr;

		}
		break;
	}
}





void CShowPixelsDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default

	CDialogEx::OnTimer(nIDEvent);
	if (nIDEvent == TIMER_ID)
		if (::GetForegroundWindow()==m_hWnd)
		{
					if (GetAsyncKeyState(VK_SNAPSHOT) & 0x80000000) //Pause pressed
					{
						HCURSOR hOld = GetCursor();
						while (GetAsyncKeyState(VK_SNAPSHOT) & 0x80000000)
						{
							SetCursor(LoadCursor(NULL, IDC_HAND/*MAKEINTRESOURCE(32649)*/));
							Sleep(100);
						}
						OnCapture();
						SetCursor(hOld);					
					}
					else
					{
						for (int i=0; i<=9; i++)
							if (GetAsyncKeyState('0'+i) & 0x80000000)
							{
								HCURSOR hOld = GetCursor();
								while (GetAsyncKeyState('0'+i) & 0x80000000)
								{
									SetCursor(LoadCursor(NULL, IDC_CROSS/*MAKEINTRESOURCE(32649)*/));
									Sleep(100);
								}
								m_n = i;
								POINT pt;
								GetCursorPos(&pt);
								OnBnClickedButtonXnEnd(&pt);
								SetCursor(hOld);					
							}
					}
		}
}


void CShowPixelsDlg::OnGlobalSavethelastcaptureonfile()
{
	int NoSnapShot = 0;
	if (!SnapShotData::IsSnapShotValid(NoSnapShot, _T("ExportBitmap"))) return ;	
	SaveJPG(NoSnapShot, L"FFCapture", 95);
}
