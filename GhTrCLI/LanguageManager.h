#pragma once
#include <windows.h>
#include <iostream>
#include <string>
#include <unordered_map>
#include <fstream>
#include <locale>
#include <codecvt>
#include <sstream>
#include <stdexcept>

class SimpleJsonParser
{
public:
    static std::unordered_map<std::wstring, std::wstring> ParseJson(const std::wstring& jsonString)
    {
        std::unordered_map<std::wstring, std::wstring> result;

        // 去除开头和结尾的空白字符
        std::wstring trimmedJson = Trim(jsonString);

        // 假设 JSON 格式是简单的 key: value 对, 这里没有处理嵌套结构
        size_t startPos = 0;
        while (startPos < trimmedJson.size())
        {
            // 找到键值对的分隔符
            size_t keyStart = trimmedJson.find(L"\"", startPos);
            if (keyStart == std::wstring::npos) break;

            size_t keyEnd = trimmedJson.find(L"\"", keyStart + 1);
            if (keyEnd == std::wstring::npos) break;

            std::wstring key = trimmedJson.substr(keyStart + 1, keyEnd - keyStart - 1);

            // 查找冒号分隔符
            size_t colonPos = trimmedJson.find(L":", keyEnd);
            if (colonPos == std::wstring::npos) break;

            // 查找值的起始位置
            size_t valueStart = trimmedJson.find(L"\"", colonPos + 1);
            if (valueStart == std::wstring::npos) break;

            size_t valueEnd = trimmedJson.find(L"\"", valueStart + 1);
            if (valueEnd == std::wstring::npos) break;

            std::wstring value = trimmedJson.substr(valueStart + 1, valueEnd - valueStart - 1);

            result[key] = value;

            // 跳过处理过的部分
            startPos = valueEnd + 1;
        }

        return result;
    }

private:
    static std::wstring Trim(const std::wstring& str)
    {
        size_t first = str.find_first_not_of(L" \t\n\r");
        if (first == std::wstring::npos) return L"";
        size_t last = str.find_last_not_of(L" \t\n\r");
        return str.substr(first, (last - first + 1));
    }
};

#define LANG_EN_JSON                    103
#define LANG_ZH_JSON                    104

class ResourceManager
{
public:
    static std::wstring LoadResourceString(const std::wstring& resourceName)
    {
        // 查找嵌入的资源
        int resourceID = resourceName == L"LANG_EN_JSON" ? LANG_EN_JSON : LANG_ZH_JSON;
        HRSRC hRes = FindResource(NULL, MAKEINTRESOURCE(resourceID), RT_RCDATA);
        if (hRes == NULL)
        {
            std::wcerr << L"找不到资源: " << resourceName << std::endl;
            return L"";
        }
        // 加载资源
        HGLOBAL hData = LoadResource(NULL, hRes);
        if (hData == NULL)
        {
            std::wcerr << L"无法加载资源: " << resourceName << std::endl;
            return L"";
        }
        // 获取资源大小并提取数据
        DWORD dwSize = SizeofResource(NULL, hRes);
        void* pData = LockResource(hData);
        if (pData == NULL)
        {
            std::wcerr << L"无法锁定资源: " << resourceName << std::endl;
            return L"";
        }

        // 将资源数据视为 UTF-16 LE 宽字符
        const wchar_t* pWideData = reinterpret_cast<wchar_t*>(pData);
        size_t wcharCount = dwSize / sizeof(wchar_t);

        // 检查是否有 BOM (0xFEFF), 如果有，跳过
        if (wcharCount >= 1 && pWideData[0] == 0xFEFF)
        {
            // 跳过 BOM
            pWideData += 1;
            wcharCount -= 1;
        }

        // 构造 wstring
        return std::wstring(pWideData, wcharCount);
    }
};

class LanguageManager
{
public:
    enum class Language
    {
        English,
        Chinese
    };

    LanguageManager() : currentLanguage(Language::English) {}

    void LoadLanguage(Language lang)
    {
        std::wstring resourceName = lang == Language::English ? L"LANG_EN_JSON" : L"LANG_ZH_JSON";
        std::wstring jsonString = ResourceManager::LoadResourceString(resourceName);
        if (jsonString.empty())
        {
            std::wcerr << L"无法加载语言资源: " << resourceName << std::endl;
            return;
        }

        try
        {
            mLanguageStrings = SimpleJsonParser::ParseJson(jsonString);
            currentLanguage = lang;
        }
        catch (const std::exception& e)
        {
            std::wcerr << L"解析 JSON 错误: " << e.what() << std::endl;
        }
    }

    std::wstring GetString(const std::wstring& key)
    {
        auto it = mLanguageStrings.find(key);
        if (it != mLanguageStrings.end())
        {
            return it->second;
        }
        return L"";
    }

    void SetLanguage(Language lang)
    {
        if (lang != currentLanguage)
        {
            LoadLanguage(lang);
        }
    }

    Language GetLanguage()
    {
        return currentLanguage;
    }

private:
    Language currentLanguage;
    std::unordered_map<std::wstring, std::wstring> mLanguageStrings;
};