/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */
 
#include "TopConfiguration/TreeFilter.h"
#include "TopConfiguration/Tokenize.h"

#include <iostream>
#include <regex>
#include <sstream>

namespace top {
  
  TreeFilter::TreeFilter(){}
  
  TreeFilter::TreeFilter(const std::string& longstring){
    init(longstring);
  }
  
  void TreeFilter::init(const std::string& longstring){
    std::vector<std::string> helpvec_str;
    tokenize(longstring, helpvec_str, ",");
    
    m_vecFilters.clear();
    for (const std::string& x : helpvec_str) {
      std::istringstream isstr(x);
      std::copy(std::istream_iterator<std::string>(isstr),
                std::istream_iterator<std::string>(), std::back_inserter(m_vecFilters));
    }
    
    for (std::string& filter : m_vecFilters) {
      // replace "*" with ".*"
      filter = regex_replace(filter, std::regex("\\*"), ".*");
    }
    
    
  }
  
  bool TreeFilter::filterTree(const std::string& treename) const{
    
    for (const std::string& filter : m_vecFilters) {
      // check for a match
      std::smatch match;
      std::regex_search(treename, match, std::regex("^"+filter));
      if (!match.empty()) return false;
    }
    
    
    return true;
  }
  
  
}
