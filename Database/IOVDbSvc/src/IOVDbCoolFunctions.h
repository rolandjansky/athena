/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//@file IOVDbCoolFunctions.h
//@brief Numeric and COOL/Coral dependent helper functions
//@author Shaun Roe
#ifndef IOVDbSvc_IOVDbCoolFunctions_h
#define IOVDbSvc_IOVDbCoolFunctions_h
#include "CoolKernel/ChannelId.h"
#include "CoolKernel/ValidityKey.h"

#include "AthenaKernel/IOVTime.h"
//
#include <utility>
#include <vector>
#include <algorithm>
#include <functional>
#include <string>


class IOVRange;
class IOVDbConn;

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
  ///All the lumi blocks in one run
  constexpr unsigned long long ALL_LUMI_BLOCKS{0xFFFFFFFF};

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
  
  ///return the AttributeListSpecification of an AttributeList
  const coral::AttributeListSpecification &
  attrList2Spec(const coral::AttributeList& atrlist);
  
  ///return the size (in bytes) of an Attribute
  unsigned int
  attributeSize(const coral::Attribute & attribute);
  
  ///Return a bool indicating whether the size of a given Attribute can be determined
  bool
  typeSizeIsKnown(const coral::Attribute & attribute);
  
  ///return the size (in bytes) of an AttributeList
  unsigned int
  attributeListSize(const coral::AttributeList& atrlist);
  
  ///Count the number of selected channels in a vector of channels according to cool::Channel selection
  int
  countSelectedChannels(const std::vector<cool::ChannelId> & channels, const cool::ChannelSelection & selected);
  
  ///Create an IOVTime in ns of epoch or run-lumi (determined by the bool) from a ValidityKey
  IOVTime
  makeEpochOrRunLumi(const cool::ValidityKey key, const bool timeIsEpoch);
  
  ///Create a long long time in ns from s
  unsigned long long 
  iovTimeFromSeconds(const unsigned long long seconds);
  
  ///Create a long long representing the IOV from run, lumi
  unsigned long long
  iovTimeFromRunLumi(const unsigned long long run, const unsigned long long lumi=0);
  
  ///Return a [run,lumi] pair from an IOV time
  std::pair<unsigned long long, unsigned long long>
  runLumiFromIovTime(const unsigned long long iovTime);
  
  ///Make an IOVRange from two validity keys
  IOVRange 
  makeRange(const cool::ValidityKey since,const cool::ValidityKey until,const bool timeIsEpoch);
  
  ///Create a ChannelId from a string; if string is empty, return the default channel number given
  cool::ChannelId 
  makeChannel(const std::string& strval, const cool::ChannelId defchan);
  
  ///Retrieve (bool) multiversion flag and folder description  
  std::pair<bool, std::string>
  folderMetadata(IOVDbConn * pConnection, const std::string & folderName);
  
  ///Retrieve channel information
  std::pair<std::vector<cool::ChannelId>, std::vector<std::string>>
  channelList(IOVDbConn * pConnection, const std::string & folderName, const bool named=false);

}

#endif