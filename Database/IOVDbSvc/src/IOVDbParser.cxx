/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IOVDbParser.cxx
// implementation for simple XML parser for IOVDbSvc

#include "GaudiKernel/MsgStream.h"
#include "IOVDbParser.h"

IOVDbParser::IOVDbParser(const std::string& input, MsgStream& log) :
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
    std::string::size_type iofs1=input.find("<",iofs);
    if (iofs1>iofs && iofs1!=std::string::npos) {
      // take any unmarked-up text into the 'outside' data slot
      m_keys[""]+=spaceStrip(input.substr(iofs,iofs1-iofs));
    }
    if (iofs1!=std::string::npos) {
      // have an opening XML tag - process it
      // first find the end of the tag, either '>' or ' ', whichever first
      std::string::size_type iofs2=input.find(">",iofs1);
      std::string::size_type iofs3=input.find("/>",iofs1);
      if (iofs2!=std::string::npos && iofs2<iofs3) {
        // found a closing >, so tag is standard <tag>value</tag> form
        std::string tag=spaceStrip(input.substr(iofs1+1,iofs2-iofs1-1));
        // now need to find the closing </tag>
        std::string::size_type iofs4=input.find("</"+tag,iofs2+1);
        if (iofs4!=std::string::npos) {
          // found closing tag, store tag and text
          m_keys[tag]=spaceStrip(input.substr(iofs2+1,iofs4-iofs2-1));
          // advance to the next part of the string, after '>' on closing tag
          iofs=input.find(">",iofs4)+1;
        } else {
          m_msg << MSG::ERROR << 
            "Badly formed XML string, no closing tag for " << tag <<
            " in " << input << endmsg;
          m_valid=false;
          iofs=std::string::npos;
        }
      } else if (iofs3!=std::string::npos) {
        // found a />, so tag is of form <tag values info/>
        // find the end of the tag part to see if a value is present
        std::string::size_type iofs4=input.find(" ",iofs1+1);
        std::string value,tag;
        if (iofs4!=std::string::npos && iofs4<iofs3) {
          value=spaceStrip(input.substr(iofs4,iofs3-iofs4));
          tag=spaceStrip(input.substr(iofs1+1,iofs4-iofs1-1));
        } else {
          tag=input.substr(iofs1+1,iofs3-iofs1-1);
          value="";
        }
        m_keys[tag]=spaceStrip(value);
        // advance to next part of string after closing />
        iofs=iofs3+2;
      } else {
        // found a < but no closing >
        m_msg << MSG::ERROR << 
          "Badly formed XML string, no closing < in input " <<
          input << endmsg;
        iofs=std::string::npos;
        m_valid=false;
      }
    } else {
      // no more < in input, take the rest into 'outside' data slot
      m_keys[""]+=spaceStrip(input.substr(iofs));
      iofs=len;
    }
  }
  this->clean(); //rectify obsolete key names
  if (m_msg.level()<=MSG::VERBOSE) {
    m_msg << MSG::VERBOSE << 
      "parseXML processed input string: " << input << endmsg;
    for (KeyValMap::const_iterator itr=m_keys.begin();itr!=m_keys.end();++itr) {
      m_msg << MSG::VERBOSE << "Key: " << itr->first << " value:" << 
      itr->second << endmsg;
    }
  }
}

bool IOVDbParser::getKey(const std::string& key, const std::string& defvalue,
                         std::string& value) const {
  // check if key is present in keyval, if so set value to it
  // if not set value to supplied default
  // return true or false depending on whether a matching key was found
  if (!m_valid) {
    value=defvalue;
    return false;
  }
  KeyValMap::const_iterator kitr=m_keys.find(key);
  if (kitr!=m_keys.end()) {
    value=kitr->second;
    return true;
  } else {
    value=defvalue;
    return false;
  }
}

std::string IOVDbParser::spaceStrip(const std::string& input) const {
  // return the input string stripped of leading/trailing spaces
  std::string::size_type idx1=input.find_first_not_of(" ");
  std::string::size_type idx2=input.find_last_not_of(" ");
  if (idx1==std::string::npos || idx2==std::string::npos) {
    return "";
  } else {
    return input.substr(idx1,1+idx2-idx1);
  }
}

const std::string& IOVDbParser::folderName() const {
  return m_keys.at("");
}

void IOVDbParser::clean() {
  auto it=m_keys.find("dbConnection");
  if (it!=m_keys.end()) {
    std::string connection=it->second;
    m_keys.erase(it);
    m_keys["db"]=connection;
  }
  return;
}



unsigned IOVDbParser::applyOverrides(const IOVDbParser& other, MsgStream & log) {
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
    
bool IOVDbParser::operator==(const IOVDbParser& other) const {
  return ((this->m_keys) == other.m_keys);
}

std::string IOVDbParser::toString() const {
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
