#include "GhTrCLI.h"
#include <sstream>

GhTrCLI::GhTrCLI()
{
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
}

void GhTrCLI::Run()
{
    Logger::Initialize();
    Logger::PrintHeader(_S("欢迎使用 ") + gProjectName + _S(" (版本 ") + gProjectVersion + _S(") !"));
    Logger::PrintInfo(_S("输入 help 查看帮助，exit 退出"));
    if (ProcessHelper::IsProcessRunning(_S("PlantsVsZombies.exe")))
    {
        Logger::PrintInfo(_S("检测到 PlantsVsZombies.exe 正在运行"));
        Logger::Print(_S("是否附加到该进程？(y/n)："), Logger::TextColor::Yellow);
        std::wstring theResponse;
        std::getline(std::wcin, theResponse);
        if (theResponse == _S("y") || theResponse == _S("Y"))
        {
            mProcessHelper.SetProcessName(_S("PlantsVsZombies.exe"));
            if (mProcessHelper.Attach())
            {
                Logger::PrintSuccess(_S("已成功附加到进程PlantsVsZombies.exe"));
            }
            else
            {
                Logger::PrintError(_S("无法附加到进程,请检查游戏版本和进程状态"));
            }
        }
    }

    while (true)
    {
        Logger::PrintStarter();
        std::wstring aInput;
        if (!std::getline(std::wcin, aInput))
        {
            break;
        }

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
            Logger::PrintError(_S("未知命令: ") + aCommandName);
        }
    }

    Logger::PrintInfo(_S("CLI 已退出"));
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
    Logger::PrintHeader(_S("GhTrCLI 帮助信息："));
    Logger::Print(_S("[当前版本] ") + gProjectVersion, Logger::TextColor::Yellow);
    Logger::Print(_S("[当前支持GhTr版本] ") + gSupportingVersion, Logger::TextColor::Yellow);
    Logger::Print(_S("[项目简介] ") + gProjectDesc, Logger::TextColor::Yellow);

    Logger::Print(_S("[可用命令] "), Logger::TextColor::Green);
    Logger::Print(_S(" · help            显示此帮助信息"), Logger::TextColor::Cyan);
    Logger::Print(_S(" · cls            清空命令行信息"), Logger::TextColor::Cyan);
    Logger::Print(_S(" · exit 或 quit    退出 CLI"), Logger::TextColor::Cyan);
    Logger::Print(_S(" · attach      尝试附加到 PlantsVsZombies.exe"), Logger::TextColor::Cyan);
    Logger::Print(_S(" · attach [进程名]      尝试附加到指定的进程"), Logger::TextColor::Cyan);

    Logger::Print(_S("[游戏操作命令] "), Logger::TextColor::Green);
    Logger::Print(_S(" · set-sun <阳光数量>      修改已附加进程游戏的阳光数量"), Logger::TextColor::Cyan);

    Logger::Print(_S("功能仅供测试使用 使用后会为您的存档添加上「帮助模式」标记"), Logger::TextColor::Yellow);
}

void GhTrCLI::ExitCommand(const std::vector<std::wstring>& /*aArgs*/)
{
    if (mProcessHelper.IsAttached())
    {
        mProcessHelper.Detach();
        Logger::PrintInfo(_S("已断开与目标进程的连接"));
    }
    Logger::PrintSuccess(_S("正在退出 ") + gProjectName + _S("..."));
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
        Logger::PrintSuccess(_S("已成功附加到目标"));
        return;
    }
    if (aArgs.empty())
    {
        // 没有提供进程名称，默认检查 PlantsVsZombies.exe
        targetProcessName = _S("PlantsVsZombies.exe");
        if (ProcessHelper::IsProcessRunning(targetProcessName))
        {
            Logger::PrintInfo(_S("检测到 ") + targetProcessName + _S(" 正在运行"));
            Logger::Print(_S("是否附加到该进程？(y/n)："), Logger::TextColor::Yellow);
            std::wstring theResponse;
            std::getline(std::wcin, theResponse);
            if (theResponse != _S("y") && theResponse != _S("Y"))
            {
                Logger::PrintInfo(_S("操作已取消"));
                return;
            }
        }
        else
        {
            Logger::PrintError(targetProcessName + _S(" 未运行"));
            Logger::Print(_S("请使用 'attach [进程名]' 命令指定要附加的进程"), Logger::TextColor::Yellow);
            return;
        }
    }
    else
    {
        // 使用用户提供的进程名称
        targetProcessName = aArgs[0];
        if (!ProcessHelper::IsProcessRunning(targetProcessName))
        {
            Logger::PrintError(_S("未找到进程 ") + targetProcessName + _S(""));
            return;
        }
    }

    mProcessHelper.SetProcessName(targetProcessName);

    if (mProcessHelper.Attach())
    {
        Logger::PrintSuccess(_S("已成功附加到进程 ") + targetProcessName + _S(""));
    }
    else
    {
        Logger::PrintError(_S("无法附加到进程 ") + targetProcessName + _S(" ,请检查游戏版本和进程状态"));
    }
}

void GhTrCLI::SetSunCommand(const std::vector<std::wstring>& aArgs)
{
    if (aArgs.empty())
    {
        Logger::PrintError(_S("缺少阳光数参数"));
        return;
    }

    int theSunValue = _wtoi(aArgs[0].c_str());
    if (theSunValue < 0)
    {
        Logger::PrintError(_S("无效的阳光数值"));
        return;
    }

    if (!mProcessHelper.IsAttached())
    {
        Logger::PrintError(_S("尚未附加到任何进程"));
        return;
    }

    GhTrMemory aGameMemory(mProcessHelper);
    if (!aGameMemory.Initialize())
    {
        Logger::PrintError(_S("初始化游戏内存失败，可能是游戏版本错误或进程无法打开"));
        return;
    }
    if (!aGameMemory.GetBoardAddress())
    {
        Logger::PrintError(_S("未进入草坪，无法修改阳光"));
        return;
    }
    if (aGameMemory.SetSunValue(theSunValue))
    {
        Logger::PrintSuccess(_S("成功设置阳光数值为: ") + std::to_wstring(theSunValue));
    }
    else
    {
        Logger::PrintError(_S("无法设置阳光数值"));
    }
}