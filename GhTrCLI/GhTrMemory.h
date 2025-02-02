#pragma once
#include "ProcessHelper.h"

class GhTrMemory {
public:
    GhTrMemory(ProcessHelper& mProcessHelper);
    bool Initialize();
    bool Refresh();
    DWORD_PTR GetModuleAddress();
    DWORD_PTR GetLawnAPPAddress();
    DWORD_PTR GetBoardAddress();
    DWORD_PTR GetSunAddress();
    DWORD_PTR GetFileAddress();
    DWORD_PTR GetUnlimitedSunAddress();
    bool SetSunValue(int sunValue);
    int GetSunValue();
    bool ToggleUnlimitedSun(bool state);
    bool GetUnlimitedSunState();
private:
    bool RetrieveModuleAddress();
    bool RetrieveLawnAPPAddress();
    bool RetrieveBoardAddress();
    bool RetrieveSunAddress();
    bool RetrieveFileAddress();
    bool RetrieveUnlimitedSunAddress();

    ProcessHelper& mProcessHelper;
    DWORD_PTR mModuleAddress;
    DWORD_PTR mLawnAPPAddress;
    DWORD_PTR mBoardAddress;
    DWORD_PTR mSunAddress;
    DWORD_PTR mFileAddress;
    DWORD_PTR mUnlimitedSunAddress;
};