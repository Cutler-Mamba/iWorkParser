#ifndef __UTIL_H__
#define __UTIL_H__
#include <iostream>
#include <vector>

std::wstring FileSuffix(std::wstring const &path);
std::wstring BaseName(std::wstring const &path);

class C
{
public:
    static std::string Uni2UTF8Str(std::wstring pwszSrc);
    static std::wstring UTF82UniStr(std::string utf8Str);
    static std::wstring Ansi2Unicode(const std::string& str);
    static std::string Unicode2Ansi(const std::wstring& wstr, char rep);
    static std::string UnicodeToANSI(const std::wstring &wstr);
    static std::wstring ANSIToUnicode(const std::string &str);

    static void SplitString(const std::string& s, std::vector<std::string>& v, const std::string& c);
};
#endif //__UTIL_H__