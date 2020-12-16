/////////////////////////////////////////////////////////////////////////////
// Name : DMain.h
// Desc : Game Main Loop
// Date : 2002.11.19
// Mail : jhook@hanmail.net
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// INCLUDE
/////////////////////////////////////////////////////////////////////////////
#if !defined _DMAIN_H_
#define _DMAIN_H_

#include <windows.h>

#include "CDDraw.h"
#include "CDBitmap.h"
#include "CDSprite.h"
#include "CDMap.h"
#include "CDInput.H"

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// DEFINE
/////////////////////////////////////////////////////////////////////////////
#define WINDOW_CLASS_NAME	"Defenders"

#define SCREEN_WIDTH        640
#define SCREEN_HEIGHT       480				
#define SCREEN_BPP          32				

#define AnchorSpeed			8

#define KEY_DOWN(vk_code)	((GetAsyncKeyState(vk_code)&0x8000)?1:0)
#define KEY_UP(vk_code)		((GetAsyncKeyState(vk_code)&0x8000)?0:1)

#define PointBind(a,b,c)			{ a.x = b; a.y = c;	}
#define ClippingPoint(a,b,c,d,e)	{if(a.x<b){a.x=b;} if(a.x>d){a.x=d;} if(a.y<c){a.y=c;} if(a.y>e){a.y=e;}}

#define DIR_EA		0
#define DIR_NE		1
#define DIR_NO		2
#define DIR_NW		3
#define DIR_WE		4
#define DIR_SW		5
#define DIR_SO		6
#define DIR_SE		7
#define DEATH		8
#define NOthing		9
#define PlayCut		0
#define PlayScene	1

#define EAFire      10
#define NEFire		11
#define NOFire		12
#define NWFire		13
#define WEFire		14
#define SWFire		15
#define SOFire		16
#define SEFire		17


#define Stop		0
#define LButtonDown 1
#define LButtonUp   2

#define FootPosX    28
#define FootPosY    67

/////////////////////////////////////////////////////////////////////////////
// GLOBAL
/////////////////////////////////////////////////////////////////////////////
HWND		g_hWnd;						// save the window handle
HINSTANCE	g_hInstance;				// save the instance
RECT		g_rcWindow;
BOOL		g_bActive = TRUE;
DWORD		g_dwLastTick;
char		buff[256];

POINT		ptArroScrePix;					// use it for holding screen pixel coordinate of cursor
POINT		ptDestScrePix;				// use it for holding screen pixel coordinate of destination
POINT		ptPlayScrePix;
int			nMoveState;				// use it for checking state of player movement

CDDraw			DDraw;
CDBitmap		DBitmap;
CDSprite		DSprite;
CDMap			DMap;
CDInput			DInput;

CDBitmap::BITMAP_FILE	bitmap;
CDBitmap::BITMAP_IMAGE	image;
CDSprite::SPRITE sprite;


/////////////////////////////////////////////////////////////////////////////
//	TYPE
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//	PROTOTYPES
/////////////////////////////////////////////////////////////////////////////
VOID Game_Init();
VOID Game_Main();
VOID Game_Shutdown();

HRESULT KeyTest();
HRESULT MouseTest();

#endif