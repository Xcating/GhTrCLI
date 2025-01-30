#pragma once
#include "ProcessHelper.h"

class GhTrMemory {
public:
    GhTrMemory(ProcessHelper& mProcessHelper);
    bool Initialize();
    bool Refresh();
    DWORD_PTR GetBaseAddress();
    DWORD_PTR GetBoardAddress();
    DWORD_PTR GetSunAddress();
    bool SetSunValue(int sunValue);
private:
    bool RetrieveBaseAddress();
    bool RetrieveBoardAddress();
    bool RetrieveSunAddress();

    ProcessHelper& mProcessHelper;
    DWORD_PTR mBaseAddress;
    DWORD_PTR mBoardAddress;
    DWORD_PTR mSunAddress;
};