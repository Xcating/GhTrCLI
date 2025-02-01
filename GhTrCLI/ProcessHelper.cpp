#include "ProcessHelper.h"
#include <tlhelp32.h>
#include "Logger.h"

ProcessHelper::ProcessHelper()
    : mProcessHandle(nullptr), mProcessID(0)
{}

ProcessHelper::ProcessHelper(const std::wstring& processName)
    : mProcessName(processName), mProcessHandle(nullptr), mProcessID(0)
{}

ProcessHelper::~ProcessHelper() {
    Detach();
}

void ProcessHelper::SetProcessName(const std::wstring& processName) {
    mProcessName = processName;
}

bool ProcessHelper::Attach() {
    if (mProcessName.empty())
        return false;

    // 获取进程ID
    mProcessID = GetProcessIDByName(mProcessName);
    if (mProcessID == 0)
        return false;

    // 打开进程
    mProcessHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, mProcessID);
    return mProcessHandle != nullptr;
}

void ProcessHelper::Detach() {
    if (mProcessHandle) {
        CloseHandle(mProcessHandle);
        mProcessHandle = nullptr;
        mProcessID = 0;
    }
}

bool ProcessHelper::IsAttached() const {
    return mProcessHandle != nullptr;
}

const std::wstring& ProcessHelper::GetProcessName() const {
    return mProcessName;
}

bool ProcessHelper::ReadMemory(LPCVOID address, void* buffer, SIZE_T size) {
    if (address == nullptr || buffer == nullptr || size == 0) {
        return false;
    }
    SIZE_T bytesRead;
    return ReadProcessMemory(mProcessHandle, address, buffer, size, &bytesRead);
}

bool ProcessHelper::WriteMemory(LPVOID address, const void* buffer, SIZE_T size) {
    SIZE_T bytesWritten;
    return WriteProcessMemory(mProcessHandle, address, buffer, size, &bytesWritten);
}

DWORD_PTR ProcessHelper::GetModuleBaseAddress(const std::wstring& moduleName) {
    MODULEENTRY32 moduleEntry = { sizeof(MODULEENTRY32) };
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, mProcessID);
    DWORD_PTR baseAddress = 0;

    if (Module32First(snapshot, &moduleEntry)) {
        do {
            if (_wcsicmp(moduleEntry.szModule, moduleName.c_str()) == 0) {
                baseAddress = (DWORD_PTR)moduleEntry.modBaseAddr;
                break;
            }
        } while (Module32Next(snapshot, &moduleEntry));
    }
    CloseHandle(snapshot);
    return baseAddress;
}

bool ProcessHelper::IsProcessRunning(const std::wstring& processName) {
    PROCESSENTRY32 entry = { sizeof(PROCESSENTRY32) };
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (snapshot == INVALID_HANDLE_VALUE) {
        Logger::PrintError(_S("无法创建进程快照"));
        return false;
    }

    bool found = false;
    if (Process32First(snapshot, &entry)) {
        do {
            if (_wcsicmp(entry.szExeFile, processName.c_str()) == 0) {
                found = true;
                break;
            }
        } while (Process32Next(snapshot, &entry));
    }

    CloseHandle(snapshot);
    return found;
}

DWORD ProcessHelper::GetProcessIDByName(const std::wstring& processName) {
    PROCESSENTRY32 entry = { sizeof(PROCESSENTRY32) };
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (snapshot == INVALID_HANDLE_VALUE) {
        Logger::PrintError(_S("无法创建进程快照"));
        return 0;
    }

    DWORD processID = 0;
    if (Process32First(snapshot, &entry)) {
        do {
            if (_wcsicmp(entry.szExeFile, processName.c_str()) == 0) {
                processID = entry.th32ProcessID;
                break;
            }
        } while (Process32Next(snapshot, &entry));
    }

    CloseHandle(snapshot);
    return processID;
}