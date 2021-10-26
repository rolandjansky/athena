/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
//@file IOVDbStringFunctions.h
//@brief Helper functions for string manipulation and parsing
//@author Shaun Roe
#ifndef IOVDbSvc_IOVDbStringFunctions_h
#define IOVDbSvc_IOVDbStringFunctions_h

#include <string>
#include <vector>
#include <utility>
#include <limits>

namespace Gaudi{
  class StoreGateSvc;
}
class MsgStream;

namespace IOVDbNamespace{

  ///Trim leading and trailing spaces,return a new trimmed string
  std::string 
  spaceStrip( std::string_view input);
  
  ///Produce a channel number from the string; default to 'defchan' if empty
  int 
  makeChannel(const std::string& strval, const int defchan);
  
  ///Parse a channel specification string of the format ":3, 65:67,100:120, 130: " into
  ///a vector of pairs
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

  ///Take a string integer giving a time in seconds and convert it to a ULL in nanoseconds
  unsigned long long
  iovFromTimeString(const std::string & iovString);
  
  ///Take a string run number and convert it to an ULL representing run<<32
  unsigned long long
  iovFromRunString(const std::string & runString);
  
  ///String representation of lumiblock just converted to LL (as in original code) and
  /// returned as a ULL
  unsigned long long
  iovFromLumiBlockString(const std::string & lbString);
  
  /// Extract the Class ID (an integer) from a string of 
  /// form <addrHeader><address_header service_type="256" clid="12345" /></addrHeader>
  int 
  parseClid(const std::string & addrHeaderStr);
  
  ///Extract the typename from a folder description
  std::string 
  parseTypename(const std::string & description);
  
  ///Delete characters of a matching regex from the input string
  std::string 
  deleteRegex(const std::string & original, const std::string & regex);
  
  ///Enclose a string in "
  std::string
  quote(const std::string & sentence);
  
  ///Replace the '/' of a file path with '^'
  std::string 
  sanitiseFilename(const std::string & fname);
  
  ///return valid CREST tag name from folder name
  std::string 
  sanitiseCrestTag(const std::string & fname);
  
  ///replace an uppercase NULL (such as returned by oracle) in string with a lowercase null
  ///(such as used in JSON)
  std::string
  replaceNULL(const std::string & possibleNULL);
  
  ///for use when converting cool folder description JSON
  std::string
  sanitiseXml(const std::string & pseudoXmlString);
  
  ///Sanitise json string, escaping raw carriage returns
  std::string
  sanitiseJsonString(const std::string & dataString);
  
  ///Looks like it should be magic
  bool
  looksLikeMagicTag(const std::string & candidateTag);
  
  ///Resolve magic tag
  bool
  tagIsMagic(const std::string & candidateTag);
  
  ///Takes a tag of form TagInfo{Major|Minor}/<tag> or TagInfo{Major|Minor}/<prefix>/<tag>
  ///and resolve it to a string vector{"Major|Minor", <prefix>, <tag>}
  ///returns an empty vector if a match is not found
  std::vector<std::string>
  parseMagicTag(const std::string & candidateTag);
  
  ///Takes a vector<string> containing {"<fulltag>", "Major|Minor", "<prefix>", "<tag>"}
  ///and returns a pair of {"<prefix>", "<tag>"}. Note "<prefix>" is optional in the input
  ///and if missing will be an empty string on output.
  std::pair<std::string, std::string>
  tag2PrefixTarget(const std::vector<std::string>& parseResult);

  bool
  replaceServiceType71(std::string & addrHeader);
  
  std::string
  unescapeQuotes(const std::string & original);
  //! Parse string of format "A:X::B:C" to "A" , "X::B", "C"
  std::vector<std::string>
  parseLinkNames(const std::string &linktext);
  
  
}
  
#endif
