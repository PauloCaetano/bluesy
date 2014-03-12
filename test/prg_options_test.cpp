#include <boost/test/unit_test.hpp>

#include "app_config/prog_options.h"
using pt::pcaetano::bluesy::config::AppOptions;
#include "app_config/appconfigexception.h"
using pt::pcaetano::bluesy::config::config_error_string;
using pt::pcaetano::bluesy::config::ConfigInvalidOption;
using pt::pcaetano::bluesy::config::ConfigRequiredOptionMissing;

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include <iostream>
using std::cout;
using std::endl;
#include <ostream>
using std::ostream;
#include <sstream>
using std::stringstream;
#include <string>
using std::string;
#include <vector>
using std::vector;

char const* PO_TEST_STR_IGNORE = "IGNORE";
char const* PO_TEST_OPTION_HEADER = "Test App Options";
char const* PO_TEST_FILENAME = "file.ext";

class TestAppOptions
{
public:
    void DefineOptions(po::options_description& desc);
    std::string GetHelpOption() const { return "help"; }
    void Validate();

    std::string GetFileName() const { return fileName; }
    std::string GetOperation() const { return operation; }
    std::vector<std::string> GetList() const { return list; }
    bool WantsValidFieldNr() const { return validateFieldNr; }
private:
    friend std::ostream& operator<<(std::ostream& os, TestAppOptions const& obj);

    std::string fileName;
    bool validateFieldNr = false;
    std::string operation;
    std::vector<string> list;
};

void TestAppOptions::DefineOptions(po::options_description& desc)
{
    // bool_switch() provides us with a way to set a value depending on whether
    desc.add_options()
        ("help,h", "Mensagem de ajuda")
        ("fich,f", po::value<string>(&fileName)->required(), "Ficheiro a processar")
        ("validnc,c", po::bool_switch(&validateFieldNr)->default_value(false),
            "Valida nr. de campos por linha e termina")
        ("oper,o", po::value<string>(&operation)->required(), "Operação. C - Oper C; P - Oper P")
        ("list,l", po::value<vector<string>>(&list)->multitoken(), "Lista")
    ;
}

void TestAppOptions::Validate()
{
    if ((operation == "C") || (operation == "P"))
    {
        return;
    }

    BOOST_THROW_EXCEPTION(ConfigInvalidOption() << config_error_string("Operacao nao reconhecida: " + operation));
}

ostream& operator<<(ostream& os, TestAppOptions const& obj)
{
    os << "Ficheiro: " << obj.fileName;

    return os;
}



BOOST_AUTO_TEST_SUITE(prg_options)

//BOOST_AUTO_TEST_CASE(po_control)
//{
//    BOOST_CHECK( 2+2 == 4 );
//}

// Expects console code page 850
BOOST_AUTO_TEST_CASE(po_help_convert)
{
    char const* args[] = { "", "-h" };
    int cnt = sizeof(args) / sizeof(*args);
    string expected{PO_TEST_OPTION_HEADER};
    expected += ":\n  -h [ --help ]         Mensagem de ajuda\n"
        "  -f [ --fich ] arg     Ficheiro a processar\n"
        "  -c [ --validnc ]      Valida nr. de campos por linha e termina\n"
        "  -o [ --oper ] arg     Opera\x87\xC6o. C - Oper C; P - Oper P\n"
        "  -l [ --list ] arg     Lista\n";
    stringstream out;

    AppOptions<TestAppOptions> ao(cnt, args, PO_TEST_OPTION_HEADER, out);

    BOOST_REQUIRE_EQUAL(out.str(), expected);
    BOOST_REQUIRE_EQUAL(ao.HaveShownHelp(), true);
}

BOOST_AUTO_TEST_CASE(po_help_no_convert)
{
    char const* args[] = { "", "-h" };
    int cnt = sizeof(args) / sizeof(*args);
    string expected{PO_TEST_OPTION_HEADER};
    expected += ":\n  -h [ --help ]         Mensagem de ajuda\n"
        "  -f [ --fich ] arg     Ficheiro a processar\n"
        "  -c [ --validnc ]      Valida nr. de campos por linha e termina\n"
        "  -o [ --oper ] arg     Operação. C - Oper C; P - Oper P\n"
        "  -l [ --list ] arg     Lista\n";
    stringstream out;

    AppOptions<TestAppOptions> ao(cnt, args, PO_TEST_OPTION_HEADER, out, false);

    BOOST_REQUIRE_EQUAL(out.str(), expected);
    BOOST_REQUIRE_EQUAL(ao.HaveShownHelp(), true);
}

BOOST_AUTO_TEST_CASE(po_required_option_missing)
{
    char const* args[] = { "", "x" };
    int cnt = sizeof(args) / sizeof(*args);

    BOOST_REQUIRE_THROW(AppOptions<TestAppOptions> ao(cnt, args, PO_TEST_OPTION_HEADER),
        ConfigRequiredOptionMissing);
}

// The option can be one of a list of values, but the supplied argument
// is not an element of that list.
BOOST_AUTO_TEST_CASE(po_invalid_option_value)
{
    char const* args[] = { "", "-o", "X", "-f", PO_TEST_STR_IGNORE };
    int cnt = sizeof(args) / sizeof(*args);

    BOOST_REQUIRE_THROW(AppOptions<TestAppOptions> ao(cnt, args, PO_TEST_OPTION_HEADER), ConfigInvalidOption);
}

BOOST_AUTO_TEST_CASE(po_filename)
{
    char const* args[] = { "", "-f", PO_TEST_FILENAME, "-o", "C" };
    int cnt = sizeof(args) / sizeof(*args);
    AppOptions<TestAppOptions> ao(cnt, args, PO_TEST_OPTION_HEADER);
    string expected = PO_TEST_FILENAME;

    BOOST_REQUIRE_EQUAL(ao.GetOptions().GetFileName(), expected);
}

// Checks for the presence of an option that takes no value
BOOST_AUTO_TEST_CASE(po_validate_fn)
{
    char const* args[] = { "", "-f", PO_TEST_STR_IGNORE, "-o", "C", "-c" };
    int cnt = sizeof(args) / sizeof(*args);
    AppOptions<TestAppOptions> ao(cnt, args, PO_TEST_OPTION_HEADER);
    bool expected = true;

    BOOST_REQUIRE_EQUAL(ao.GetOptions().WantsValidFieldNr(), expected);
}

// Checks for the absence of an option that takes no value
BOOST_AUTO_TEST_CASE(po_no_validate_fn)
{
    char const* args[] = { "", "-f", PO_TEST_STR_IGNORE, "-o", "C" };
    int cnt = sizeof(args) / sizeof(*args);
    AppOptions<TestAppOptions> ao(cnt, args, PO_TEST_OPTION_HEADER);
    bool expected = false;

    BOOST_REQUIRE_EQUAL(ao.GetOptions().WantsValidFieldNr(), expected);
}

BOOST_AUTO_TEST_CASE(po_list)
{
    char const* args[] = { "", "-f", PO_TEST_STR_IGNORE, "-o", "C", "-l", "a", "bc", "def" };
    int cnt = sizeof(args) / sizeof(*args);
    AppOptions<TestAppOptions> ao(cnt, args, PO_TEST_OPTION_HEADER);
    vector<string> actual = ao.GetOptions().GetList();
    vector<string> expected{ "a", "bc", "def" };

    BOOST_REQUIRE_EQUAL_COLLECTIONS(actual.cbegin(), actual.cend(), expected.cbegin(), expected.cend());
}

BOOST_AUTO_TEST_SUITE_END()
