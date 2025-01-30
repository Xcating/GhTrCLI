#pragma once
#include <string>
#include <windows.h>

class ProcessHelper {
public:
    ProcessHelper();
    ProcessHelper(const std::wstring& processName);
    ~ProcessHelper();

    void SetProcessName(const std::wstring& processName);
    bool Attach();
    void Detach();
    bool IsAttached() const;
    const std::wstring& GetProcessName() const;

    bool ReadMemory(LPCVOID address, void* buffer, SIZE_T size);
    bool WriteMemory(LPVOID address, const void* buffer, SIZE_T size);

    DWORD_PTR GetModuleBaseAddress(const std::wstring& moduleName);

    static bool IsProcessRunning(const std::wstring& processName);
    static DWORD GetProcessIDByName(const std::wstring& processName);

private:
    std::wstring mProcessName;
    HANDLE mProcessHandle;
    DWORD mProcessID;
};