/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "./svec2dvec.h"
#include "./ArgStrToDouble.h"
#include <algorithm>

std::vector<double>  svec2dvec(const std::vector<std::string>& svec){
  
  std::vector<double> result;
  result.resize(svec.size());
  
  std::transform(svec.cbegin(), svec.cend(), result.begin(), ArgStrToDouble());
  return result;
}
