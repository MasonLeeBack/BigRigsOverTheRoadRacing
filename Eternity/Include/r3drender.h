#ifndef __R3D_RENDER_H_
#define __R3D_RENDER_H_

#include "r3d.h"

struct r3dDisplayResolution
{
    int Width;
    int Height;
};

class r3dRenderLayer
{
public:
    int Version;
    HWND HLibWin;

    static const D3DFORMAT BACK_BUFFER_FORMAT = D3DFMT_X8R8G8B8;
    static const D3DFORMAT DISPLAY_FORMAT_32 = D3DFMT_X8R8G8B8;
    static const D3DFORMAT DISPLAY_FORMAT_16 = D3DFMT_R5G6B5;

    D3DDEVTYPE	DeviceType;

    IDirectInput8* pdi;
    IDirect3D9* pd3d;

    D3DPRESENT_PARAMETERS	d3dpp;
    
    IDirect3DDevice9* pd3ddev = NULL;

    int CurrentBPP;

    r3dRenderLayer();
    ~r3dRenderLayer();

    int Init(HWND hWindow, const char* displayName);

    int SetMode(int xRes, int yRes, int BPP, int flags);

    int	IsTextureFormatAvailable(D3DFORMAT fmt);

    r3dDisplayResolution resolution;
};

#endif //__R3D_RENDER_H_
