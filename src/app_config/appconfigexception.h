#ifndef APPCONFIGEXCEPTION_H
#define APPCONFIGEXCEPTION_H

#include "base/exception.h"

#include <boost/exception/all.hpp>
#include <string>

namespace pt { namespace pcaetano { namespace bluesy {
namespace config
{

using error_message = base::error_message;

struct ConfigBaseException : virtual base::PCBBaseException { };
struct ConfigPropertyNotFoundError : virtual ConfigBaseException { };
struct ConfigOpenFileError : virtual ConfigBaseException { };
struct ConfigRequiredOptionMissing : virtual ConfigBaseException { };
struct ConfigInvalidOption : virtual ConfigBaseException { };

} // namespace config
}}}

#endif // APPCONFIGEXCEPTION_H
