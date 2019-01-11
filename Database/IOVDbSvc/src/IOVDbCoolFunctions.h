/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//@file IOVDbCoolFunctions.h
//@brief Numeric and COOL/Coral dependent helper functions
//@author Shaun Roe
#ifndef IOVDbCoolFunctions_h
#define IOVDbCoolFunctions_h
#include "CoolKernel/ChannelId.h"
#include "CoolKernel/ValidityKey.h"
#include "AthenaKernel/IOVTime.h"
//
#include <utility>
#include <vector>
#include <algorithm>
#include <functional>


namespace coral{
  class AttributeListSpecification;
  class AttributeSpecification;
  class AttributeList;
  class Attribute;
}
namespace cool{
  class ChannelSelection;
}

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
  
  const coral::AttributeListSpecification &
  attrList2Spec(const coral::AttributeList& atrlist);
  
  unsigned int
  attributeSize(const coral::Attribute & attribute);
  
  bool
  typeSizeIsKnown(const coral::Attribute & attribute);
  
  unsigned int
  attributeListSize(const coral::AttributeList& atrlist);
  
  int
  countSelectedChannels(const std::vector<cool::ChannelId> & channels, const cool::ChannelSelection & selected);
  
  IOVTime
  makeEpochOrRunLumi(const cool::ValidityKey key, const bool timeIsEpoch);

}

#endif