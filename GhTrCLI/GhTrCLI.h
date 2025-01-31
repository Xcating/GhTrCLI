#pragma once

#include <string>
#include <vector>
#include <locale>
#include <unordered_map>
#include <functional>
#include <windows.h>
#include <tlhelp32.h>
#include "Logger.h"
#include "ProcessHelper.h"
#include "GhTrMemory.h"
#include <unordered_map>
#include "LanguageManager.h"

#define _S(str) L##str

static const std::wstring gProjectName = _S("GhTrCLI");
static const std::wstring gProjectVersion = _S("ver.0.01a");
static const std::wstring gSupportingVersion = _S("ver.0.17d");

class GhTrCLI
{
public:
    using CommandFunction = std::function<void(const std::vector<std::wstring>&)>;

    GhTrCLI();
    void Run();

private:
    std::unordered_map<std::wstring, CommandFunction> mCommands;
    LanguageManager mLanguageManager;

    void RegisterCommands();
    int StringToInt(const std::wstring& str, bool& success);
    void DisplaySuggestions(const std::vector<std::wstring>& suggestions);
    std::vector<std::wstring> AutoComplete(const std::wstring& prefix);
    std::vector<std::wstring> ParseCommand(const std::wstring& aInput);

    void HelpCommand(const std::vector<std::wstring>& aArgs);
    void ExitCommand(const std::vector<std::wstring>& aArgs);
    void ClsCommand(const std::vector<std::wstring>& aArgs);
    void AttachCommand(const std::vector<std::wstring>& aArgs);
    void SetSunCommand(const std::vector<std::wstring>& aArgs);
    void SetLangCommand(const std::vector<std::wstring>& aArgs);

    ProcessHelper mProcessHelper;
};