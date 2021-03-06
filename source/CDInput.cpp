/////////////////////////////////////////////////////////////////////////////
// File : Basic_Main.cpp
// Comment : Window Creat & Main Loop
// E-mail : jhook@lycos.co.kr
// August.1.2002		by jhook 
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//	INCLUDES
/////////////////////////////////////////////////////////////////////////////
#include "CDInput.H"
/////////////////////////////////////////////////////////////////////////////
//	DEFINES
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//	MACRO
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//	Type
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//	GLOBALS
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//	Direct Draw Object
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//	PROTOTYPES
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//	FUNCTIONS
/////////////////////////////////////////////////////////////////////////////
CDInput::CDInput()
{
	lpdi			= NULL;		// dinput object
	lpdikey			= NULL;		// dinput keyboard
	lpdimouse		= NULL;
}

CDInput::~CDInput()
{
	DInput_Release_Keyboard();
	DInput_Release_Mouse();
	DInput_Shutdown();
}

/////////////////////////////////////////////////////////////////////////////
//	Name : DInput_Init()
//  Desc : this function initializes directinput
/////////////////////////////////////////////////////////////////////////////
HRESULT CDInput::DInput_Init(HINSTANCE hinstance)
{
	if (FAILED(DirectInput8Create(hinstance,DIRECTINPUT_VERSION,IID_IDirectInput8,(void **)&lpdi,NULL)))
	{
		OutputDebugString("FAILED DirectInputCreate");
		return E_FAIL;
	}
	// return success
	return S_OK;
} // end DInput_Init

/////////////////////////////////////////////////////////////////////////////
//	Name : DInput_Shutdown()
//  Desc : this function shuts down directinput
/////////////////////////////////////////////////////////////////////////////
HRESULT CDInput::DInput_Shutdown(void)
{
	if (lpdi)
	{
		lpdi->Release();
	}
		return S_OK;
} // end DInput_Shutdown

/////////////////////////////////////////////////////////////////////////////
//	Name : DInput_Shutdown()
//  Desc : this function initializes,create the keyboard device
/////////////////////////////////////////////////////////////////////////////
HRESULT CDInput::DInput_Init_Keyboard(HWND hwnd)
{
	if (lpdi->CreateDevice(GUID_SysKeyboard,&lpdikey, NULL)!=DI_OK)
	{
		return E_FAIL;
	}
	if (lpdikey->SetCooperativeLevel(hwnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND)!=DI_OK)
    {
		return E_FAIL;
	}
	if (lpdikey->SetDataFormat(&c_dfDIKeyboard)!=DI_OK)
	{
		return E_FAIL;
	}
	if (lpdikey->Acquire()!=DI_OK)
	{
				return E_FAIL;
	}
	return S_OK;
} // end DInput_Init_Keyboard

/////////////////////////////////////////////////////////////////////////////
//	Name : DInput_Read_Keyboard()
//  Desc : this function reads the state of the keyboard
/////////////////////////////////////////////////////////////////////////////
HRESULT CDInput::DInput_Read_Keyboard(void)
{
	if (lpdikey)
    {
		if (lpdikey->GetDeviceState(256,(LPVOID)keyboard_state)!=DI_OK)
		{
			return E_FAIL;
		}
    }
	else
    {
		memset(keyboard_state,0,sizeof(keyboard_state));
		return E_FAIL;
    } // end else
	return S_OK;
} // end DInput_Read_Keyboard

/////////////////////////////////////////////////////////////////////////////
//	Name : DInput_Release_Keyboard(void)
//  Desc : this function unacquires and releases the keyboard
/////////////////////////////////////////////////////////////////////////////
HRESULT CDInput::DInput_Release_Keyboard()
{
	if (lpdikey)
    {
		lpdikey->Unacquire();
		lpdikey->Release();
    } // end if

	return S_OK;
} // end DInput_Release_Keyboard

/////////////////////////////////////////////////////////////////////////////
//	Name : DInput_Init_Mouse(void)
//  Desc : this function intializes the mouse
/////////////////////////////////////////////////////////////////////////////
HRESULT CDInput::DInput_Init_Mouse(HWND hwnd)
{
	// this function intializes the mouse
	// create a mouse device 
	if (lpdi->CreateDevice(GUID_SysMouse, &lpdimouse, NULL)!=DI_OK)
	{
		return E_FAIL;
	}
	// set cooperation level
	if (lpdimouse->SetCooperativeLevel(hwnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND)!=DI_OK)
	{
		return E_FAIL;
	}
	// set data format
	if (lpdimouse->SetDataFormat(&c_dfDIMouse)!=DI_OK)
	{
		return E_FAIL;
	}
	// acquire the mouse
	if (lpdimouse->Acquire()!=DI_OK)
	{
		return E_FAIL;
	}
	// return success
	return S_OK;
} // end DInput_Init_Mouse

/////////////////////////////////////////////////////////////////////////////
//	Name : DInput_Read_Mouse()
//  Desc : this function reads  the mouse state
/////////////////////////////////////////////////////////////////////////////
HRESULT CDInput::DInput_Read_Mouse(void)
{
	// this function reads  the mouse state
	if (lpdimouse)    
    {
		if (lpdimouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&mouse_state)!=DI_OK)
        {
		return E_FAIL;
		}
    }
	else
    {
		// mouse isn't plugged in, zero out state
		memset(&mouse_state,0,sizeof(mouse_state));
		// return error
		return E_FAIL;
    } // end else
	// return sucess
	return S_OK;
} // end DInput_Read_Mouse

/////////////////////////////////////////////////////////////////////////////
//	Name : DInput_Release_Mouse()
//  Desc : this function reads  the mouse state
/////////////////////////////////////////////////////////////////////////////
HRESULT CDInput::DInput_Release_Mouse(void)
{
	// this function unacquires and releases the mouse
	if (lpdimouse)
    {    
		lpdimouse->Unacquire();
		lpdimouse->Release();
    } // end if
	return S_OK;
} // end DInput_Release_Mouse
