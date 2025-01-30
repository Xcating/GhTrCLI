#include "GhTrCLI.h"
#include <windows.h>

#ifdef _WIN32
#include <windows.h>
void EnableVirtualTerminalProcessing() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
}
#endif

int main()
{
    #ifdef _WIN32
        EnableVirtualTerminalProcessing();
    #endif
    GhTrCLI aCli;
    aCli.Run();
    return 0;
}