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
			}
		break;
	case DLL_PROCESS_DETACH : {
		StopGDIplus();
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