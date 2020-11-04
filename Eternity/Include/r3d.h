#ifndef __R3D__H
#define __R3D__H

#define R3D_ETERNITY_VERSION	0x00030001

#include "r3dSys_WIN.h"

extern bool r3dOutToLog(const char* Str, ...);

extern_nspace(game, void	PreInit());
extern_nspace(game, void	Init());
extern_nspace(game, void	MainLoop());
extern_nspace(game, void	Shutdown());

extern_nspace(win, HINSTANCE	hInstance);
extern_nspace(win, HWND	hWnd);
extern_nspace(win, const char* szWinName);
extern_nspace(win, HICON	hWinIcon);

extern	HANDLE		r3d_MainThread;

#include "r3drender.h"

extern 	r3dRenderLayer* r3dRenderer;

extern 	void 		r3dInitLogFile();

#endif // __R3D__H
