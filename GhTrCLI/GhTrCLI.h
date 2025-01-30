#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <windows.h>
#include <tlhelp32.h>
#include "Logger.h"
#include "ProcessHelper.h"
#include "GhTrMemory.h"

#define _S(str) L##str

static const std::wstring gProjectName = _S("GhTrCLI");
static const std::wstring gProjectVersion = _S("ver.0.01a");
static const std::wstring gSupportingVersion = _S("ver.0.17d");
static const std::wstring gProjectDesc = _S("GhTr 游戏的外置命令行工具，用于与游戏进行交互");

class GhTrCLI
{
public:
    using CommandFunction = std::function<void(const std::vector<std::wstring>&)>;

    GhTrCLI();
    void Run();

private:
    std::unordered_map<std::wstring, CommandFunction> mCommands;

    void RegisterCommands();
    int StringToInt(const std::wstring& str, bool& success);
    std::vector<std::wstring> ParseCommand(const std::wstring& aInput);

    void HelpCommand(const std::vector<std::wstring>& aArgs);
    void ExitCommand(const std::vector<std::wstring>& aArgs);
    void ClsCommand(const std::vector<std::wstring>& aArgs);
    void AttachCommand(const std::vector<std::wstring>& aArgs);
    void SetSunCommand(const std::vector<std::wstring>& aArgs);

    ProcessHelper mProcessHelper;
};