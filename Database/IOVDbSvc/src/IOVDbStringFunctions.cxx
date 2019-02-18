/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#include "IOVDbStringFunctions.h"
#include <regex>
#include <iostream>
#include <algorithm>

namespace IOVDbNamespace{
  std::string 
  spaceStrip(const std::string& input){
    // return the input string stripped of leading/trailing spaces
    std::string::size_type idx1=input.find_first_not_of(" ");
    std::string::size_type idx2=input.find_last_not_of(" ");
    if (idx1==std::string::npos || idx2==std::string::npos) {
      return "";
    } else {
      return input.substr(idx1,1+idx2-idx1);
    }
  }
  
  int 
  makeChannel(const std::string& strval, const int defchan) {
    // construct a cool channelId from the string value (numeric)
    // if empty, use the default value
    if (strval!="") return std::stoi(strval);
    return defchan;
  }
  
  unsigned long long
  iovFromTimeString(const std::string & iovString){
    unsigned long long time=std::stoi(iovString);
    return time*1000000000LL;
  }
  
  unsigned long long
  iovFromRunString(const std::string & runString){
    unsigned long long run=std::stoi(runString);
    return run<<32LL;
  }
  
  unsigned long long
  iovFromLumiBlockString(const std::string & lbString){
    unsigned long long lb=std::stoll(lbString.c_str());
    return lb;
  }
  
  int 
  parseClid(const std::string & addrHeaderStr){
    //string of form
    //<addrHeader><address_header service_type="256" clid="12345" /></addrHeader>
    std::string regex=R"delim(clid\s*=\s*"([0-9]*)")delim";
    std::regex re(regex);
    std::smatch clidMatch;
    bool match=std::regex_search(addrHeaderStr, clidMatch,re);
    return (match) ? std::stoi(clidMatch[1]) : -1;
  }
  
  std::string 
  parseTypename(const std::string & description){
    std::string regex=R"delim(<typeName>\s*([^\s]+)\s*</typeName>)delim";
    std::regex re(regex);
    std::smatch typeMatch;
    bool match=std::regex_search(description, typeMatch,re);
    return (match) ? std::string(typeMatch[1]) : std::string("");
  }
  
  std::string 
  deleteRegex(const std::string & original, const std::string & regex){
    const std::regex delre(regex);
    const std::string result = std::regex_replace(original,delre,"");
    return result;
  }
  
  
  
  std::string
  quote(const std::string & sentence){
    const std::string q("\"");
    return q+sentence+q;
  }
  
  std::string 
  sanitiseFilename(const std::string & fname){
    std::string newName{fname};
    std::replace(newName.begin(), newName.end(), '/', '_');
    return newName;
  }
  
  std::string
  replaceNULL(const std::string & possibleNULL){
    std::string original{possibleNULL};
    const std::string regex=R"delim( NULL)delim";
    const std::regex nullre(regex);
    const std::string result = std::regex_replace(original,nullre," null");
    return result;
  }
  
  std::string
  sanitiseXml(const std::string & pseudoXmlString){
    std::string result;
    unsigned int strSize(pseudoXmlString.size());
    unsigned int bufsize(strSize*1.1);
    result.reserve(bufsize);
    for(size_t pos = 0; pos != strSize; ++pos) {
      switch(pseudoXmlString[pos]) {
        //case '&':  result.append("&amp;");       break;
        case '\"': result.append("\\\"");      break;
        //case '\'': result.append("&apos;");      break;
        //case '<':  result.append("&lt;");        break;
        //case '>':  result.append("&gt;");        break;
        default:   result.append(&pseudoXmlString[pos], 1); break;
      }
    }
    return result;
  }
  
  bool
  tagIsMagic(const std::string & candidateTag){
    const std::string regex=R"delim(TagInfo(Major|Minor)/.*)delim";
    const std::regex magicx(regex);
    return std::regex_match(candidateTag, magicx);
  }
  
  std::vector<std::string>
  parseMagicTag(const std::string & v){
    std::vector<std::string> result;
    std::string regex7=R"delim(TagInfo(Major|Minor)/([^/]*)/?([^/]*)?)delim";
    std::regex matchmagic(regex7);
    std::smatch x;
    bool foundmagic=std::regex_match(v,x,matchmagic);
    if (foundmagic){
      for (const auto & i:x) 
      if (i!="") result.push_back(i);
    }
    return result;
  }
  
  std::vector<std::string>
  parseLinkNames(const std::string linktext){
    std::vector<std::string> v{};
    //regex: 
    //(anything except colon, multiple times) then _possibly_ (two colons and string of anything except colons)
    // anything except colon) then (colon or end-of-line)
    std::string linkRegexStr{"([^:]*(::[^:]*)?)(:|$)"};
    std::regex linkMatchSpec(linkRegexStr); 
    //give a token iterator using the regex and returning the first substring (i.e. the 
    //bit before a single colon or line end, which would be for example "ALink" or "MyContext::AnotherLink" )
    std::sregex_token_iterator pos(linktext.cbegin(), linktext.cend(),linkMatchSpec,{1});
    std::sregex_token_iterator e;
    for (;pos!=e;++pos)  {
      // the resulting text _should not_ contain spaces, but strip it to be sure.
      if (not pos->str().empty()) v.push_back(spaceStrip(pos->str()));                           
    }
    return v;                    
  }
  
  std::pair<std::string, std::string>
  tag2PrefixTarget(const std::vector<std::string> & tagParseResults){
    std::string prefix{}, target{};
    if (tagParseResults.size() == 4){ //4 is the size of result set if there is a prefix
      prefix = tagParseResults[2]; //index of first path
      target = tagParseResults[3]; //index of second path
    } else {
      target = tagParseResults[2];
    }
    return std::make_pair(prefix, target);
  }
  
  bool
  replaceServiceType71(std::string & addrHeader){
    const std::size_t svcType = addrHeader.find("service_type=\"71\"");
    if (svcType != std::string::npos) {
      addrHeader.replace(svcType, 17, "service_type=\"256\"");
      return true;
    }
    return false;
  }

  
}
