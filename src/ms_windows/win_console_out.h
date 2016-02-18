// Copyright (c) 2013-2016, Paulo Caetano
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

#ifndef WIN_CONSOLE_OUT_H
#define WIN_CONSOLE_OUT_H

// The functions defined in this module output messages on an
// MS Windows console, performing the necessary conversions
// to correctly display non-ASCII characters.
//
// Requires Boost Locale and UCI.

// This has been successfully tested in the following scenarios:
// 1. Using cout to output char * / std::string with portuguese characters.

// This module is useful ONLY when using Mingw.
// When using MSVC, the following line should solve any problems
// with the console and non-ASCII characters (WARNING! Not thouroughly tested):
// std::locale::global(std::locale(""));
//
// There's no need to use this module on MSVC, and it may actually cause problems,
// due to string literals encoding. I was only able to make this work reliably in
// MSVC by using the undocumented #pragma execution_character_set("utf-8")
// So, my advice is: Don't use this on MSVC, unless setting the global locale,
// as shown above, isn't working for you. If that is the case,
// just #define PCBLUESY_MSWINDCON_FORCEMSVC. However, I repeat the warning:
// I was only able to make this work reliably in MSVC by using the
// undocumented #pragma execution_character_set("utf-8").
#if defined(_MSC_VER) && !defined(PCBLUESY_MSWINDCON_FORCEMSVC)
#error("This was designed for mingw, and it does not work reliably with MSVC. If you still want to use it anyway, \
    #define PCBLUESY_MSWINDCON_FORCEMSVC, but you'd be better off trying something like std::locale::global(std::locale("") before outputting your string.")
#endif

#include <string>

// Uncomment this #define if most of your work is with wstring/wchar_t.
//#define PCBLUESY_MSWINDCON_DEFAULT_WCHART

namespace pt { namespace pcaetano { namespace bluesy {
namespace ms_windows {

#ifdef PCBLUESY_MSWINDCON_DEFAULT_WCHART
using DefaultCharT = wchar_t;
#else
using DefaultCharT = char;
#endif

// Converts a string for console output, according to the console's
// code page.
// TODO: Consider std::wstring return type?
template <typename CharT>
std::string ConvertOutput(CharT const* s);

// The two overloads below call the function above.

template <typename CharT>
std::string ConvertOutput(std::basic_string<CharT> const& s);

// I considered creating an interface for types that don't support operator<<,
// but have some way of converting to std::(w)string. The cleanest approach
// would be an overload of ConvertOuput() that took a callable parameter
// (e.g., fn obj or lambda). However, it would have to be a generic
// parameter, i.e., ConvertOutput(T const& t) which would lead to
// resolution ambiguity.
// The solution, for now, requires clients to convert to std::(w)string
// before calling ConvertOutput().

// We need to have the CharT first, so we can do something like this:
// ConvertOutput<wchar_t>(s2)
// However, we want to have a default CharT, so we don't have to
// always specify it. That means we need to give the 2nd param
// a default value as well, even though it makes no sense.
template <typename CharT = DefaultCharT, typename T = void>
std::string ConvertOutput(T const& t);

} // namespace ms_windows
}}}

#if defined(PCBLUESY_MSWINDCON_FULLHEADER) || defined(PCBLUESY_ALL_FULLHEADER)
#include "win_console_out.ipp"
#endif

#endif // WIN_CONSOLE_OUT_H
