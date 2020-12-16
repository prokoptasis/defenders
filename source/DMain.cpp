/////////////////////////////////////////////////////////////////////////////
// Name : DMain.cpp
// Desc : Game Main Loop
// Date : 2002.11.17
// Mail : jhook@hanmail.net
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// INCLUDE
/////////////////////////////////////////////////////////////////////////////
#include "DMain.h"
/////////////////////////////////////////////////////////////////////////////
// DEFINE
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
//	Test
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//	FUNCTIONS
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//	Name : WinProc
//  Desc : 
/////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK WindowProc(HWND hwnd,UINT msg,WPARAM wparam,LPARAM lparam)
{
	PAINTSTRUCT ps;
	HDC			hdc;

	static int SM_ScreenX,SM_ScreenY;
	int Window_Gap;

	switch(msg)
	{
		case WM_CREATE:		
		{ 
			SM_ScreenX = GetSystemMetrics(SM_CXSCREEN);
			SM_ScreenY = GetSystemMetrics(SM_CYSCREEN);
			return 0L;
		}
	
		case WM_WINDOWPOSCHANGING:
		{
			// Creating Magnetic Window
			Window_Gap = ((LPWINDOWPOS)lparam)->x;

			if (Window_Gap < 30)
			{
				Window_Gap = 0;
			}
			
			if (Window_Gap + ((LPWINDOWPOS)lparam)->cx > SM_ScreenX - 30)
			{
				Window_Gap = SM_ScreenX - ((LPWINDOWPOS)lparam)->cx;
			}

			((LPWINDOWPOS)lparam)->x = Window_Gap;

			Window_Gap = ((LPWINDOWPOS)lparam)->y;

			if (Window_Gap < 30) 
			{
				Window_Gap=0;
			}

			if (Window_Gap + ((LPWINDOWPOS)lparam)->cy + 30 > SM_ScreenY - 30)
			{
				Window_Gap = SM_ScreenY - ((LPWINDOWPOS)lparam)->cy - 28;
			}
			
			((LPWINDOWPOS)lparam)->y = Window_Gap;
			
			return 0L;
		}

		case WM_PAINT:		
		{ 
			hdc=BeginPaint(hwnd,&ps);
			EndPaint(hwnd,&ps);
			return 0L;
		}
		
		case WM_DESTROY:	
		{ 
			PostQuitMessage(0);
			return 0L;
		}
		
		case WM_COMMAND:	
		{
			switch( LOWORD(wparam) )
			{
				case IDM_TOGGLEFULLSCREEN:
				{
					Game_Init();
				} return 0L;
			}
		}

        case WM_GETMINMAXINFO:
		{
			MINMAXINFO* pMinMax = (MINMAXINFO*) lparam;

			DWORD dwFrameWidth    = GetSystemMetrics( SM_CXSIZEFRAME );
			DWORD dwFrameHeight   = GetSystemMetrics( SM_CYSIZEFRAME );
			DWORD dwMenuHeight    = GetSystemMetrics( SM_CYMENU );
			DWORD dwCaptionHeight = GetSystemMetrics( SM_CYCAPTION );

			pMinMax->ptMinTrackSize.x = SCREEN_WIDTH  + dwFrameWidth * 2;
			pMinMax->ptMinTrackSize.y = SCREEN_HEIGHT + dwFrameHeight * 2 + dwCaptionHeight;

			pMinMax->ptMaxTrackSize.x = pMinMax->ptMinTrackSize.x;
			pMinMax->ptMaxTrackSize.y = pMinMax->ptMinTrackSize.y;
		} return 0L;

		case WM_ACTIVATE:
		{
			if(LOWORD(wparam) == WA_INACTIVE)
			{
				g_bActive = FALSE;
			}
			else
			{
				g_bActive = TRUE;
			} return 0L;
		}

		case WM_SYSCOMMAND:
		{
			switch( wparam)
            {
                case SC_MOVE:
                case SC_SIZE:
                case SC_MAXIMIZE:
				case SC_MONITORPOWER:
	                return TRUE;
            }
		} break;

		case WM_MOUSEMOVE:
		{
			PointBind(ptArroScrePix,LOWORD(lparam),HIWORD(lparam));
		} break;

		case WM_LBUTTONDOWN:
		{
			PointBind(ptDestScrePix,LOWORD(lparam),HIWORD(lparam));
			PointBind(ptPlayScrePix,sprite.PosX+FootPosX,sprite.PosY+FootPosY);
			nMoveState = LButtonDown;
		} break;

		case WM_LBUTTONUP:
		{
			PointBind(ptDestScrePix,LOWORD(lparam),HIWORD(lparam));
			PointBind(ptPlayScrePix,sprite.PosX+FootPosX,sprite.PosY+FootPosY);
			nMoveState = LButtonUp;
		} break;

		case WM_RBUTTONDOWN:
		{

		} break;
			
	}
	return (DefWindowProc(hwnd,msg,wparam,lparam));
}

/////////////////////////////////////////////////////////////////////////////
//	Name : WinMain
//  Desc : 
/////////////////////////////////////////////////////////////////////////////
int WINAPI WinMain (HINSTANCE hinstance, HINSTANCE hprevinstance, LPSTR lpcmdline, int ncmdshow)
{
	WNDCLASSEX	winclass;
	HWND		hwnd;
	MSG			msg;
	HACCEL		haccel;

	g_dwLastTick = GetTickCount();
	srand( GetTickCount() );

	winclass.cbSize			= sizeof(WNDCLASSEX);
	winclass.lpfnWndProc	= WindowProc;
	winclass.cbClsExtra		= 0;
	winclass.cbWndExtra		= 0;
	winclass.hInstance		= hinstance;
	winclass.style			= CS_DBLCLKS|CS_OWNDC|CS_HREDRAW|CS_VREDRAW;
	winclass.lpszClassName	= WINDOW_CLASS_NAME;
	winclass.lpszMenuName	= NULL;
	winclass.hIcon			= LoadIcon(NULL,IDI_APPLICATION);
	winclass.hIconSm		= LoadIcon(NULL,IDI_APPLICATION);
	winclass.hCursor		= LoadCursor(NULL,IDC_ARROW);
	winclass.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);

	if (!RegisterClassEx(&winclass))
		return(0);

	haccel = LoadAccelerators(hinstance,MAKEINTRESOURCE(IDR_MAIN_ACCEL));	//for ALT-TAB
    
	DWORD dwFrameWidth    = GetSystemMetrics( SM_CXSIZEFRAME );
    DWORD dwFrameHeight   = GetSystemMetrics( SM_CYSIZEFRAME );
    DWORD dwCaptionHeight = GetSystemMetrics( SM_CYCAPTION );
    DWORD dwWindowWidth   = SCREEN_WIDTH  + dwFrameWidth * 2;
    DWORD dwWindowHeight  = SCREEN_HEIGHT + dwFrameHeight * 2 +  dwCaptionHeight;

	DWORD dwStyle = WS_OVERLAPPED;

	if (!(hwnd=CreateWindowEx(NULL,WINDOW_CLASS_NAME,WINDOW_CLASS_NAME,dwStyle,0,0,dwWindowWidth,dwWindowHeight,NULL,NULL,hinstance,NULL)))
		return(0);

	ShowWindow (hwnd,ncmdshow);
	UpdateWindow (hwnd);

	g_hWnd		= hwnd;
	g_hInstance	= hinstance;

	GetWindowRect( g_hWnd, &g_rcWindow );

	Game_Init();

	while(TRUE)
	{
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
			{
				PostQuitMessage(0);
				break;
			}
			if(!TranslateAccelerator(hwnd,haccel,&msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			if(g_bActive)
			{
				Game_Main();
			}
			else
			{
				WaitMessage();
				g_dwLastTick = GetTickCount();
			}
		}
	}	// end while

	Game_Shutdown();
	return(msg.wParam);
}	// end WinMain

/////////////////////////////////////////////////////////////////////////////
//	Name : Game_Init
//  Desc : 
/////////////////////////////////////////////////////////////////////////////
VOID Game_Init()
{

	// Destroy Input Object
	DInput.~CDInput();
	DInput.DInput_Init(g_hInstance);
	DInput.DInput_Init_Keyboard(g_hWnd);
	DInput.DInput_Init_Mouse(g_hWnd);

	PointBind(ptArroScrePix,ViewWidth/2,ViewHeight/2);

	// Destroy All Object
	DDraw.~CDDraw();						// Destroy Direct Object
	DBitmap.Destroy_Bitmap(&image);			// Destroy Bitmap Object
	DBitmap.Unload_Bitmap_File(&bitmap);	// Destroy Bitmap File Object
	DSprite.DestroySprite(&sprite);			// Destroy Sprite Object
	DMap.~CDMap();			// Destroy Map Object
	
	// Switching Window Screen
	DDraw.SwitchingWindowScreen(g_hWnd,SCREEN_WIDTH,SCREEN_HEIGHT,SCREEN_BPP);

	// Bitmap for BackGround
	DBitmap.Load_Bitmap_File(&bitmap,"Boarder.bmp");				// Load Bitmap File
	DBitmap.Create_Bitmap(&image,0,0,640,480);
	DBitmap.Load_Image_Bitmap(&image,&bitmap,0,0,BITMAP_EXTRACT_MODE_ABS);
	DBitmap.Unload_Bitmap_File(&bitmap);

	// Bitmap for Character
	DBitmap.Load_Bitmap_File(&bitmap,"Sprite.bmp");				// Load Bitmap File
	DSprite.CreateSprite(DDraw.lpdd7, &sprite,56,72,56,NULL);	// Create Sprite Object
	DSprite.LoadSprite(&sprite,&bitmap);						// Load Sprite Object from Bitmap
	DBitmap.Unload_Bitmap_File(&bitmap);

	// Array and Setting for Animation Sequence
	int AniList[18][10]={	
							{ 4, 0, 1, 0, 2},
							{ 4, 6, 7, 6, 8},
							{ 4,12,13,12,14},
							{ 4,18,19,18,20},
							{ 4,24,25,24,26},
							{ 4,30,31,30,32},
							{ 4,36,37,36,38},	
							{ 4,42,43,42,44},
							{ 7,48,49,50,51,52,53,54},
							{ 1,55},	
							{ 2,5,4},							// EA Fire
							{ 3,5+6,4+6,5+6},					// NE Fire
							{ 3,5+6*2,4+6*2,5+6*2},				// NO Fire
							{ 2,4+6*3,5+6*3},				// NW Fire
							{ 3,4+6*4,5+6*4,4+6*4},				// WE Fire
							{ 3,4+6*5,5+6*5,4+6*5},				// SW Fire
							{ 3,4+6*6,5+6*6,4+6*6},				// SO Fire
							{ 3,4+6*7,5+6*7,4+6*7},				// SE Fire

														};
	
	for(int direction=0; direction < 18; direction++)
	{
		DSprite.SetAniSprite(&sprite,AniList[direction],direction);
	}

	// Bitmap for Map
	DBitmap.Load_Bitmap_File(&bitmap,"MapObj.bmp");
	DMap.CreateMapObj(DDraw.lpdd7,&DMap.MapObj,64,32,9,NULL);
	DMap.LoadMapObj(&bitmap,&DMap.MapObj);
	DBitmap.Unload_Bitmap_File(&bitmap);
}

/////////////////////////////////////////////////////////////////////////////
//	Name : Game_Main
//  Desc : 
/////////////////////////////////////////////////////////////////////////////
VOID Game_Main()
{
	if(KEY_DOWN(VK_ESCAPE)) { PostMessage(g_hWnd,WM_DESTROY,0,0); }	// Escape Key Control

	DWORD dwCurrTick = GetTickCount();
	DWORD dwTickDiff = dwCurrTick - g_dwLastTick;
	if (dwTickDiff < 33) { return; }	// is it work?

	if	(KEY_DOWN(VK_RETURN)){ DSprite.AnimateSprite(&sprite,DEATH,0,0,1,1); }

	KeyTest();
	MouseTest();

	DSprite.AnimateSprite(&sprite,sprite.CurDirection,0,0,sprite.AniState,sprite.AniAtt);

	DMap.DrawMapObj(DDraw.lpdd7,DDraw.lpddsback,ptArroScrePix);
	DSprite.DrawSprite(g_hWnd,DDraw.m_bWindowed,&sprite,DDraw.lpddsback);

	DMap.DrawPoint(DDraw.lpddsback);

	DDraw.DDraw_Lock_Back_Surface();
	DBitmap.Draw_Bitmap(g_hWnd,DDraw.m_bWindowed,&image,DDraw.back_buffer,DDraw.back_lpitch,_RGB32BIT(0,255,0,0));
	DDraw.DDraw_Unlock_Back_Surface();

	DDraw.DDraw_Flip();

	g_dwLastTick = GetTickCount();

}

/////////////////////////////////////////////////////////////////////////////
//	Name : Game_Shutdown
//  Desc : 
/////////////////////////////////////////////////////////////////////////////
VOID Game_Shutdown()
{
	DDraw.~CDDraw();
	DBitmap.Destroy_Bitmap(&image);
	DBitmap.Unload_Bitmap_File(&bitmap);
	DSprite.DestroySprite(&sprite);
	DMap.~CDMap();
}

/////////////////////////////////////////////////////////////////////////////
//	Name : KeyTest
//  Desc : 
/////////////////////////////////////////////////////////////////////////////
HRESULT KeyTest()
{
	DInput.DInput_Read_Keyboard();

	sprite.AniState = 1;

	POINT ptAnchorSpeed;
	PointBind(ptAnchorSpeed,0,0);

	if (DInput.keyboard_state[DIK_RIGHT])
	{ 
		if (DInput.keyboard_state[DIK_RIGHT] && DInput.keyboard_state[DIK_UP])			{ sprite.CurDirection = DIR_NE; PointBind(ptAnchorSpeed,+AnchSpeed,-AnchSpeed); }
		else if (DInput.keyboard_state[DIK_RIGHT] && DInput.keyboard_state[DIK_DOWN])	{ sprite.CurDirection = DIR_SE; PointBind(ptAnchorSpeed,+AnchSpeed,+AnchSpeed); }
		else																			{ sprite.CurDirection = DIR_EA; PointBind(ptAnchorSpeed,+AnchSpeed, 0); }
	}	// Key Moving
	else if (DInput.keyboard_state[DIK_LEFT])										
	{
		if (DInput.keyboard_state[DIK_LEFT] && DInput.keyboard_state[DIK_UP])			{ sprite.CurDirection = DIR_NW; PointBind(ptAnchorSpeed,-AnchSpeed,-AnchSpeed); }
		else if (DInput.keyboard_state[DIK_LEFT] && DInput.keyboard_state[DIK_DOWN])	{ sprite.CurDirection = DIR_SW; PointBind(ptAnchorSpeed,-AnchSpeed,+AnchSpeed); }
		else																			{ sprite.CurDirection = DIR_WE; PointBind(ptAnchorSpeed,-AnchSpeed, 0); }
	}
	else if (DInput.keyboard_state[DIK_UP])											
	{
		if (DInput.keyboard_state[DIK_RIGHT] && DInput.keyboard_state[DIK_UP])			{ sprite.CurDirection = DIR_NE; PointBind(ptAnchorSpeed,+AnchSpeed,-AnchSpeed); }
		else if (DInput.keyboard_state[DIK_LEFT] && DInput.keyboard_state[DIK_UP])		{ sprite.CurDirection = DIR_NW; PointBind(ptAnchorSpeed,-AnchSpeed,-AnchSpeed); }
		else																			{ sprite.CurDirection = DIR_NO; PointBind(ptAnchorSpeed, 0,-AnchSpeed); }
	}
	else if (DInput.keyboard_state[DIK_DOWN])
	{
		if (DInput.keyboard_state[DIK_RIGHT] && DInput.keyboard_state[DIK_DOWN])		{ sprite.CurDirection = DIR_SE; PointBind(ptAnchorSpeed,+AnchSpeed,+AnchSpeed); }
		else if (DInput.keyboard_state[DIK_LEFT] && DInput.keyboard_state[DIK_DOWN])	{ sprite.CurDirection = DIR_SW; PointBind(ptAnchorSpeed,-AnchSpeed,+AnchSpeed); }
		else																			{ sprite.CurDirection = DIR_SO; PointBind(ptAnchorSpeed, 0,+AnchSpeed); }
	}

	if(!DMap.TestMapObjectCollision(ptAnchorSpeed.x,ptAnchorSpeed.y))
	{
		DMap.AutoDirection(&sprite.CurDirection,&ptAnchorSpeed);
	}

	if(	ptAnchorSpeed.x == 0 && ptAnchorSpeed.y == 0 )
	{
		sprite.AniState = 0;
	}

	PointBind(DMap.ptAnchWorlPix,DMap.ptAnchWorlPix.x+ptAnchorSpeed.x,DMap.ptAnchWorlPix.y+ptAnchorSpeed.y);

	ClippingPoint(DMap.ptAnchWorlPix,32,16,65000,15000);

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
//	Name : MouseTest()
//  Desc : 
/////////////////////////////////////////////////////////////////////////////
HRESULT MouseTest()
{
	DInput.DInput_Read_Mouse();

	if(nMoveState == LButtonDown)
	{
		DMap.DirectionChange(ptArroScrePix, ptPlayScrePix, &sprite.CurDirection);
		DMap.DirectionWalking(ptArroScrePix, ptPlayScrePix,&nMoveState,&sprite.CurDirection,&sprite.AniState);
		sprite.AniState = 1;
	}
	
	if(nMoveState == LButtonUp || nMoveState == 3)
	{
		DMap.DirectionChange(ptArroScrePix, ptPlayScrePix, &sprite.CurDirection);
		DMap.WayPointWalking(ptPlayScrePix,ptDestScrePix,&nMoveState,&sprite.CurDirection,&sprite.AniState);
	}

	return S_OK;
}
