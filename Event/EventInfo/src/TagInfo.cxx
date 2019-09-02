/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

/**
 * @file TagInfo.cxx
 *
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 *
 * $Id: TagInfo.cxx,v 1.6 2009-04-29 07:31:00 schaffer Exp $
 */

#include "EventInfo/TagInfo.h"
#include "GaudiKernel/MsgStream.h"
#include <algorithm>


namespace{
  /// Return an iterator for a tag found
  TagInfo::NameTagPairVec::iterator 
  findTagIt(const std::string& name,TagInfo::NameTagPairVec& tags){
    // Use lower_bound to find tag
    TagInfo::NameTagPair pair;
    pair.first = name;
    TagInfo::NameTagPairVec::iterator it = std::lower_bound(tags.begin(),tags.end(),pair);
    // We are now just below the tag, check whether we have found it
    if (it != tags.end()) {
      if (it->first == name) {
        return(it);
      }
    }
    return(tags.end());
  }
  
  
  /// Return an iterator for a tag found
  TagInfo::NameTagPairVec::const_iterator 
  findTagIt(const std::string& name,const TagInfo::NameTagPairVec& tags){
     // Use lower_bound to find tag
    TagInfo::NameTagPair pair;
    pair.first = name;
    TagInfo::NameTagPairVec::const_iterator it = std::lower_bound(tags.begin(),tags.end(),pair);
    // We are now just below the tag, check whether we have found it
    if (it != tags.end()) {
      if (it->first == name) {
        return(it);
      }
    }
    return(tags.end());
  }

  std::string
  findTagImpl(const std::string & name, const TagInfo::NameTagPairVec & thisVec){
    std::string result{};
    const auto & it = findTagIt(name, thisVec);
    if (it != thisVec.end()) result = it->second;
    return result;
  }
  
  bool
  addTagImpl(const TagInfo::NameTagPair& pair, TagInfo::NameTagPairVec & thisVec, const bool override = false){
    /// Return success if tag name not found, or override is set
    TagInfo::NameTagPairVec::iterator it = findTagIt(pair.first, thisVec);
    if (it != thisVec.end()) {
      // Tag name exists - check override
      if (override) {
        // Over ride the existing tag value
        it->second = pair.second;
        return true;
      } else {
        return false;
      }
    }
    // New tag - add to vector and sort
    thisVec.push_back(pair);
    std::sort(thisVec.begin(), thisVec.end());
    return true;
  }
  std::string
  formatTagPair(const TagInfo::NameTagPair& pair){
    return std::string("    ") + pair.first + " " + pair.second;
  }


}

TagInfo::TagInfo(){}

TagInfo::~TagInfo(){}



// Access to DetDescr tags
void
TagInfo::findTag(const std::string& name, std::string& tag) const {
  tag = findTag(name);
}

std::string
TagInfo::findTag(const std::string& name) const {
  return findTagImpl(name, m_tags);
}

// Access to DetDescr tags
void
TagInfo::findInputTag(const std::string& name, std::string& tag) const {
   tag = findInputTag(name);
}

std::string
TagInfo::findInputTag(const std::string& name) const {
  return findTagImpl(name, m_inputTags);;
}


void
TagInfo::getTags(NameTagPairVec& pairs) const {
  pairs = m_tags;
}

TagInfo::NameTagPairVec
TagInfo::getTags() const {
  return m_tags;
}

void
TagInfo::getInputTags(NameTagPairVec& pairs) const {
  pairs = m_inputTags;
}

TagInfo::NameTagPairVec
TagInfo::getInputTags() const {
  return m_inputTags;
}

std::string
TagInfo::tagInfoTag() const {
  return(m_myTag);
}

bool
TagInfo::operator < (const TagInfo& rhs) const {
  if (this->tagInfoTag() != rhs.tagInfoTag()) {
    return(this->tagInfoTag() < rhs.tagInfoTag());
  }
  if (this->m_tags.size() != rhs.m_tags.size()) {
    return(this->m_tags.size() < rhs.m_tags.size());
  }
  for (NameTagPairVec::const_iterator i = this->m_tags.begin(), j = rhs.m_tags.begin();
       i != this->m_tags.end(); ++i, ++j) {
    if (i->first != j->first) {
      return(i->first < j->first);
    }
    if (i->second != j->second) {
      return(i->second < j->second);
    }
  }
  if (this->m_inputTags.size() != rhs.m_inputTags.size()) {
    return(this->m_inputTags.size() < rhs.m_inputTags.size());
  }
  for (NameTagPairVec::const_iterator i = this->m_inputTags.begin(), j = rhs.m_inputTags.begin();
       i != this->m_inputTags.end(); ++i, ++j) {
    if (i->first != j->first) {
      return(i->first < j->first);
    }
    if (i->second != j->second) {
      return(i->second < j->second);
    }
  }
  return(false);
}

// Set methods:
StatusCode
TagInfo::addTag(const NameTagPair& pair, bool override) {
  /// Return success if tag name not found, or override is set
  return addTagImpl(pair,m_tags,override)? (StatusCode::SUCCESS): (StatusCode::FAILURE);
}

StatusCode
TagInfo::addInputTag(const NameTagPair& pair, bool override) {
  /// Return success if tag name not found, or override is set
  return addTagImpl(pair,m_inputTags,override)? (StatusCode::SUCCESS): (StatusCode::FAILURE);
}

void
TagInfo::setTagInfoTag(const std::string& tag) {
  m_myTag = tag;
}

void
TagInfo::printTags(MsgStream& log) const {
  if (log.level() <= MSG::DEBUG) {
    log << MSG::DEBUG << "TagInfo tag: " << m_myTag << endmsg;
    log << MSG::DEBUG << "Current tags: " << endmsg;
    for (const auto &thisPair : m_tags) {
      log << MSG::DEBUG << formatTagPair(thisPair) << endmsg;
    }
    log << MSG::DEBUG << "Input tags: " << endmsg;
    for (const auto &thisPair : m_inputTags) {
      log << MSG::DEBUG << formatTagPair(thisPair) << endmsg;
    }
  }
}

std::string
TagInfo::str() const {
  std::string m{};
  m+="TagInfo tag: " + m_myTag + "\n";
  m+="Current tags: \n";
  for (const auto &thisPair : m_tags) {
    m+= formatTagPair(thisPair) + "\n";
  }
  m+= "Input tags: \n";
  for (const auto &thisPair : m_inputTags) {
    m += formatTagPair(thisPair) + "\n";
  }
  return m;
}

