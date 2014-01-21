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

#include "StdAfx.h"
#include "Tracer.h"
#include <conio.h>
#include <stdarg.h>
#include <varargs.h>
#include <time.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include <string.h>

// Global / static objects
CTracer Tracer;
TCHAR CTracer::m_sErrorMsg[1024]="";


// Class CTracer
CTracer::CTracer(int DebugMode, const char* _sFileName, bool _bAppend)
{
	m_FichierBackup = NULL;
	ChangeMode(DebugMode);
	Open(_sFileName, _bAppend);	
}

// Open the file for MYTRACE, if needed
void CTracer::Open(const char* _sFileName, bool _bAppend)
{
	if (_sFileName && (DEBUG_FILE & m_DebugModeTxt))
		if (m_FichierBackup)
			m_Fichier = m_FichierBackup;
		else
		{
			m_FichierBackup = m_Fichier = fopen(_sFileName, _bAppend ? "a":"w");			
		    time_t ltime;
			char timebuf[26];
			time( &ltime);

			errno_t err = ctime_s(timebuf, 26, &ltime);
			fprintf(m_Fichier,  _T("\n****************************** FastFind %s (c)FastFrench 2013 ********\n\t\tStarted"), FFVersion() );
			if (err)
				fprintf(m_Fichier,  _T("\n"));
			else
				fprintf(m_Fichier,  _T(" %26s\n"), timebuf );
		}
	else
		m_Fichier = NULL;
}

// Changement du paramétrage des fonctions de debuggage
void CTracer::ChangeMode(int NewDebugMode, const char* _sFileName, bool _bAppend)
{
	m_bDebugGraphique = (NewDebugMode & DEBUG_GRAPHIC)>0;
	if ( ((NewDebugMode & DEBUG_ORIGIN_BITS) == DEBUG_ORIGIN_BITS)) // Si toutes les origines - incluant les erreurs - sont exclues
		m_DebugModeTxt = 0;
	else {
		m_DebugModeTxt = NewDebugMode & (0xFFFFFFFF ^ DEBUG_GRAPHIC);
		if (m_DebugModeTxt & DEBUG_NOSYSTEM) // Si on ne veut pas les traces système, on ne veut pas non plus le détail
			m_DebugModeTxt |= DEBUG_NOSYSTEM_DETAILS;		
	}
	Open(_sFileName, _bAppend);
	Format(DEBUG_STREAM_SYSTEM, _T("ChangeMode(%d) => m_bDebugGraphique=%d, m_DebugModeTxt=%4X\n"), NewDebugMode, m_bDebugGraphique, m_DebugModeTxt);
}

CTracer::~CTracer(void)
{
	if (m_FichierBackup)
		fclose(m_FichierBackup);
	m_FichierBackup = NULL;
	StopGDIplus(); // On le met arbitrairement dans ce destructeur, en principe appelé une seule fois
}

/* Explications à propos du Filtre :
#define DEBUG_NONE    0x00000000
#define DEBUG_CONSOLE 0x00000001
#define DEBUG_FILE    0x00000002
#define DEBUG_GRAPHIC 0x00000004
#define DEBUG_MSGBOX  0x00000008

#define DEBUG_NOSYSTEM          0x00000010 // Traces internes de la DLL
#define DEBUG_NOSYSTEM_DETAILS  0x00000020 // Traces internes de la DLL (détails)
#define DEBUG_NOUSER            0x00000040 // Traces externes (de l'application)
#define DEBUG_NOTHING           0x00000070 // Désactivation de toutes les traces

#define DEBUG_ERROR             0x00000080 // Message d'erreur (prioritaire)

#define DEBUG_SAME_LINE         0x00080000 // Astuce pour poursuivre sur la même ligne, sans TimeStamp
*/

// La méthode Format permet d'envoyer le message de debug sur les différents canaux sélectionnés. Le filtre précise si, 
// en fonction du paramètre qui a été passé à ChangeMode, le message doit être ignoré pour pas. 
void CTracer::Format(int Filtre, const TCHAR * format, ...)
{
	if ( !m_DebugModeTxt ) return; // Optimisation du fonctionnement quand le debuggage est totalement désactivé ou purement graphique
	if ((Filtre & DEBUG_ORIGIN_BITS) & m_DebugModeTxt) return; // Filtre selon l'origine du message
	
	int OutModes = (Filtre & m_DebugModeTxt); // Says the chanels to use
	
	// Force use of all chanels for errors
	if ((Filtre & DEBUG_ERROR)>0) 
		OutModes = DEBUG_CHANNEL_BITS; // Al chanels active 
	
	if (OutModes == 0) return; // To chanel selected

	va_list args;
		 
	if ((OutModes & DEBUG_FILE) && m_Fichier) { // File output
		if ((Filtre & DEBUG_SAME_LINE)==0) _ftprintf(m_Fichier, "%20s|", m_Chrono.GetTime(true));
		if (Filtre & DEBUG_MSGBOX) { // In this case, replace all \n with \t
			TCHAR *sMsg = _tcsdup (format);
			TCHAR *sPos, *sLast;
			while ( (sPos = _tcschr(sMsg, _T('\n')))!=NULL )
				*sPos = _T('\t');
			sLast = _tcsrchr(sMsg, _T('\t'));
			if (sLast)
				*sLast = _T('\n'); // We keep the last \n
			va_start (args, format);
			_vftprintf(m_Fichier, sMsg, args);
			free(sMsg);
		}
		else {
			va_start (args, format);
			_vftprintf(m_Fichier, format, args);
		}
	}
	if ((OutModes & DEBUG_CONSOLE)) { // Console output
		va_start (args, format);
		_vtcprintf(format, args);
	}
	if ((OutModes & DEBUG_MSGBOX)) { // MessageBox
		TCHAR sMsg[1024];
		TCHAR *pMsg=sMsg;
		if ((Filtre & DEBUG_ERROR)>0)
			pMsg=m_sErrorMsg;
		va_start (args, format);
		vsprintf(pMsg, format, args);
		MessageBox(0, pMsg, "DEBUG", MB_OK);
	}
	va_end (args);		
	
}


// Interface DLL
// Pour passer en C, on utiliser un objet global instanciant la classe CTracer
void WINAPI SetDebugMode(int NewMode) { Tracer.ChangeMode(NewMode);}

void WINAPI DebugTrace(LPCTSTR  sString) { Tracer.Format(DEBUG_USER_MESSAGE, sString);}
void WINAPI DebugError(LPCTSTR sString) { Tracer.Format(DEBUG_USER_ERROR,  sString);}
LPCTSTR WINAPI GetLastErrorMsg(void) { return CTracer::m_sErrorMsg;}
LPCTSTR WINAPI FFVersion(void) { return _T("2.1");}
