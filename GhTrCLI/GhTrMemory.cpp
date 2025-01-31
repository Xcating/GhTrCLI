#include "GhTrMemory.h"

// 定义常量来替代硬编码的偏移量
constexpr auto BASE_MODULE_NAME = L"PlantsVsZombies.exe";
constexpr DWORD_PTR LAWNAPP_OFFSET = 0x2CB1E4; // LawnAPP指针的偏移量
constexpr DWORD_PTR BOARD_OFFSET = 0x730;            // LawnAPP指针到板对象的偏移量
constexpr DWORD_PTR SUN_OFFSET = 0x460;              // Board对象到阳光值的偏移量
constexpr DWORD_PTR FILE_OFFSET = 0x758;            // LawnAPP指针到存档对象的偏移量
constexpr DWORD_PTR UNLIMITED_SUN_OFFSET = 0x2CE;              // 存档对象到无限阳光的偏移量

GhTrMemory::GhTrMemory(ProcessHelper& theProcessHelper)
    : mProcessHelper(theProcessHelper),
    mModuleAddress(0),
    mLawnAPPAddress(0),
    mBoardAddress(0),
    mSunAddress(0),
    mFileAddress(0),
    mUnlimitedSunAddress(0)
{}

bool GhTrMemory::Initialize() {
    if (!RetrieveModuleAddress())
        return false;
    if (!RetrieveLawnAPPAddress())
        return false;
    RetrieveBoardAddress();
    RetrieveSunAddress();
    RetrieveFileAddress();
    RetrieveUnlimitedSunAddress();
    return true;
}

bool GhTrMemory::Refresh() {
    return Initialize();
}

DWORD_PTR GhTrMemory::GetModuleAddress() {
    RetrieveModuleAddress();
    return mModuleAddress;
}

DWORD_PTR GhTrMemory::GetLawnAPPAddress() {
    RetrieveLawnAPPAddress();
    return mLawnAPPAddress;
}

DWORD_PTR GhTrMemory::GetBoardAddress() {
    RetrieveBoardAddress();
    return mBoardAddress;
}

DWORD_PTR GhTrMemory::GetSunAddress() {
    RetrieveSunAddress();
    return mSunAddress;
}

DWORD_PTR GhTrMemory::GetFileAddress() {
    RetrieveFileAddress();
    return mFileAddress;
}

DWORD_PTR GhTrMemory::GetUnlimitedSunAddress() {
    RetrieveUnlimitedSunAddress();
    return mUnlimitedSunAddress;
}

bool GhTrMemory::SetSunValue(int sunValue) {
    RetrieveSunAddress();
    if (!mProcessHelper.IsAttached() || mSunAddress == 0)
        return false;
    return mProcessHelper.WriteMemory(reinterpret_cast<LPVOID>(mSunAddress), &sunValue, sizeof(sunValue));
}

bool GhTrMemory::ToggleUnlimitedSun(bool state) {
    RetrieveUnlimitedSunAddress();
    if (!mProcessHelper.IsAttached() || mUnlimitedSunAddress == 0)
        return false;
    return mProcessHelper.WriteMemory(reinterpret_cast<LPVOID>(mUnlimitedSunAddress), &state, sizeof(state));
}

bool GhTrMemory::GetUnlimitedSunState() {
    RetrieveUnlimitedSunAddress();
    if (!mProcessHelper.IsAttached() || mUnlimitedSunAddress == 0)
    {
        return false;
    }
    bool state=false;
    if (!mProcessHelper.ReadMemory(reinterpret_cast<LPCVOID>(mUnlimitedSunAddress), &state, sizeof(state)))
    {
        return false;
    }
    return state;
}

// 刷新模块地址
bool GhTrMemory::RetrieveModuleAddress() {
    mModuleAddress = mProcessHelper.GetModuleBaseAddress(BASE_MODULE_NAME);
    return (mModuleAddress != 0);
}

// 刷新LawnAPP地址
bool GhTrMemory::RetrieveLawnAPPAddress() {
    RetrieveModuleAddress();
    if (mModuleAddress == 0)
        return false;
    // 计算存储板指针的地址
    mLawnAPPAddress = mModuleAddress + LAWNAPP_OFFSET;
    return true;
}

// 刷新板地址
bool GhTrMemory::RetrieveBoardAddress() {
    RetrieveLawnAPPAddress();
    if (mLawnAPPAddress == 0)
        return false;
    char* pLawnAPPAddress = nullptr;

    // 读取板指针
    if (!mProcessHelper.ReadMemory(reinterpret_cast<LPCVOID>(mLawnAPPAddress), &pLawnAPPAddress, sizeof(DWORD)))
        return false;

    // 解引用板指针以获取板地址
    DWORD_PTR boardAddress = reinterpret_cast<DWORD_PTR>(pLawnAPPAddress) + BOARD_OFFSET;
    if (!mProcessHelper.ReadMemory(reinterpret_cast<LPCVOID>(boardAddress), &mBoardAddress, sizeof(DWORD)))
        return false;

    return true;
}

// 刷新阳光地址
bool GhTrMemory::RetrieveSunAddress() {
    RetrieveBoardAddress();
    if (mBoardAddress == 0)
        return false;

    // 根据板地址计算阳光地址
    mSunAddress = mBoardAddress + SUN_OFFSET;
    return true;
}

// 刷新存档地址
bool GhTrMemory::RetrieveFileAddress() {
    RetrieveLawnAPPAddress();
    if (mLawnAPPAddress == 0)
        return false;
    char* pLawnAPPAddress = nullptr;

    // 读取板指针
    if (!mProcessHelper.ReadMemory(reinterpret_cast<LPCVOID>(mLawnAPPAddress), &pLawnAPPAddress, sizeof(DWORD)))
        return false;

    // 解引用板指针以获取板地址
    DWORD_PTR fileAddress = reinterpret_cast<DWORD_PTR>(pLawnAPPAddress) + FILE_OFFSET;
    if (!mProcessHelper.ReadMemory(reinterpret_cast<LPCVOID>(fileAddress), &mFileAddress, sizeof(DWORD)))
        return false;

    return true;
}

// 刷新无限阳光地址
bool GhTrMemory::RetrieveUnlimitedSunAddress() {
    RetrieveFileAddress();
    if (mFileAddress == 0)
        return false;

    // 根据板地址计算无限阳光地址
    mUnlimitedSunAddress = mFileAddress + UNLIMITED_SUN_OFFSET;
    return true;
}