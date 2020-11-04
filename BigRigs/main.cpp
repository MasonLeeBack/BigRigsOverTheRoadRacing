#include <r3dPCH.h>
#include <r3d.h>

#include "res/resource.h"

void InitRender()
{
    FILE* sceneFile;

    int width = 800;
    int height = 600;
    int bpp = 32;
    int fullscreen = 0;
    int texfilter = 1;

    int flags;

    sceneFile = fopen("scene3d.ini", "rt");
    if (sceneFile)
    {
        int x = fscanf(sceneFile, "%d %d %d %d %d", &width, &height, &bpp, &fullscreen, &texfilter); // this is innapropriate behavior, but we're rolling with it for now
        fclose(sceneFile);
    }

    if (bpp == 32)
        flags = 8;
    if (fullscreen == 1)
        flags |= 4;
    if (texfilter == 0)
        flags |= 32;

    r3dRenderer = new r3dRenderLayer();
    r3dRenderer->Init(win::hWnd, NULL);

    r3dOutToLog("Setting mode:  %dx%dx%d Flags=%d\n", width, height, bpp, flags);
    r3dRenderer->SetMode(width, height, bpp, flags);

    MoveWindow(
        win::hWnd,
        NULL,
        NULL,
        r3dRenderer->resolution.Width,
        r3dRenderer->resolution.Height,
        NULL);

    ShowWindow(win::hWnd, TRUE);
    /*
    sub_417190((int)dword_4AB81C, 1);
    sub_4157B0((_DWORD **)dword_4AB81C);
    SetCursorPos((unsigned __int64)*((float *)dword_4AB81C + 166), (unsigned __int64)*((float *)dword_4AB81C + 167));
    sub_415050(dword_4AB820);
    */
}

namespace game {
    void PreInit() {
        win::hWinIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
        win::szWinName = "Big Rig Racing"; // Update for SKU?
    }
    void Init() {
        InitRender();
    }
    void MainLoop() {
        // this isn't proper, but whatever, it'll do
        while (1) {

        }
    }
    void Shutdown() {

    }
}