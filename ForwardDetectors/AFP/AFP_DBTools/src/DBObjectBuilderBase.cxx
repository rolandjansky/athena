/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   DBObjectBuilderBase.cxx
 * @author Grzegorz Gach <grzegorz.gach@cern.ch>
 * @date   2018-01-19
 * 
 * @brief  Implementation file for DBObjectBuilderBase
 * 
 */

#include "../AFP_DBTools/DBObjectBuilderBase.h"
#include <sstream>
#include <list>

namespace AFP
{

  std::string DBObjectBuilderBase::setSpecification (const std::string specification) 
  {
    parsePayloadSpec(specification, m_specification);
    return generateBuilderCommands();
  }

  void DBObjectBuilderBase::trimSpaces (std::vector<std::string>& toTrim) const
  {
    for (std::string& element : toTrim)
      trimSpaces (element);
  }

  void DBObjectBuilderBase::trimSpaces (std::string& toTrim) const
  {
    const size_t first = toTrim.find_first_not_of(' ');
    if (std::string::npos == first) {
      toTrim = "";
    }
    else {
      const size_t last = toTrim.find_last_not_of(' ');
      toTrim = toTrim.substr(first, (last - first + 1));
    }
  }
  
  void DBObjectBuilderBase::parsePayloadSpec (const std::string& input, std::vector< std::vector<std::string> >& output) const
  {
    using namespace std;

    // split string into element strings (i.e. text with pairs name:type)
    stringstream inputStream (input);
    
    const char elementsSeparator = ',';
    std::list<string> inputElements;

    string token;
    while ( getline(inputStream, token, elementsSeparator) ) 
      inputElements.push_back(token);

    // save final output as a vector and parse element strings
    const char typeSeparator = ':';
    output.resize(inputElements.size());
    unsigned int index = 0;
    for (const string& element : inputElements) {
      stringstream inputElementStream (element);
      // element strings should be pairs of strings separated with a colon
      output[index].resize(2);
      // read name of the variable
      getline(inputElementStream, token, typeSeparator); 
      output[index][0] = token;
      // read type of the variable
      getline(inputElementStream, token, typeSeparator); 
      output[index][1] = token; // trim spaces and assign
      // trim spaces and assign
      trimSpaces(output[index]);

      ++index;
    }
  }

} // AFP namespace
