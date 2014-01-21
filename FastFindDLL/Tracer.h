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

#pragma once

#define DEBUG_NONE    0x0000
#define DEBUG_CONSOLE 0x0001
#define DEBUG_FILE    0x0002
#define DEBUG_GRAPHIC 0x0004
#define DEBUG_MSGBOX  0x0008
#define DEBUG_CHANNEL_BITS           0x000F // Masque pour les bits précisant les canaux de debuggage utilisés

#define DEBUG_NOSYSTEM				0x0010 // Traces internes de la DLL
#define DEBUG_NOSYSTEM_DETAILS		0x0020 // Traces internes de la DLL (détails)
#define DEBUG_NOUSER				0x0040 // Traces externes (de l'application)
#define DEBUG_NOTHING_EXCEPT_ERRORS 0x0070 // Désactivation de toutes les traces (sauf les erreurs)
#define DEBUG_ERROR					0x0080 // Message d'erreur (prioritaire)
#define DEBUG_ORIGIN_BITS           0x00F0 // Masque pour les bits marquant l'origine du paquet

#define DEBUG_SAME_LINE				0x80000 // Astuce pour poursuivre sur la même ligne, sans TimeStamp

//#define DEBUG_STREAM  0x03 // Console and File

#define DEBUG_STREAM_SYSTEM			0x13 // Console and File - System Message
#define DEBUG_STREAM_SYSTEM_DETAIL  0x33 // Console and File - Detailed System Message
#define DEBUG_MB_SYSTEM             0x3B // Console, File and MB - System Message
#define DEBUG_SYSTEM_ERROR          0x7F8F // Console, File and MB - Any Message
#define DEBUG_USER_MESSAGE          0x43 // Console and File - User Message
#define DEBUG_USER_MB               0x4B // Console, File and MB - User Message
#define DEBUG_USER_ERROR            0x7F8F // Console and File and MB - Any Message

//#define DEBUG_TEXT_MB 0x0B // Console, File and MessageBox
//#define DEBUG_ERROR   0xFF // Console, File and MessageBox

#define DEBUG_DEFAULT_FILENAME "TRACER.TXT"
class CTracer
{
public:
	// si _sFileName=NULL, la sortie se fait sur la console
	CTracer(int DebugMode=0, const char* _sFileName=DEBUG_DEFAULT_FILENAME, bool _bAppend=true);
	~CTracer(void);
	void ChangeMode(int NewDebugMode, const char* _sFileName=DEBUG_DEFAULT_FILENAME, bool _bAppend=true);
	int m_DebugModeTxt;
	bool m_bDebugGraphique;
	FILE *m_Fichier;
	FILE *m_FichierBackup;
	CChrono m_Chrono;
	void Open(const char* _sFileName=DEBUG_DEFAULT_FILENAME, bool _bAppend=true);
	void Format(int Filtre, const TCHAR * format, ...);
	bool GraphicDebug() {return m_bDebugGraphique;}
	bool TextDebug() {return m_DebugModeTxt != 0;}

	static TCHAR m_sErrorMsg[1024];		 
};

extern CTracer Tracer;

