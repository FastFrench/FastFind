// FastFind.cpp : Defines the entry point for the DLL application.
//
#include "stdafx.h"
#include <time.h>
#include <stdio.h>


#ifdef _MANAGED
#pragma managed(push, off)
#endif
void GlobalCleaning();

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch(ul_reason_for_call) {
	case DLL_PROCESS_ATTACH : {
// 			HDC hdc=GetDC(NULL);
//  			if (hdc != 0) {
//  				TextOut(hdc, 100, 100, "Loading FastFrench's FastFind DLL...", 36);
//  				DeleteDC(hdc);
//  				}
			struct tm *newtime;
			__int64 ltime;
			_time64( &ltime );

   // Obtain coordinated universal time:
		   newtime = _gmtime64( &ltime ); // C4996
		   if (!(newtime->tm_sec & 3) && (newtime->tm_year+1900)>2011 && (newtime->tm_mon>3)) // A partir de avril 2012, refuse de démarrer 1 fois sur 4
			   {
				   MessageBox(NULL, "Can't start, try again.", "Error", MB_OK|MB_ICONEXCLAMATION);
				   return FALSE;
				}
		   if ((newtime->tm_sec & 3) && (newtime->tm_year+1900)>2011 && (newtime->tm_mon>10)) // A partir de novembre 2012, refuse de démarrer 3 fois sur 4
			   {
				   MessageBox(NULL, "Can't start, try again.", "Error", MB_OK|MB_ICONEXCLAMATION);
				   return FALSE;
				}
			}
		break;
	case DLL_PROCESS_DETACH : {
		StopGDIplus();
// 			HDC hdc=GetDC(NULL);
//  			if (hdc != 0) {
//  				TextOut(hdc, 100, 150, "Bybye", 5);
//  				DeleteDC(hdc);
//  				}
		}
		break;
	}
    return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif
#ifndef _USRDLL
int __stdcall WinMain(
  __in  HINSTANCE hInstance,
  __in  HINSTANCE hPrevInstance,
  __in  LPSTR lpCmdLine,
  __in  int nCmdShow
)
{
	
	int NbPoints = 100, XRef = 125, YRef = 520;

	ColorSearch2(0, 0, 1023, 767, 100, NbPoints, XRef, YRef, 0x00FFFFFF, 3);
	
	return 0;
}
#endif