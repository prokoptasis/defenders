/////////////////////////////////////////////////////////////////////////////
// Name : CDMAP.h
// Desc : Direct Map Object
// Date : 2002.11.27
// Mail : jhook@hanmail.net
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// INCLUDE
/////////////////////////////////////////////////////////////////////////////
#if !defined _CDMAP_H_
#define _CDMAP_H_

#include <ddraw.h>
#include <io.h>

#include "CDBitmap.h"

/////////////////////////////////////////////////////////////////////////////
// DEFINE
/////////////////////////////////////////////////////////////////////////////
#define CDMAP_INITSTRUCT(d)  { memset(&d,0,sizeof(d)); d.dwSize=sizeof(d); }
#define CDMAP_SAFERELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }
#define CDMAP_PointBind(a,b,c)	{ a.x = b; a.y = c;	}

#define WorldMapWidth   1000
#define WorldMapHeight  1000

#define ViewWidth  640
#define ViewHeight 480

#define CheckRectWidth  14
#define CheckRectHeight 36

#define MapCellNum		7
#define MapObstacle		999

#define CheckPointNum	10

#define MapPathX 40
#define MapPathY 30

#define WayPointCellWidth  16
#define WayPointCellHeight 16

#define AnchSpeed 16

#define _RGB32BIT(a,r,g,b)			((b) + ((g) << 8) + ((r) << 16) + ((a) << 24))
#define CDMAP_ClippingPoint(a,b,c,d,e)	{if(a.x<b){a.x=b;} if(a.x>d){a.x=d;} if(a.y<c){a.y=c;} if(a.y>e){a.y=e;}}
/////////////////////////////////////////////////////////////////////////////
// GLOBAL
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//	TYPE
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//	CLASS
/////////////////////////////////////////////////////////////////////////////
class CDMap  
{
	public:
	CDMap();
	~CDMap();

	typedef struct MAP_OBJ
	{
		int CellIndex;
		int CellWidth;
		int CellHeight;
		int CellNum;

		LPDIRECTDRAWSURFACE7 Cell[256];
	} MAP_OBJ, *MAP_OBJ_PTR;

	MAP_OBJ MapObj;

	typedef struct WayPoint_OBJ
	{
		int index;
		int lowvalue;
		bool passed;
		
		POINT WayPoint;
		POINT WayPointPixel;		
		
		WayPoint_OBJ *next;
		WayPoint_OBJ *prev;
	} WayPoint_OBJ, *WayPoint_PTR;

	WayPoint_PTR pCurrWayPoint;
	WayPoint_PTR pTempWayPoint;

	void CreateMapObj(LPDIRECTDRAW7 lpdd7,MAP_OBJ_PTR Map_Obj,int cellwidth,int cellheight,int cellnum,int MemFlag);
	void LoadMapObj(CDBitmap::BITMAP_FILE_PTR bitmap, MAP_OBJ_PTR Map_Obj);
	void DestroyMapObj(MAP_OBJ_PTR Map_Obj);



	HRESULT DrawMapObj(LPDIRECTDRAW7 lpdd7, LPDIRECTDRAWSURFACE7 lpdds, POINT Arrow);
	HRESULT TileCoord(POINT *Arrow);

	int  Map[WorldMapHeight][WorldMapWidth];

	POINT ptAnchWorlPix;

	int  MapPath[MapPathY][MapPathX];
	bool MapMark[MapPathY][MapPathX];

	HRESULT WayPointWalking(POINT ptPlayScrePix,POINT ptDestScrePix,int *nMoveState,int *CurDirection,int *AniState);
	
	void InitMapPath(POINT ptDestScrePix);
	void MarkMapPath();
	void LinkingWayPoint(POINT ptPlayScrePix,POINT ptDestWorlPix);
	void DrawPoint(LPDIRECTDRAWSURFACE7 lpdds);
	void DirectionChange(POINT ptDestScrePix, POINT ptPlayScrePix,int *nNewDirection);
	void DirectionWalking(POINT ptDestScrePix,POINT ptPlayScrePix,int *nMoveState,int *CurDirection,int *AniState);
	
	bool TestMapObjectCollision(int AnchSpdX,int AnchSpdY);
	bool TestMapObjectCollision(POINT ptCenterPosition);

	void AutoDirection(int *CurDirection,POINT *ptAnchorSpeed);	

	void SetAnchSpeed(int CurDirection,POINT *ptAnchorSpeed);
	void SetAnchSpeed(POINT ptPlayScrePix, POINT ptDestScrePix, POINT *ptAnchorSpeed);
};

#endif 
