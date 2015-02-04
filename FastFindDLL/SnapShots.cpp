/*
	FastFind 
	    Copyright (c) 2010 - 2013 FastFrench (antispam@laposte.net)

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
*/

#include "stdafx.h" // pre-compiled headers


// SnapShotData : This class lanages screen captures in memory
//
SnapShotData GtSnapShotData[NB_SNAP_SHOT_MAX];
bool SnapShotData::bIsScreen16Bits = false;// Capture d'un écran ayant 16 bits / pixel (variable static)

bool SnapShotData::IsSnapShotValid(int NoSnapShot, LPCTSTR sFunctionName)
	{
	if (NoSnapShot<0 || NoSnapShot>=sizeof(GtSnapShotData)/sizeof(GtSnapShotData[0])){
#ifdef MYTRACE
		Tracer.Format(DEBUG_SYSTEM_ERROR, _T("%s : The SnapShot N°%d do not exist\n"), sFunctionName, NoSnapShot);
#endif
		return 0;
		}
	if (GtSnapShotData[NoSnapShot].SnapShotPixels == NULL) {
#ifdef MYTRACE
		Tracer.Format(DEBUG_SYSTEM_ERROR, _T("%s : The SnapShot N°%d is not properly initialized\n"), sFunctionName, NoSnapShot);
#endif
		return 0;
		}
	return true;
	}	


#define DEBUGSNAP 1

// Returns full Client or Window area, with coordinates relative to the Window
void GetFullArea(HWND hWnd, RECT *pRect, bool bClientOnly, bool RelativeToWindow)
{
	ZeroMemory(pRect, sizeof(RECT)); 
	static 	RECT rWindow={0,0,0,0};
	static RECT rClient={0,0,0,0};
	if (RelativeToWindow)
	{
		::GetWindowRect(hWnd, &rWindow);
		if (bClientOnly)
		{
			::GetClientRect(hWnd, &rClient);
			ClientToScreen(hWnd, (POINT*)(&rClient.left));
			ClientToScreen(hWnd, (POINT*)(&rClient.right));
			pRect->right = rClient.right - rWindow.left; 
			pRect->bottom = rClient.bottom - rWindow.top; 
			pRect->left = rClient.left - rWindow.left;
			pRect->top = rClient.top- rWindow.top;
		}	
		else
		{
			pRect->bottom = rWindow.bottom - rWindow.top; 
			pRect->right = rWindow.right - rWindow.left; 
		}
		return;
	}
	// Coordonnées relative à la zone recherchée => left,top = 0,0
	if (bClientOnly)
		::GetClientRect(hWnd, pRect);
	else
	{
		::GetWindowRect(hWnd, &rWindow);
		pRect->right = rWindow.right-rWindow.left;
		pRect->bottom = rWindow.bottom-rWindow.top; 
	}
	
}

// Convert client-relative coordinates to Window-relative coordinates
void ClientToWindow(HWND hWnd, POINT *pPoint)
{
	RECT rWindow={0,0,0,0};
	::GetWindowRect(GhWnd, &rWindow);
	ClientToScreen(GhWnd,  pPoint);
	pPoint->x -= rWindow.left,
	pPoint->y -= rWindow.top;
}

// Makes a capture of the screen in memory 
// If aLeft, aTop, aRight and aBottom are all set to 0, then makes a capture of the full Window. 
int WINAPI SnapShot(int aLeft, int aTop, int aRight, int aBottom, int NoSnapShot)
{

#ifdef MYTRACE
	GChrono.Restart();
#endif
#ifdef MYTRACE
#ifdef DEBUGSNAP
		TCHAR sTitle[128];
		::GetWindowText(GhWnd, sTitle, 128);
		Tracer.Format(DEBUG_STREAM_SYSTEM | DEBUG_SAME_LINE, _T("SnapShot( (%d,%d,%d,%d), %d) hWnd:%08X \"%s\" Client Only:%s => "), aLeft, aTop, aRight, aBottom, NoSnapShot, GhWnd, sTitle, GbClientOnly?"true":"false");
#endif
#endif

	// Vérification des limites (en cas de limites non cohérentes, échec du SnapShot - jusqu'en version 1.5, c'était accepté et adapté). 
	if (NoSnapShot<0 || NoSnapShot>=NB_SNAP_SHOT_MAX)
		{
#ifdef MYTRACE
			Tracer.Format(DEBUG_SYSTEM_ERROR, _T("SnapShot(%d,%d,%d,%d,%d) : NoSnapShot is not in proper range [0 - %d]!\n"), aLeft, aTop, aRight, aBottom, NoSnapShot, NB_SNAP_SHOT_MAX-1);
#endif
			return 0; 		
		}
	if (aLeft < 0 || aTop < 0 || aRight<aLeft || aBottom<aTop ) 
		{
#ifdef MYTRACE
			Tracer.Format(DEBUG_SYSTEM_ERROR, _T("SnapShot(left:%d,top:%d,right:%d,bottom:%d,SnapNb:%d) : SnapShot limits are invalid\n"), aLeft, aTop, aRight, aBottom, NoSnapShot);
#endif
			return 0; 		
		}

	RECT rWindow={0,0,0,0};
	RECT FullRect={0,0,0,0}; // Rectangle correspondant à la fenetre ou zone client complète, en coordonnées relatives à la fenêtre
	GetFullArea(GhWnd, &FullRect, GbClientOnly, true);
			
	if (aRight>(FullRect.right-FullRect.left) || aBottom>(FullRect.bottom-FullRect.top)) // aRight/aLeft/aTop/aBottom are relativ to the target area (either client area, or full window)
		{
#ifdef MYTRACE
			if (Tracer.TextDebug()) 
			{
			TCHAR sTitle[128];
			::GetWindowText(GhWnd, sTitle, 128);
			Tracer.Format(DEBUG_SYSTEM_ERROR, _T("SnapShot(left:%d,top:%d,right:%d,bottom:%d,NoSnapShot:%d) failed : the window \"%s\" %s is not big enough (%d, %d, %d, %d)\n"), aLeft, aTop, aRight, aBottom, NoSnapShot, sTitle, GbClientOnly?"client area":"full window area",  0, 0, FullRect.right-FullRect.left, FullRect.bottom-FullRect.top);
			}
#endif
			return 0; 		
		}
	
#ifdef MYTRACE
#ifdef DEBUGSNAP
		//char sTitle[128];
		//::GetWindowText(GhWnd, sTitle, 128);
		//Tracer.Format(DEBUG_STREAM_SYSTEM | DEBUG_SAME_LINE, _T(" => area modified = (%d,%d,%d,%d) Wnd or Client rect: (%d,%d,%d,%d) name:\"%s\"\n "), aLeft, aTop, aRight, aBottom, rParent.left, rParent.top, rParent.right, rParent.bottom, sTitle);
#endif
#endif

	if (aLeft==0 && aTop==0 && aRight==0 && aBottom==0)
	{
		aLeft=0;  aTop=0; aRight=FullRect.right-FullRect.left; aBottom=FullRect.bottom-FullRect.top;
		if (Tracer.TextDebug()) {
			RECT rParent={0,0,0,0};
			::GetWindowRect(GhWnd, &rParent);
			TCHAR sTitle[128];
			::GetWindowText(GhWnd, sTitle, 128);
#ifdef MYTRACE
			Tracer.Format(DEBUG_STREAM_SYSTEM, _T("SnapShot(N°:%d, HWND:0x%08X) => Window Position (Title:\"%s\") = (%d, %d, %d, %d) => (%d, %d, %d, %d)\n"), NoSnapShot, GhWnd, sTitle, rParent.left, rParent.top, rParent.right, rParent.bottom, aLeft, aTop, aRight, aBottom );	
#endif
		}
	}
	
	if (aLeft>aRight || aTop>aBottom) return 0;
	GtSnapShotData[NoSnapShot].hWnd = GhWnd;
	GtSnapShotData[NoSnapShot].bClientArea = GbClientOnly;
	HDC hdc = GtSnapShotData[NoSnapShot].GetDC();
	GtSnapShotData[NoSnapShot].x1 = aLeft;
	GtSnapShotData[NoSnapShot].y1 = aTop;
	GtSnapShotData[NoSnapShot].x2 = aRight;
	GtSnapShotData[NoSnapShot].y2 = aBottom;
	//GtSnapShotData[NoSnapShot].TopLeftCapture représente le coin sup gauche en coordonnées écran
	GtSnapShotData[NoSnapShot].TopLeftCapture.x = aLeft;
	GtSnapShotData[NoSnapShot].TopLeftCapture.y = aRight;
	if (GtSnapShotData[NoSnapShot].bClientArea)
			ClientToScreen(GtSnapShotData[NoSnapShot].hWnd, &GtSnapShotData[NoSnapShot].TopLeftCapture);
		else {
			RECT Rect;	
			GetWindowRect(GtSnapShotData[NoSnapShot].hWnd, &Rect);
			GtSnapShotData[NoSnapShot].TopLeftCapture.x += Rect.left;
			GtSnapShotData[NoSnapShot].TopLeftCapture.y += Rect.top;
		}
	
	if (!hdc)
	{
#ifdef MYTRACE
		Tracer.Format(DEBUG_SYSTEM_ERROR, _T("SnapShot(%d,%d,%d,%d,N°:%d) : GetDC(%8X) failed\n"), aLeft, aTop, aRight, aBottom, NoSnapShot, GhWnd);
#endif
		return 0; 		
	}

	HDC sdc = NULL;
	HBITMAP hbitmap_screen = NULL;
	HGDIOBJ sdc_orig_select = NULL;
	bool bSuccess = false;

	// Create an empty bitmap to hold all the pixels currently visible on the screen that lie within the search area:
	int search_width = aRight - aLeft + 1;
	int search_height = aBottom - aTop + 1;
	if (   !(sdc = CreateCompatibleDC(hdc))    )  {
#ifdef MYTRACE
		Tracer.Format(DEBUG_SYSTEM_ERROR, _T("SnapShot(%d,%d,%d,%d,%d) : CreateCompatibleDC(%8X) failed\n"), aLeft, aTop, aRight, aBottom, NoSnapShot, hdc);
#endif
		goto end;
		}

	if (   !(hbitmap_screen = CreateCompatibleBitmap(hdc, search_width, search_height))   )  {
#ifdef MYTRACE
		Tracer.Format(DEBUG_SYSTEM_ERROR, _T("SnapShot(%d,%d,%d,%d,%d) : CreateCompatibleBitmap(%8X, %d, %d) failed\n"), aLeft, aTop, aRight, aBottom, NoSnapShot, hdc, search_width, search_height);
#endif
		goto end;
	}

	if (   !(sdc_orig_select = SelectObject(sdc, hbitmap_screen))   ) {
#ifdef MYTRACE
		Tracer.Format(DEBUG_SYSTEM_ERROR, _T("SnapShot(%d,%d,%d,%d,%d) : SelectObject(%8X, %8X) failed\n"), aLeft, aTop, aRight, aBottom, NoSnapShot, sdc, hbitmap_screen);
#endif
		goto end;
	}
	//if (GbClientOnly)
	//	::ClientToWindow(GhWnd, &TopLeftRelativeToWindow);
	// Copy the pixels in the search-area of the screen into the DC to be searched:
	if (   !(BitBlt(sdc, 0, 0, search_width, search_height, hdc, aLeft, aTop, SRCCOPY))   ) {
#ifdef MYTRACE
		Tracer.Format(DEBUG_SYSTEM_ERROR, _T("SnapShot(%d,%d,%d,%d,%d) : BitBlt(relPos:%d,%d) failed\n"), aLeft, aTop, aRight, aBottom, NoSnapShot, aLeft, aTop);
#endif
		goto end;
	}
	
	if (   !(GtSnapShotData[NoSnapShot].SnapShotPixels = getbits(GtSnapShotData[NoSnapShot].SnapShotPixels, hbitmap_screen, sdc, GtSnapShotData[NoSnapShot].lScreenWidth, GtSnapShotData[NoSnapShot].lScreenHeight, GtSnapShotData[NoSnapShot].bIsScreen16Bits))   ){
#ifdef MYTRACE
		Tracer.Format(DEBUG_SYSTEM_ERROR, _T("SnapShot(%d,%d,%d,%d,%d) : getbits() failed\n"), aLeft, aTop, aRight, aBottom, NoSnapShot);
#endif
		goto end;
	}
		// If either is 16-bit, convert *both* to the 16-bit-compatible 32-bit format:
	long screen_pixel_count = GtSnapShotData[NoSnapShot].GetPixelCount();
	
	// Filtre sur les pixels, pour conserver les seules données utiles
	LPCOLORREF screen_pixel = GtSnapShotData[NoSnapShot].SnapShotPixels;
	if (GtSnapShotData[NoSnapShot].bIsScreen16Bits)
	{
		for (int i = 0; i < screen_pixel_count; ++i)
			screen_pixel[i] &= 0x00F8F8F8; 
	}
	else
		for (int i = 0; i < screen_pixel_count; ++i)
			screen_pixel[i] &= 0x00FFFFFF;

	bSuccess = true;
end:
	GtSnapShotData[NoSnapShot].ReleaseDC(hdc);
	if (sdc)
	{
		if (sdc_orig_select) // i.e. the original call to SelectObject() didn't fail.
			SelectObject(sdc, sdc_orig_select); // Probably necessary to prevent memory leak.
		DeleteDC(sdc);
	}
	if (hbitmap_screen)
		DeleteObject(hbitmap_screen);
#ifdef MYTRACE
	Tracer.Format(DEBUG_MB_SYSTEM, _T("SnapShot : %s - hWnd = %8X - Size = %dx%d - \n\nProcessing time: %s\n"), bSuccess ? _T("Success") : _T("Failed"), GhWnd, search_width, search_height, GChrono.GetTime());
#endif		
	return bSuccess;
}

bool WINAPI DuplicateSnapShot(int Src, int Dst)
{
	if (!SnapShotData::IsSnapShotValid(Src, _T("HasChanged (1)"))) return false;	
	if (Dst<0 || Dst>=NB_SNAP_SHOT_MAX)
		{
#ifdef MYTRACE
			Tracer.Format(DEBUG_SYSTEM_ERROR, _T("DuplicateSnapShot(src:%d, dest:%d) : NoSnapShot 'dest' is not in proper range [0 - %d]!\n"), Src, Dst, NB_SNAP_SHOT_MAX-1);
#endif
			return false; 		
		}	
	/*GtSnapShotData[Dst].x1 = GtSnapShotData[Src].x1;
	GtSnapShotData[Dst].x2 = GtSnapShotData[Src].x2;
	GtSnapShotData[Dst].y1 = GtSnapShotData[Src].y1;
	GtSnapShotData[Dst].y2 = GtSnapShotData[Src].y2;
	GtSnapShotData[Dst].hWnd = GtSnapShotData[Src].hWnd;
	GtSnapShotData[Dst].bClientArea = GtSnapShotData[Src].bClientArea;
	GtSnapShotData[Dst].bIsScreen16Bits = GtSnapShotData[Src].bIsScreen16Bits;*/
	COLORREF *pOld = GtSnapShotData[Dst].SnapShotPixels;
	CopyMemory(&GtSnapShotData[Dst], &GtSnapShotData[Src], sizeof(SnapShotData));
	GtSnapShotData[Dst].SnapShotPixels = (LPCOLORREF)realloc(pOld, GtSnapShotData[Src].GetPixelCount() * sizeof(COLORREF)); 
	CopyMemory(GtSnapShotData[Dst].SnapShotPixels, GtSnapShotData[Src].SnapShotPixels, GtSnapShotData[Src].GetPixelCount() * sizeof(COLORREF)); 
	return true;
}

int * WINAPI GetRawData(int NoSnapShot, int &NbBytes)
{
	NbBytes = 0;
	if (!SnapShotData::IsSnapShotValid(NoSnapShot, _T("GetRawData"))) return NULL;	
	NbBytes = GtSnapShotData[NoSnapShot].GetPixelCount()*sizeof(COLORREF);
	return (int *)GtSnapShotData[NoSnapShot].GetPixels();
}

				SnapShotData::SnapShotData()   {ZeroMemory(this, sizeof(SnapShotData));} // On ne fait pas dans le détail => tout à zéro à la construction !
				SnapShotData::~SnapShotData()  {if (SnapShotPixels) free(SnapShotPixels); SnapShotPixels = NULL; }
	int			SnapShotData::GetAreaWidth()	{return x2-x1+1;}
	int			SnapShotData::GetAreaHeight()	{return y2-y1+1;}
	long		SnapShotData::GetPixelCount() {return lScreenWidth * lScreenHeight;}
	LPCOLORREF  SnapShotData::GetPixels()     {return SnapShotPixels;}
	void		SnapShotData::ReleasePixels() {} // Fonction non utilisée, prévue si un mécanisme de verrouillage de la mémoire est utilisé (GlobalAlloc)
	void		SnapShotData::AdjustColor(COLORREF &Color) { if (bIsScreen16Bits) Color &= 0x00F8F8F8; else Color &= 0x00FFFFFF; }
	void		SnapShotData::AdjustColor(int &Color) { if (bIsScreen16Bits) Color &= 0x00F8F8F8; else Color &= 0x00FFFFFF; }
	COLORREF	SnapShotData::SSGetPixel(int x, int y) {return SnapShotPixels[(x-x1) + (y-y1)*lScreenWidth];}
	COLORREF	SnapShotData::SSGetPixelRelative(int x, int y) {return SnapShotPixels[x + y*lScreenWidth];}
	void SnapShotData::SSSetPixel(int x, int y, COLORREF color) {SnapShotPixels[(x-x1) + (y-y1)*lScreenWidth] = color;}
	bool		SnapShotData::IsInSnapShot(int x, int y) {return (x>=x1 && x<=x2 && y>=y1 && y<=y2);}	
	bool		SnapShotData::IsInSnapShotRelative(int x, int y) {return (x>=0 && x<GetAreaWidth() && y>=0 && y<=GetAreaHeight());}	
	int SnapShotData::GetPixelFromScreen(int x, int y) {
			x -= TopLeftCapture.x;
			y -= TopLeftCapture.y;
			if (x<0 || x>=GetAreaWidth() || y<0 || y>=GetAreaHeight())
				return -1;
			return (int)SSGetPixel(x, y);
		}
	
int WINAPI GetPixelFromScreen(int x, int y, int NoSnapShot)
	{
		return GtSnapShotData[NoSnapShot].GetPixelFromScreen(x, y);
	}

void SnapShotData::ReleaseDC(HDC hdc)
{
	::ReleaseDC(hWnd, hdc);
}
	
HDC   SnapShotData::GetDC()
{
	if (bClientArea)
		return ::GetDC(hWnd);
	else
		return ::GetWindowDC(hWnd);
}
	
	// x et y sont ici relatifs à la zone de capture (et non à l'écran ou la fenêtre associée à la capture)
bool SnapShotData::_IsExcluded(int x, int y) {
		return ::IsExcluded(x+x1, y+y1, hWnd); // => coordonnées de la fenetre associée à la capture (ou zone client)
}
