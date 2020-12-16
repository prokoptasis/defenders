/////////////////////////////////////////////////////////////////////////////
// Name : CDBitmap.h
// Desc : Bitmap Class
// Date : 2002.11.18
// Mail : jhook@hanmail.net
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// INCLUDE
/////////////////////////////////////////////////////////////////////////////
#if !defined _CDBITMAP_H_
#define _CDBITMAP_H_

#include <windows.h>
#include <windowsx.h>
#include <stdio.h>		// SEEK_END
#include <io.h>			// file I/O
#include <ddraw.h>

/////////////////////////////////////////////////////////////////////////////
// DEFINE
/////////////////////////////////////////////////////////////////////////////
#define SCREEN_WIDTH				640
#define SCREEN_HEIGHT				480
#define SCREEN_BPP					32
#define PALETTE_MAX_COLORS			256
#define CDBITMAP_INITSTRUCT(d)		{ memset(&d,0,sizeof(d)); d.dwSize=sizeof(d); }
#define CDBITMAP_SAFERELEASE(p)		{ if(p) { (p)->Release(); (p)=NULL; } }
#define _RGB16BIT555(r,g,b)			((b%32) + ((g%32) << 5) + ((r%32) << 10))
#define _RGB16BIT565(r,g,b)			((b%32) + ((g%64) << 6) + ((r%32) << 11))
#define _RGB32BIT(a,r,g,b)			((b) + ((g) << 8) + ((r) << 16) + ((a) << 24))
#define BITMAP_ID					0x4D42 // universal id for a bitmap
#define BITMAP_STATE_DEAD			0
#define BITMAP_STATE_ALIVE			1
#define BITMAP_STATE_DYING			2 
#define BITMAP_ATTR_LOADED			128
#define BITMAP_EXTRACT_MODE_CELL	0
#define BITMAP_EXTRACT_MODE_ABS		1
/////////////////////////////////////////////////////////////////////////////
// GLOBAL
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//	TYPE
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//	PROTOTYPES
/////////////////////////////////////////////////////////////////////////////
class CDBitmap  
{
	public:
	CDBitmap();
	~CDBitmap();

	typedef struct BITMAP_IMAGE_TYP
	{
		int state;          // state of bitmap
		int attr;           // attributes of bitmap
		int x,y;            // position of bitmap
		int width, height;  // size of bitmap
		int num_bytes;      // total bytes of bitmap
		UCHAR *buffer;      // pixels of bitmap
	} BITMAP_IMAGE, *BITMAP_IMAGE_PTR;

	typedef struct BITMAP_FILE_TAG
	{
		BITMAPFILEHEADER bitmapfileheader;  // this contains the bitmapfile header
		BITMAPINFOHEADER bitmapinfoheader;  // this is all the info including the palette
		PALETTEENTRY     palette[256];      // we will store the palette here
		UCHAR            *buffer;           // this is a pointer to the data
	} BITMAP_FILE, *BITMAP_FILE_PTR;

	LPDIRECTDRAW7			lpdd7;					// dd object
	LPDIRECTDRAWSURFACE7	lpddsprimary;			// dd primary surface
	LPDIRECTDRAWSURFACE7	lpddsback;				// dd back surface
	DDSURFACEDESC2			ddsd;					// a direct draw surface description struct
	DDSCAPS2				ddscaps;				// a direct draw surface capabilities struct
	HRESULT					hr;					// result back from dd calls
	
	BITMAP_FILE				bitmap;					// a 8 bit bitmap file
	BITMAP_IMAGE			reactor;

	BOOL					g_bWindowed;
	RECT					m_rcWindow ;

	HRESULT Load_Bitmap_File(BITMAP_FILE_PTR bitmap, char *filename);
	HRESULT Unload_Bitmap_File(BITMAP_FILE_PTR bitmap);	
	HRESULT Create_Bitmap(BITMAP_IMAGE_PTR image, int x, int y, int width, int height);
	HRESULT Destroy_Bitmap(BITMAP_IMAGE_PTR image);
	HRESULT Load_Image_Bitmap(BITMAP_IMAGE_PTR image, BITMAP_FILE_PTR bitmap, int cx,int cy, int mode);
	HRESULT Scan_Image_Bitmap(HWND m_hWnd,BOOL m_bWindowed,BITMAP_FILE_PTR bitmap,LPDIRECTDRAWSURFACE7 lpdds, int cx,int cy);	
	HRESULT Draw_Bitmap(HWND m_hWnd,BOOL m_bWindowed,BITMAP_IMAGE_PTR source_bitmap,UCHAR *dest_buffer, int lpitch, DWORD transparent);
	HRESULT Flip_Bitmap(UCHAR *image, int bytes_per_line, int height);
	HRESULT Scroll_Bitmap(void);
	HRESULT Copy_Bitmap(void);
};

#endif