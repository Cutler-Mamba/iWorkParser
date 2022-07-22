#include <iostream>
#include <locale> 
#include <codecvt> 
#include <locale>
#include "util.hpp"

std::wstring FileSuffix(std::wstring const &path) 
{
  int pos = path.find_last_of(L".");
  //if there is no suffix
  if (pos < 0) 
  {
    return L"";
  }
  return path.substr(pos + 1);
}

std::wstring BaseName(std::wstring const &path)
{
#ifdef _WIN32
    int pos = path.find_last_of(L"/\\");
    if (pos < 0) {
        return path;
     }
    return path.substr(pos + 1);
#else
    int pos = path.find_last_of(L"/");
    if (pos < 0) {
        return path;
    }
    return path.substr(pos + 1);
#endif
    return L"";
}

std::string C::Uni2UTF8Str(std::wstring pwszSrc) 
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> conv; 
    std::string narrowStr = conv.to_bytes(pwszSrc);
    return narrowStr;
}

std::wstring C::UTF82UniStr(std::string utf8Str)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
    std::wstring wideStr = conv.from_bytes(utf8Str);
    return wideStr;
}

std::wstring C::Ansi2Unicode(const std::string& str)
{
    std::wstring wstr(str.size(), 0);
#if _MSC_VER >= 1400    // use Microsofts Safe libraries if possible (>=VS2005)
    std::use_facet<std::ctype<wchar_t> >(std::locale())._Widen_s
        (&str[0], &str[0]+str.size(), &wstr[0], wstr.size());
#else
    std::use_facet<std::ctype<wchar_t> >(std::locale()).widen
        (&str[0], &str[0]+str.size(), &wstr[0]);
#endif
    return wstr;
}

std::string C::Unicode2Ansi(const std::wstring& wstr, char rep = '_')
{
    std::string str(wstr.size(), 0);
#if _MSC_VER >= 1400
    std::use_facet<std::ctype<wchar_t> >(std::locale())._Narrow_s
        (&wstr[0], &wstr[0]+wstr.size(), rep, &str[0], str.size());
#else
    std::use_facet<std::ctype<wchar_t> >(std::locale()).narrow
        (&wstr[0], &wstr[0]+wstr.size(), rep, &str[0]);
#endif
    return str;
}


// use c api to convert between ansi and
std::string C::UnicodeToANSI(const std::wstring &wstr) 
{
    std::string ret;
    std::mbstate_t state = {};
    const wchar_t *src = wstr.data();
    size_t len = std::wcsrtombs(nullptr, &src, 0, &state);
    if (static_cast<size_t>(-1) != len) {
        std::unique_ptr<char[]> buff(new char[len + 1]);
        len = std::wcsrtombs(buff.get(), &src, len, &state);
        if (static_cast<size_t>(-1) != len) {
            ret.assign(buff.get(), len);
        }
    }
    return ret;
}

std::wstring C::ANSIToUnicode(const std::string &str) 
{
    std::wstring ret;
    std::mbstate_t state = {};
    const char *src = str.data();

    size_t len = std::mbsrtowcs(nullptr, &src, 0, &state);
    if (static_cast<size_t>(-1) != len) {
        std::unique_ptr<wchar_t[]> buff(new wchar_t[len + 1]);
        len = std::mbsrtowcs(buff.get(), &src, len, &state);
        if (static_cast<size_t>(-1) != len) {
            ret.assign(buff.get(), len);
        }
    }
    return ret;
}

void C::SplitString(const std::string& s, std::vector<std::string>& v, const std::string& c)
{
    std::string::size_type pos1, pos2;
    pos2 = s.find(c);
    pos1 = 0;
    while (std::string::npos != pos2)
    {
        v.push_back(s.substr(pos1, pos2 - pos1));

        pos1 = pos2 + c.size();
        pos2 = s.find(c, pos1);
    }
    if (pos1 != s.length())
        v.push_back(s.substr(pos1));
}