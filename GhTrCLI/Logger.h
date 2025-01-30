#pragma once

#include <iostream>
#include <string>

#define _S(str) L##str

class Logger
{
public:
    static void Initialize()
    {
        // 设置本地化
        std::locale::global(std::locale(""));
        // 设置 wcout 的本地化
        std::wcout.imbue(std::locale());
        // 同步 C++ 和 C 的标准流
        std::ios_base::sync_with_stdio(false);
    }

    enum class TextColor
    {
        Default = 39,
        Black = 30,
        Red = 31,
        Green = 32,
        Yellow = 33,
        Blue = 34,
        Magenta = 35,
        Cyan = 36,
        White = 37
    };

    enum class BackgroundColor
    {
        Default = 49,
        Black = 40,
        Red = 41,
        Green = 42,
        Yellow = 43,
        Blue = 44,
        Magenta = 45,
        Cyan = 46,
        White = 47
    };

    enum class TextStyle
    {
        Reset = 0,
        Bold = 1,
        Underline = 4
    };

    static void PrintWithoutEndl(const std::wstring& text, TextColor textColor = TextColor::Default,
        BackgroundColor bgColour = BackgroundColor::Default, TextStyle style = TextStyle::Reset)
    {
        std::wcout << "\033[" << static_cast<int>(style) << _S(";")
            << static_cast<int>(textColor) << _S(";")
            << static_cast<int>(bgColour) << _S("m")
            << text
            << _S("\033[0m");
    }

    static void Print(const std::wstring& text, TextColor textColor = TextColor::Default,
         BackgroundColor bgColour = BackgroundColor::Default, TextStyle style = TextStyle::Reset)
    {
        PrintWithoutEndl(text + _S("\n"), textColor, bgColour, style);
    }

    static void PrintSuccess(const std::wstring& text)
    {
        Print(text, TextColor::Green);
    }

    static void PrintError(const std::wstring& text)
    {
        Print(text, TextColor::Red);
    }

    static void PrintWarning(const std::wstring& text)
    {
        Print(text, TextColor::Yellow);
    }

    static void PrintInfo(const std::wstring& text)
    {
        Print(text, TextColor::Cyan);
    }

    static void PrintHeader(const std::wstring& text)
    {
        Print(text, TextColor::Green, BackgroundColor::Default, TextStyle::Bold); // 修改颜色
    }

    static void PrintStarter()
    {
        PrintWithoutEndl(_S("> "), TextColor::White, BackgroundColor::Default,TextStyle::Bold); // 修改颜色
    }
};