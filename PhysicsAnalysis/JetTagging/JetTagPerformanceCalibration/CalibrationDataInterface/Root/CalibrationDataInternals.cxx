#include <boost/algorithm/string.hpp>

#include "CalibrationDataInterface/CalibrationDataInternals.h"

namespace Analysis {
  namespace CalibrationDataInterface {

    // local utility function: split string into a vector of substrings separated by a specified separator
    std::vector<std::string> split(const std::string& str, const char token) {
      std::vector<std::string> result;
      boost::split(result, str, [&token] (char c) { return c == token ;});
      for (std::string& element : result) {
	boost::trim(element);
      }
      return result;
    }

  }
}
