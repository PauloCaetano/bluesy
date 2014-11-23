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

#ifndef WIN_CONSOLE_OUT_H
#define WIN_CONSOLE_OUT_H

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-local-typedefs"
#include "boost/locale.hpp"
#pragma GCC diagnostic pop

#include "windows.h"

#include <cassert>
#include <locale>
#include <string>
#include <sstream>

// Uncomment this #define if most of your work is with wstring/wchar_t.
//#define PCBLUESY_MSWINDCON_DEFAULT_WCHART

namespace pt { namespace pcaetano { namespace bluesy {
namespace ms_windows {



#ifdef PCBLUESY_MSWINDCON_DEFAULT_WCHART
using DefaultCharT = wchar_t;
#else
using DefaultCharT = char;
#endif

// The functions defined in this module output messages on an
// MS Windows console, performing the necessary conversions
// to correctly display non-ASCII characters.
// Requires Boost Locale and UCI.

// -----------------------------------------------------------------------------
// This section contains auxiliary functions.
std::string ConvertText(char const* strToConv, std::string const& encTo, std::string const& encFrom);
std::string ConvertText(wchar_t const* strToConv, std::string const& encTo, std::string const&);
std::string GetEncoding(UINT codePage);
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// This section contains the functions called to perform the conversion to the
// console's code page. These functions are this module's interface.

// Converts a string for console output, according to the console's
// code page.
// The two overloads below call this function.
// TODO: Allow for std::wstring return type?
template <typename CharT>
std::string ConvertOutput(CharT const* s)
{
    assert(s != nullptr);

#ifndef PCBLUESY_UNIT_TEST
    std::string enc = GetEncoding(GetConsoleOutputCP());
#else
    // When we're building for unit tests, we force the
    // console code page
    std::string enc = GetEncoding(850);
#endif

    boost::locale::generator gen;
    std::locale loc = gen("");

    return ConvertText(s, enc, std::use_facet<boost::locale::info>(loc).encoding());
}


template <typename CharT>
std::string ConvertOutput(std::basic_string<CharT> const& s)
{
    return ConvertOutput(s.c_str());
}

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
std::string ConvertOutput(T const& t)
{
    std::basic_stringstream<CharT> ss;
    ss << t;
    return ConvertOutput(ss.str());
}
// -----------------------------------------------------------------------------

} // namespace ms_windows
}}}

#endif // WIN_CONSOLE_OUT_H
