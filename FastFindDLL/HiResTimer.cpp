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
//FF - 
// Variables globales
CChrono GChrono;
TCHAR   GsChrono1[ST_CHRONO_SIZE], GsChrono2[ST_CHRONO_SIZE], GsChrono3[ST_CHRONO_SIZE];

//***********************************************
CHiResTimer::CHiResTimer() : m_qwStart(0), m_qwStop(0)
{
	m_bTimerInstalled = QueryPerformanceFrequency((LARGE_INTEGER*)&m_qwFreq) ? true : false;
	if (m_bTimerInstalled) 
		Reset();
}

//***********************************************
void CHiResTimer::Start(void)
{
	Reset();
	QueryPerformanceCounter((LARGE_INTEGER*)&m_qwStart);
}

//***********************************************
void CHiResTimer::Stop(void)
{
	QueryPerformanceCounter((LARGE_INTEGER*)&m_qwStop);
}

//***********************************************
float CHiResTimer::GetTime(void)
{
	float fTime = 0.0;
	if(m_bTimerInstalled && m_qwFreq != 0)
		fTime = (float)((m_qwStop - m_qwStart) / (float)m_qwFreq);

	return fTime;
}

LPCTSTR CHiResTimer::sGetTime(bool bRestart)
{
	if (m_qwStop==0)
		Stop();
	float fTime = GetTime();
	if (fTime<=0.0) return _tcscpy(m_Buf,_T("0"));
	else
	if (fTime<=1.0e-06) _stprintf(m_Buf, _T("%5.3fnS"), fTime*1.0e09);
	else
		if (fTime<=1.0e-03) _stprintf(m_Buf, _T("%5.3fµS"), fTime*1.0e06);
	else
		if (fTime<=1.0) _stprintf(m_Buf, _T("%5.3fmS"), fTime*1.0e03);
	else
		if (fTime<60.0) _stprintf(m_Buf, _T("%6.3fS"), fTime); //ss.mmmS
	else
	{
		long dTime = (long)fTime;
		if (dTime<3600) _stprintf(m_Buf, _T("%02ld:%02ld"), dTime/60, dTime%60); // mm:ss
		else
			_stprintf(m_Buf, _T("%02ld:%02ld:%02d"), dTime/3600, (dTime/60)%60, dTime%60); // hh:mm:ss
	}
		if (bRestart)
			Start();
	return m_Buf;	
}

//***********************************************
void CHiResTimer::Reset(void)
{
	m_qwStart = m_qwStop = 0;
}
