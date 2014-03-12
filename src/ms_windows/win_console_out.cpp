// Copyright (c) 2013-2014, Paulo Caetano
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//
//     * Neither the name of the copyright holder nor the names of any other
//       contributors may be used to endorse or promote products derived from
//       this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
// ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "win_console_out.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-local-typedefs"
#include "boost/locale.hpp"
using boost::locale::conv::between;
using boost::locale::conv::from_utf;
#pragma GCC diagnostic pop

#include <cassert>
#include <exception>
using std::logic_error;
// TODO: Use cbegin()/cend() when it becomes available (C++14)
#include <iterator>
using std::begin;
using std::end;
#include <string>
using std::string;

namespace pt { namespace pcaetano { namespace bluesy {
namespace ms_windows {

// This is the result of several attempts at solving this problem,
// such as imbued streams and SetConsoleCP(), all with no success,
// and including setting the console's code page prior to running the app,
// which caused cout to go invalid right after the first write.
// In the end, I settled for this solution.

// Converts strToConv from the encoding encFrom to encTo.
// TODO: Add template parameter to allow for std::wstring return type?
string ConvertText(char const* strToConv, string const& encTo, string const& encFrom)
{
    assert(strToConv != nullptr);
    return between(strToConv, encTo, encFrom);
}

// Onverload of the previous function.
// The wchar_t overload ignores encFrom, but I kept it in the interface so it can be
// seamlessly called from ConvertOutput.
// TODO: Add template parameter to allow for std::wstring return type?
string ConvertText(wchar_t const* strToConv, string const& encTo, string const&)
{
    assert(strToConv != nullptr);
    return from_utf(strToConv, encTo);
}

// Copied from Boost Locale, with a few trivial changes
struct windows_encoding
{
    windows_encoding() : name(nullptr), codepage(0) {}
    windows_encoding(char const* n, unsigned int cp) : name(n), codepage(cp) {}
    char const *name;
    unsigned int codepage = 0;
};

bool operator==(windows_encoding const &l, windows_encoding const &r)
{
    return l.codepage == r.codepage;
}

windows_encoding const all_windows_encodings[] =
{
    { "cp850",      850 },
    { "cp858",      858 },
    { "cp874",      874 },
//  { "windows874", 874 },
    { "cp932",      932 },
//  { "shiftjis",   932 },
//  { "sjis",       932 },
//  { "windows932", 932 },
    { "cp936",      936 },
//  { "gbk",        936 },
//  { "ms936",      936 },
//  { "windows936", 936 },
    { "big5",       950 },
    { "cp1250",     1250 },
//  { "windows1250",1250 },
    { "cp1251",     1251 },
//  { "windows1251",1251 },
    { "cp1252",     1252 },
//  { "windows1252",1252 },
    { "cp1253",     1253 },
//  { "windows1253",1253 },
    { "cp1254",     1254 },
//  { "windows1254",1254 },
    { "cp1255",     1255 },
//  { "windows1255",1255 },
    { "cp1256",     1256 },
//  { "windows1256",1256 },
    { "cp1257",     1257 },
//  { "windows1257",1257 },
    { "usascii",    20127 },
    { "koi8r",      20866 },
    { "eucjp",      20932 },
    { "gb2312",     20936 },
    { "koi8u",      21866 },
    { "iso88591",   28591 },
    { "iso88592",   28592 },
    { "iso88593",   28593 },
    { "iso88594",   28594 },
    { "iso88595",   28595 },
    { "iso88596",   28596 },
    { "iso88597",   28597 },
    { "iso88598",   28598 },
    { "iso88599",   28599 },
    { "iso885913",  28603 },
    { "iso885915",  28605 },
    { "iso2022jp",  50220 },
    { "iso2022kr",  50225 },
    { "euckr",      51949 },
    { "gb18030",    54936 },
    { "utf8",       65001 },
};

// Retrieves the textual representation of the encoding
// associated with a code page. E.g., GetEncoding(850) returns "cp850".
string GetEncoding(UINT codePage)
{
    windows_encoding ref;
    ref.codepage = codePage;

    windows_encoding const *b = begin(all_windows_encodings);
    windows_encoding const *e = end(all_windows_encodings);
    windows_encoding const *ptr = std::find(b, e, ref);

    if (ptr == e)
    {
        //TODO: Create exception for this?
        throw logic_error("Encoding not found");
    }

    return ptr->name;
}

} // namespace ms_windows
}}}
