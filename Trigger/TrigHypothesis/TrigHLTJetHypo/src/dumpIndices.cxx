/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#include "dumpIndices.h"
#include <sstream>
// 

std::string dumpIndices(const std::vector<std::size_t>& v){
  std::stringstream ss;
  ss <<  "[" << v.size()<<"] ";
  for(auto i : v){ss << i << " ";}
  ss <<'\n';
  return ss.str();
}
