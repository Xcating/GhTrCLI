#include "GhTrCLI.h"
#include <conio.h>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <algorithm> // std::find_if

GhTrCLI::GhTrCLI()
    : mLastPrefix(L"")
    , mSuggestionIndex(0)
{
    // 初始化多语言
    mLanguageManager.LoadLanguage(PRIMARYLANGID(GetUserDefaultUILanguage()) == LANG_CHINESE
        ? LanguageManager::Language::Chinese
        : LanguageManager::Language::English);

    // 注册已有的命令
    RegisterCommands();

    // 初始化二级命令、三级命令可能的补全项
    // 这里只示例了 gamerule -> unlimitedSun -> [true, false]
    mSecondLevelMap[L"gamerule"] = { L"unlimitedSun" };

    // 把“gamerule unlimitedSun”视为一个 key，后面是第三层可选
    mThirdLevelMap[L"gamerule unlimitedSun"] = { L"true", L"false" };
}

void GhTrCLI::RegisterCommands()
{
    mCommands[_S("help")] = [this](const std::vector<std::wstring>& aArgs) {
        HelpCommand(aArgs);
        };
    mCommands[_S("exit")] = [this](const std::vector<std::wstring>& aArgs) {
        ExitCommand(aArgs);
        };
    mCommands[_S("quit")] = [this](const std::vector<std::wstring>& aArgs) {
        ExitCommand(aArgs);
        };
    mCommands[_S("cls")] = [this](const std::vector<std::wstring>& aArgs) {
        ClsCommand(aArgs);
        };
    mCommands[_S("attach")] = [this](const std::vector<std::wstring>& aArgs) {
        AttachCommand(aArgs);
        };
    mCommands[_S("set-sun")] = [this](const std::vector<std::wstring>& aArgs) {
        SetSunCommand(aArgs);
        };
    mCommands[_S("set-lang")] = [this](const std::vector<std::wstring>& aArgs) {
        SetLangCommand(aArgs);
        };
    mCommands[_S("gamerule")] = [this](const std::vector<std::wstring>& aArgs) {
        GameRuleCommand(aArgs);
        };
}

void GhTrCLI::Run()
{
    Logger::Initialize();
    Logger::PrintHeader(mLanguageManager.GetString(L"welcome_message"));
    Logger::PrintInfo(mLanguageManager.GetString(L"help_message"));

    if (ProcessHelper::IsProcessRunning(_S("PlantsVsZombies.exe")))
    {
        Logger::PrintInfo(mLanguageManager.GetString(L"detect_pvz_running"));
        Logger::Print(mLanguageManager.GetString(L"ask_attach_process"), Logger::TextColor::Yellow);
        std::wstring theResponse;
        std::getline(std::wcin, theResponse);
        if (theResponse == _S("y") || theResponse == _S("Y"))
        {
            mProcessHelper.SetProcessName(_S("PlantsVsZombies.exe"));
            if (mProcessHelper.Attach())
            {
                Logger::PrintSuccess(mLanguageManager.GetString(L"attach_pvz_success"));
            }
            else
            {
                Logger::PrintError(mLanguageManager.GetString(L"attach_pvz_fail"));
            }
        }
    }

    std::vector<std::wstring> history;  // 用于保存命令历史
    int historyIndex = -1;              // 当前历史命令索引
    std::wstring aInput;                // 当前输入的命令

    while (true)
    {
        Logger::PrintStarter();
        aInput.clear();              // 清空输入
        historyIndex = (int)history.size();
        std::wcout.flush();

        while (true)
        {
            if (_kbhit())  // 检测键盘输入
            {
                char ch = _getch();
                if (ch == 13)  // 回车键 (Enter)
                {
                    if (!aInput.empty()) {
                        history.push_back(aInput);  // 保存命令到历史
                        historyIndex = (int)history.size();
                    }
                    break;
                }
                else if (ch == 9)  // Tab键
                {
                    // 自动补全
                    HandleTabKey(aInput);
                }
                else if (ch == 8)  // Backspace键
                {
                    if (!aInput.empty())
                    {
                        aInput.pop_back();  // 删除最后一个字符
                        std::wcout << L"\r> " << aInput << L" ";  // 重新绘制输入框
                        std::wcout << L"\r> " << aInput;          // 保持光标在正确位置
                    }
                }
                else if (ch == -32)
                {
                    // 方向键
                    ch = _getch();
                    if (ch == 72) // Up
                    {
                        // 查看上一条历史记录
                        if (historyIndex > 0) {
                            --historyIndex;
                            aInput = history[historyIndex];
                            // 用空格覆盖旧字符（如果新命令更短）
                            std::wcout << L"\r> " << aInput << L"                                        ";
                            std::wcout << L"\r> " << aInput;
                        }
                    }
                    else if (ch == 80) // Down
                    {
                        // 查看下一条历史记录
                        if ((size_t)historyIndex < history.size() - 1) {
                            ++historyIndex;
                            aInput = history[historyIndex];
                            // 用空格覆盖旧字符
                            std::wcout << L"\r> " << aInput << L"                                        ";
                            std::wcout << L"\r> " << aInput;
                        }
                        else if ((size_t)historyIndex == history.size() - 1)
                        {
                            // 再往下则清空
                            ++historyIndex;
                            aInput.clear();
                            std::wcout << L"\r>                                        ";
                            std::wcout << L"\r> ";
                        }
                    }
                }
                else
                {
                    // 普通字符输入
                    aInput.push_back(ch);
                    std::wcout << ch;  // 显示输入的字符
                }
            }
        }

        std::wcout << std::endl;  // 确保回车后换行
        auto aTokens = ParseCommand(aInput);
        if (aTokens.empty())
        {
            continue;
        }

        const auto& aCommandName = aTokens[0];
        auto aIt = mCommands.find(aCommandName);
        if (aIt != mCommands.end())
        {
            std::vector<std::wstring> aArgs(aTokens.begin() + 1, aTokens.end());
            aIt->second(aArgs);
        }
        else
        {
            Logger::PrintError(mLanguageManager.GetString(L"unknown_command") + aCommandName);
        }
    }

    Logger::PrintInfo(mLanguageManager.GetString(L"cli_exited"));
}

/*
 *  新增：在 GhTrCLI 类中维护多级命令的映射
 *  例如：
 *    gamerule -> [unlimitedSun]
 *    gamerule unlimitedSun -> [true, false]
 */
std::vector<std::wstring> GhTrCLI::AutoCompleteEx(const std::vector<std::wstring>& tokens,
    const std::wstring& prefix,
    bool endsWithSpace)
{
    // 最终要返回的候选列表
    std::vector<std::wstring> results;

    // 如果还没有输入任何命令，或者只输入了一个尚未空格结束的命令（例如 "gam"）
    // 则进行"一级命令"的匹配
    if (tokens.empty() || (tokens.size() == 1 && !endsWithSpace))
    {
        // 针对所有已注册的命令进行前缀匹配
        for (const auto& kv : mCommands)
        {
            if (kv.first.rfind(prefix, 0) == 0) // 以 prefix 开头
            {
                results.push_back(kv.first);
            }
        }
    }
    // 如果恰好输入了一个完整的命令并且按下了空格，或者输入了两个 token 但是第二个还没结束（第2种情况常见于二级命令）
    else if ((tokens.size() == 1 && endsWithSpace) || (tokens.size() == 2 && !endsWithSpace))
    {
        // tokens[0] 是已经输入完成的“一级命令”
        auto command = tokens[0];
        // 在 second-level map 中查找
        auto it = mSecondLevelMap.find(command);
        if (it != mSecondLevelMap.end())
        {
            // 遍历所有可能的二级子命令，做前缀匹配
            for (auto& subcmd : it->second)
            {
                if (subcmd.rfind(prefix, 0) == 0) // 以 prefix 开头
                {
                    results.push_back(subcmd);
                }
            }
        }
    }
    // 如果是正在输入第三级参数：即 ( tokens.size() == 2 && endsWithSpace ) 或者 ( tokens.size() == 3 && !endsWithSpace )
    else if ((tokens.size() == 2 && endsWithSpace) || (tokens.size() == 3 && !endsWithSpace))
    {
        // 先拿到 "一级命令" + "二级命令" 的组合
        auto combinedKey = tokens[0] + L" " + tokens[1];
        auto it = mThirdLevelMap.find(combinedKey);
        if (it != mThirdLevelMap.end())
        {
            // 遍历所有可能的第三级子命令（例如 true/false）
            // 如果前缀为空，则返回全部（便于循环）
            // 如果前缀不为空，则只匹配前缀
            for (auto& param : it->second)
            {
                if (prefix.empty())
                {
                    // prefix 为空则全部返回，用于后续循环
                    results.push_back(param);
                }
                else
                {
                    // prefix 不为空则做前缀匹配
                    if (param.rfind(prefix, 0) == 0)
                    {
                        results.push_back(param);
                    }
                }
            }
        }
    }

    return results;
}

// 根据当前的 tokens 和一个“要补全的单词” 来重组输入行
// endsWithSpace=true 表示当前是在输入一个新token
std::wstring GhTrCLI::BuildCompletionString(const std::vector<std::wstring>& tokens,
    bool endsWithSpace,
    const std::wstring& completion)
{
    std::wstring result;
    if (tokens.empty())
    {
        // 说明还没输入第一个命令，直接用 completion
        result = completion;
    }
    else
    {
        // 如果是在输入一个全新的 token（也就是输入行以空格结尾）：
        //   则 tokens 仍然代表之前的内容，需要在后面追加一个新token
        // 否则表示要替换最后一个 token。
        if (endsWithSpace)
        {
            // 先把前面的 tokens 全部组合回去
            for (size_t i = 0; i < tokens.size(); ++i)
            {
                result += tokens[i];
                if (i < tokens.size())
                    result += L" ";
            }
            // 然后在最后加一个新的 token
            result += completion;
        }
        else
        {
            // 替换最后一个 token
            for (size_t i = 0; i < tokens.size() - 1; ++i)
            {
                result += tokens[i];
                result += L" ";
            }
            result += completion;
        }
    }
    return result;
}

// 处理用户按下 Tab 键时的逻辑
void GhTrCLI::HandleTabKey(std::wstring& aInput)
{
    // 1. 先解析当前输入行
    auto tokens = ParseCommand(aInput);

    // 2. 判断输入行是否以空格结尾（意味着要补全一个新token）
    bool endsWithSpace = false;
    if (!aInput.empty() && (aInput.back() == L' ' || aInput.back() == L'\t'))
    {
        endsWithSpace = true;
    }

    // 3. 计算当前要匹配的 prefix：
    //    - 如果 endsWithSpace=true，则 prefix 为空(即要补全一个新的 token)
    //    - 否则 prefix 为 tokens.back()
    std::wstring prefix;
    if (endsWithSpace)
    {
        prefix.clear();
    }
    else
    {
        if (!tokens.empty())
            prefix = tokens.back();
        else
            prefix.clear();
    }

    // 4. 调用扩展后的自动补全函数
    auto newSuggestions = AutoCompleteEx(tokens, prefix, endsWithSpace);

    // 5. 如果 prefix 或者候选列表和上一次的不一样，则重置循环计数
    if (prefix != mLastPrefix || newSuggestions != mLastSuggestions)
    {
        mLastPrefix = prefix;
        mLastSuggestions = newSuggestions;
        mSuggestionIndex = 0;
    }

    // 6. 根据新候选列表的数量决定如何补全
    if (mLastSuggestions.empty())
    {
        // 没有可用的匹配，什么都不做(可以考虑 beep 或提示)
        // 这里简单返回
        return;
    }
    else if (mLastSuggestions.size() == 1)
    {
        // 只有一个候选，直接补全
        aInput = BuildCompletionString(tokens, endsWithSpace, mLastSuggestions[0]);
        std::wcout << L"\r> " << aInput; // 重绘输入行
    }
    else
    {
        // 有多个候选项
        // 判断是否存在“单独的前缀匹配”，如果数量>1说明有多个匹配
        // 我们这里简化处理：直接轮流循环
        aInput = BuildCompletionString(tokens, endsWithSpace, mLastSuggestions[mSuggestionIndex]);
        std::wcout << L"\r> " << aInput;
        // 更新索引，准备下次 Tab 时跳到下一个
        mSuggestionIndex = (mSuggestionIndex + 1) % mLastSuggestions.size();
    }
}

std::vector<std::wstring> GhTrCLI::ParseCommand(const std::wstring& aInput)
{
    std::vector<std::wstring> aTokens;
    std::wistringstream aIss(aInput);
    std::wstring aToken;
    while (aIss >> aToken)
    {
        aTokens.push_back(aToken);
    }
    return aTokens;
}

void GhTrCLI::DisplaySuggestions(const std::vector<std::wstring>& suggestions)
{
    std::wcout << L"\n匹配命令: ";
    for (const auto& suggestion : suggestions)
    {
        std::wcout << suggestion << L" ";
    }
    std::wcout << std::endl;
}

void GhTrCLI::HelpCommand(const std::vector<std::wstring>& aArgs)
{
    Logger::PrintHeader(mLanguageManager.GetString(L"help_header"));

    Logger::Print(mLanguageManager.GetString(L"current_version") + gProjectVersion, Logger::TextColor::Yellow);
    Logger::Print(mLanguageManager.GetString(L"supporting_version") + gSupportingVersion, Logger::TextColor::Yellow);
    Logger::Print(mLanguageManager.GetString(L"project_desc_prefix") + mLanguageManager.GetString(L"project_desc"), Logger::TextColor::Yellow);

    Logger::Print(mLanguageManager.GetString(L"available_commands"), Logger::TextColor::Green);
    Logger::Print(mLanguageManager.GetString(L"cmd_help"), Logger::TextColor::Cyan);
    Logger::Print(mLanguageManager.GetString(L"cmd_cls"), Logger::TextColor::Cyan);
    Logger::Print(mLanguageManager.GetString(L"cmd_exit"), Logger::TextColor::Cyan);
    Logger::Print(mLanguageManager.GetString(L"cmd_set_lang"), Logger::TextColor::Cyan);
    Logger::Print(mLanguageManager.GetString(L"cmd_set_lang_point"), Logger::TextColor::Cyan);
    Logger::Print(mLanguageManager.GetString(L"cmd_attach_pvz"), Logger::TextColor::Cyan);
    Logger::Print(mLanguageManager.GetString(L"cmd_attach_process"), Logger::TextColor::Cyan);

    Logger::Print(mLanguageManager.GetString(L"game_commands"), Logger::TextColor::Green);
    Logger::Print(mLanguageManager.GetString(L"cmd_set_sun"), Logger::TextColor::Cyan);
    Logger::Print(mLanguageManager.GetString(L"cmd_gamerule"), Logger::TextColor::Cyan);
    Logger::Print(mLanguageManager.GetString(L"cmd_gamerule_ignore"), Logger::TextColor::Cyan);
    Logger::Print(mLanguageManager.GetString(L"cmd_gamerule_unlimited_sun"), Logger::TextColor::Cyan);
    Logger::Print(mLanguageManager.GetString(L"test_warning"), Logger::TextColor::Yellow);

    // 可以在这里再提示：gamerule unlimitedSun true/false
}

void GhTrCLI::ExitCommand(const std::vector<std::wstring>& /*aArgs*/)
{
    if (mProcessHelper.IsAttached())
    {
        mProcessHelper.Detach();
        Logger::PrintInfo(mLanguageManager.GetString(L"detached_process"));
    }
    Logger::PrintSuccess(mLanguageManager.GetString(L"exiting_prefix") + gProjectName + mLanguageManager.GetString(L"exiting_suffix"));
    std::exit(0);
}

void GhTrCLI::ClsCommand(const std::vector<std::wstring>& /*aArgs*/)
{
    system("cls");
}

void GhTrCLI::AttachCommand(const std::vector<std::wstring>& aArgs)
{
    std::wstring targetProcessName;
    if (aArgs.empty() && mProcessHelper.IsAttached()) {
        Logger::PrintSuccess(mLanguageManager.GetString(L"attach_success"));
        return;
    }

    if (aArgs.empty())
    {
        // 没有提供进程名称，默认检查 PlantsVsZombies.exe
        targetProcessName = _S("PlantsVsZombies.exe");
        if (ProcessHelper::IsProcessRunning(targetProcessName))
        {
            Logger::PrintInfo(mLanguageManager.GetString(L"detect_process_running_prefix") + targetProcessName + mLanguageManager.GetString(L"detect_process_running_suffix"));
            Logger::Print(mLanguageManager.GetString(L"ask_attach_process"), Logger::TextColor::Yellow);
            std::wstring theResponse;
            std::getline(std::wcin, theResponse);
            if (theResponse != _S("y") && theResponse != _S("Y"))
            {
                Logger::PrintInfo(mLanguageManager.GetString(L"operation_canceled"));
                return;
            }
        }
        else
        {
            Logger::PrintError(targetProcessName + mLanguageManager.GetString(L"not_running"));
            Logger::Print(mLanguageManager.GetString(L"please_use_attach"), Logger::TextColor::Yellow);
            return;
        }
    }
    else
    {
        // 使用用户提供的进程名称
        targetProcessName = aArgs[0];
        if (!ProcessHelper::IsProcessRunning(targetProcessName))
        {
            Logger::PrintError(mLanguageManager.GetString(L"process_not_found_prefix") + targetProcessName + mLanguageManager.GetString(L"process_not_found_suffix"));
            return;
        }
    }

    mProcessHelper.SetProcessName(targetProcessName);
    if (mProcessHelper.Attach())
    {
        Logger::PrintSuccess(mLanguageManager.GetString(L"attach_success_prefix") + targetProcessName + mLanguageManager.GetString(L"attached_success_suffix"));
    }
    else
    {
        Logger::PrintError(mLanguageManager.GetString(L"attach_fail_prefix") + targetProcessName + mLanguageManager.GetString(L"attach_fail_suffix"));
    }
}

int GhTrCLI::StringToInt(const std::wstring& str, bool& success)
{
    try
    {
        int value = std::stoi(str);
        success = true;
        return value;
    }
    catch (const std::invalid_argument&)
    {
        success = false;
        return 0;
    }
    catch (const std::out_of_range&)
    {
        success = false;
        return 0;
    }
}

void GhTrCLI::SetSunCommand(const std::vector<std::wstring>& aArgs)
{
    if (aArgs.empty())
    {
        Logger::PrintError(mLanguageManager.GetString(L"missing_sun_arg"));
        return;
    }
    bool success = false;
    int theSunValue = StringToInt(aArgs[0], success);
    if (!success)
    {
        Logger::PrintError(mLanguageManager.GetString(L"invalid_number"));
        return;
    }
    if (theSunValue < 0)
    {
        Logger::PrintError(mLanguageManager.GetString(L"invalid_sun_value"));
        return;
    }
    if (!mProcessHelper.IsAttached())
    {
        Logger::PrintError(mLanguageManager.GetString(L"no_process_attached"));
        return;
    }

    GhTrMemory aGameMemory(mProcessHelper);
    if (!aGameMemory.Initialize())
    {
        Logger::PrintError(mLanguageManager.GetString(L"init_memory_fail"));
        return;
    }
    if (!aGameMemory.GetBoardAddress())
    {
        Logger::PrintError(mLanguageManager.GetString(L"board_not_found"));
        return;
    }
    if (aGameMemory.SetSunValue(theSunValue))
    {
        Logger::PrintSuccess(mLanguageManager.GetString(L"sun_set_success") + std::to_wstring(theSunValue));
    }
    else
    {
        Logger::PrintError(mLanguageManager.GetString(L"sun_set_fail"));
    }
}

void GhTrCLI::SetLangCommand(const std::vector<std::wstring>& aArgs)
{
    if (aArgs.empty())
    {
        bool isChinese = mLanguageManager.GetLanguage() == LanguageManager::Language::Chinese;
        mLanguageManager.SetLanguage(isChinese ? LanguageManager::Language::English : LanguageManager::Language::Chinese);
        Logger::PrintSuccess(mLanguageManager.GetString(isChinese ? L"lang_set_en_success" : L"lang_set_zh_success"));
        return;
    }
    const std::vector<std::wstring> theEnglishKeys = { L"English", L"english", L"Eng", L"eng", L"En", L"en", L"英语", L"英文" };
    const std::vector<std::wstring> theChineseKeys = { L"Chinese", L"chinese", L"cn", L"zh", L"zh-cn", L"cn-zh", L"中文", L"汉语" };

    if (std::find(theEnglishKeys.begin(), theEnglishKeys.end(), aArgs[0]) != theEnglishKeys.end())
    {
        mLanguageManager.SetLanguage(LanguageManager::Language::English);
        Logger::PrintSuccess(mLanguageManager.GetString(L"lang_set_en_success"));
    }
    else if (std::find(theChineseKeys.begin(), theChineseKeys.end(), aArgs[0]) != theChineseKeys.end())
    {
        mLanguageManager.SetLanguage(LanguageManager::Language::Chinese);
        Logger::PrintSuccess(mLanguageManager.GetString(L"lang_set_zh_success"));
    }
    else
    {
        Logger::PrintError(mLanguageManager.GetString(L"lang_set_fail"));
    }
}

void GhTrCLI::GameRuleCommand(const std::vector<std::wstring>& aArgs)
{
    if (aArgs.empty())
    {
        Logger::PrintError(mLanguageManager.GetString(L"missing_gamerule_arg"));
        return;
    }

    if (aArgs[0] == L"unlimitedSun")
    {
        if (!mProcessHelper.IsAttached())
        {
            Logger::PrintError(mLanguageManager.GetString(L"no_process_attached"));
            return;
        }

        GhTrMemory aGameMemory(mProcessHelper);
        if (!aGameMemory.Initialize())
        {
            Logger::PrintError(mLanguageManager.GetString(L"init_memory_fail"));
            return;
        }
        if (!aGameMemory.GetBoardAddress())
        {
            Logger::PrintError(mLanguageManager.GetString(L"board_not_found"));
            return;
        }

        // 如果只输入了 gamerule unlimitedSun
        if (aArgs.size() < 2)
        {
            // 切换状态
            bool currentState = aGameMemory.GetUnlimitedSunState();
            aGameMemory.ToggleUnlimitedSun(!currentState);
            if (!currentState)
            {
                Logger::PrintSuccess(mLanguageManager.GetString(L"unlimited_sun_enabled"));
            }
            else
            {
                Logger::Print(mLanguageManager.GetString(L"unlimited_sun_disabled"), Logger::TextColor::Cyan);
            }
        }
        else
        {
            // 带了第二个参数 (true/false/1/0)
            const auto& boolArg = aArgs[1];
            bool newState = false;
            bool validParam = true;

            if (boolArg == L"true" || boolArg == L"1" || boolArg == L"on")
            {
                newState = true;
            }
            else if (boolArg == L"false" || boolArg == L"0" || boolArg == L"off")
            {
                newState = false;
            }
            else
            {
                validParam = false;
            }

            if (!validParam)
            {
                Logger::PrintError(mLanguageManager.GetString(L"invalid_bool_arg"));
                return;
            }

            aGameMemory.ToggleUnlimitedSun(newState);
            if (newState)
            {
                Logger::PrintSuccess(mLanguageManager.GetString(L"unlimited_sun_enabled"));
            }
            else
            {
                Logger::Print(mLanguageManager.GetString(L"unlimited_sun_disabled"), Logger::TextColor::Cyan);
            }
        }
    }
    else
    {
        // 不支持的 gamerule
        Logger::PrintError(mLanguageManager.GetString(L"unknown_gamerule") + aArgs[0]);
    }
}