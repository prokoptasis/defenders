/////////////////////////////////////////////////////////////////////////////
// Name : CDMap.cpp
// Desc : DDraw Map Class
// Date : 2002.11.27
// Mail : jhook@hanmail.net
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// INCLUDE
/////////////////////////////////////////////////////////////////////////////
#include "CDMap.h"
#include <math.h>
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
CDMap::CDMap()
{
	pCurrWayPoint = NULL;
	pTempWayPoint = NULL;

	for (int j = 0; j < WorldMapHeight; j++)
	{
		for (int i = 0; i < WorldMapWidth; i ++)
		{
			Map[j][i] = rand()%30;
			if(Map[j][i] < 29)
			{
				Map[j][i] = rand()%2+1;
			}
			else if(Map[j][i] == 29)
			{		
				Map[j][i] = 7; 
			}

			//Map[j][i] = rand()%2+1 ;MapCellNum+1;
		}
	}
}

CDMap::~CDMap()
{
	DestroyMapObj(&MapObj);
}

/////////////////////////////////////////////////////////////////////////////
//	Name : CreateMapObj()
//  Desc : 
/////////////////////////////////////////////////////////////////////////////
void CDMap::CreateMapObj(LPDIRECTDRAW7 lpdd7,MAP_OBJ_PTR Map_Obj,int cellwidth,int cellheight,int cellnum,int MemFlag)
{
	// Create MapObj Structure
	Map_Obj->CellWidth	= cellwidth;
	Map_Obj->CellHeight	= cellheight;
	Map_Obj->CellNum		= cellnum;

	for (int index=0; index<Map_Obj->CellNum; index++)
    {
		DDSURFACEDESC2 ddsd;
		CDMAP_INITSTRUCT(ddsd);
		ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
		ddsd.dwWidth  = Map_Obj->CellWidth;
		ddsd.dwHeight = Map_Obj->CellHeight;
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | MemFlag;

		Map_Obj->Cell[index] = NULL;

		if (FAILED(lpdd7->CreateSurface(&ddsd,&(Map_Obj->Cell[index]),NULL)))
		{
			OutputDebugString("Map Object Offsurface Create Failed");
		}

	    DDCOLORKEY color_key;
		color_key.dwColorSpaceLowValue  = 0;
		color_key.dwColorSpaceHighValue = 0;

	    (Map_Obj->Cell[index])->SetColorKey(DDCKEY_SRCBLT, &color_key);
	}
}
/////////////////////////////////////////////////////////////////////////////
//	Name : LoadMapObj()
//  Desc : 
/////////////////////////////////////////////////////////////////////////////
void CDMap::LoadMapObj(CDBitmap::BITMAP_FILE_PTR bitmap,MAP_OBJ_PTR Map_Obj)
{
	// Load Bitmap
	if (!bitmap)
	{
		OutputDebugString("Map Object Bitmpa Empty");
	}

	DWORD offsetX,offsetY;
	int index_off_X = 0;
	int index_off_Y = 0;

	for(int index=0;index<Map_Obj->CellNum;index++)
	{

		DDSURFACEDESC2 ddsd;
		CDMAP_INITSTRUCT(ddsd);

		if(FAILED(Map_Obj->Cell[index]->Lock(NULL,&ddsd,DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR,NULL)))
		{
			OutputDebugString("Map Object Cell Locking Failed");
		}
		
		DWORD *dest_buffer= (DWORD *)ddsd.lpSurface;		

		if( 1+(1+Map_Obj->CellWidth)*(index_off_X) >= bitmap->bitmapinfoheader.biWidth )
		{
			index_off_X = 0;
			index_off_Y++;
		}

		offsetX = 1+(1+Map_Obj->CellWidth)*index_off_X;
		offsetY = 1+(1+Map_Obj->CellHeight)*index_off_Y;
		
		int etc = bitmap->bitmapinfoheader.biWidth%4;

		for(int index_y=0;index_y<Map_Obj->CellHeight;index_y++)
		{
			for(int index_x=0;index_x<Map_Obj->CellWidth;index_x++)
			{
				DWORD	blue  = (bitmap->buffer[(offsetY+index_y)*(bitmap->bitmapinfoheader.biWidth*3+etc) + (offsetX+index_x)*3 + 0]),
						green = (bitmap->buffer[(offsetY+index_y)*(bitmap->bitmapinfoheader.biWidth*3+etc) + (offsetX+index_x)*3 + 1]),
						red   = (bitmap->buffer[(offsetY+index_y)*(bitmap->bitmapinfoheader.biWidth*3+etc) + (offsetX+index_x)*3 + 2]);

				DWORD pixel = _RGB32BIT(0,red,green,blue);

				dest_buffer[((index_x)*4+(index_y)*ddsd.lPitch)>>2] = pixel;

			}
		}
		Map_Obj->Cell[index]->Unlock(NULL);

		index_off_X++;
	}
}

/////////////////////////////////////////////////////////////////////////////
//	Name : DestroyMapObj(MAP_OBJ_PTR MapObj)
//  Desc : 
/////////////////////////////////////////////////////////////////////////////
void CDMap::DestroyMapObj(MAP_OBJ_PTR Map_Obj)
{
	for(int index = 0;index < Map_Obj->CellNum;index++)
	{
		CDMAP_SAFERELEASE(Map_Obj->Cell[index]);
	}
}

/////////////////////////////////////////////////////////////////////////////
//	Name : DrawMapObj()
//  Desc : 
/////////////////////////////////////////////////////////////////////////////
HRESULT CDMap::DrawMapObj(LPDIRECTDRAW7 lpdd7, LPDIRECTDRAWSURFACE7 lpdds, POINT ptArrow)
{
	RECT  rcBlitRect;				// use it for Blitting source Rect

	POINT ptOffSet;					// use it for offset margin which occur number of line
	RECT  rcOffRect;				// World Tile Coordinate Rect	
	CDMAP_PointBind(ptOffSet,0,0);	// initialize offset margin (it will occur when blit rect be on even line or odd line)


	// Calculate World Tile Coordinate Rectangle
	rcOffRect.left =  ptAnchWorlPix.x / MapObj.CellWidth;	
	rcOffRect.top  = (ptAnchWorlPix.y / MapObj.CellHeight) * 2 ;
	
	if ( ptAnchWorlPix.x % MapObj.CellWidth  < MapObj.CellWidth /2 && rcOffRect.left > 0 ) { rcOffRect.left--; ptOffSet.x = MapObj.CellWidth;}
	if ( ptAnchWorlPix.y % MapObj.CellHeight < MapObj.CellHeight/2 && rcOffRect.top > 0) { rcOffRect.top--;  ptOffSet.y = MapObj.CellHeight/2; }

	rcOffRect.right   =  rcOffRect.left + ViewWidth / MapObj.CellWidth + 2;
	rcOffRect.bottom  =  rcOffRect.top  + (ViewHeight / MapObj.CellHeight) * 2 + 2;
	// Calculate World Tile Coordinate Rectangle

	LPDIRECTDRAWSURFACE7 offsurface;	// Creat off surface

	DDSURFACEDESC2 ddsd; 
	CDMAP_INITSTRUCT(ddsd);
	
	ddsd.dwFlags		= DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
	ddsd.dwWidth		= (rcOffRect.right-rcOffRect.left)*MapObj.CellWidth + MapObj.CellWidth/2;
	ddsd.dwHeight		= ((rcOffRect.bottom-rcOffRect.top)/2+(rcOffRect.bottom-rcOffRect.top)%2)*MapObj.CellHeight + ((rcOffRect.bottom-rcOffRect.top+1)%2)*(MapObj.CellHeight/2);
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | NULL;

	if (FAILED(lpdd7->CreateSurface(&ddsd,&offsurface,NULL))) { return E_FAIL; }

    DDCOLORKEY color_key;	// Set Color Key
	color_key.dwColorSpaceLowValue  = RGB(0,0,0);
	color_key.dwColorSpaceHighValue = RGB(0,0,0);

	offsurface->SetColorKey(DDCKEY_SRCBLT, &color_key);

	DDBLTFX ddbltfx;	// Clear Surface
	CDMAP_INITSTRUCT(ddbltfx);
	ddbltfx.dwFillColor = 0;

	offsurface->Blt(NULL,NULL,NULL,DDBLT_COLORFILL|DDBLT_WAIT,&ddbltfx);

	for (int index_y = 0 ; index_y < rcOffRect.bottom - rcOffRect.top ; index_y++)		// Blit Map Object to offsurface
	{
		for (int index_x = 0 ; index_x < rcOffRect.right-rcOffRect.left ; index_x++)
		{
			rcBlitRect.left   = index_x*MapObj.CellWidth+((rcOffRect.top+index_y)%2)*(MapObj.CellWidth/2);
			rcBlitRect.top    = index_y*(MapObj.CellHeight/2);
			rcBlitRect.right  = rcBlitRect.left + MapObj.CellWidth;
			rcBlitRect.bottom = rcBlitRect.top  + MapObj.CellHeight;

			offsurface->Blt( &rcBlitRect, MapObj.Cell[Map[rcOffRect.top + index_y][rcOffRect.left + index_x]], NULL,DDBLT_WAIT | DDBLT_KEYSRC,NULL);
		}
	}	

	TileCoord(&ptArrow);
	
	rcBlitRect.left   = (ptArrow.x-rcOffRect.left)*MapObj.CellWidth + ((ptArrow.y)%2)*(MapObj.CellWidth/2);
	rcBlitRect.top    = (ptArrow.y-rcOffRect.top)*(MapObj.CellHeight/2);
	rcBlitRect.right  = rcBlitRect.left + MapObj.CellWidth;
	rcBlitRect.bottom = rcBlitRect.top  + MapObj.CellHeight;

	offsurface->Blt( &rcBlitRect, MapObj.Cell[0], NULL,DDBLT_WAIT | DDBLT_KEYSRC,NULL);

	rcBlitRect.left    = ptAnchWorlPix.x%MapObj.CellWidth  + ptOffSet.x;
	rcBlitRect.top     = ptAnchWorlPix.y%MapObj.CellHeight + ptOffSet.y;
	rcBlitRect.right   = rcBlitRect.left + ViewWidth ;	
	rcBlitRect.bottom  = rcBlitRect.top + ViewHeight ;	

	lpdds->Blt(NULL,offsurface, &rcBlitRect, DDBLT_WAIT | DDBLT_KEYSRC,NULL);
	
	CDMAP_SAFERELEASE(offsurface);

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
//	Name : TileCoord( POINT *ptArrow)
//  Desc : Output World Tile Coordinate from Scrren Pixel Coordinate
/////////////////////////////////////////////////////////////////////////////
HRESULT CDMap::TileCoord( POINT *ptArrow)
{
	int BeforeDecideX = (ptAnchWorlPix.x + ptArrow->x)%MapObj.CellWidth;
	int BeforeDecideY = (ptAnchWorlPix.y + ptArrow->y)%MapObj.CellHeight;

	int AfterX,AfterY, Local, Area, Final;

	Local = 0; Area = 0; Final = 0;

	if (BeforeDecideY < MapObj.CellHeight/2) 
	{
		if	 (BeforeDecideX < MapObj.CellWidth/2)	{ AfterX = BeforeDecideX; AfterY = BeforeDecideY; Area = 0; }
		else										{ AfterX = BeforeDecideX%(MapObj.CellWidth/2); AfterY = BeforeDecideY; Area = 1; }
	}
	else 
	{
		if   (BeforeDecideX < MapObj.CellWidth/2)	{ AfterX = BeforeDecideX; AfterY = BeforeDecideY%(MapObj.CellHeight/2); Area = 2; }
		else										{  AfterX = BeforeDecideX%(MapObj.CellWidth/2); AfterY = BeforeDecideY%(MapObj.CellHeight/2); Area = 3; }
	}

	if ( (MapObj.CellHeight/2)*AfterX/(MapObj.CellWidth/2) > AfterY )								// Local 1 or 2
	{
		if ( (MapObj.CellHeight/2)-(MapObj.CellHeight/2)*AfterX/(MapObj.CellWidth/2) > AfterY )	{ Local = 1; }
		else																						{ Local = 2; }
	}
	else																							// Local 0 or 3
	{
		if ( (MapObj.CellHeight/2)-(MapObj.CellHeight/2)*AfterX/(MapObj.CellWidth/2) > AfterY )	{ Local = 0; }
		else																						{ Local = 3; }		
	}

	if(Area == 0)		{ if(Local == 0 || Local == 1) { Final = 1; } }
	else if (Area ==1)	{ if(Local == 1 || Local == 2) { Final = 2; } }
	else if (Area ==2)	{ if(Local == 0 || Local == 3) { Final = 3; } }
	else if (Area ==3)	{ if(Local == 2 || Local == 3) { Final = 4; } }

	ptArrow->x = (ptAnchWorlPix.x + ptArrow->x)/MapObj.CellWidth;
	ptArrow->y = ((ptAnchWorlPix.y + ptArrow->y)/MapObj.CellHeight) *2; 
	
	if		(Final == 1) { ptArrow->x--; ptArrow->y--; }
	else if	(Final == 2) { ptArrow->y--; }
	else if	(Final == 3) { ptArrow->x--; ptArrow->y++; }
	else if	(Final == 4) { ptArrow->y++; }

	return S_OK;
}

void CDMap::InitMapPath(POINT ptDestScrePix)
{
	int y,x,index_y,index_x;

	for(y = 0; y < MapPathY; y++)
	{
		for(x = 0; x < MapPathX; x++)
		{
			POINT ptTempWorlTil;
			bool ObstaclePoint = false;

			for (index_y = 0 ; index_y < 3; index_y++)	// 중심에서 구석의 9점을 테스트하고 장애물일 경우 표시한다.
			{
				for(index_x = 0 ; index_x < 3; index_x++)
				{
					CDMAP_PointBind(ptTempWorlTil,WayPointCellWidth*x+index_x*(WayPointCellWidth/2),WayPointCellHeight*y+index_y*(WayPointCellHeight/2));

					TileCoord(&ptTempWorlTil);

					if(Map[ptTempWorlTil.y][ptTempWorlTil.x] == 7)
					{
						ObstaclePoint = true;
					}
				}
			}

			if(ObstaclePoint)
			{
				MapPath[y][x] = 999;
				MapMark[y][x] =false;
			}
			else
			{
				MapPath[y][x] = -1;
				MapMark[y][x] =false;
			}
		}
	}

	MapPath[ptDestScrePix.y/WayPointCellHeight][ptDestScrePix.x/WayPointCellWidth] = 0;
}

void CDMap::MarkMapPath()
{
	bool found;
	int y,x,index_y,index_x;

	do
	{
		found = false;

		for(y=0;y<MapPathY;y++)
		{
			for(x=0;x<MapPathX;x++)	// 오프 서피스의 넓이 만큼 돈다 (타일 단위)
			{
				if(MapPath[y][x]==-1)			// 아직 추적하지 않은 값이라면 
				{
					for( index_y = 0; index_y < 3; index_y++)		// 센타에서 이소메트릭스에 맞게 주위를 돈다
					{
						for( index_x = 0; index_x < 3; index_x++)	// 센타에서 이소메트릭스에 맞게 주위를 돈다
						{
							if( x+(index_x-1) >= 0 && y+(index_y-1) >= 0 && x+(index_x-1) < MapPathX && y+(index_y-1) < MapPathY )
							{
								if( MapPath[y+(index_y-1)][x+(index_x-1)] >= 0 && MapPath[y+(index_y-1)][x+(index_x-1)] != MapObstacle )	
								{
									MapMark[y][x]=true;
									found=true;
								}
							}
						}
					}
				}
			}
		}

		for(y=0;y<MapPathY;y++)
		{
			for(x=0;x<MapPathX;x++)	// 오프 서피스의 넓이 만큼 돈다 (타일 단위)
			{
				if(MapMark[y][x])	// 해당 좌표가 true로 마킹 되어 있다면
				{
					int lowvalue = MapObstacle;

					for( index_y = 0; index_y < 3; index_y++)	// 센타에서 이소메트릭스에 맞게 주위를 돈다
					{
						for( index_x = 0; index_x < 3; index_x++)	// 센타에서 이소메트릭스에 맞게 주위를 돈다
						{
							if( x+(index_x-1) >= 0 && y+(index_y-1) >= 0 && x+(index_x-1) < MapPathX && y+(index_y-1) < MapPathY )
							{
								if( MapPath[y+(index_y-1)][x+(index_x-1)] >= 0 )
								{
									if( MapPath[y+(index_y-1)][x+(index_x-1)] < lowvalue )
									{
										lowvalue = MapPath[y+(index_y-1)][x+(index_x-1)];
									}
								}
							}
						}
					}

					MapPath[y][x] = lowvalue + 1;
				}
			}
		}

	} while (found);
}

void CDMap::LinkingWayPoint(POINT ptPlayScrePix,POINT ptDestWorlPix)
{
	int y,index_x,index_y;

	if(pCurrWayPoint != NULL)
	{
		while (pCurrWayPoint->next != NULL)
		{
			pTempWayPoint = pCurrWayPoint->next;
			delete pCurrWayPoint;
			pCurrWayPoint = pTempWayPoint;
		}
		pCurrWayPoint = NULL;
		pTempWayPoint = NULL;
	}

	// 맵 테두리에서 안쪽으로 웨이포인트가 잡히지 않도록 바꾸어 주어야 함. 현재로서는 우선... 임시 땜빵.
	if(ptDestWorlPix.x < 32+ 292 + 20 || ptDestWorlPix.x > 65000 - 292 - 20)
	{
		return;
	}
	else if (ptDestWorlPix.y < 16 + 204 + 16 || ptDestWorlPix.y > 15000 - 204 - 16)
	{
		return;
	}

	if(MapPath[ptPlayScrePix.y/WayPointCellHeight][ptPlayScrePix.x/WayPointCellWidth]==MapObstacle)
	{
		int i = 8;
		bool Exit = false;

		do
		{
			for(index_y = 0; index_y < 3; index_y ++)
			{
				for(index_x = 0; index_x < 3; index_x ++)
				{
					POINT ptTempPoint;
					ptTempPoint.x  = ptPlayScrePix.x + i*(index_x-1);
					ptTempPoint.y  = ptPlayScrePix.y + i*(index_y-1);

					if(MapPath[ptTempPoint.y/WayPointCellHeight][ptTempPoint.x/WayPointCellWidth]!=MapObstacle)
					{
						ptPlayScrePix = ptTempPoint;
						Exit = true;
					}
				}
			}
			i=i+8;
		} while(!Exit);
	}

	for( y = 0; y < MapPath[ptPlayScrePix.y/WayPointCellHeight][ptPlayScrePix.x/WayPointCellWidth]+1; y++ ) // 현재 포지션의 로우벨류 만큼 웨이포인트를 찍는다.
	{
		if(y == 0)
		{
			pCurrWayPoint = new WayPoint_OBJ;

			pCurrWayPoint->next		= NULL;
			pCurrWayPoint->prev		= NULL;
				
			pTempWayPoint = pCurrWayPoint;

			pCurrWayPoint->index	= y;
			pCurrWayPoint->passed   = false;
			pCurrWayPoint->WayPoint.x = ptPlayScrePix.x/WayPointCellWidth;
			pCurrWayPoint->WayPoint.y = ptPlayScrePix.y/WayPointCellHeight;
			pCurrWayPoint->WayPointPixel.x = (pCurrWayPoint->WayPoint.x*WayPointCellWidth + WayPointCellWidth/2)+ptAnchWorlPix.x;
			pCurrWayPoint->WayPointPixel.y = (pCurrWayPoint->WayPoint.y*WayPointCellHeight + WayPointCellHeight/2)+ptAnchWorlPix.y;
			pCurrWayPoint->lowvalue = MapPath[pCurrWayPoint->WayPoint.y][pCurrWayPoint->WayPoint.x];
		}
		else
		{
			pCurrWayPoint = new WayPoint_OBJ;

			pTempWayPoint->next = pCurrWayPoint;
				
			pCurrWayPoint->prev	= pTempWayPoint;
			pCurrWayPoint->next	= NULL;
				
			pTempWayPoint = pCurrWayPoint;

			pCurrWayPoint->index	= y;
			pCurrWayPoint->lowvalue = pCurrWayPoint->prev->lowvalue - 1;
			pCurrWayPoint->passed   = false;
			pCurrWayPoint->WayPoint.x = -1;
			pCurrWayPoint->WayPoint.y = -1;
			pCurrWayPoint->WayPointPixel.x = -1;
			pCurrWayPoint->WayPointPixel.y = -1;

			for( index_y = 0; index_y < 3; index_y++)	// 센타에서 이소메트릭스에 맞게 주위를 돈다
			{
				for( index_x = 0; index_x < 3; index_x++)	// 센타에서 이소메트릭스에 맞게 주위를 돈다
				{
					int CheckTileX = pCurrWayPoint->prev->WayPoint.x + (index_x-1);
					int CheckTileY = pCurrWayPoint->prev->WayPoint.y + (index_y-1);
					int CheckPixeX = ((pCurrWayPoint->prev->WayPoint.x + (index_x-1))*WayPointCellWidth + WayPointCellWidth/2)+ptAnchWorlPix.x;
					int CheckPixeY = ((pCurrWayPoint->prev->WayPoint.y + (index_y-1))*WayPointCellHeight + WayPointCellHeight/2)+ptAnchWorlPix.y;

					if(CheckTileX >= 0 && CheckTileY >= 0 && CheckTileX < MapPathX && CheckTileY < MapPathY )//&& (CheckTileX != pCurrWayPoint->prev->WayPoint.x && CheckTileY != pCurrWayPoint->prev->WayPoint.y)\					
					{
						if(MapPath[CheckTileY][CheckTileX] == pCurrWayPoint->lowvalue)// && MapPath[CheckTileY][CheckTileX] != MapObstacle)
						{
							POINT ptCenterPosition;
							ptCenterPosition.x = ((pCurrWayPoint->prev->WayPoint.x + (index_x-1))*WayPointCellWidth + WayPointCellWidth/2);
							ptCenterPosition.y = ((pCurrWayPoint->prev->WayPoint.y + (index_y-1))*WayPointCellHeight + WayPointCellHeight/2);

							if(TestMapObjectCollision(ptCenterPosition))
							{
							if (pCurrWayPoint->WayPoint.x < 0 && pCurrWayPoint->WayPoint.y < 0)
							{
								pCurrWayPoint->WayPoint.x = CheckTileX;
								pCurrWayPoint->WayPoint.y = CheckTileY;
								pCurrWayPoint->WayPointPixel.x = CheckPixeX;
								pCurrWayPoint->WayPointPixel.y = CheckPixeY;
							}
							else 
							{
								float CDistance = 10;//(float)sqrt(abs(ptDestWorlPix.x-CheckPixeX)*abs(ptDestWorlPix.x-CheckPixeX) + abs(ptDestWorlPix.y-CheckPixeY)*abs(ptDestWorlPix.y-CheckPixeY));
								float PDistance = 10;//(float)sqrt(abs(ptDestWorlPix.x-pCurrWayPoint->WayPointPixel.x)*abs(ptDestWorlPix.x-pCurrWayPoint->WayPointPixel.x) + abs(ptDestWorlPix.y-pCurrWayPoint->WayPointPixel.y)*abs(ptDestWorlPix.y-pCurrWayPoint->WayPointPixel.y));
								float CPDistance = 10;//(float)sqrt(abs((ptPlayScrePix.x+ptAnchWorlPix.x)-CheckPixeX)*abs((ptPlayScrePix.x+ptAnchWorlPix.x)-CheckPixeX) + abs((ptPlayScrePix.y+ptAnchWorlPix.y)-CheckPixeY)*abs((ptPlayScrePix.y+ptAnchWorlPix.y)-CheckPixeY));
								float PPDistance = 10;//(float)sqrt(abs((ptPlayScrePix.x+ptAnchWorlPix.x)-pCurrWayPoint->WayPointPixel.x)*abs((ptPlayScrePix.x+ptAnchWorlPix.x)-pCurrWayPoint->WayPointPixel.x) + abs((ptPlayScrePix.y+ptAnchWorlPix.y)-pCurrWayPoint->WayPointPixel.y)*abs((ptPlayScrePix.y+ptAnchWorlPix.y)-pCurrWayPoint->WayPointPixel.y));
								
								if(CDistance <= PDistance && CPDistance <= PPDistance)
								{
									pCurrWayPoint->WayPoint.x = CheckTileX;
									pCurrWayPoint->WayPoint.y = CheckTileY;
									pCurrWayPoint->WayPointPixel.x = CheckPixeX;
									pCurrWayPoint->WayPointPixel.y = CheckPixeY;
								}
							}
							}
						}
					}
				}
			}
		}

	}

	if(pCurrWayPoint != NULL)
	{
		while (pCurrWayPoint->index != 0)
		{		
			if(pCurrWayPoint->prev != NULL)
			{
				pTempWayPoint = pCurrWayPoint->prev;
				pCurrWayPoint = pTempWayPoint;
			}
		}
	}


}

void CDMap::DrawPoint( LPDIRECTDRAWSURFACE7 lpdds)
{
	int i = 0;
	
	if(pCurrWayPoint != NULL)
	{
		i = pCurrWayPoint->index;

		while(pCurrWayPoint->next != NULL)
		{
			RECT rcBlitRect;
			rcBlitRect.left   = (pCurrWayPoint->WayPointPixel.x-ptAnchWorlPix.x);
			rcBlitRect.top    =	(pCurrWayPoint->WayPointPixel.y-ptAnchWorlPix.y);
			rcBlitRect.right  = rcBlitRect.left+WayPointCellWidth;
			rcBlitRect.bottom = rcBlitRect.top+WayPointCellHeight;

			RECT rcSourceRect;
			rcSourceRect.left = 0;
			rcSourceRect.top = 0;
			rcSourceRect.right = 16;
			rcSourceRect.bottom = 16;

			lpdds->Blt(&rcBlitRect,MapObj.Cell[8],&rcSourceRect, DDBLT_WAIT | DDBLT_KEYSRC,NULL);

			pCurrWayPoint = pCurrWayPoint->next;
			pTempWayPoint = pCurrWayPoint;
		}			
		
		while (pCurrWayPoint->index != i)
		{		
			if(pCurrWayPoint->prev != NULL)
			{
				pTempWayPoint = pCurrWayPoint->prev;
				pCurrWayPoint = pTempWayPoint;
			}
		}
	}
}

void CDMap::DirectionChange(POINT ptDestScrePix, POINT ptPlayScrePix, int *nNewDirection)
{
	int MarginX = (ptDestScrePix.x) - (ptPlayScrePix.x);
	int MarginY = (ptDestScrePix.y) - (ptPlayScrePix.y);

	if(MarginX >= 0 && MarginY <= 0) // Local 1
	{
		if		(abs(MarginX) < 64) { *nNewDirection = 2;}
		else if (abs(MarginY) < 32) { *nNewDirection = 0;}
		else						{ *nNewDirection = 1;}
	}
	else if(MarginX <= 0 && MarginY <= 0) // Local 2
	{
		if		(abs(MarginX) < 64) { *nNewDirection = 2;}
		else if (abs(MarginY) < 32) { *nNewDirection = 4;}
		else						{ *nNewDirection = 3;}
	}
	else if(MarginX <= 0 && MarginY >= 0) // Local 3
	{
		if		(abs(MarginX) < 64) { *nNewDirection = 6;}
		else if (abs(MarginY) < 32) { *nNewDirection = 4;}
		else			 			{ *nNewDirection = 5;}
	}
	else if(MarginX >= 0 && MarginY >= 0) // Local 4
	{
		if		(abs(MarginX) < 64) { *nNewDirection = 6;}
		else if (abs(MarginY) < 32) { *nNewDirection = 0;}
		else						{ *nNewDirection = 7;}
	}
}

bool CDMap::TestMapObjectCollision(int AnchSpdX,int AnchSpdY)
{
	POINT ptPlayerPosition;
	ptPlayerPosition.x = 292 + 20 + AnchSpdX;
	ptPlayerPosition.y = 204 + 60 + AnchSpdY;

	for(int index_y = 0; index_y < 3; index_y++)
	{
		for(int index_x = 0; index_x < 3; index_x++)
		{
			POINT ptTempPosition;
			ptTempPosition.x = ptPlayerPosition.x + index_x*8;
			ptTempPosition.y = ptPlayerPosition.y + index_y*4;
			TileCoord(&ptTempPosition);

			if( Map[ptTempPosition.y][ptTempPosition.x] == 7 )
			{
				return false;
			}
		}
	}
	return true;
}

bool CDMap::TestMapObjectCollision(POINT ptCenterPosition)
{
	POINT ptTestPosition;
	ptTestPosition.x = ptCenterPosition.x; //= ptCenterPosition.x - 292 - 20;
	ptTestPosition.y = ptCenterPosition.y; //= ptCenterPosition.y - 204 - 60;

	for(int index_y = 0; index_y < 3; index_y++)
	{
		for(int index_x = 0; index_x < 3; index_x++)
		{
			POINT ptTempPosition;
			ptTempPosition.x = ptTestPosition.x + (index_x-1)*8;
			ptTempPosition.y = ptTestPosition.y + (index_y-1)*4;
			TileCoord(&ptTempPosition);

			if( Map[ptTempPosition.y][ptTempPosition.x] == 7 )
			{
				return false;
			}
		}
	}
	return true;
}

void CDMap::AutoDirection(int *CurDirection,POINT *ptAnchorSpeed)
{
	POINT ptTestSpeed;
	int gap[7]={1,-1,2,-2,3,-3,4};	
	
	for(int index = 0; index < 7; index ++)
	{
		int TestDirection = *CurDirection+gap[index];

		if(TestDirection > 7)
		{
			TestDirection = TestDirection%8;
		}
		else if( TestDirection < 0)
		{
			TestDirection = 8 + TestDirection;
		}

		SetAnchSpeed(TestDirection,&ptTestSpeed);

		if(TestMapObjectCollision(ptTestSpeed.x,ptTestSpeed.y))
		{
			*CurDirection = TestDirection;
			*ptAnchorSpeed = ptTestSpeed;
			return;
		}
	}

	ptAnchorSpeed->x = 0;	
	ptAnchorSpeed->y = 0;
	return;
}

void CDMap::SetAnchSpeed(int CurDirection,POINT *ptAnchorSpeed)
{	
	if(CurDirection == 0)
	{
		ptAnchorSpeed->x = AnchSpeed;
		ptAnchorSpeed->y = 0;
	}
	else if(CurDirection == 1)
	{
		ptAnchorSpeed->x = AnchSpeed;
		ptAnchorSpeed->y = -AnchSpeed;
	}
	else if(CurDirection == 2)
	{
		ptAnchorSpeed->x = 0;
		ptAnchorSpeed->y = -AnchSpeed;
	}
	else if(CurDirection == 3)
	{
		ptAnchorSpeed->x = -AnchSpeed;
		ptAnchorSpeed->y = -AnchSpeed;
	}
	else if(CurDirection == 4)
	{
		ptAnchorSpeed->x = -AnchSpeed;
		ptAnchorSpeed->y = 0;
	}
	else if(CurDirection == 5)
	{
		ptAnchorSpeed->x = -AnchSpeed;
		ptAnchorSpeed->y = AnchSpeed;
	}
	else if(CurDirection == 6)
	{
		ptAnchorSpeed->x = 0;
		ptAnchorSpeed->y = AnchSpeed;
	}
	else if(CurDirection == 7)
	{
		ptAnchorSpeed->x = AnchSpeed;
		ptAnchorSpeed->y = AnchSpeed;
	}
}

void CDMap::SetAnchSpeed(POINT ptPlayScrePix, POINT ptDestScrePix, POINT *ptAnchorSpeed)
{
	ptAnchorSpeed->x = ptDestScrePix.x - ptPlayScrePix.x;
	ptAnchorSpeed->y = ptDestScrePix.y - ptPlayScrePix.y;
}

void CDMap::DirectionWalking(POINT ptDestScrePix,POINT ptPlayScrePix,int *nMoveState,int *CurDirection,int *AniState)
{
	POINT ptAnchSpeed;
	CDMAP_PointBind(ptAnchSpeed,0,0);
	
	DirectionChange(ptDestScrePix,ptPlayScrePix,CurDirection);
	SetAnchSpeed(*CurDirection,&ptAnchSpeed);

	if(!TestMapObjectCollision(ptAnchSpeed.x,ptAnchSpeed.y))
	{
		AutoDirection(CurDirection,&ptAnchSpeed);
	}

	if(	ptAnchSpeed.x == 0 && ptAnchSpeed.y == 0 )
	{
		*AniState = 0;
	}

	CDMAP_PointBind(ptAnchWorlPix,ptAnchWorlPix.x+ptAnchSpeed.x,ptAnchWorlPix.y+ptAnchSpeed.y);
}


/////////////////////////////////////////////////////////////////////////////
//	Name : WayPointWalking( POINT *ptArrow)
//  Desc : Output World Tile Coordinate from Scrren Pixel Coordinate
/////////////////////////////////////////////////////////////////////////////
HRESULT CDMap::WayPointWalking( POINT ptPlayScrePix,POINT ptDestScrePix,int *nMoveState,int *CurDirection,int *AniState)
{
	POINT ptDestWorlPix;													// Destination Position World Pixel Coordinate
	ptDestWorlPix.x = ptAnchWorlPix.x + ptDestScrePix.x;
	ptDestWorlPix.y = ptAnchWorlPix.y + ptDestScrePix.y;

	POINT ptPlayWorlPix;													// Player Position World Pixel Coordinate
	ptPlayWorlPix.x = ptAnchWorlPix.x +ptPlayScrePix.x;
	ptPlayWorlPix.y = ptAnchWorlPix.y +ptPlayScrePix.y;

	POINT ptAnchWorlTil;													// Anchor World Tile Coordinate
	CDMAP_PointBind(ptAnchWorlTil,0,0);
	TileCoord(&ptAnchWorlTil);										
	
	POINT ptDestWorlTil;													// Destination World Tile Coordinate
	CDMAP_PointBind(ptDestWorlTil,ptDestScrePix.x,ptDestScrePix.y);
	TileCoord(&ptDestWorlTil);										

	POINT ptPlayWorlTil;													// PlayerPosition World Tile Coordinate
	CDMAP_PointBind(ptPlayWorlTil,ptPlayScrePix.x,ptPlayScrePix.y);
	TileCoord(&ptPlayWorlTil);										

	if(*nMoveState == 2)
	{
		InitMapPath(ptDestScrePix);
		MarkMapPath();
		LinkingWayPoint(ptPlayScrePix,ptDestWorlPix);

		*nMoveState = 3;
	}

	if(*nMoveState == 3)
	{
		if(pCurrWayPoint != NULL)
		{
			if(pCurrWayPoint->next != NULL)
			{
				POINT ptTempPoint;
				ptTempPoint.x = pCurrWayPoint->WayPointPixel.x - ptAnchWorlPix.x;
				ptTempPoint.y = pCurrWayPoint->WayPointPixel.y - ptAnchWorlPix.y;

				POINT ptAnchSpeed;
				CDMAP_PointBind(ptAnchSpeed,0,0);

				int TempDirection = *CurDirection;										// 방향은 바꾸지 않는다.
				DirectionChange(ptTempPoint,ptPlayScrePix,&TempDirection);				// 새로운 방향을 설정한다.

				SetAnchSpeed(ptPlayScrePix,ptTempPoint,&ptAnchSpeed);								// 새로운 방향에 따른 속도 설정

				CDMAP_PointBind(ptAnchWorlPix,ptAnchWorlPix.x+ptAnchSpeed.x,ptAnchWorlPix.y+ptAnchSpeed.y); // 이동한다.

				pCurrWayPoint->passed = true;
				pCurrWayPoint = pCurrWayPoint->next;
				pTempWayPoint = pCurrWayPoint;

				*AniState = 1;
			}
			else
			{
				*AniState = 0;
				*nMoveState = 0;
			}
		}
	}

	return S_OK;
}
