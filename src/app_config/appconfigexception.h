#ifndef APPCONFIGEXCEPTION_H
#define APPCONFIGEXCEPTION_H

#include <boost/exception/all.hpp>
#include <string>

namespace pt { namespace pcaetano { namespace bluesy {
namespace config
{

using config_error_id = boost::error_info<struct tag_config_error_id, int>;
using config_error_string = boost::error_info<struct tag_config_error_string, std::string>;

struct ConfigBaseException : virtual std::exception, virtual boost::exception { };
struct ConfigPropertyNotFoundError : virtual ConfigBaseException { };
struct ConfigOpenFileError : virtual ConfigBaseException { };
struct ConfigRequiredOptionMissing : virtual ConfigBaseException { };
struct ConfigInvalidOption : virtual ConfigBaseException { };

} // namespace config
}}}

#endif // APPCONFIGEXCEPTION_H
