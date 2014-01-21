/*
Base utilisée : 
	la fonction getbits vient d'ImageSearch:
		Copyright 2003-2007 Chris Mallett (support@autohotkey.com)
		DLL conversion 2008: kangkengkingkong@hotmail.com

All other parts are from FastFrench (c) 2011 FastFrench (antispam@laposte.net)

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
//#include "PixelProcessing.h"
#include "io.h"
#include <Gdiplus.h>
#include <atlimage.h>

#include <Gdiplusimaging.h>


inline COLORREF rgb_to_bgr(DWORD aRGB)
// Fancier methods seem prone to problems due to byte alignment or compiler issues.
{
	return RGB(GetBValue(aRGB), GetGValue(aRGB), GetRValue(aRGB));
}


int LastFileNameNb=0;


LPCOLORREF getbits(LPCOLORREF &image_pixel, HBITMAP ahImage, HDC hdc, LONG &aWidth, LONG &aHeight, bool &aIs16Bit, int aMinColorDepth)
// Helper function used by PixelSearch below.
// Returns an array of pixels to the caller, which it must free when done.  Returns NULL on failure,
// in which case the contents of the output parameters is indeterminate.
{
	HDC tdc = CreateCompatibleDC(hdc);
	if (!tdc)
		return NULL;

	// From this point on, "goto end" will assume tdc is non-NULL, but that the below
	// might still be NULL.  Therefore, all of the following must be initialized so that the "end"
	// label can detect them:
	HGDIOBJ tdc_orig_select = NULL;
	bool success = false;

	// Confirmed:
	// Needs extra memory to prevent buffer overflow due to: "A bottom-up DIB is specified by setting
	// the height to a positive number, while a top-down DIB is specified by setting the height to a
	// negative number. THE BITMAP COLOR TABLE WILL BE APPENDED to the BITMAPINFO structure."
	// Maybe this applies only to negative height, in which case the second call to GetDIBits()
	// below uses one.
	struct BITMAPINFO3
	{
		BITMAPINFOHEADER    bmiHeader;
		RGBQUAD             bmiColors[260];  // v1.0.40.10: 260 vs. 3 to allow room for color table when color depth is 8-bit or less.
	} bmi;

	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biBitCount = 0; // i.e. "query bitmap attributes" only.
	if (!GetDIBits(tdc, ahImage, 0, 0, NULL, (LPBITMAPINFO)&bmi, DIB_RGB_COLORS)
		|| bmi.bmiHeader.biBitCount < aMinColorDepth) // Relies on short-circuit boolean order.
		goto end;

	// Set output parameters for caller:
	aIs16Bit = (bmi.bmiHeader.biBitCount == 16);
	aWidth = bmi.bmiHeader.biWidth;
	aHeight = bmi.bmiHeader.biHeight;

	int image_pixel_count = aWidth * aHeight;
	if (   !(image_pixel = (LPCOLORREF)realloc(image_pixel, image_pixel_count * sizeof(COLORREF)))   )
		goto end;

	// v1.0.40.10: To preserve compatibility with callers who check for transparency in icons, don't do any
	// of the extra color table handling for 1-bpp images.  Update: For code simplification, support only
	// 8-bpp images.  If ever support lower color depths, use something like "bmi.bmiHeader.biBitCount > 1
	// && bmi.bmiHeader.biBitCount < 9";
	bool is_8bit = (bmi.bmiHeader.biBitCount == 8);
	if (!is_8bit)
		bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biHeight = -bmi.bmiHeader.biHeight; // Storing a negative inside the bmiHeader struct is a signal for GetDIBits().

	// Must be done only after GetDIBits() because: "The bitmap identified by the hbmp parameter
	// must not be selected into a device context when the application calls GetDIBits()."
	// (Although testing shows it works anyway, perhaps because GetDIBits() above is being
	// called in its informational mode only).
	// Note that this seems to return NULL sometimes even though everything still works.
	// Perhaps that is normal.
	tdc_orig_select = SelectObject(tdc, ahImage); // Returns NULL when we're called the second time?

	// Appparently there is no need to specify DIB_PAL_COLORS below when color depth is 8-bit because
	// DIB_RGB_COLORS also retrieves the color indices.
	if (   !(GetDIBits(tdc, ahImage, 0, aHeight, image_pixel, (LPBITMAPINFO)&bmi, DIB_RGB_COLORS))   )
		goto end;

	if (is_8bit) // This section added in v1.0.40.10.
	{
		// Convert the color indicies to RGB colors by going through the array in reverse order.
		// Reverse order allows an in-place conversion of each 8-bit color index to its corresponding
		// 32-bit RGB color.
		LPDWORD palette = (LPDWORD)_alloca(256 * sizeof(PALETTEENTRY));
		GetSystemPaletteEntries(tdc, 0, 256, (LPPALETTEENTRY)palette); // Even if failure can realistically happen, consequences of using uninitialized palette seem acceptable.
		// Above: GetSystemPaletteEntries() is the only approach that provided the correct palette.
		// The following other approaches didn't give the right one:
		// GetDIBits(): The palette it stores in bmi.bmiColors seems completely wrong.
		// GetPaletteEntries()+GetCurrentObject(hdc, OBJ_PAL): Returned only 20 entries rather than the expected 256.
		// GetDIBColorTable(): I think same as above or maybe it returns 0.

		// The following section is necessary because apparently each new row in the region starts on
		// a DWORD boundary.  So if the number of pixels in each row isn't an exact multiple of 4, there
		// are between 1 and 3 zero-bytes at the end of each row.
		int remainder = aWidth % 4;
		int empty_bytes_at_end_of_each_row = remainder ? (4 - remainder) : 0;

		// Start at the last RGB slot and the last color index slot:
		BYTE *byte = (BYTE *)image_pixel + image_pixel_count - 1 + (aHeight * empty_bytes_at_end_of_each_row); // Pointer to 8-bit color indices.
		DWORD *pixel = image_pixel + image_pixel_count - 1; // Pointer to 32-bit RGB entries.

		int row, col;
		for (row = 0; row < aHeight; ++row) // For each row.
		{
			byte -= empty_bytes_at_end_of_each_row;
			for (col = 0; col < aWidth; ++col) // For each column.
				*pixel-- = rgb_to_bgr(palette[*byte--]); // Caller always wants RGB vs. BGR format.
		}
	}
	
	// Since above didn't "goto end", indicate success:
	success = true;

end:
	if (tdc_orig_select) // i.e. the original call to SelectObject() didn't fail.
		SelectObject(tdc, tdc_orig_select); // Probably necessary to prevent memory leak.
	DeleteDC(tdc);
	if (!success && image_pixel)
	{
		free(image_pixel);
		image_pixel = NULL;
	}
	return image_pixel;
}


using namespace Gdiplus;
ULONG_PTR gdiplusToken=0; 
GdiplusStartupInput startupInput; 

void WINAPI StartGDIplus()
{
if (!gdiplusToken)
	GdiplusStartup(&gdiplusToken, &startupInput, 0); 
}
void WINAPI StopGDIplus()
{
if (gdiplusToken)
	GdiplusStartup(&gdiplusToken, &startupInput, 0); 
gdiplusToken = 0;
}

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
   UINT  num = 0;          // number of image encoders
   UINT  size = 0;         // size of the image encoder array in bytes

   ImageCodecInfo* pImageCodecInfo = NULL;

   GetImageEncodersSize(&num, &size);
   if(size == 0)
      return -1;  // Failure

   pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
   if(pImageCodecInfo == NULL)
      return -1;  // Failure

   GetImageEncoders(num, size, pImageCodecInfo);

   for(UINT j = 0; j < num; ++j)
   {
      if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
      {
         *pClsid = pImageCodecInfo[j].Clsid;
         free(pImageCodecInfo);
         return j;  // Success
      }    
   }

   free(pImageCodecInfo);
   return -1;  // Failure
}

CLSID JPG_CLSID = GUID();
CLSID BMP_CLSID = GUID();

//INT WINAPI GetEncoderClsid(const WCHAR* format, CLSID* pClsid);  // helper function

static WCHAR* ToWChar(char * str) 
{
  // in GDI+, all the parameters about the symbol are WCHAR type
  // this funciton is a tranformation function

  static WCHAR buffer[1024];
  _wcsset(buffer,0);
  MultiByteToWideChar(CP_ACP,0,str,strlen(str),buffer,1024);
  return buffer;
}

int WINAPI GetLastFileSuffix() {return LastFileNameNb;}

bool WINAPI SaveJPG(int NoSnapShot, LPCSTR szFileName /* With no extension*/, ULONG uQuality) 
{
	if (!SnapShotData::IsSnapShotValid(NoSnapShot, _T("SaveJPG"))) 
		{
#ifdef MYTRACE
			Tracer.Format(DEBUG_SYSTEM_ERROR, _T("SaveJPG(%d, %s) : failed as SnapShot is invalid\n"), NoSnapShot, szFileName);	
#endif
			return false;	
		}
	
			
	char FileName[_MAX_PATH];
	int suffixe=0;
	do 
	{
		if (suffixe++)
			sprintf(FileName, "%s%d.jpg", szFileName, suffixe);
		else
			sprintf(FileName, "%s.jpg", szFileName);
	} while (_access(FileName, 0) != -1); // loop while the file exists

    //Create a new file for writing
	FILE *pFile = fopen(FileName, "wb");
    if(pFile == NULL)
    {
#ifdef MYTRACE
		Tracer.Format(DEBUG_SYSTEM_ERROR, _T("SaveJPG(%d, %s) failed : can't create %s\n"), NoSnapShot, szFileName, FileName);
#endif
        return false;
    }
	fclose(pFile);
	
	StartGDIplus();

#ifdef MYTRACE
	Tracer.Format(DEBUG_STREAM_SYSTEM, _T("SaveJPG(%d, %s) saved into %s (%s area : %d x %d)\n"), NoSnapShot, szFileName, FileName, GtSnapShotData[NoSnapShot].bClientArea?"Client":"full", GtSnapShotData[NoSnapShot].GetAreaWidth(), GtSnapShotData[NoSnapShot].GetAreaHeight());
#endif

	BITMAPINFO bmi;
	
	COLORREF *newBuf = new COLORREF[GtSnapShotData[NoSnapShot].GetPixelCount()];
	COLORREF *oldBuf = GtSnapShotData[NoSnapShot].GetPixels();
	for (int iLigne=GtSnapShotData[NoSnapShot].lScreenHeight-1; iLigne>=0; iLigne--)
		{
			//COLORREF *pOldLigne = oldBuf+GtSnapShotData[NoSnapShot].lScreenWidth*iLigne;
			COLORREF *pNewLigne = newBuf+GtSnapShotData[NoSnapShot].lScreenWidth*iLigne;
			memcpy(pNewLigne, oldBuf, GtSnapShotData[NoSnapShot].lScreenWidth * sizeof(COLORREF));
			oldBuf += GtSnapShotData[NoSnapShot].lScreenWidth;
			//nWrittenDIBDataSize += fwrite(pLigne, 1, GtSnapShotData[NoSnapShot].lScreenWidth*sizeof(COLORREF), pFile);
			//for (int iCol=0; iCol<GtSnapShotData[NoSnapShot].lScreenWidth; iCol++)
			//	nWrittenDIBDataSize += fwrite(pLigne++, 1, 3, pFile);
		}

 
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biWidth = GtSnapShotData[NoSnapShot].GetAreaWidth();//lScreenWidth;// m_rDrawingSurface.Width();
    bmi.bmiHeader.biHeight = GtSnapShotData[NoSnapShot].GetAreaHeight();
    bmi.bmiHeader.biSizeImage = ((((bmi.bmiHeader.biWidth * bmi.bmiHeader.biBitCount) 
                           + 31) & ~31) >> 3) * bmi.bmiHeader.biHeight;
	Bitmap bm( &bmi, /*GtSnapShotData[NoSnapShot].GetPixels()*/newBuf); 
	EncoderParameters *encoderParameters = (EncoderParameters *) new char[sizeof(EncoderParameters)+sizeof(EncoderParameter)];
	encoderParameters->Count = 1;
	encoderParameters->Parameter[0].Guid = EncoderQuality;
	encoderParameters->Parameter[0].NumberOfValues = 1;
	encoderParameters->Parameter[0].Type = EncoderParameterValueTypeLong;
	encoderParameters->Parameter[0].Value = &uQuality;
	//encoderParameters->Parameter[1].Guid = EncoderTransformation;
	//encoderParameters->Parameter[1].NumberOfValues = 1;
	//ULONG shVal = EncoderValueTransformFlipHorizontal;
	//encoderParameters->Parameter[1].Type = EncoderParameterValueTypeLong;
	//encoderParameters->Parameter[1].Value = &shVal;
	
	static bool bJPG_Inited=false;
	if (!bJPG_Inited) 
	{
		if (GetEncoderClsid(L"image/jpeg", &JPG_CLSID) < 0)
		{
			#ifdef MYTRACE
			Tracer.Format(DEBUG_SYSTEM_ERROR, _T("The JPG encoder is not installed.\n"));
			#endif

			return false;
		}

		bJPG_Inited = true;
	}

	//CLSID imageCLSID;     
	//GetEncoderClsid(L"image/jpeg", &imageCLSID); 
	
	if (bm.Save(ToWChar(FileName), &JPG_CLSID, encoderParameters)!= Gdiplus::Status::Ok)
	{		
		#ifdef MYTRACE
		Tracer.Format(DEBUG_SYSTEM_ERROR, _T("Failed to save Bitmap into %s (%d).\n"), szFileName, GetLastError());
		#endif
	}
	delete newBuf;
	delete encoderParameters;
	return true;
}

bool WINAPI DrawSnapShot(int NoSnapShot) 
{
	if (!SnapShotData::IsSnapShotValid(NoSnapShot, _T("DrawSnapShot"))) 
		{
#ifdef MYTRACE
			Tracer.Format(DEBUG_STREAM_SYSTEM, _T("DrawSnapShot(%d) failed - (%s area : %d x %d)\n"), NoSnapShot, GtSnapShotData[NoSnapShot].bClientArea?"Client":"full", GtSnapShotData[NoSnapShot].GetAreaWidth(), GtSnapShotData[NoSnapShot].GetAreaHeight());
#endif
			return false;	
		}
	
			
	StartGDIplus();

#ifdef MYTRACE
	Tracer.Format(DEBUG_STREAM_SYSTEM, _T("DrawSnapShot(%d) - (%s area : %d x %d)\n"), NoSnapShot, GtSnapShotData[NoSnapShot].bClientArea?"Client":"full", GtSnapShotData[NoSnapShot].GetAreaWidth(), GtSnapShotData[NoSnapShot].GetAreaHeight());
#endif

	BITMAPINFO bmi;
	
	COLORREF *newBuf = new COLORREF[GtSnapShotData[NoSnapShot].GetPixelCount()];
	COLORREF *oldBuf = GtSnapShotData[NoSnapShot].GetPixels();
	for (int iLigne=GtSnapShotData[NoSnapShot].lScreenHeight-1; iLigne>=0; iLigne--)
		{
			//COLORREF *pOldLigne = oldBuf+GtSnapShotData[NoSnapShot].lScreenWidth*iLigne;
			COLORREF *pNewLigne = newBuf+GtSnapShotData[NoSnapShot].lScreenWidth*iLigne;
			memcpy(pNewLigne, oldBuf, GtSnapShotData[NoSnapShot].lScreenWidth * sizeof(COLORREF));
			oldBuf += GtSnapShotData[NoSnapShot].lScreenWidth;
			//nWrittenDIBDataSize += fwrite(pLigne, 1, GtSnapShotData[NoSnapShot].lScreenWidth*sizeof(COLORREF), pFile);
			//for (int iCol=0; iCol<GtSnapShotData[NoSnapShot].lScreenWidth; iCol++)
			//	nWrittenDIBDataSize += fwrite(pLigne++, 1, 3, pFile);
		}

 
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biWidth = GtSnapShotData[NoSnapShot].GetAreaWidth();//lScreenWidth;// m_rDrawingSurface.Width();
    bmi.bmiHeader.biHeight = GtSnapShotData[NoSnapShot].GetAreaHeight();
    bmi.bmiHeader.biSizeImage = ((((bmi.bmiHeader.biWidth * bmi.bmiHeader.biBitCount) 
                           + 31) & ~31) >> 3) * bmi.bmiHeader.biHeight;
	Bitmap bm( &bmi, /*GtSnapShotData[NoSnapShot].GetPixels()*/newBuf); 
	Gdiplus::Graphics graphics(GtSnapShotData[NoSnapShot].GetDC());
	graphics.DrawImage(&bm, GtSnapShotData[NoSnapShot].x1, GtSnapShotData[NoSnapShot].y1);
	delete newBuf;
	return true;
}

bool WINAPI SaveBMP(int NoSnapShot, LPCSTR szFileName /* With no extension*/) 
{
	if (!SnapShotData::IsSnapShotValid(NoSnapShot, _T("SaveBMP"))) 
		{
#ifdef MYTRACE
			Tracer.Format(DEBUG_SYSTEM_ERROR, _T("SaveBMP(%d, %s) : failed as SnapShot is invalid\n"), NoSnapShot, szFileName);	
#endif
			return false;	
		}				
			
	char FileName[_MAX_PATH];
	int suffixe=0;
	do 
	{
		if (suffixe++)
			sprintf(FileName, "%s%d.bmp", szFileName, suffixe);
		else
			sprintf(FileName, "%s.bmp", szFileName);
	} while (_access(FileName, 0) != -1); // loop while the file exists

    //Create a new file for writing
	FILE *pFile = fopen(FileName, "wb");
    if(pFile == NULL)
    {
#ifdef MYTRACE
		Tracer.Format(DEBUG_SYSTEM_ERROR, _T("SaveBMP(%d, %s) failed : can't create %s\n"), NoSnapShot, szFileName, FileName);
#endif
        return false;
    }
	fclose(pFile);
	
	StartGDIplus();

#ifdef MYTRACE
	Tracer.Format(DEBUG_STREAM_SYSTEM, _T("SaveBMP(%d, %s) saved into %s (%s area : %d x %d)\n"), NoSnapShot, szFileName, FileName, GtSnapShotData[NoSnapShot].bClientArea?"Client":"full", GtSnapShotData[NoSnapShot].GetAreaWidth(), GtSnapShotData[NoSnapShot].GetAreaHeight());
#endif

	BITMAPINFO bmi;
	
	COLORREF *newBuf = new COLORREF[GtSnapShotData[NoSnapShot].GetPixelCount()];
	COLORREF *oldBuf = GtSnapShotData[NoSnapShot].GetPixels();
	for (int iLigne=GtSnapShotData[NoSnapShot].lScreenHeight-1; iLigne>=0; iLigne--)
		{
			//COLORREF *pOldLigne = oldBuf+GtSnapShotData[NoSnapShot].lScreenWidth*iLigne;
			COLORREF *pNewLigne = newBuf+GtSnapShotData[NoSnapShot].lScreenWidth*iLigne;
			memcpy(pNewLigne, oldBuf, GtSnapShotData[NoSnapShot].lScreenWidth * sizeof(COLORREF));
			oldBuf += GtSnapShotData[NoSnapShot].lScreenWidth;
			//nWrittenDIBDataSize += fwrite(pLigne, 1, GtSnapShotData[NoSnapShot].lScreenWidth*sizeof(COLORREF), pFile);
			//for (int iCol=0; iCol<GtSnapShotData[NoSnapShot].lScreenWidth; iCol++)
			//	nWrittenDIBDataSize += fwrite(pLigne++, 1, 3, pFile);
		}

 
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biWidth = GtSnapShotData[NoSnapShot].GetAreaWidth();//lScreenWidth;// m_rDrawingSurface.Width();
    bmi.bmiHeader.biHeight = GtSnapShotData[NoSnapShot].GetAreaHeight();
    bmi.bmiHeader.biSizeImage = ((((bmi.bmiHeader.biWidth * bmi.bmiHeader.biBitCount) 
                           + 31) & ~31) >> 3) * bmi.bmiHeader.biHeight;
	Bitmap bm( &bmi, /*GtSnapShotData[NoSnapShot].GetPixels()*/newBuf); 
	
	static bool bBMP_Inited=false;
	if (!bBMP_Inited) 
	{
		if (GetEncoderClsid(L"image/bmp", &BMP_CLSID)<0)
		{
			#ifdef MYTRACE
			Tracer.Format(DEBUG_SYSTEM_ERROR, _T("The BMP encoder is not installed.\n"));
			#endif
			return false;
		}
		bBMP_Inited = true;
	}

	//CLSID imageCLSID;     
	//GetEncoderClsid(L"image/jpeg", &imageCLSID); 
	if (bm.Save(ToWChar(FileName), &BMP_CLSID) != Gdiplus::Status::Ok)
	{		
		#ifdef MYTRACE
		Tracer.Format(DEBUG_SYSTEM_ERROR, _T("Failed to save Bitmap into %s (%d).\n"), szFileName, GetLastError());
		#endif
	}
	delete newBuf;
	return true;
}