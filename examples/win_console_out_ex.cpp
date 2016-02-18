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

// An example of how we're using the ConvertOutput() function to output_iterator
// non-ASCII characters on a Windows console.

// By including the header like this, you'll be using explicit instantiation.
// You'll need to add win_console_out_inst.cpp to your build, or you won't be
// able to build this example, due to undefined symbols.
// Here's a brief overview of the way the code's organized:
// http://cidebycide.blogspot.pt/2016/02/explicit-template-instantiation-as.html
// http://cidebycide.blogspot.pt/2016/02/explicit-template-instantiation-exhibit.html
//
// If you prefer to avoid explicit instantiation, uncomment this line:
// #define PCBLUESY_MSWINDCON_FULLHEADER
#include "ms_windows/win_console_out.h"
using pt::pcaetano::bluesy::ms_windows::ConvertOutput;

#include <iostream>
using std::cout;
#include <string>
using std::string;
using std::wstring;

int main(int argc, char *argv[])
{
    cout << ConvertOutput("Ol�\n");

    string s{"Isto � um teste\n"};
    cout << ConvertOutput(s);

    wstring ws{L"Um Teste Final�ssimo\n"};
    cout << ConvertOutput(ws);	
}