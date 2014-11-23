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

#include <boost/test/unit_test.hpp>

#include "ms_windows/win_console_out.h"
using pt::pcaetano::bluesy::ms_windows::GetEncoding;
using pt::pcaetano::bluesy::ms_windows::ConvertOutput;
#include "ms_windows/win_exception.h"
using pt::pcaetano::bluesy::ms_windows::EncodingNotFoundException;

#include <ostream>
using std::basic_ostream;
using std::ostream;
using std::wostream;
#include <string>
using std::basic_string;
using std::string;
using std::wstring;

BOOST_AUTO_TEST_SUITE(mswindows_console)

BOOST_AUTO_TEST_CASE(mswcon_get_encoding)
{
    string enc = GetEncoding(850);

    BOOST_REQUIRE_EQUAL(enc, "cp850");
}

BOOST_AUTO_TEST_CASE(mswcon_get_encoding_not_exists)
{
    BOOST_REQUIRE_THROW(GetEncoding(0), EncodingNotFoundException);
}

char const* origC = "áàâãäéèêëíìîïóòôõöúùûüÁÀÂÃÄÉÈÊËÍÌÎÏÓÒÔÕÖÚÙÛÜ£§ºªçÇ«»";
wchar_t const* origW = L"áàâãäéèêëíìîïóòôõöúùûüÁÀÂÃÄÉÈÊËÍÌÎÏÓÒÔÕÖÚÙÛÜ£§ºªçÇ«»";

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnarrowing"
// From http://en.wikipedia.org/wiki/Code_page_850#Code_page_layout
char const expectedC[] = {0xA0, 0x85, 0x83, 0xC6, 0x84, 0x82, 0x8A, 0x88, 0x89, 0xA1, 0x8d, 0x8c, 0x8B,
    0xA2, 0x95, 0x93, 0xE4, 0x94, 0xA3, 0x97, 0x96, 0x81, 0xB5, 0xB7, 0xB6, 0xC7, 0x8E, 0x90, 0xD4, 0xD2,
    0xD3, 0xD6, 0xDe, 0xD7, 0xD8, 0xE0, 0xE3, 0xE2, 0xE5, 0x99, 0xE9, 0xEB, 0xEA, 0x9A, 0x9C, 0xF5, 0xA7,
    0xA6, 0x87, 0x80, 0xAE, 0xAF, 0x00};
#pragma GCC diagnostic pop

BOOST_AUTO_TEST_CASE(mswcon_convert_char_ptr)
{
    string expected{expectedC};
    string actual{ConvertOutput(origC)};

    BOOST_REQUIRE_EQUAL(expected, actual);
}

BOOST_AUTO_TEST_CASE(mswcon_convert_wchart_ptr)
{
    string expected{expectedC};
    string actual{ConvertOutput(origW)};

    BOOST_REQUIRE_EQUAL(expected, actual);
}

BOOST_AUTO_TEST_CASE(mswcon_convert_string)
{
    string orig{origC};
    string expected{expectedC};
    string actual{ConvertOutput(orig)};

    BOOST_REQUIRE_EQUAL(expected, actual);
}

BOOST_AUTO_TEST_CASE(mswcon_convert_wstring)
{
    wstring orig{origW};
    string expected{expectedC};
    string actual{ConvertOutput(orig)};

    BOOST_REQUIRE_EQUAL(expected, actual);
}

template <typename CharT>
struct TestConvert
{
    explicit TestConvert(CharT const* s) : val1{s} {}
    basic_string<CharT> val1;
};

template <typename CharT>
basic_ostream<CharT>& operator<<(basic_ostream<CharT>& os, TestConvert<CharT> const& tc)
{
    os << tc.val1;

    return os;
}

BOOST_AUTO_TEST_CASE(mswcon_convert_out_operator_char)
{
    TestConvert<char> tc{origC};

    string expected{expectedC};
    string actual{ConvertOutput(tc)};

    BOOST_REQUIRE_EQUAL(expected, actual);
}

BOOST_AUTO_TEST_CASE(mswcon_convert_out_operator_wchar)
{
    TestConvert<wchar_t> tc{origW};

    string expected{expectedC};
    string actual{ConvertOutput<wchar_t>(tc)};

    BOOST_REQUIRE_EQUAL(expected, actual);
}

BOOST_AUTO_TEST_SUITE_END()
