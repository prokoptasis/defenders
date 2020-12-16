/////////////////////////////////////////////////////////////////////////////
// Name : CDDraw.cpp
// Desc : DDraw Class
// Date : 2002.11.17
// Mail : jhook@hanmail.net
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// INCLUDE
/////////////////////////////////////////////////////////////////////////////
#include "CDDraw.h"

/////////////////////////////////////////////////////////////////////////////
// Define
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// GLOBAL
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//	TYPE
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//	PROTOTYPES
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//	FUNCTIONS
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//	Name : CDDraw()
//  Desc : 
/////////////////////////////////////////////////////////////////////////////
CDDraw::CDDraw()
{
	lpdd7			= NULL;			// dd object
	lpddsprimary	= NULL;			// dd primary surface
	lpddsback		= NULL;
	lpddclipper		= NULL;

	m_bWindowed		= TRUE;

	primary_buffer	= NULL;
	back_buffer		= NULL;
	primary_lpitch	= NULL;
	back_lpitch		= NULL;
}

/////////////////////////////////////////////////////////////////////////////
//	Name : ~CDDraw()
//  Desc : 
/////////////////////////////////////////////////////////////////////////////
CDDraw::~CDDraw()
{
	DestoryCDDraw();
}

/////////////////////////////////////////////////////////////////////////////
//	Name : CreateFullScreen(HWND hWnd,DWORD dwWidth,DWORD dwHeight,DWORD dwBPP)
//  Desc : 
/////////////////////////////////////////////////////////////////////////////
HRESULT CDDraw::CreateFullScreen(HWND hWnd,DWORD dwWidth,DWORD dwHeight,DWORD dwBPP)
{
    DestoryCDDraw();

    if( FAILED( hr = DirectDrawCreateEx( NULL, (VOID**)&lpdd7, IID_IDirectDraw7, NULL ) ) )
        return E_FAIL;

    hr = lpdd7->SetCooperativeLevel( hWnd, DDSCL_EXCLUSIVE|DDSCL_FULLSCREEN );
    if( FAILED(hr) )
        return E_FAIL;

    if( FAILED( lpdd7->SetDisplayMode( dwWidth, dwHeight, dwBPP, 0, 0 ) ) )
        return E_FAIL;

	CDDRAW_INITSTRUCT(ddsd);
    ddsd.dwFlags           = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
    ddsd.ddsCaps.dwCaps    = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP |
                             DDSCAPS_COMPLEX | DDSCAPS_3DDEVICE;
 
	ddsd.dwBackBufferCount = 1;

    if( FAILED( hr = lpdd7->CreateSurface( &ddsd, &lpddsprimary,NULL ) ) )
        return E_FAIL;

    // Get a pointer to the back buffer
    ZeroMemory( &ddscaps, sizeof( ddscaps ) );
    ddscaps.dwCaps = DDSCAPS_BACKBUFFER;

    if( FAILED( hr = lpddsprimary->GetAttachedSurface( &ddscaps,&lpddsback ) ) )
        return E_FAIL;

    lpddsback->AddRef();

    m_hWnd      = hWnd;
    UpdateBounds();

    return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
//	Name : CreateWindowedScreen(HWND hWnd,DWORD dwWidth,DWORD dwHeight)
//  Desc : 
/////////////////////////////////////////////////////////////////////////////
HRESULT CDDraw::CreateWindowedScreen(HWND hWnd,DWORD dwWidth,DWORD dwHeight)
{
	DestoryCDDraw();

    if( FAILED( hr = DirectDrawCreateEx( NULL, (VOID**)&lpdd7,IID_IDirectDraw7, NULL ) ) )
        return E_FAIL;

    hr = lpdd7->SetCooperativeLevel( hWnd, DDSCL_NORMAL );
    if( FAILED(hr) )
        return E_FAIL;

    RECT  rcWork;
    RECT  rc;
    DWORD dwStyle;

    dwStyle  = GetWindowStyle( hWnd );
    dwStyle &= ~WS_POPUP;
    dwStyle |= WS_OVERLAPPED | WS_CAPTION | WS_THICKFRAME;
    SetWindowLong( hWnd, GWL_STYLE, dwStyle );

	SetRect( &rc, 0, 0, dwWidth, dwHeight );

    AdjustWindowRectEx( &rc, GetWindowStyle(hWnd), GetMenu(hWnd) != NULL,GetWindowExStyle(hWnd) );

    SetWindowPos( hWnd, NULL, 0, 0, rc.right-rc.left, rc.bottom-rc.top,SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE );

    SetWindowPos( hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE );

    SystemParametersInfo( SPI_GETWORKAREA, 0, &rcWork, 0 );
    GetWindowRect( hWnd, &rc );
    if( rc.left < rcWork.left ) rc.left = rcWork.left;
    if( rc.top  < rcWork.top )  rc.top  = rcWork.top;
    SetWindowPos( hWnd, NULL, rc.left, rc.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE );

	CDDRAW_INITSTRUCT(ddsd);
    ddsd.dwFlags        = DDSD_CAPS;
    ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

    if( FAILED( lpdd7->CreateSurface( &ddsd, &lpddsprimary, NULL ) ) )
        return E_FAIL;

    // Create the backbuffer surface
    ddsd.dwFlags        = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;    
    ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_3DDEVICE;
    ddsd.dwWidth        = dwWidth;
    ddsd.dwHeight       = dwHeight;

    if( FAILED( hr = lpdd7->CreateSurface( &ddsd, &lpddsback, NULL ) ) )
        return E_FAIL;

    m_hWnd      = hWnd;
    UpdateBounds();

    return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
//	Name : HRESULT CDDraw::SwitchingWindowScreen(HWND hWnd,int Width,int Height,int Bpp)
//  Desc : 
/////////////////////////////////////////////////////////////////////////////
HRESULT CDDraw::SwitchingWindowScreen(HWND hWnd,int Width,int Height,int Bpp)
{
    DestoryCDDraw();

	if( m_bWindowed )
    {
        if(FAILED(CreateWindowedScreen(hWnd,Width,Height)))
			return E_FAIL;

        DWORD dwStyle = WS_OVERLAPPED | WS_CAPTION;
        SetWindowLong( hWnd, GWL_STYLE, dwStyle );
		ShowWindow( hWnd, SW_SHOW );
		UpdateWindow( hWnd );
    }
    else
    {
		DWORD dwStyle  = WS_POPUP & ~WS_CAPTION;
        SetWindowLong( hWnd, GWL_STYLE, dwStyle );
		ShowWindow( hWnd, SW_SHOW );
		UpdateWindow( hWnd );

		if(FAILED(CreateFullScreen(hWnd,Width,Height,Bpp)))
			return E_FAIL;
    }

	m_bWindowed = !m_bWindowed;

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
//	Name : HRESULT CDDraw::DestoryCDDraw()
//  Desc : 
/////////////////////////////////////////////////////////////////////////////
HRESULT CDDraw::DestoryCDDraw()
{
	CDDRAW_SAFERELEASE(lpddclipper);
	CDDRAW_SAFERELEASE(lpddsback);
	CDDRAW_SAFERELEASE(lpddsprimary);
	
    if( lpdd7 )
      lpdd7->SetCooperativeLevel( m_hWnd, DDSCL_NORMAL );

	CDDRAW_SAFERELEASE(lpdd7);

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
//	Name : HRESULT CDDraw::UpdateBounds()
//  Desc : 
/////////////////////////////////////////////////////////////////////////////
HRESULT CDDraw::UpdateBounds()
{
    if( m_bWindowed )
    {
        GetClientRect( m_hWnd, &m_rcWindow );
        ClientToScreen( m_hWnd, (POINT*)&m_rcWindow );
        ClientToScreen( m_hWnd, (POINT*)&m_rcWindow+1 );
    }
    else
    {
        SetRect( &m_rcWindow, 0, 0, GetSystemMetrics(SM_CXSCREEN),GetSystemMetrics(SM_CYSCREEN) );
    }

    return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
//	Name : UCHAR* CDDraw::DDraw_Lock_Surface(LPDIRECTDRAWSURFACE7 lpdds, int *lpitch)
//  Desc : locks the sent surface and returns a pointer to it
/////////////////////////////////////////////////////////////////////////////
UCHAR* CDDraw::DDraw_Lock_Surface(LPDIRECTDRAWSURFACE7 lpdds, int *lpitch)
{

	if (!lpdds)
		return(NULL);

	CDDRAW_INITSTRUCT(ddsd);
	lpdds->Lock(NULL,&ddsd,DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR,NULL); 

	if (lpitch)
		*lpitch = ddsd.lPitch;

	return((UCHAR *)ddsd.lpSurface);
} // end DDraw_Lock_Surface

/////////////////////////////////////////////////////////////////////////////
//	Name : *DDraw_Lock_Primary_Surface
//  Desc : locks the priamary surface and returns a pointer to it
/////////////////////////////////////////////////////////////////////////////
UCHAR* CDDraw::DDraw_Lock_Primary_Surface(void)
{
	if (primary_buffer)
		return(primary_buffer);

	CDDRAW_INITSTRUCT(ddsd);
	lpddsprimary->Lock(NULL,&ddsd,DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR,NULL); 

	primary_buffer = (UCHAR *)ddsd.lpSurface;
	primary_lpitch = ddsd.lPitch;

	return(primary_buffer);
} // end DDraw_Lock_Primary_Surface

/////////////////////////////////////////////////////////////////////////////
//	Name : *DDraw_Lock_Back_Surface
//  Desc : locks the secondary back surface and returns a pointer to it
///////////////////////////////////////////////////////////////////////////// 
UCHAR* CDDraw::DDraw_Lock_Back_Surface(void)
{
	if (back_buffer)
		return(back_buffer);

	CDDRAW_INITSTRUCT(ddsd);
	lpddsback->Lock(NULL,&ddsd,DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR,NULL); 

	back_buffer = (UCHAR *)ddsd.lpSurface;
	back_lpitch = ddsd.lPitch;

	return(back_buffer);
} // end DDraw_Lock_Back_Surface

/////////////////////////////////////////////////////////////////////////////
//	Name : DDraw_Unlock_Surface
//  Desc : unlocks a general surface
/////////////////////////////////////////////////////////////////////////////
HRESULT CDDraw::DDraw_Unlock_Surface(LPDIRECTDRAWSURFACE7 lpdds)
{
	if (!lpdds)
		return E_FAIL;

	lpdds->Unlock(NULL);

	return S_OK;
} // end DDraw_Unlock_Surface

/////////////////////////////////////////////////////////////////////////////
//	Name : DDraw_Unlock_Primary_Surface
//  Desc : 
///////////////////////////////////////////////////////////////////////////// 
HRESULT CDDraw::DDraw_Unlock_Primary_Surface(void)
{

	if (!primary_buffer)
		return E_FAIL;

	lpddsprimary->Unlock(NULL);

	primary_buffer = NULL;
	primary_lpitch = 0;

	return S_OK;
} // end DDraw_Unlock_Primary_Surface

/////////////////////////////////////////////////////////////////////////////
//	Name : DDraw_Unlock_Back_Surface
//  Desc : 
///////////////////////////////////////////////////////////////////////////// 
HRESULT CDDraw::DDraw_Unlock_Back_Surface(void)
{
	if (!back_buffer)
		return E_FAIL;

	lpddsback->Unlock(NULL);

	back_buffer = NULL;
	back_lpitch = 0;

	return S_OK;
} // end DDraw_Unlock_Back_Surface

/////////////////////////////////////////////////////////////////////////////
//	Name : DDraw_Attach_Clipper
//  Desc : creates a clipper from the sent clip list
/////////////////////////////////////////////////////////////////////////////
LPDIRECTDRAWCLIPPER CDDraw::DDraw_Attach_Clipper(LPDIRECTDRAWSURFACE7 lpdds,int num_rects,LPRECT clip_list)
{
	int index;                         // looping var
	LPDIRECTDRAWCLIPPER lpddclipper;   // pointer to the newly created dd clipper
	LPRGNDATA region_data;             // pointer to the region data that contains

	if (FAILED(lpdd7->CreateClipper(0,&lpddclipper,NULL)))
		return(NULL);

	region_data = (LPRGNDATA)malloc(sizeof(RGNDATAHEADER)+num_rects*sizeof(RECT));

	memcpy(region_data->Buffer, clip_list, sizeof(RECT)*num_rects);

	region_data->rdh.dwSize          = sizeof(RGNDATAHEADER);
	region_data->rdh.iType           = RDH_RECTANGLES;
	region_data->rdh.nCount          = num_rects;
	region_data->rdh.nRgnSize        = num_rects*sizeof(RECT);

	region_data->rdh.rcBound.left    =  64000;
	region_data->rdh.rcBound.top     =  64000;
	region_data->rdh.rcBound.right   = -64000;
	region_data->rdh.rcBound.bottom  = -64000;

	for (index=0; index<num_rects; index++)
    {
		if (clip_list[index].left < region_data->rdh.rcBound.left)
			region_data->rdh.rcBound.left = clip_list[index].left;

		if (clip_list[index].right > region_data->rdh.rcBound.right)
			region_data->rdh.rcBound.right = clip_list[index].right;

		if (clip_list[index].top < region_data->rdh.rcBound.top)
			region_data->rdh.rcBound.top = clip_list[index].top;

		if (clip_list[index].bottom > region_data->rdh.rcBound.bottom)
			region_data->rdh.rcBound.bottom = clip_list[index].bottom;
    } // end for index

	if (FAILED(lpddclipper->SetClipList(region_data, 0)))
	{
		free(region_data);
		return(NULL);
	} // end if

	if (FAILED(lpdds->SetClipper(lpddclipper)))
	{
		free(region_data);
		return(NULL);
	} // end if

	free(region_data);
	return(lpddclipper);
} // end DDraw_Attach_Clipper

/////////////////////////////////////////////////////////////////////////////
//	Name : DDraw_Wait_For_Vsync
//  Desc : waits for a vertical blank to begin
/////////////////////////////////////////////////////////////////////////////
HRESULT CDDraw::DDraw_Wait_For_Vsync(void)
{
	lpdd7->WaitForVerticalBlank(DDWAITVB_BLOCKBEGIN,0);
	return S_OK;
} // end DDraw_Wait_For_Vsync

/////////////////////////////////////////////////////////////////////////////
//	Name : DDraw_Fill_Surface
//  Desc : Fill the object Surface with color
/////////////////////////////////////////////////////////////////////////////
HRESULT CDDraw::DDraw_Fill_Surface(HWND m_hWnd, LPDIRECTDRAWSURFACE7 lpdds,int color)
{
	CDDRAW_INITSTRUCT(ddbltfx);
	ddbltfx.dwFillColor = color;

	lpdds->Blt(NULL,NULL,NULL,DDBLT_COLORFILL|DDBLT_WAIT,&ddbltfx);

	/*
	GetWindowRect(m_hWnd,&m_rcWindow);
	
	DWORD dwFrameWidth    = GetSystemMetrics( SM_CXSIZEFRAME );
    DWORD dwFrameHeight   = GetSystemMetrics( SM_CYSIZEFRAME );
    DWORD dwCaptionHeight = GetSystemMetrics( SM_CYCAPTION );

	m_rcWindow.left   = m_rcWindow.left + dwFrameWidth;
	m_rcWindow.top    = m_rcWindow.top  + dwFrameHeight + dwCaptionHeight;
	m_rcWindow.right  = m_rcWindow.right - dwFrameWidth;
	m_rcWindow.bottom = m_rcWindow.bottom - dwFrameHeight;
  
	if(m_bWindowed)
	{
		lpdds->Blt(NULL,NULL,NULL,DDBLT_COLORFILL|DDBLT_WAIT,&ddbltfx);
	}
	else
	{
		lpdds->Blt(NULL,NULL,NULL,DDBLT_COLORFILL|DDBLT_WAIT,&ddbltfx);
	}
	*/
	return S_OK;
} // end DDraw_Fill_Surface

/////////////////////////////////////////////////////////////////////////////
//	Name : DDraw_Flip()
//  Desc : 
///////////////////////////////////////////////////////////////////////////// 
HRESULT CDDraw::DDraw_Flip(void)
{
	if (primary_buffer || back_buffer)
		return E_FAIL;

	if (m_bWindowed)
	{
		while(FAILED(lpddsprimary->Flip(NULL, DDFLIP_WAIT)));
	}
	else
	{

		GetWindowRect(m_hWnd,&m_rcWindow);
	
		DWORD dwFrameWidth    = GetSystemMetrics( SM_CXSIZEFRAME );
		DWORD dwFrameHeight   = GetSystemMetrics( SM_CYSIZEFRAME );
	    DWORD dwCaptionHeight = GetSystemMetrics( SM_CYCAPTION );

		m_rcWindow.left   = m_rcWindow.left + dwFrameWidth;
		m_rcWindow.top    = m_rcWindow.top  + dwFrameHeight + dwCaptionHeight;
		m_rcWindow.right  = m_rcWindow.right - dwFrameWidth;
		m_rcWindow.bottom = m_rcWindow.bottom - dwFrameHeight;
	
		lpddsprimary->Blt(&m_rcWindow,lpddsback,NULL, DDBLT_WAIT,NULL);

	}
	
	DDraw_Fill_Surface(m_hWnd,lpddsback,0);

//	CDDRAW_INITSTRUCT(ddbltfx);
//	ddbltfx.dwFillColor = 0;
//	lpddsback->Blt(NULL,NULL,NULL,DDBLT_COLORFILL|DDBLT_WAIT,&ddbltfx);

	return S_OK;
} // end DDraw_Flip


/////////////////////////////////////////////////////////////////////////////
//	Name : LPDIRECTDRAWSURFACE7 CDDraw::CreateOffSurface(int Width, int Height, int MemFlag)
//  Desc : 
/////////////////////////////////////////////////////////////////////////////
LPDIRECTDRAWSURFACE7 CDDraw::CreateOffSurface(int Width, int Height, int MemFlag)
{
	LPDIRECTDRAWSURFACE7 lpdds;
	CDDRAW_INITSTRUCT(ddsd);

	ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
	ddsd.dwWidth  =  Width;
	ddsd.dwHeight =  Height;
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | MemFlag;

	if (FAILED(lpdd7->CreateSurface(&ddsd,&lpdds,NULL)))
		return (0);

	DDCOLORKEY color_key;
	color_key.dwColorSpaceLowValue  = RGB(0,0,0);
	color_key.dwColorSpaceHighValue = RGB(0,0,0);

	lpdds->SetColorKey(DDCKEY_SRCBLT, &color_key);

	return(lpdds);
}
