#include <boost/algorithm/string.hpp>

#include "CalibrationDataInterface/CalibrationDataInternals.h"

namespace Analysis {
  namespace CalibrationDataInterface {

    // // local utility function: trim leading and trailing whitespace in the property strings
    // std::string trim(const std::string& str,
    // 		   const std::string& whitespace = " \t") {
    //   const auto strBegin = str.find_first_not_of(whitespace);
    //   if (strBegin == std::string::npos)
    //       return ""; // no content

    //   const auto strEnd = str.find_last_not_of(whitespace);
    //   const auto strRange = strEnd - strBegin + 1;

    //   return str.substr(strBegin, strRange);
    // }

    // // local utility function: split string into a vector of substrings separated by a specified separator
    // std::vector<std::string> split(const std::string& str, char token = ';') {
    //   std::vector<std::string> result;
    //   if (str.size() > 0) {
    //     std::string::size_type end;
    //     std::string tmp(str);
    //     do {
    // 	end = tmp.find(token);
    // 	std::string entry = trim(tmp.substr(0,end));
    // 	if (entry.size() > 0) result.push_back(entry); 
    // 	if (end != std::string::npos) tmp = tmp.substr(end+1);
    //     } while (end != std::string::npos);
    //   }
    //   return result;
    // }

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
