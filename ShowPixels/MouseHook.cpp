#include "stdafx.h"
#include "MouseHook.h"
#include "ShowPixelsDlg.h"
/*
wParam :
WM_MOUSEMOVE = 0x200,
WM_LBUTTONDOWN = 0x201,
WM_LBUTTONUP = 0x202,
WM_LBUTTONDBLCLK = 0x203,
WM_RBUTTONDOWN = 0x204,
WM_RBUTTONUP = 0x205,
WM_RBUTTONDBLCLK = 0x206,
WM_MBUTTONDOWN = 0x207,
WM_MBUTTONUP = 0x208,
WM_MBUTTONDBLCLK = 0x209,
WM_MOUSEWHEEL = 0x20A,
WM_XBUTTONDOWN = 0x20B,
WM_XBUTTONUP = 0x20C,
WM_XBUTTONDBLCLK = 0x20D,
WM_MOUSEHWHEEL = 0x20E


*/
HHOOK hMousHook = NULL;
CShowPixelsDlg *pClass = NULL;
void RegisterHook(CShowPixelsDlg *pParent)
{
	if (hMousHook!=NULL) return;
	pClass = pParent;
	hMousHook = SetWindowsHookEx(WH_MOUSE_LL,  LowLevelMouseProc, GetModuleHandle(NULL), 0);
}

void UnRegisterHook()
{
	if (hMousHook!=NULL)
		UnhookWindowsHookEx(hMousHook);
	hMousHook = NULL;
}
//HOOKPROC
//SetWindowsHookEx(
LRESULT CALLBACK LowLevelMouseProc(
  __in  int nCode,
  __in  WPARAM wParam,
  __in  LPARAM lParam
)
{			
	 if (nCode == HC_ACTION)
    {
		UnRegisterHook();
	
		//static bool hasBeenEntered = false;
		//if(hasBeenEntered){ //Prevent the mouse event being sent to other hook procs and the window procedure.
							//Might not be what you want.
		//	return 1;
		//}
		//hasBeenEntered = true;
		//acquire lock, call windows API functions
		if (wParam==WM_LBUTTONDOWN || wParam==WM_RBUTTONDOWN)
		{
			MSLLHOOKSTRUCT *pMouseStruct = (MSLLHOOKSTRUCT *)lParam;
			//if (wParam==WM_LBUTTONDOWN)
			//	TRACE(L"Left Mouse Down in (%d,%d)\n",pMouseStruct->pt.x, pMouseStruct->pt.y);
			//else
			//	TRACE(L"Right Mouse Down in (%d,%d)\n",pMouseStruct->pt.x, pMouseStruct->pt.y);
			if (pClass)
				pClass->CShowPixelsDlg::OnBnClickedButtonXnEnd(&pMouseStruct->pt);

		}

		//hasBeenEntered = false;
	 }
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}
