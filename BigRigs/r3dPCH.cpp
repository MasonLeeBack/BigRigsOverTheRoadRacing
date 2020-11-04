#include <r3dPCH.h>

extern bool r3dOutToLog(const char* Str, ...)
{
    FILE* logFile;
    char Buffer[80];
    va_list va;

    va_start(va, Str);
    vsprintf(Buffer, Str, va);
    logFile = fopen("r3dlog.txt", "at");
    if (logFile) {
        fprintf(logFile, "%s", Buffer);
        fclose(logFile);
    }
    return true;
}