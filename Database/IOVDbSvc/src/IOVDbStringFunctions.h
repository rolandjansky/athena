/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//@file IOVDbStringFunctions.h
//@brief Helper functions for string manipulation and parsing
//@author Shaun Roe
#ifndef IOVDbStringFunctions_H
#define IOVDbStringFunctions_H

#include <string>
#include <vector>
#include <utility>
#include <limits>

namespace IOVDbNamespace{

  ///Trim leading and trailing spaces,return a new trimmed string
  std::string 
  spaceStrip(const std::string& input);
  
  int makeChannel(const std::string& strval, const int defchan);
  
  ///Parse a channel specification string of the format ":3, 65:67,100:120, 130: "
  template <class IntType>
  std::vector<std::pair<IntType, IntType>>
  parseChannelSpec(const std::string & chanspecString){
    typedef std::pair<IntType, IntType> ChanRange;
    const auto defaults=ChanRange(0,std::numeric_limits<IntType>::max());
    //
    std::vector<ChanRange> result;
    std::string::size_type index;
    std::string rangespec, chanspec(chanspecString);
    bool first=true;
    do {
        index=chanspec.find(',');
        if (index!=std::string::npos) {
            rangespec=chanspec.substr(0,index);
            chanspec=chanspec.substr(index+1,chanspec.size());
        } else {
            rangespec=chanspec;
        }
        if (rangespec!="") {
            // parse the range specification, look for : separator between limits
            std::string::size_type icol=rangespec.find(':');
            int lower,upper;
            if (icol!=std::string::npos) {
                lower=makeChannel(spaceStrip(rangespec.substr(0,icol)),defaults.first);
                upper=makeChannel(spaceStrip(rangespec.substr(icol+1,rangespec.size())),defaults.second);
            } else {
                lower=atol(spaceStrip(rangespec).c_str());
                upper=lower;
            }
            if (first) {
                first=false;
            } 
            result.push_back(ChanRange(lower,upper));
        }
    } while (index!=std::string::npos);
    return result;
  }

  unsigned long long
  iovFromTimeString(const std::string & iovString);
  
  unsigned long long
  iovFromRunString(const std::string & runString);
  
  unsigned long long
  iovFromLumiBlockString(const std::string & lbString);
  
  int 
  parseClid(const std::string & addrHeaderStr);
  
  std::string 
  parseTypename(const std::string & description);
  
  std::string 
  deleteRegex(const std::string & original, const std::string & regex);
  
  std::string
  quote(const std::string & sentence);
  
  std::string 
  sanitiseFilename(const std::string & fname);
  
  std::string
  replaceNULL(const std::string & possibleNULL);
  
  std::string
  sanitiseXml(const std::string & pseudoXmlString);
  
  bool
  tagIsMagic(const std::string & candidateTag);
  
  std::vector<std::string>
  parseMagicTag(const std::string & candidateTag);
  
  std::pair<std::string, std::string>
  tag2PrefixTarget(const std::vector<std::string>& parseResult);

  bool
  replaceServiceType71(std::string & addrHeader);

  
  //! Parse string of format "A:X::B:C" to "A" , "X::B", "C"
  std::vector<std::string>
  parseLinkNames(const std::string linktext);
  
  
}
  
#endif
