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
    // 注册命令
    void RegisterCommands();

    // 常用功能
    int StringToInt(const std::wstring& str, bool& success);
    void DisplaySuggestions(const std::vector<std::wstring>& suggestions);

    // 新增：处理多级自动补全
    void HandleTabKey(std::wstring& aInput);
    std::vector<std::wstring> AutoCompleteEx(const std::vector<std::wstring>& tokens,
        const std::wstring& prefix,
        bool endsWithSpace);
    std::wstring BuildCompletionString(const std::vector<std::wstring>& tokens,
        bool endsWithSpace,
        const std::wstring& completion);

    // 如果还保留了旧的简单补全，可继续留着
    std::vector<std::wstring> AutoComplete(const std::wstring& prefix);

    // 解析输入命令
    std::vector<std::wstring> ParseCommand(const std::wstring& aInput);

    // 命令处理函数
    void HelpCommand(const std::vector<std::wstring>& aArgs);
    void ExitCommand(const std::vector<std::wstring>& aArgs);
    void ClsCommand(const std::vector<std::wstring>& aArgs);
    void AttachCommand(const std::vector<std::wstring>& aArgs);
    void SetSunCommand(const std::vector<std::wstring>& aArgs);
    void SetLangCommand(const std::vector<std::wstring>& aArgs);
    void GameRuleCommand(const std::vector<std::wstring>& aArgs);

private:
    // 新增：多级自动补全需要的私有成员
    std::wstring mLastPrefix;                            // 记录上一次匹配前缀
    std::vector<std::wstring> mLastSuggestions;          // 上一次候选列表
    size_t mSuggestionIndex;                             // 在候选列表中的当前位置
    std::unordered_map<std::wstring, std::vector<std::wstring>> mSecondLevelMap; // 二级命令映射
    std::unordered_map<std::wstring, std::vector<std::wstring>> mThirdLevelMap;  // 三级命令映射

    // 其它现有成员
    std::unordered_map<std::wstring, CommandFunction> mCommands;
    LanguageManager mLanguageManager;
    ProcessHelper mProcessHelper;
};