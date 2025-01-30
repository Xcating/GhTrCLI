#include "GhTrMemory.h"

// 定义常量来替代硬编码的偏移量
constexpr auto BASE_MODULE_NAME = L"PlantsVsZombies.exe";
constexpr DWORD_PTR BOARD_POINTER_OFFSET = 0x2CB1E4; // 存储板指针的偏移量
constexpr DWORD_PTR BOARD_OFFSET = 0x730;            // 板指针到实际板对象的偏移量
constexpr DWORD_PTR SUN_OFFSET = 0x460;              // 板对象到阳光值的偏移量

GhTrMemory::GhTrMemory(ProcessHelper& theProcessHelper)
    : mProcessHelper(theProcessHelper),
    mBaseAddress(0),
    mBoardAddress(0),
    mSunAddress(0)
{}

bool GhTrMemory::Initialize() {
    if (!RetrieveBaseAddress())
        return false;
    RetrieveBoardAddress();
    RetrieveSunAddress();
    return true;
}

bool GhTrMemory::Refresh() {
    return Initialize();
}

DWORD_PTR GhTrMemory::GetBaseAddress() {
    Refresh();
    return mBaseAddress;
}

DWORD_PTR GhTrMemory::GetBoardAddress() {
    Refresh();
    return mBoardAddress;
}

DWORD_PTR GhTrMemory::GetSunAddress() {
    Refresh();
    return mSunAddress;
}

bool GhTrMemory::SetSunValue(int sunValue) {
    if (!mProcessHelper.IsAttached() || mSunAddress == 0)
        return false;
    return mProcessHelper.WriteMemory(reinterpret_cast<LPVOID>(mSunAddress), &sunValue, sizeof(sunValue));
}

// 获取基地址
bool GhTrMemory::RetrieveBaseAddress() {
    mBaseAddress = mProcessHelper.GetModuleBaseAddress(BASE_MODULE_NAME);
    return (mBaseAddress != 0);
}

// 获取板地址
bool GhTrMemory::RetrieveBoardAddress() {
    if (mBaseAddress == 0)
        return false;

    // 计算存储板指针的地址
    DWORD_PTR boardPointerAddress = mBaseAddress + BOARD_POINTER_OFFSET;
    char* pBoardPointer = nullptr;

    // 读取板指针
    if (!mProcessHelper.ReadMemory(reinterpret_cast<LPCVOID>(boardPointerAddress), &pBoardPointer, sizeof(pBoardPointer)))
        return false;

    // 解引用板指针以获取板地址
    DWORD_PTR boardAddress = reinterpret_cast<DWORD_PTR>(pBoardPointer) + BOARD_OFFSET;
    if (!mProcessHelper.ReadMemory(reinterpret_cast<LPCVOID>(boardAddress), &mBoardAddress, sizeof(mBoardAddress)))
        return false;

    return true;
}

// 获取阳光地址
bool GhTrMemory::RetrieveSunAddress() {
    if (mBoardAddress == 0)
        return false;

    // 根据板地址计算阳光地址
    mSunAddress = mBoardAddress + SUN_OFFSET;
    return true;
}