/////////////////////////////////////////////////////////////////////////////
// File : Basic_Main.h
// Comment : Window Creat & Main Loop
// E-mail : jhook@lycos.co.kr
// August.1.2002		by jhook 
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//	INCLUDES
/////////////////////////////////////////////////////////////////////////////
#if !defined _CDINPUT_H_
#define _CDINPUT_H_

#include <dinput.h>


/////////////////////////////////////////////////////////////////////////////
//	DEFINES
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//	Type
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//	PROTOTYPES
/////////////////////////////////////////////////////////////////////////////

class CDInput
{
	public:
	CDInput();
	~CDInput();

	HRESULT	DInput_Init(HINSTANCE hinstance);
	HRESULT DInput_Shutdown(void);

	HRESULT DInput_Init_Keyboard(HWND hwnd);
	HRESULT DInput_Read_Keyboard(void);
	HRESULT DInput_Release_Keyboard(void);

	HRESULT DInput_Init_Mouse(HWND hwnd);
	HRESULT DInput_Read_Mouse(void);
	HRESULT DInput_Release_Mouse(void);

	LPDIRECTINPUT8			lpdi;			// dinput object
	LPDIRECTINPUTDEVICE8	lpdikey;		// dinput keyboard
	LPDIRECTINPUTDEVICE8	lpdimouse;		// dinput mouse

	DIMOUSESTATE			mouse_state;				// contains state of mouse
	UCHAR					keyboard_state[256];				// contains keyboard state table
};

#endif