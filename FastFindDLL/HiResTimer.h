// Gestion simplifiée des timer très haute résolution
// Ecrit par FastFrench - 2011
#ifndef _HIRESTIMER_H_
#define _HIRESTIMER_H_

//#include "Common.h"
#ifndef QWORD
typedef unsigned __int64 QWORD;
#endif
#define ST_CHRONO_SIZE 32

class CHiResTimer
{
public:
	CHiResTimer();
	virtual ~CHiResTimer() {}

	bool IsTimerInstalled(void) const {return m_bTimerInstalled;}

	void Start(void);
	void Stop(void);
	float GetTime(void);
	LPCTSTR sGetTime(bool bRestart=false);
	void Reset(void);

protected:
	TCHAR m_Buf[ST_CHRONO_SIZE];
	bool m_bTimerInstalled;
	QWORD m_qwStart, m_qwStop, m_qwFreq;
};

// Classe simplifiée : le constructeur lance le chrono. GetTime permet d'avoir le résultat. 
class CChrono : private CHiResTimer
{
public : 
	CChrono() : CHiResTimer()  {Start();}
	LPCTSTR GetTime(bool bRestart=false) {return sGetTime(bRestart);}
	void Restart() {Start();}

};

// Variable globale
extern CChrono GChrono;
extern TCHAR   GsChrono1[ST_CHRONO_SIZE], GsChrono2[ST_CHRONO_SIZE], GsChrono3[ST_CHRONO_SIZE];

#endif
