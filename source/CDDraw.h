/////////////////////////////////////////////////////////////////////////////
// Name : CDDraw.h
// Desc : DDraw Class
// Date : 2002.11.17
// Mail : jhook@hanmail.net
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// INCLUDE
/////////////////////////////////////////////////////////////////////////////
#if !defined _CDDRAW_H_
#define _CDDRAW_H_

#include <windows.h>
#include <windowsx.h>
#include <stdio.h>		// SEEK_END
#include <io.h>			// file I/O
#include <ddraw.h>
#include "CDDraw.h"
/////////////////////////////////////////////////////////////////////////////
// DEFINE
/////////////////////////////////////////////////////////////////////////////
#define CDDRAW_INITSTRUCT(d)  { ZeroMemory(&d,sizeof(d)); d.dwSize=sizeof(d); }
#define CDDRAW_SAFERELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }

/////////////////////////////////////////////////////////////////////////////
// GLOBAL
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//	TYPE
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//	CLASS
/////////////////////////////////////////////////////////////////////////////
class CDDraw  
{
	public:
	CDDraw();
	~CDDraw();

	HRESULT CreateFullScreen(HWND hWnd,DWORD dwWidth,DWORD dwHeight,DWORD dwBPP);
	HRESULT CreateWindowedScreen(HWND hWnd,DWORD dwWidth,DWORD dwHeight);
	HRESULT SwitchingWindowScreen(HWND hWnd,int Width,int Height,int Bpp);
	LPDIRECTDRAWCLIPPER DDraw_Attach_Clipper(LPDIRECTDRAWSURFACE7 lpdds,int num_rects,LPRECT clip_list);
	HRESULT DestoryCDDraw();
	HRESULT UpdateBounds();

	UCHAR*	DDraw_Lock_Surface(LPDIRECTDRAWSURFACE7 lpdds, int *lpitch);
	UCHAR*	DDraw_Lock_Primary_Surface(void);
	UCHAR*	DDraw_Lock_Back_Surface(void);

	HRESULT DDraw_Unlock_Surface(LPDIRECTDRAWSURFACE7 lpdds);
	HRESULT DDraw_Unlock_Primary_Surface(void);
	HRESULT DDraw_Unlock_Back_Surface(void);

	HRESULT DDraw_Fill_Surface(HWND m_hWnd,LPDIRECTDRAWSURFACE7 lpdds,int color);
	HRESULT DDraw_Flip(void);

	HRESULT DDraw_Wait_For_Vsync(void);

	LPDIRECTDRAWSURFACE7 CreateOffSurface(int Width, int Height, int MemFlag);

	LPDIRECTDRAW7			lpdd7;					// dd object
	LPDIRECTDRAWSURFACE7	lpddsprimary;			// dd primary surface
	LPDIRECTDRAWSURFACE7	lpddsback;				// dd back surface

	LPDIRECTDRAWCLIPPER		lpddclipper;			// dd clipper
	DDSURFACEDESC2			ddsd;					// a direct draw surface description struct
	DDBLTFX					ddbltfx;				// used to fill	
	DDSCAPS2				ddscaps;				// a direct draw surface capabilities struct
	HRESULT					hr;					// result back from dd calls
	UCHAR*					primary_buffer;			// primary video buffer
	UCHAR*					back_buffer;			// secondary back buffer
	int						primary_lpitch;			// memory line pitch
	int						back_lpitch;

	HWND					m_hWnd;
	BOOL					m_bWindowed;
	RECT					m_rcWindow ;
};
#endif
