/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// IOVDbParser.cxx
// implementation for simple XML parser for IOVDbSvc

#include "GaudiKernel/MsgStream.h"
#include "IOVDbParser.h"
#include "IOVDbStringFunctions.h"

IOVDbParser::IOVDbParser(std::string_view input, MsgStream& log) :
  m_msg(log),
  m_valid(true)
{
  // parse the input string as XML, decode into Key/Value pairs
  m_keys.clear();
  // slot for the data outside of XML
  m_keys[""]="";
  // work through the string to build list of tags
  std::string::size_type iofs=0;
  std::string::size_type len=input.size();
  while (iofs!=std::string::npos && iofs<len) {
    // look for the start of the next XML tag
    std::string::size_type iofs1=input.find('<',iofs);
    if (iofs1>iofs && iofs1!=std::string::npos) {
      // take any unmarked-up text into the 'outside' data slot
      m_keys[""]+=IOVDbNamespace::spaceStrip(input.substr(iofs,iofs1-iofs));
    }
    if (iofs1!=std::string::npos) {
      // have an opening XML tag - process it
      // first find the end of the tag, either '>' or ' ', whichever first
      std::string::size_type iofs2=input.find('>',iofs1);
      std::string::size_type iofs3=input.find("/>",iofs1);
      bool noClosingTag = (iofs2 == std::string::npos);
      if (noClosingTag){
        m_msg << MSG::FATAL << 
            "Badly formed XML string, no closing tag in " << input << endmsg;
        m_valid=false;
        iofs=std::string::npos;
        return;
      }
      if (iofs2!=std::string::npos && iofs2<iofs3) {
        // found a closing >, so tag is standard <tag>value</tag> form
        std::string tag=IOVDbNamespace::spaceStrip(input.substr(iofs1+1,iofs2-iofs1-1));
        // now need to find the closing </tag>
        std::string::size_type iofs4=input.find("</"+tag,iofs2+1);
        if (iofs4!=std::string::npos) {
          // found closing tag, store tag and text
          m_keys[tag]=IOVDbNamespace::spaceStrip(input.substr(iofs2+1,iofs4-iofs2-1));
          // advance to the next part of the string, after '>' on closing tag
          iofs=input.find('>',iofs4);
          if (iofs == std::string::npos) {
            m_msg << MSG::FATAL << 
              "Badly formed XML string, no closing tag in " << input << endmsg;
            m_valid=false;
            iofs=std::string::npos;
            return;
          } else {
            iofs+=1;
          }          
        } else {
          m_msg << MSG::FATAL << 
            "Badly formed XML string, no closing tag in " << input << endmsg;
          m_valid=false;
          iofs=std::string::npos;
          return;
        }
      } else if (iofs3!=std::string::npos) {
        // found a />, so tag is of form <tag values info/>
        // find the end of the tag part to see if a value is present
        std::string::size_type iofs4=input.find(' ',iofs1+1);
        std::string value,tag;
        if (iofs4!=std::string::npos && iofs4<iofs3) {
          value=IOVDbNamespace::spaceStrip(input.substr(iofs4,iofs3-iofs4));
          tag=IOVDbNamespace::spaceStrip(input.substr(iofs1+1,iofs4-iofs1-1));
        } else {
          tag=input.substr(iofs1+1,iofs3-iofs1-1);
          value="";
        }
        m_keys[tag]=IOVDbNamespace::spaceStrip(value);
        // advance to next part of string after closing />
        iofs=iofs3+2;
      } else {
        // found a < but no closing >
        m_msg << MSG::FATAL << "Badly formed XML string, no closing > in input " <<
          input << endmsg;
        iofs=std::string::npos;
        m_valid=false;
        return;
      }
    } else {
      // no more < in input, take the rest into 'outside' data slot
      m_keys[""]+=IOVDbNamespace::spaceStrip(input.substr(iofs));
      iofs=len;
    }
  }
  this->clean(); //rectify obsolete key names
  if (m_msg.level()<=MSG::VERBOSE) {
    m_msg << MSG::VERBOSE << "parseXML processed input string: " << input << endmsg;
    for (KeyValMap::const_iterator itr=m_keys.begin();itr!=m_keys.end();++itr) {
      m_msg << MSG::VERBOSE << "Key: " << itr->first << " value:" << 
      itr->second << endmsg;
    }
  }
}

bool 
IOVDbParser::getKey(const std::string& key, const std::string& defvalue,
                         std::string& value) const {
  // check if key is present in keyval, if so set value to it
  // if not set value to supplied default
  // return true or false depending on whether a matching key was found
  if (!m_valid) {
    value=defvalue;
    return false;
  }
  const auto [theValue,found] = at(key,defvalue);
  value=theValue;
  return found;
}

std::pair<std::string, bool>
IOVDbParser::at(const std::string & searchKey, const std::string &defaultValue) const{
  KeyValMap::const_iterator it = m_keys.find(searchKey);
  if (it != m_keys.end()) {
    //may contain return or newline character
    return std::pair<std::string, bool>(IOVDbNamespace::spaceStrip(it->second), true);
  }
  return std::pair<std::string, bool> (defaultValue, false);
}

std::string 
IOVDbParser::folderName() const {
  return at("").first;
}

std::string
IOVDbParser::key() const {
  return at("key",folderName()).first;
}

bool 
IOVDbParser::hasKey() const{
  return at("key").second;
}

std::string
IOVDbParser::tag() const {
  return at("tag").first;
}

std::string 
IOVDbParser::eventStoreName() const{
  return at("eventStoreName","StoreGateSvc").first;
}

bool 
IOVDbParser::timebaseIs_nsOfEpoch() const{
  return (at("timeStamp").first=="time");
}

std::string
IOVDbParser::cache() const{
  return at("cache").first;
}

int
IOVDbParser::cachehint() const{
  auto valuePair=at("cachehint");
  return valuePair.second ? std::stoi(valuePair.first) : 0;
}

bool 
IOVDbParser::named() const{
  return at("named").second;
}

bool 
IOVDbParser::onlyReadMetadata() const{
  return at("metaOnly").second;
}

bool 
IOVDbParser::extensible() const{
  return at("extensible").second;
}


CLID 
IOVDbParser::classId() const{
  CLID result{};
  auto [addrHeader,foundHeader]=at("addrHeader");
  if (foundHeader) {
    IOVDbNamespace::replaceServiceType71(addrHeader);
    m_msg << MSG::DEBUG <<"Decode addrHeader "<< addrHeader << endmsg;
    IOVDbParser addrH(addrHeader,m_msg);
    if (auto addrPair=addrH.at("address_header");addrPair.second) {
      result = IOVDbNamespace::parseClid(addrPair.first);
      m_msg << MSG::DEBUG << "Got CLID " << result << " from " << addrPair.first << endmsg;
    }
  }
  return result;
}

std::string 
IOVDbParser::addressHeader() const{
  return at("addrHeader").first;
}

std::vector<std::string> 
IOVDbParser::symLinks() const{
  const auto & symLinkString = at("symlinks").first;
  return IOVDbNamespace::parseLinkNames(symLinkString);
}

bool 
IOVDbParser::noTagOverride() const{
  return at("noover").second;
}


void IOVDbParser::clean() {
  auto it=m_keys.find("dbConnection");
  if (it!=m_keys.end()) {
    std::string connection=std::move(it->second);
    m_keys.erase(it);
    m_keys["db"]=std::move(connection);
  }
  return;
}


unsigned 
IOVDbParser::applyOverrides(const IOVDbParser& other, MsgStream & log) {
  unsigned keyCounter=0;
  for (const auto& otherKeyValue : other.m_keys) {
    const std::string& otherKey=otherKeyValue.first;
    const std::string& otherValue=otherKeyValue.second;
    if (!otherKey.size()) continue; //Ignore Foldername
    if (otherKey=="prefix") continue; //Ignore prefix
    KeyValMap::iterator it=m_keys.find(otherKey);
    if (it==m_keys.end()) {
      log << MSG::INFO << "Folder " << m_keys[""] << ", adding new key " << otherKey 
           << " with value " << otherValue << endmsg; 
      m_keys[otherKey]=otherValue;
    }
    else {
      log << MSG::INFO << "Folder " << m_keys[""] << ", Key: " << otherKey 
           <<  "Overriding existing value " << m_keys[otherKey] << " to new value " << otherValue << endmsg;
      it->second=otherValue;
    }
    ++keyCounter;
  } //End loop over keys in other
  return keyCounter;
}
    
bool 
IOVDbParser::operator==(const IOVDbParser& other) const {
  return ((this->m_keys) == other.m_keys);
}

bool 
IOVDbParser::overridesIov() const{
  //no check on folder time unit compatibility
  return overridesIovImpl(false);
}

bool 
IOVDbParser::overridesIov(const bool folderIs_nsOfEpoch) const {
  //check folder time unit compatibility
  return overridesIovImpl(true, folderIs_nsOfEpoch);
}

bool 
IOVDbParser::overridesIovImpl(const bool performFolderCheck,const bool folderIs_nsOfEpoch) const{
  bool overrideIs_nsEpochIov{true};
  const bool overridingTimestamp=(m_keys.find("forceTimestamp")!=m_keys.end());
  const bool overridingRun=(m_keys.find("forceRunNumber")!=m_keys.end());
  const bool overridingLumi=(m_keys.find("forceLumiblockNumber")!=m_keys.end());
  //check for nonsense scenarios:
  //1. overriding Lumi but not the Run number
  if (overridingLumi and not overridingRun){
    m_msg << MSG::WARNING<<"Trying to override lumi block without specifying the run"<<endmsg;
    return false;
  }
  //2. Trying to override both
  if (overridingRun and overridingTimestamp){
    m_msg << MSG::WARNING<<"Trying to override using both run-lumi and ns timestamp"<<endmsg;
    return false;
  }
  // now we are consistent, so set the 'is_ns' variable if it's different from default
  if (overridingRun) overrideIs_nsEpochIov=false;
  //3. Overriding a folder in ns format with a run-lumi IOV, or folder in run-lumi with ns timestamp
  if (performFolderCheck and (overrideIs_nsEpochIov != folderIs_nsOfEpoch)){
    m_msg << MSG::WARNING<<"Trying to override run-lumi for a ns folder, or ns for a run-lumi folder"<<endmsg;
    return false;
  }
  return (overridingTimestamp or overridingRun);
}

unsigned long long 
IOVDbParser::iovOverrideValue() const{
  unsigned long long value{};
  if (not overridesIov()) return value;
  auto pTsPair = m_keys.find("forceTimestamp");
  if (pTsPair!=m_keys.end()){
    value = IOVDbNamespace::iovFromTimeString(pTsPair->second);
  }
  else {
    auto pRunPair = m_keys.find("forceRunNumber");
    auto pLumiPair = m_keys.find("forceLumiblockNumber");
    const auto & runString = (pRunPair!=m_keys.end()) ? pRunPair->second : "";
    const auto & lumiString = (pLumiPair!=m_keys.end()) ? pLumiPair->second : "";
    //could check that values were actually given here
    value = IOVDbNamespace::iovFromRunString(runString) + IOVDbNamespace::iovFromLumiBlockString(lumiString);
  }
  return value;
}

std::string 
IOVDbParser::toString() const {
  std::stringstream retval;
  retval <<"Folder:";
  retval << folderName();
  retval << ", Attributes: ";
  auto it=m_keys.begin();
  auto it_e=m_keys.end();
  for (;it!=it_e;++it) {
    if (it->first.size()==0) continue;
    retval << "[" << it->first << ":" << it->second << "] "; 
  }
  return retval.str();

}
MsgStream& operator<<(MsgStream& os, const IOVDbParser& fldr) {
  os << fldr.toString();
  return os;
} 
