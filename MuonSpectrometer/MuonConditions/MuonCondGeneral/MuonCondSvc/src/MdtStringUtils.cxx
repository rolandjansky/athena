/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <string>
#include <vector>
#include "MuonCondSvc/MdtStringUtils.h"
#include <charconv>

namespace MuonCalib {

void  MdtStringUtils::tokenize(const std::string& str,
                      std::vector<std::string>& tokens,
                      const std::string& delimiters)
{
    // Skip delimiters at beginning.
    std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    std::string::size_type pos     = str.find_first_of(delimiters, lastPos);

    while (std::string::npos != pos || std::string::npos != lastPos)
    {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
	//	std::cout << "Added token : " << tokens[tokens.size()-1] << "!" << std::endl;
    }
}

void  MdtStringUtils::tokenize(const std::string& str,
                      std::vector<std::string>& tokens,
                      char delimiter)
{
    // Skip delimiters at beginning.
    std::string::size_type lastPos = str.find_first_not_of(delimiter, 0);
    // Find first "non-delimiter".
    std::string::size_type pos     = str.find_first_of(delimiter, lastPos);

    while (std::string::npos != pos || std::string::npos != lastPos)
    {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiter, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiter, lastPos);
    //  std::cout << "Added token : " << tokens[tokens.size()-1] << "!" << std::endl;
    }
}

std::vector<std::string_view>  MdtStringUtils::tokenize(std::string_view str,
                      char delimiter)
{
    // Skip delimiters at beginning.
    std::string::size_type lastPos = str.find_first_not_of(delimiter, 0);
    // Find first "non-delimiter".
    std::string::size_type pos     = str.find_first_of(delimiter, lastPos);
    std::vector<std::string_view> tokens;
    while (std::string::npos != pos || std::string::npos != lastPos)
    {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiter, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiter, lastPos);
    //  std::cout << "Added token : " << tokens[tokens.size()-1] << "!" << std::endl;
    }
    return tokens;
}

int MdtStringUtils::atoi(std::string_view str){
   int result=-9999;
   std::from_chars(str.data(), str.data() + str.size(), result);
   return result;
}

float MdtStringUtils::stof(std::string_view str){
    //TODO: Replace this with from_chars once the compilers support it!
   return std::stof(std::string(str));
}
}
