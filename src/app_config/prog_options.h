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

#ifndef PROG_OPTIONS_H
#define PROG_OPTIONS_H

#include "appconfigexception.h"
#include "ms_windows/win_console_out.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-local-typedefs"
#include <boost/program_options.hpp>
namespace po = boost::program_options;
#pragma GCC diagnostic pop

#include <cassert>
#include <iostream>

namespace pt { namespace pcaetano { namespace bluesy {
namespace config
{

// The AppOptions class template abstracts the generic part of setting up
// Boost Program Options.
//
// The SpecificOptions template parameter supplies the class
// that will deal with each app's specific details.
// Thus, each app will define its own SpecificOptions class.
//
// SpecificOptions requirements:
// - default ctor
// - std::string GetHelpOption(). Must return the option corresponding
//      to help (e.g., "help").
// - void DefineOptions(po::options_description& desc). Defines the app's
//      actual options.
// - void Validate(). Should throw an exception in case of error.
// - std::ostream& operator<<(std::ostream& os, SpecificOptions const& obj)
template <typename SpecificOptions>
class AppOptions
{
public:
    AppOptions() = delete;

    // argc and argv, as received in main(). opTitle is the caption
    // shown on the first line when we output the options description,
    // aka, the "help message".
    // The os param is the output stream to which we want to output
    // the help message. Usually, it'll be cout.
    // Why the char const* const argv[]? Check this:
    // http://www.parashift.com/c++-faq-lite/constptrptr-conversion.html
    template <typename CharT = char>
    AppOptions(int argc, char const* const argv[], const char* opTitle,
        std::basic_ostream<CharT>& os = std::cout, bool wantConvert = true);

    // Prints the options description, aka, the "help message".
    // TODO: wantConvert should be true by default only on MS Windows.
    template <typename CharT = char>
    void ShowHelp(std::basic_ostream<CharT>& os = std::cout, bool wantConvert = true) const;

    // Exposes the instance of SpecificOptions, the class that deals
    // with the actual app options. We need this because the app creates an
    // instance of AppOptions, which then creates an instance of
    // SpecificOptions, i.e., the app has no way of referring to
    // this instance of SpecificOptions.
    const SpecificOptions& GetOptions() const { return so; }

    // If the user wishes to see the app's "help message", the app needs
    // a way to know it, since the most common scenario is showing it and
    // exiting. Thus, after creating an instance of AppOptions, the app
    // should call HaveShownHelp(), to determine what to do next.
    bool HaveShownHelp() const { return shownHelp; }
private:
    template <typename SpecOpt, typename CharT>
    friend std::basic_ostream<CharT>&
    operator<<(std::basic_ostream<CharT>& os, AppOptions<SpecOpt> const& obj);

    bool GotRequestForHelp(po::variables_map const& vm, SpecificOptions const& so) const
        { return vm.count(so.GetHelpOption()); }

    bool shownHelp;
    po::options_description desc;
    SpecificOptions so;
};

template <typename SpecificOptions>
template <typename CharT>
AppOptions<SpecificOptions>::AppOptions(int argc, const char* const argv[], char const* optTitle,
    std::basic_ostream<CharT>& os, bool wantConvert) :
    shownHelp{false}, desc{optTitle}
{
    assert(optTitle != nullptr);

    so.DefineOptions(desc);
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);

    // Idea from:
    // http://stackoverflow.com/questions/5395503/required-and-optional-arguments-using-boost-library-program-options
    // We check if the user requested the "help message" and, if so, show it
    // and don't process/validate the options. No need to show an error message
    // for required options when all the user wants is
    // to see the "help message".
    if (GotRequestForHelp(vm, so))
    {
        shownHelp = true;
        ShowHelp(os, wantConvert);
    }
    else
    {
        try
        {
            // notify() will perform the basic option validation - required
            // options missing, incompatible types, etc.
            po::notify(vm);

            // On Validate(), SpecificOptions performs its own specific
            // validations.
            // I considered custom validators, since it could potentially
            // provide a cleaner design. I've decided against it because:
            // 1. The docs mention it as a way to costumize conversion.
            // 2. All the examples I found implied building a new instance of
            //    a specific class for a single option. I didn't feel the need
            //    to create a class for each option just because of this.
            // 3. It makes the job of validating related options harder.
            //    Actually, boost's example that shows this (real.cpp) also
            //    puts this validation outside of the notify() call.
            so.Validate();
        }
        catch (po::required_option& ro)
        {
            BOOST_THROW_EXCEPTION(ConfigRequiredOptionMissing() <<
                error_message("Required option missing: " + ro.get_option_name()));

        }
    }
}

// When we're on MS Windows and we're outputting to cout, we
// probably want to convert the output to the console code page.
template <typename SpecificOptions>
template <typename CharT>
void AppOptions<SpecificOptions>::ShowHelp(std::basic_ostream<CharT>& os, bool wantConvert) const
{
    if (wantConvert)
    {
        os << ms_windows::ConvertOutput(desc);
    }
    else
    {
        os << desc;
    }
}


template <typename SpecificOptions, typename CharT>
std::basic_ostream<CharT>&
operator<<(std::basic_ostream<CharT>& os, AppOptions<SpecificOptions> const& obj)
{
    os << obj.so;

    return os;
}

} // namespace config
}}}

#endif // PROG_OPTIONS_H
