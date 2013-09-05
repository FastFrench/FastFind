/*
	FastFind 
	    Copyright 2011 FastFrench (antispam@laposte.net)

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

extern bool GbDofusDistance;
_inline int Distance(int dx, int dy);

void WINAPI setDofusDistanceMode(bool bDofusDistance)
{
	GbDofusDistance = bDofusDistance;
}

// No security check
#define IsRed(Color) (((((Color)>>16)& 0xFF) > (((Color)>>8)& 0xFF)) && ((((Color)>>16)& 0xFF) > ((Color)& 0xFF)) )
#define IsBlue(Color) ((((Color)& 0xFF) > (((Color)>>8)& 0xFF)) && (((Color)& 0xFF) > (((Color)>>16)& 0xFF)) )
#define IsBlack(Color) ( ((Color)& 0xFFFFFF)==0 )

_inline bool IsCenterOfRedCircle(int x, int y, int NoSnapShot, bool bUnchangedAreaInBlack)
{
	SnapShotData &SS = GtSnapShotData[NoSnapShot];
#ifdef ASSERT
	ASSERT(SS.IsInSnapShotRelative(x-22, y-13)); 
	ASSERT(SS.IsInSnapShotRelative(x+23, y+13));
#endif
	if (!IsRed(SS.SSGetPixelRelative(x-19, y)) || 
		!IsRed(SS.SSGetPixelRelative(x+20, y)) || 
		!IsRed(SS.SSGetPixelRelative(x-18, y)) || 
		!IsRed(SS.SSGetPixelRelative(x+19, y)) || 
		!IsRed(SS.SSGetPixelRelative(x-17, y)) || 
		!IsRed(SS.SSGetPixelRelative(x+18, y)) || 
		!IsRed(SS.SSGetPixelRelative(x, y-9)) || 
		!IsRed(SS.SSGetPixelRelative(x, y+9)) ||
		!IsRed(SS.SSGetPixelRelative(x, y+8)) ) return false;

	if (!bUnchangedAreaInBlack) return true; // Pas d'autres tests dans ce cas (risque d'erreur nettement accru)

	if ( !IsBlack(SS.SSGetPixelRelative(x, y-13)) ||
		 !IsBlack(SS.SSGetPixelRelative(x, y+13)) ||
		 !IsBlack(SS.SSGetPixelRelative(x-22, y)) ||
		 !IsBlack(SS.SSGetPixelRelative(x+23, y)) )
		 return false;

	for (int Xr=x-13; Xr<=x+14; Xr++)
		if (!IsBlack(SS.SSGetPixelRelative(Xr, y))) return false;
	for (int Yr=y-5; Yr<=y+4; Yr++)
		if (!IsBlack(SS.SSGetPixelRelative(x, Yr))) return false;
	return true;	
}

_inline bool IsCenterOfBlueCircle(int x, int y, int NoSnapShot, bool bUnchangedAreaInBlack)
{
	SnapShotData &SS = GtSnapShotData[NoSnapShot];
#ifdef ASSERT
	ASSERT(SS.IsInSnapShotRelative(x-22, y-13)); 
	ASSERT(SS.IsInSnapShotRelative(x+23, y+13));
#endif
	if (!IsBlue(SS.SSGetPixelRelative(x-19, y)) || 
		!IsBlue(SS.SSGetPixelRelative(x+20, y)) || 
		!IsBlue(SS.SSGetPixelRelative(x-18, y)) || 
		!IsBlue(SS.SSGetPixelRelative(x+19, y)) || 
		!IsBlue(SS.SSGetPixelRelative(x-17, y)) || 
		!IsBlue(SS.SSGetPixelRelative(x+18, y)) || 
		!IsBlue(SS.SSGetPixelRelative(x, y-9)) || 
		!IsBlue(SS.SSGetPixelRelative(x, y+9)) ||
		!IsBlue(SS.SSGetPixelRelative(x, y+8)) ) return false;

	if (!bUnchangedAreaInBlack) return true; // Pas d'autres tests dans ce cas (risque d'erreur nettement accru)

	if ( !IsBlack(SS.SSGetPixelRelative(x, y-13)) ||
		 !IsBlack(SS.SSGetPixelRelative(x, y+13)) ||
		 !IsBlack(SS.SSGetPixelRelative(x-22, y)) ||
		 !IsBlack(SS.SSGetPixelRelative(x+23, y)) )
		 return false;

	for (int Xr=x-13; Xr<=x+14; Xr++)
		if (!IsBlack(SS.SSGetPixelRelative(Xr, y))) return false;
	for (int Yr=y-5; Yr<=y+4; Yr++)
		if (!IsBlack(SS.SSGetPixelRelative(x, Yr))) return false;
	return true;	
}

bool WINAPI FindRedCircle(int &x0, int &y0, int NoSnapShot, bool bUnchangedAreaInBlack)
{
	if (!SnapShotData::IsSnapShotValid(NoSnapShot, _T("FindRedCircle"))) return false;
	x0 -= GtSnapShotData[NoSnapShot].x1;
	y0 -= GtSnapShotData[NoSnapShot].y1;

	bool bFound = false;
	int BestDistance = 0x7FFFFFFF;
	int BestX=x0, BestY=y0;
	//ASSERT(SS.IsInSnapShotRelative(x-22, y-13)); 
	//ASSERT(SS.IsInSnapShotRelative(x+23, y+13));
	
	for (int x=22; x<GtSnapShotData[NoSnapShot].GetAreaWidth()-23; x++)
		for (int y=13; y<GtSnapShotData[NoSnapShot].GetAreaHeight()-13; y++)
			if (IsCenterOfRedCircle(x, y, NoSnapShot, bUnchangedAreaInBlack))
			{
				bFound = true;
				int nDistance = Distance(x0-x, y0-y);
				if (nDistance<BestDistance)
				{
					BestDistance = nDistance;
					BestX = x;
					BestY = y;
				}
			}
	x0 = BestX + GtSnapShotData[NoSnapShot].x1;
	y0 = BestY + GtSnapShotData[NoSnapShot].y1;
	if (bFound)
	{
		if (Tracer.GraphicDebug() && bFound) { // Si debuggage visuel, on affiche à l'écran la zone trouvée, le point renvoyé et le point de référence fourni
		HDC hdc = GtSnapShotData[NoSnapShot].GetDC();
		if (hdc) {
			HGDIOBJ hOldPen = ::SelectObject( hdc, ::GetStockObject(DC_PEN));
			::SetDCPenColor(hdc, RGB(255,0,0));
			HGDIOBJ hOldBrush = ::SelectObject(hdc, ::GetStockObject(DC_BRUSH));
			::SetDCBrushColor(hdc, RGB(255,0,0));
			::Rectangle(hdc, x0-10, y0-10, x0+10, y0+10); // Affichage du rectangle
			::SelectObject( hdc, hOldPen);
			::SelectObject( hdc, hOldBrush);
			GtSnapShotData[NoSnapShot].ReleaseDC(hdc);	
		}
		}
	}

	return bFound;
}

bool WINAPI FindBlueCircle(int &x0, int &y0, int NoSnapShot, bool bUnchangedAreaInBlack)
{
	if (!SnapShotData::IsSnapShotValid(NoSnapShot, _T("FindRedCircle"))) return false;
	bool bFound = false;
	x0 -= GtSnapShotData[NoSnapShot].x1;
	y0 -= GtSnapShotData[NoSnapShot].y1;
	int BestDistance = 0x7FFFFFFF;
	int BestX=x0, BestY=y0;
	for (int x=22; x<GtSnapShotData[NoSnapShot].GetAreaWidth()-23; x++)
		for (int y=13; y<GtSnapShotData[NoSnapShot].GetAreaHeight()-13; y++)
			if (IsCenterOfBlueCircle(x, y, NoSnapShot, bUnchangedAreaInBlack))
			{
				bFound = true;
				int nDistance = Distance(x0-x, y0-y);
				if (nDistance<BestDistance)
				{
					BestDistance = nDistance;
					BestX = x;
					BestY = y;
				}
			}
	x0 = BestX + GtSnapShotData[NoSnapShot].x1;
	y0 = BestY + GtSnapShotData[NoSnapShot].y1;
	if (bFound)
	{
		if (Tracer.GraphicDebug() && bFound) { // Si debuggage visuel, on affiche à l'écran la zone trouvée, le point renvoyé et le point de référence fourni
		HDC hdc = GtSnapShotData[NoSnapShot].GetDC();
		if (hdc) {
			HGDIOBJ hOldPen = ::SelectObject( hdc, ::GetStockObject(DC_PEN));
			::SetDCPenColor(hdc, RGB(0,0,255));
			HGDIOBJ hOldBrush = ::SelectObject(hdc, ::GetStockObject(DC_BRUSH));
			::SetDCBrushColor(hdc, RGB(0,0,255));
			::Rectangle(hdc, x0-10, y0-10, x0+10, y0+10); // Affichage du rectangle
			::SelectObject( hdc, hOldPen);
			::SelectObject( hdc, hOldBrush);
			GtSnapShotData[NoSnapShot].ReleaseDC(hdc);	
		}
		}
	}
	return bFound;
}
// 0 pour noir
// 1 pour variable
// 2 pour couleur à identifier
/*byte SearchPattern[27][46] = {
	{ 1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1},
	{ 1,1,1,1,1,1,1,1,1,1, 0,0,0,0,0,0,0,1,1,1, 1,1,1,1,1,1,1,1,1,0, 0,0,0,0,0,0,1,1,1,1, 1,1,1,1,1,1},
	{ 1,1,1,1,1,1,1,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0},
	{ 1,1,1,1,1,1,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0},
	{ 1,1,1,1,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0},
	{ 1,1,1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0},
	{ 1,1,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0},
	{ 1,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0},
	{ 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0},
	{ 0,1,1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0},

	{ 0,1,1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0},
	{ 0,1,1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0},
	{ 0,1,1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0},
	{ 0,1,1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0},
	{ 0,1,1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0},
	{ 0,1,1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0},
	{ 0,1,1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0},
	{ 0,1,1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0},
	{ 0,0,1,1,1,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0},
	{ 1,0,1,1,1,1,1,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0},

	{ 1,0,0,1,1,1,1,1,1,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0},
	{ 1,1,0,0,0,1,1,1,1,1, 1,2,2,2,2,2,2,2,2,2, 2,2,2,2,2,2,2,2,2,2, 2,2,2,2,2,1,1,1,1,1, 1,0,0,0,1,1},
	{ 1,1,1,1,0,0,1,1,1,1, 1,1,1,1,2,2,2,2,2,2, 2,2,2,2,2,2,2,2,2,2, 2,2,1,1,1,1,1,1,0,0, 0,0,1,1,1,1},
	{ 1,1,1,1,1,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0},
	{ 1,1,1,1,1,1,1,1,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0},
	{ 1,1,1,1,1,1,1,1,1,1, 1,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0},
	{ 1,1,1,1,1,1,1,1,1,1, 1,1,1,1,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0}
};*/