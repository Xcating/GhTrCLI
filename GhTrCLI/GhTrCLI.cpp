#include "GhTrCLI.h"
#include <conio.h>
#include <sstream>
#include <unordered_map>
#include <vector>

GhTrCLI::GhTrCLI()
{
    mLanguageManager.LoadLanguage(PRIMARYLANGID(GetUserDefaultUILanguage()) == LANG_CHINESE
        ? LanguageManager::Language::Chinese
        : LanguageManager::Language::English);
    RegisterCommands();
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
    int historyIndex = -1;  // 当前历史命令索引
    std::wstring aInput;  // 当前输入的命令
    std::wstring prefix;  // 自动补全前缀

    while (true)
    {
        Logger::PrintStarter();
        aInput.clear();  // 清空输入
        historyIndex = static_cast<int>(history.size());
        while (true)
        {
            if (_kbhit())  // 检测键盘输入
            {
                char ch = _getch();
                if (ch == 13)  // 回车键 (Enter)
                {
                    if (!aInput.empty()) {
                        history.push_back(aInput);  // 保存命令到历史
                        historyIndex = static_cast<int>(history.size());
                    }
                    break;
                }
                else if (ch == 9)  // Tab键
                {
                    // 自动补全逻辑
                    prefix = aInput;
                    auto matched = AutoComplete(prefix);
                    if (matched.size() == 1)
                    {
                        aInput = matched[0];
                        std::wcout << L"\r> " << aInput;  // 自动补全后保留">"提示符
                    }
                    else
                    {
                        DisplaySuggestions(matched);
                    }
                }
                else if (ch == 8)  // Backspace键
                {
                    if (!aInput.empty())
                    {
                        aInput.pop_back();  // 删除最后一个字符
                        std::wcout << L"\r> " << aInput << L" ";  // 重新绘制输入框
                        std::wcout << L"\r> " << aInput;  // 保持光标在正确位置
                    }
                }
                else if (ch == -32)
                {
                    ch = _getch();
                    if (ch == 72) //Up
                    {
                        if (historyIndex > 0) {
                            --historyIndex;
                            aInput = history[historyIndex];
                            std::wcout << L"\r> " << aInput << L"                                        ";  // 添加空格清除旧内容
                            std::wcout << L"\r> " << aInput;  // 重新定位光标
                        }
                    }
                    else if (ch == 80) //Down
                    {
                        if (static_cast<size_t>(historyIndex) < history.size() - 1) {
                            ++historyIndex;
                            aInput = history[historyIndex];
                            std::wcout << L"\r> " << aInput << L"                                        ";  // 添加空格清除旧内容
                            std::wcout << L"\r> " << aInput;  // 重新定位光标
                        }
                    }
                }
                else
                {
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

std::vector<std::wstring> GhTrCLI::AutoComplete(const std::wstring& prefix)
{
    std::vector<std::wstring> matchedCommands;
    for (const auto& cmd : mCommands)
    {
        if (cmd.first.find(prefix) == 0)  // 如果命令以输入的前缀开始
        {
            matchedCommands.push_back(cmd.first);
        }
    }
    return matchedCommands;
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
    Logger::Print(mLanguageManager.GetString(L"test_warning"), Logger::TextColor::Yellow);
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