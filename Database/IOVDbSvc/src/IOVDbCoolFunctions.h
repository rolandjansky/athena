/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//@file IOVDbCoolFunctions.h
//@brief Numeric and COOL/Coral dependent helper functions
//@author Shaun Roe

#include <utility>
#include <vector>
#include <algorithm>

namespace IOVDbNamespace{

  //!Function to check whether a number is in the inclusive range, given as a pair.
  //!Typically used with Channel ranges for COOL
  template <class NumericType>
  bool
  inRange(const NumericType & val, const std::pair<NumericType, NumericType> & range){
    return (val>=range.first && val<=range.second);
  }
  
  template <class NumericType>
  bool
  inRange(const NumericType & val, const std::vector<std::pair<NumericType, NumericType> > & ranges){
    auto valueInRange=[val](const std::pair<NumericType, NumericType> & range){ return inRange(val,range); };
    return std::any_of(ranges.begin(), ranges.end(), valueInRange);
  }
  

}