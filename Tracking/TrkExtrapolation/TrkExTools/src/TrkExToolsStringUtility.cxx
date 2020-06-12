/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#include "TrkExToolsStringUtility.h"
#include <regex>
#include <algorithm>
#include <functional> //for reference wrapper
#include <set>


namespace TrkExTools{
  std::string
  getToolSuffix(const std::string& fullToolName){
    return fullToolName.substr(fullToolName.find_last_of('.') + 1);
  }
  
  std::vector<std::string>
  extractToolNames(const std::vector<std::string> & toolNameVector){
    std::vector<std::string> result(toolNameVector.size());
    std::transform(toolNameVector.begin(),toolNameVector.end(),result.begin(), getToolSuffix  );
    return result;
  }

  bool
  validToolName(const std::string & toolName){
    //valid name can contain underscore or alphanumeric characters, but cannot start
    //with a number
    std::regex re{"^[a-zA-Z_][a-zA-Z0-9_]*(::)?[a-zA-Z0-9_]*$"};
    return std::regex_match(toolName, re);
  }
  
  std::string
  possibleToolNameError(const std::vector<std::string> & toolNameVector){
    std::string result{};
    auto isEmpty = [](const std::string & s){return s.empty();};
    if (std::any_of(toolNameVector.begin(), toolNameVector.end(), isEmpty) ) return "A tool name was empty.";
    if (auto pTheTool=std::find_if_not(toolNameVector.begin(), toolNameVector.end(), validToolName); pTheTool != toolNameVector.end()) {
      result = "A tool name was invalid: " + *pTheTool;
    }
    return result;
  }
  
  unsigned int
  numberOfUniqueEntries(const std::vector<std::string> & nameVector){
    std::set<std::string> setOfNames(nameVector.begin(), nameVector.end());
    return setOfNames.size();
  }



}