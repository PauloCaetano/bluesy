// Copyright (c) 2014, Paulo Caetano
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

#ifndef FILE_LINE_READER_H
#define FILE_LINE_READER_H

#include "utils/exception.h"

#include <cassert>
#include <fstream>
#include <stdexcept>
#include <string>

namespace pt { namespace pcaetano { namespace bluesy {
namespace utils
{


// Default policies for FileLineReader

// Line counting policy
template <typename LineCounterType>
class SimpleLineCounter
{
public:
    using CounterType = LineCounterType;
    CounterType GetLineCount() const { return tot_lines_; }
    void Increment() { ++tot_lines_; }
private:
    CounterType tot_lines_{};
};


// Line matching policy
struct SimpleLineMatcher
{
    bool LineMatches(std::string const& line, std::string const& match) const
    {
        return (line.find(match) != std::string::npos);
    }
};


// Alternative to default LineCounter policy, if the client doesn't want line counting.
// Even though GetLineCount() still has a body, only Increment() is called by
// FileLineReader without client intervention; GetLineCount() is only called if the
// the client calls FileLineReader::GetLineCount().
struct NoLineCounter
{
    using CounterType = int;
    constexpr CounterType GetLineCount() const { return 0; }
    void Increment() { }
};



// TODO: Move ctors. We have a problem - std::ifstream doesn't have a move ctor in gcc 4.8.2.
// TODO: ctor accepting an already open ifstream. We'd have to move it, which brings us
//          back to the problem above.
// TODO: We may remove the file_name_ data member, in the future. It's only a convenience,
//          allowing us to just store the filename here and forget about it. But it's
//          probably not the best option.

// Reads a text file, one line at a time. Thread safety: None.
// We refer to the last line read, i.e. the line currently in the read buffer as
// the current line.
//
// Why inheritance, instead of composition? Because there are cases where LineMatcher and
// LineCounter have no state, and a data member is a bit of waste (no, not a huge waste).
// Can this be abused? Probably, but you know - protect against Murphy, not Machavelli.
template <typename LineMatcher = SimpleLineMatcher,
    typename LineCounter = SimpleLineCounter<unsigned long>>
class FileLineReader : private LineMatcher, private LineCounter
{
public:
    // Upon construction, we read no line from the file.
    // TODO: Should we keep the default ctor? One of the invariants should be
    // "stream ready to read", however this ctor does not guarantee it.
    FileLineReader() = default;
    FileLineReader(std::string file_name)
        : in_file_{file_name}, curr_line_{}, file_name_{file_name}
    {
        CheckFileOpen();
    }


    FileLineReader(FileLineReader const&) = delete;
    FileLineReader& operator=(FileLineReader const&) = delete;


    // TODO: Necessary only if we keep the default ctor, otherwise drop it.
    void Open(std::string file_name)
    {
        assert(!in_file_.is_open());

        in_file_.open(file_name);
        CheckFileOpen();
        file_name_ = file_name;
    }


    // Reads the a line from the file, which becomes the current line.
    // Our goal, for now, is to maintain an interface similar to getline().
    bool ReadLine()
    {
        assert(in_file_.is_open());

        bool read_line = getline(in_file_, curr_line_, in_file_.widen('\n'));

        if (read_line)
        {
            LineCounter::Increment();
        }

        return read_line;
    }


    // How many lines have we read so far?
    typename LineCounter::CounterType GetLineCount() const
    { return LineCounter::GetLineCount(); }


    // When we use ReadLine(), we have an easy way to know if we've
    // read the line successfully. However, when we skip lines, we
    // need to know how the reading went.
    bool WasReadOK() const { return in_file_; }


    // Return the current line (const ref or copy)
    std::string const& GetCurrentLine() const { return curr_line_; }
    std::string CopyCurrentLine() const { return curr_line_; }


    // All skipping functions discard the current line, because they all begin
    // by calling ReadLine().
    //
    // The functions that skip until/while they get a match can only stop on the
    // first line that is/isn't a match. So, when these functions return,
    // the current line is the first line the caller wants, and he should process it
    // before performing any other read. IOW, this is probably not a good idea:
    //
    // FileLineReader flr{"file.txt"};
    // flr.SkipLinesUntilmatch("some string");
    // while (flr.ReadLine())
    //     etc...
    //
    // The ReadLine() on the while statement will discard the line read by
    // SkipLinesUntilmatch(), which is probably not what's intended. We
    // recommend this:
    //
    // FileLineReader flr{"file.txt"};
    // flr.SkipLinesUntilmatch("some string");
    // do
    //     etc...
    // until (!flr.ReadLine());
    //
    // NOTE: SkipNumberLines() has different semantics, see below.

    // Read a line from the file. If it matches the match string, skip it,
    // i.e., read another line.
    // Current line is the first line to process, not the last line skipped.
    void SkipMatchingLine(std::string const& match);

    // Keep skipping lines while they match the match string.
    // Current line is the first line to process, not the last line skipped.
    void SkipMatchingLines(std::string const& match);

    // Keep skipping lines until we find a match.
    // Current line is the first line to process, not the last line skipped.
    void SkipLinesUntilMatch(std::string const& match);

    // Skip number_lines lines.
    // Even though this function skips lines, it does so without any matching.
    // Thus, it's doesn't depend on LineMatcher.
    //
    // !!! ATTENTION !!!
    // Unlike the other line-skipping functions, when this function returns, the current line
    // is not the line to process. The caller should perform a read in order to get to its
    // intended line:
    //
    // FileLineReader flr{"file.txt"};
    // flr.SkipNumberLines(8);
    // while (flr.ReadLine())
    //     etc...
    //
    // We considered an implementation similar to the other functions; that would require an
    // extra read - e.g., if the caller skipped 5 lines, we would read a 6th, to position the
    // current line on the first line to be processed (rather than the last line to be skipped).
    // However, we'd still have a problem to solve with SkipNumberLines(0). In order to apply this
    // reasoning here, we'd need to skip 1 line on SkipNumberLines(0), and that makes no sense.
    // We figured it would be better to endure a lack of consistency than to have SkipNumberLines(0)
    // skip 1 line.
    void SkipNumberLines(unsigned int number_lines);


    // Utility functions.
    // LineMatches() works on the current line, i.e., doesn't
    // read a line before comparing.
    using LineMatcher::LineMatches;
    bool LineMatches(std::string const& match) const
    { return LineMatches(curr_line_, match); }

    // Checks if the current line is empty
    bool IsLineEmpty() const { return curr_line_.empty(); }
    std::string GetFileName() const { return file_name_; }
private:
    void CheckFileOpen() const
    {
        if (!in_file_.is_open())
        {
            BOOST_THROW_EXCEPTION(FileOpenException() << error_message("Error opening file " + file_name_));
        }
    }

    std::ifstream in_file_;
    std::string curr_line_;

    std::string file_name_;
};



template <typename LineMatcher, typename LineCounter>
void FileLineReader<LineMatcher, LineCounter>::SkipNumberLines(unsigned int number_lines)
{
    while ((number_lines > 0) && ReadLine())
    {
        --number_lines;
    }
}


template <typename LineMatcher, typename LineCounter>
void FileLineReader<LineMatcher, LineCounter>::SkipMatchingLine(std::string const& match)
{
    if (ReadLine() && (LineMatches(curr_line_, match)))
    {
        ReadLine();
    }
}


template <typename LineMatcher, typename LineCounter>
void FileLineReader<LineMatcher, LineCounter>::SkipMatchingLines(std::string const& match)
{
    while (ReadLine() && LineMatches(curr_line_, match))
    {
        ;
    }
}


template <typename LineMatcher, typename LineCounter>
void FileLineReader<LineMatcher, LineCounter>::SkipLinesUntilMatch(std::string const& match)
{
    while (ReadLine() && !LineMatches(curr_line_, match))
    {
        ;
    }
}

} // namespace utils
}}}

#endif // FILE_LINE_READER_H
