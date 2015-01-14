/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigNavStructure/TypedHolder.h"
#include <boost/regex.hpp>

std::string HLTNavDetails::formatSGkey(const std::string& prefix, const std::string& containername, const std::string& label){
  // format is prefix_containername_label
  // where containername is versionless and namespace delimiters
  // are replaced by underscores

  std::string cleaned = containername;
  boost::regex rx1("_v[0-9]+$");
  cleaned = boost::regex_replace(cleaned,rx1,std::string(""));
  boost::regex rx2("::");
  cleaned = boost::regex_replace(cleaned,rx2,std::string("__"));
  return prefix+"_"+cleaned+(label.size() ? "_"+label : "");
}
