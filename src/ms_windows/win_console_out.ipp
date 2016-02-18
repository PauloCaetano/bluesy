// Copyright (c) 2016, Paulo Caetano
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

#ifndef WIN_CONSOLE_OUT_IPP
#define WIN_CONSOLE_OUT_IPP

#if !defined(PCBLUESY_MSWINDCON_FULLHEADER) && !defined(PCBLUESY_ALL_FULLHEADER)
#include "win_console_out.h"
#endif

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-local-typedefs"
#include "boost/locale.hpp"
#pragma GCC diagnostic pop

#include "windows.h"

#include <cassert>
#include <locale>
#include <sstream>


namespace pt { namespace pcaetano { namespace bluesy {
namespace ms_windows {

// -----------------------------------------------------------------------------
// This section contains auxiliary functions.
std::string ConvertText(char const* strToConv, std::string const& encTo, std::string const& encFrom);
std::string ConvertText(wchar_t const* strToConv, std::string const& encTo, std::string const&);
std::string GetEncoding(UINT codePage);
// -----------------------------------------------------------------------------


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


template <typename CharT = DefaultCharT, typename T = void>
std::string ConvertOutput(T const& t)
{
    std::basic_stringstream<CharT> ss;
    ss << t;
    return ConvertOutput(ss.str());
}

} // namespace ms_windows
}}}


#endif // WIN_CONSOLE_OUT_IPP
