#include <r3dPCH.h>
#include <r3d.h>

r3dRenderLayer* r3dRenderer = NULL;

HANDLE		r3d_MainThread;

void r3d_UtilityThread(DWORD in)
{
    while (1) {
        Sleep(100);
    }
}

void r3dInitLogFile()
{
    time_t t;
    FILE* f;

    time(&t);
    if ((f = fopen("r3dlog.txt", "wt")) != NULL) {
        //fwrite(UTF8_BOM, 1, 3, f);
        fprintf(f, "R3D: Log started at %s\n", ctime(&t));
        fclose(f);
    }

    return;
}