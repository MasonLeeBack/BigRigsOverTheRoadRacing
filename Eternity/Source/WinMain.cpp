#include "r3dPCH.h"
#include "r3d.h"
//#include "r3dDebug.h"

static	int		StartWinHeight = 300;
static	int		StartWinWidth = 300;
static	int		curDispWidth = 0;
static	int		curDispHeight = 0;

namespace win
{
	HINSTANCE hInstance = NULL;
	HWND hWnd = NULL;
	const char* szWinName = "$o, yeah!$";
	HICON		hWinIcon = 0;

	BOOL Init();
}

static LRESULT CALLBACK win__WndFunc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

BOOL win::Init()
{
	static const char* szWinClassName = "r3dWin";

	WNDCLASS  wndclass;
	wndclass.style = CS_DBLCLKS | CS_GLOBALCLASS;
	wndclass.lpfnWndProc = win__WndFunc;		// window function
	wndclass.cbClsExtra = 0;				// no extra count of bytes
	wndclass.cbWndExtra = 0;				// no extra count of bytes
	wndclass.hInstance = GetModuleHandle(NULL);	// this instance
	wndclass.hIcon = (hWinIcon) ? hWinIcon : LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = szWinClassName;
	RegisterClass(&wndclass);

	HDC disp_dc;

	disp_dc = CreateIC("DISPLAY", NULL, NULL, NULL);
	curDispWidth  = GetDeviceCaps(disp_dc, HORZRES);
	curDispHeight = GetDeviceCaps(disp_dc, VERTRES);
	//disp_bpp = GetDeviceCaps(disp_dc, BITSPIXEL);
	DeleteDC(disp_dc);

	hWnd = CreateWindow(
	/* window class name */       szWinClassName,
	/* window caption*/           szWinName,
	/* window style */            WS_VISIBLE | WS_OVERLAPPEDWINDOW,
	/* initial x position */      (curDispWidth - StartWinWidth) / 2,
	/* initial y position */      (curDispHeight - StartWinHeight) / 2,
	/* initial x size */          StartWinWidth,
	/* initial y size */          StartWinHeight,
	/* parent window handle */    NULL,
	/* window menu handle*/       NULL,
	/* program instance handle */ GetModuleHandle(NULL),
	/* creation parameters */     NULL);
	
	if (!hWnd) {
		MessageBox(GetActiveWindow(), "Window Creation Failed", "Error", MB_OK);
		return FALSE;
	}

	ShowWindow(win::hWnd, FALSE);
	InvalidateRect(win::hWnd, NULL, FALSE);
	UpdateWindow(win::hWnd);
	SetFocus(win::hWnd);
	ShowCursor(FALSE);

	return TRUE;
}

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, INT iCmdShow)
{
	// mback: Big Rigs didn't support multi-threading, so we have to update r3d to
	// take out multi-threading in this branch

	game::PreInit();

	win::Init();

	game::Init();
	game::MainLoop();
	game::Shutdown();

	return 0;
}