// Run with "--log_level=message"
#include <boost/test/unit_test.hpp>

#include "utils/exception.h"
using pt::pcaetano::bluesy::utils::FileOpenException;
#include "utils/file_line_reader.h"
using pt::pcaetano::bluesy::utils::FileLineReader;

#include <array>
#include <fstream>
#include <string>

std::string const kMissingFileName{"missing.flr"};
std::string const kEmptyFileName{"flr_empty_file.flr"};
std::string const kFileName{"flr_test_file.flr"};

std::array<std::string, 10> const lines =
{{
    "[2014-01-01 00:00:00.000] match-1 This is line 0",
    "[2014-01-01 00:00:00.100] match-1 This is line 1",
    "[2014-01-01 00:00:00.200] match-1 This is line 2",
    "[2014-01-01 00:00:00.300] match-2 This is line 3",
    "[2014-01-01 00:00:00.400] match-2 This is line 4",
    "[2014-01-01 00:00:00.500] match-3 This is line 5",
    "[2014-01-01 00:00:00.600] match-4 This is line 6",
    "[2014-01-01 00:00:00.700] match-5 This is line 7",
    "[2014-01-01 00:00:00.800] match-6 This is line 8",
    "[2014-01-01 00:00:00.900] match-7 This is line 9",
}};

// Creates two files for testing - an empty file, and a file with the content of the lines array<>
struct FileFixture
{
    FileFixture()
    {
        // On GCC there's no need to specify the mode, since this is already the default.
        // However, on MSVC (and on cppreference.com), the default mode is supposed to be
        // std::ios_base::out. So, we specify what we want, to avoid future surprises.
        std::ofstream ef{kEmptyFileName, std::ios_base::out | std::ios_base::trunc};
        ef.close();

        std::ofstream of{kFileName, std::ios_base::out | std::ios_base::trunc};

        for (auto const& l : lines)
        {
            of << l << '\n';
        }
    }

    ~FileFixture() {}
};

// http://stackoverflow.com/questions/15918255/is-it-possible-to-initialize-the-fixture-only-once-and-use-it-in-multiple-test-c?rq=1
BOOST_GLOBAL_FIXTURE(FileFixture)

BOOST_AUTO_TEST_SUITE(file_line_reader)

// BOOST_AUTO_TEST_CASE(flr_control)
//{
//    BOOST_CHECK( 2+2 == 4 );
//}

BOOST_AUTO_TEST_CASE(flr_file_missing)
{
    BOOST_REQUIRE_THROW(FileLineReader<> flr{kMissingFileName}, FileOpenException);
}

BOOST_AUTO_TEST_CASE(flr_file_missing_explicit_open)
{
    FileLineReader<> flr;

    BOOST_REQUIRE_THROW(flr.Open(kMissingFileName), FileOpenException);
}

BOOST_AUTO_TEST_CASE(empty_file)
{
    FileLineReader<> flr{kEmptyFileName};
    BOOST_REQUIRE_MESSAGE(!flr.ReadLine(), "Success reading empty file");

    BOOST_REQUIRE(!flr.WasReadOK());
}

BOOST_AUTO_TEST_CASE(open_file_explicit)
{
    FileLineReader<> flr;
    flr.Open(kFileName);

    // Not the most canonical way to check if a file is opened, granted.
    BOOST_REQUIRE_EQUAL(flr.GetFileName(), kFileName);
}

BOOST_AUTO_TEST_CASE(read_one_line_read_ok)
{
    FileLineReader<> flr{kFileName};
    BOOST_REQUIRE_MESSAGE(flr.ReadLine(), "Error reading line");

    BOOST_REQUIRE(flr.WasReadOK());
}

// This test applies only to LineCounter implementations that actually count lines.
BOOST_AUTO_TEST_CASE(read_one_line_count)
{
    FileLineReader<> flr{kFileName};
    BOOST_REQUIRE_MESSAGE(flr.ReadLine(), "Error reading line");

    BOOST_WARN_EQUAL(flr.GetLineCount(), 1);
}


BOOST_AUTO_TEST_CASE(read_one_line_equality)
{
    FileLineReader<> flr{kFileName};
    BOOST_REQUIRE_MESSAGE(flr.ReadLine(), "Error reading line");

    BOOST_REQUIRE_EQUAL(flr.GetCurrentLine(), lines[0]);
    BOOST_REQUIRE_EQUAL(flr.CopyCurrentLine(), lines[0]);
}

BOOST_AUTO_TEST_CASE(line_matches)
{
    FileLineReader<> flr{kFileName};
    BOOST_REQUIRE_MESSAGE(flr.ReadLine(), "Error reading line");

    BOOST_REQUIRE(flr.LineMatches("This is line 0"));
}

BOOST_AUTO_TEST_CASE(skip_one_no_matching_line)
{
    FileLineReader<> flr{kFileName};
    flr.SkipMatchingLine("match-10");

    BOOST_REQUIRE_EQUAL(flr.GetCurrentLine(), lines[0]);
    BOOST_REQUIRE_EQUAL(flr.CopyCurrentLine(), lines[0]);
}

BOOST_AUTO_TEST_CASE(skip_one_matching_line)
{
    FileLineReader<> flr{kFileName};
    flr.SkipMatchingLine("match-1");

    BOOST_REQUIRE_EQUAL(flr.GetCurrentLine(), lines[1]);
    BOOST_REQUIRE_EQUAL(flr.CopyCurrentLine(), lines[1]);
}

BOOST_AUTO_TEST_CASE(skip_many_no_matching_line)
{
    FileLineReader<> flr{kFileName};
    flr.SkipMatchingLines("match-10");

    BOOST_REQUIRE_EQUAL(flr.GetCurrentLine(), lines[0]);
    BOOST_REQUIRE_EQUAL(flr.CopyCurrentLine(), lines[0]);
}

BOOST_AUTO_TEST_CASE(skip_many_matching_lines)
{
    FileLineReader<> flr{kFileName};
    flr.SkipMatchingLines("match-1");

    BOOST_REQUIRE_EQUAL(flr.GetCurrentLine(), lines[3]);
    BOOST_REQUIRE_EQUAL(flr.CopyCurrentLine(), lines[3]);
}

BOOST_AUTO_TEST_CASE(skip_until_match_no_matching_line)
{
    FileLineReader<> flr{kFileName};
    flr.SkipLinesUntilMatch("match-10");

    BOOST_REQUIRE(!flr.WasReadOK());
}

BOOST_AUTO_TEST_CASE(skip_lines_until_match)
{
    FileLineReader<> flr{kFileName};
    flr.SkipLinesUntilMatch("match-5");

    BOOST_REQUIRE_EQUAL(flr.GetCurrentLine(), lines[7]);
    BOOST_REQUIRE_EQUAL(flr.CopyCurrentLine(), lines[7]);
}

BOOST_AUTO_TEST_CASE(skip_number_lines_beyond_eof)
{
    FileLineReader<> flr{kFileName};
    flr.SkipNumberLines(1000);

    BOOST_REQUIRE(!flr.WasReadOK());
}

BOOST_AUTO_TEST_CASE(skip_number_lines)
{
    FileLineReader<> flr{kFileName};
    flr.SkipNumberLines(8);

    BOOST_REQUIRE_EQUAL(flr.GetCurrentLine(), lines[7]);
    BOOST_REQUIRE_EQUAL(flr.CopyCurrentLine(), lines[7]);
}

BOOST_AUTO_TEST_SUITE_END()
