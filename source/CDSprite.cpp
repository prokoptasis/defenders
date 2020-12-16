/////////////////////////////////////////////////////////////////////////////
// Name : CDSprite.cpp
// Desc : DDraw Class
// Date : 2002.11.17
// Mail : jhook@hanmail.net
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// INCLUDE
/////////////////////////////////////////////////////////////////////////////
#include "CDSprite.h"
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
//	Name : CDSprite()
//  Desc : 
/////////////////////////////////////////////////////////////////////////////
CDSprite::CDSprite()
{

}

/////////////////////////////////////////////////////////////////////////////
//	Name : ~CDSprite()
//  Desc : 
/////////////////////////////////////////////////////////////////////////////
CDSprite::~CDSprite()
{

}

/////////////////////////////////////////////////////////////////////////////
//	Name : CreateSprite()
//  Desc : 
/////////////////////////////////////////////////////////////////////////////
HRESULT CDSprite::CreateSprite(LPDIRECTDRAW7 lpdd7, SPRITE_PTR sprite,int width,int height,int numframe,int MemFlag)
{
	sprite->VelX		 = 0;
	sprite->VelY		 = 0;
	sprite->CellWidth	 = width;
	sprite->CellHeight	 = height;
	sprite->PosX		 = SCREEN_WIDTH/2 - sprite->CellWidth/2;
	sprite->PosY		 = SCREEN_HEIGHT/2 - sprite->CellHeight/2;
	sprite->NumFrame	 = numframe;
	sprite->CurFrame	 = 0;
	sprite->BMWidth		 = 0;
	sprite->BMHeight	 = 0;

	sprite->AniState	 = 0;
	sprite->AniIndex	 = 0;
	sprite->AniAtt		 = 0;
	sprite->CurDirection = 2;

	for (int index=0; index<sprite->NumFrame; index++)
    {
		DDSURFACEDESC2 ddsd;
		CDSPRITE_INITSTRUCT(ddsd);
		ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
		ddsd.dwWidth  = sprite->CellWidth;
		ddsd.dwHeight = sprite->CellHeight;
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | MemFlag;

		sprite->Cell[index] = NULL;

		if (FAILED(lpdd7->CreateSurface(&ddsd,&(sprite->Cell[index]),NULL)))
		{
			return E_FAIL;
		}

	    DDCOLORKEY color_key;
		color_key.dwColorSpaceLowValue  = 0;
		color_key.dwColorSpaceHighValue = 0;

	    (sprite->Cell[index])->SetColorKey(DDCKEY_SRCBLT, &color_key);
	}

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
//	Name : LoadSprite(SPRITE_PTR sprite, CDBitmap::BITMAP_FILE_PTR bitmap)
//  Desc : 
/////////////////////////////////////////////////////////////////////////////
HRESULT CDSprite::LoadSprite(SPRITE_PTR sprite, CDBitmap::BITMAP_FILE_PTR bitmap)
{
	if (!sprite)
		return E_FAIL;
	
	if (!bitmap)
		return E_FAIL;

	DWORD offsetX,offsetY;
	int index_off_X = 0;
	int index_off_Y = 0;

	for(int index=0;index<sprite->NumFrame;index++)
	{

		DDSURFACEDESC2 ddsd;
		CDSPRITE_INITSTRUCT(ddsd);

		if(FAILED(sprite->Cell[index]->Lock(NULL,&ddsd,DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR,NULL)))
			return E_FAIL;
		
		DWORD *dest_buffer= (DWORD *)ddsd.lpSurface;		

		if( 1+(1+sprite->CellWidth)*(index_off_X) >= bitmap->bitmapinfoheader.biWidth )
		{
			index_off_X = 0;
			index_off_Y++;
		}

		offsetX = 1+(1+sprite->CellWidth)*index_off_X;
		offsetY = 1+(1+sprite->CellHeight)*index_off_Y;
		
		int etc = bitmap->bitmapinfoheader.biWidth%4;

		for(int index_y=0;index_y<sprite->CellHeight;index_y++)
		{
			for(int index_x=0;index_x<sprite->CellWidth;index_x++)
			{
				DWORD	blue  = (bitmap->buffer[(offsetY+index_y)*(bitmap->bitmapinfoheader.biWidth*3+etc) + (offsetX+index_x)*3 + 0]),
						green = (bitmap->buffer[(offsetY+index_y)*(bitmap->bitmapinfoheader.biWidth*3+etc) + (offsetX+index_x)*3 + 1]),
						red   = (bitmap->buffer[(offsetY+index_y)*(bitmap->bitmapinfoheader.biWidth*3+etc) + (offsetX+index_x)*3 + 2]);

				DWORD pixel = _RGB32BIT(0,red,green,blue);

				dest_buffer[((index_x)*4+(index_y)*ddsd.lPitch)>>2] = pixel;

			}
		}
		sprite->Cell[index]->Unlock(NULL);

		index_off_X++;
	}

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
//	Name : DestroySprite(SPRITE_PTR sprite)
//  Desc : 
/////////////////////////////////////////////////////////////////////////////
HRESULT CDSprite::DestroySprite(SPRITE_PTR sprite)
{

	for(int index = 0;index < sprite->NumFrame;index++)
	{
		CDSPRITE_SAFERELEASE(sprite->Cell[index]);
	}
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
//	Name : SetAniSprite(SPRITE_PTR sprite,int *AniList,int direction)
//  Desc : 
/////////////////////////////////////////////////////////////////////////////
HRESULT CDSprite::SetAniSprite(SPRITE_PTR sprite,int *AniList,int direction)
{
		int frame = AniList[0];
		sprite->Animations[direction] = (int*)malloc((frame+1)*sizeof(int));
		
		for(int index_inner=0;index_inner<frame;index_inner++)
		{
			sprite->Animations[direction][index_inner]=AniList[index_inner+1];
		}
		
		sprite->Animations[direction][frame] = -1;

		return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
//	Name : DrawSprite(HWND m_hWnd,BOOL m_bWindowed,SPRITE_PTR sprite,LPDIRECTDRAWSURFACE7 lpdds)
//  Desc : 
/////////////////////////////////////////////////////////////////////////////
HRESULT CDSprite::DrawSprite(HWND m_hWnd,BOOL m_bWindowed,SPRITE_PTR sprite,LPDIRECTDRAWSURFACE7 lpdds)
{
	RECT dest_rect, source_rect;

	GetWindowRect(m_hWnd,&m_rcWindow);

    DWORD dwFrameWidth    = GetSystemMetrics( SM_CXSIZEFRAME );
    DWORD dwFrameHeight   = GetSystemMetrics( SM_CYSIZEFRAME );
    DWORD dwCaptionHeight = GetSystemMetrics( SM_CYCAPTION );

	int windowx = m_rcWindow.left+dwFrameWidth;
	int windowy = m_rcWindow.top+dwCaptionHeight+dwFrameHeight;

	dest_rect.left   = sprite->PosX;
	dest_rect.top    = sprite->PosY;
	dest_rect.right  = sprite->PosX+sprite->CellWidth;
	dest_rect.bottom = sprite->PosY+sprite->CellHeight;

	source_rect.left    = 0;
	source_rect.top     = 0;
	source_rect.right   = sprite->CellWidth;
	source_rect.bottom  = sprite->CellHeight;

	if (FAILED(lpdds->Blt(&dest_rect, sprite->Cell[sprite->Animations[sprite->CurDirection][sprite->AniIndex]],&source_rect,(DDBLT_WAIT | DDBLT_KEYSRC),NULL)))
	{
		return(0);
	}

	return(1);
}

HRESULT CDSprite::AnimateSprite(SPRITE_PTR sprite,int direction, int dx, int dy, int AniInc, int AniAtt)
{
	sprite->AniAtt = AniAtt;

	if (AniAtt == 1)
	{
		if(!(sprite->CurDirection == direction))
		{
			sprite->CurDirection = direction;
			sprite->AniIndex = 0;
		}
		else
		{
			sprite->AniIndex++;
			sprite->PosX += dx;
			sprite->PosY += dy;
		}

		if (sprite->Animations[sprite->CurDirection][sprite->AniIndex] == -1)
		{
			sprite->AniIndex--;
			sprite->AniAtt = 0;
		}

 	}
	else if(AniAtt == 0)
	{
		if(!(sprite->CurDirection == direction))
		{
			sprite->CurDirection = direction;
			sprite->AniIndex = 0;
		}
		else
		{
			sprite->AniIndex = sprite->AniIndex+AniInc;
			sprite->PosX += dx;
			sprite->PosY += dy;
		}
		
		if(	sprite->Animations[sprite->CurDirection][sprite->AniIndex] == -1)
		{
			sprite->AniIndex = 0;
		}
	}

		
	if(sprite->PosX < 0)
	{
		sprite->PosX = 0;
	}
	else if(sprite->PosX > SCREEN_WIDTH-sprite->CellWidth)
	{
		sprite->PosX = SCREEN_WIDTH-sprite->CellWidth;
	}
	else if(sprite->PosY < 0)
	{
		sprite->PosY = 0;
	}
	else if(sprite->PosY > SCREEN_HEIGHT-sprite->CellHeight)
	{
		sprite->PosY = SCREEN_HEIGHT-sprite->CellHeight;
	}

	return S_OK;
}