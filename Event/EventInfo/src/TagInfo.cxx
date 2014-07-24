/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

TagInfo::TagInfo()
{}

TagInfo::~TagInfo()
{}



/**
 **  @class NameTagPairComp
 **
 **  @brief This is a comparison class used internally in TagInfo.cxx
 **  to sort a NameTagPair stl vector
 **
 **/
class NameTagPairComp
{
public:
    /// comparison operator
    bool operator () (const TagInfo::NameTagPair& x, 
		      const TagInfo::NameTagPair& y) const 
	{
	    return (x.first < y.first);
	}
};


// Access to DetDescr tags
void                
TagInfo::findTag    (const std::string& name, std::string& tag) const
{
    NameTagPairVec::const_iterator it = findTag(name, m_tags);
    if (it != m_tags.end()) {
	tag = (*it).second;
    }
    else {
	tag = "";
    }
}

// Access to DetDescr tags
void                
TagInfo::findInputTag    (const std::string& name, std::string& tag) const
{
    NameTagPairVec::const_iterator it = findTag(name, m_inputTags);
    if (it != m_inputTags.end()) {
	tag = (*it).second;
    }
    else {
	tag = "";
    }
}

void                
TagInfo::getTags    (NameTagPairVec& pairs) const
{
    pairs = m_tags;
}
    
void                
TagInfo::getInputTags    (NameTagPairVec& pairs) const
{
    pairs = m_inputTags;
}
    
	    
std::string
TagInfo::tagInfoTag    () const
{
    return (m_myTag);
}

bool
TagInfo::operator<     (const TagInfo& rhs) const
{
   if (this->tagInfoTag() != rhs.tagInfoTag()) {
      return(this->tagInfoTag() < rhs.tagInfoTag());
   }
   if (this->m_tags.size() != rhs.m_tags.size()) {
      return(this->m_tags.size() < rhs.m_tags.size());
   }
   for (NameTagPairVec::const_iterator i = this->m_tags.begin(), j = rhs.m_tags.begin();
	   i != this->m_tags.end(); i++, j++) {
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
	   i != this->m_inputTags.end(); i++, j++) {
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
TagInfo::addTag     (const NameTagPair& pair, bool override)
{
    /// Return success if tag name not found, or override is set

    NameTagPairVec::iterator it = findTag(pair.first, m_tags);
    if (it != m_tags.end()) {

	// Tag name exists - check override
	if (override) {
	    // Over ride the existing tag value
	    (*it).second = pair.second;
	    return (StatusCode::SUCCESS);
	}
	else {
	    return (StatusCode::FAILURE);
	}	    
    }

    // New tag - add to vector and sort
    m_tags.push_back(pair);
    NameTagPairComp comp;
    std::sort (m_tags.begin(), m_tags.end(), comp);
    return (StatusCode::SUCCESS);
}

StatusCode
TagInfo::addInputTag     (const NameTagPair& pair, bool override)
{
    /// Return success if tag name not found, or override is set

    NameTagPairVec::iterator it = findTag(pair.first, m_inputTags);
    if (it != m_inputTags.end()) {

	// Tag name exists - check override
	if (override) {
	    // Over ride the existing tag value
	    (*it).second = pair.second;
	    return (StatusCode::SUCCESS);
	}
	else {
	    return (StatusCode::FAILURE);
	}	    
    }

    // New tag - add to vector and sort
    m_inputTags.push_back(pair);
    NameTagPairComp comp;
    std::sort (m_inputTags.begin(), m_inputTags.end(), comp);
    return (StatusCode::SUCCESS);
}

void
TagInfo::setTagInfoTag (const std::string& tag) 
{
    m_myTag = tag;
}


void                
TagInfo::printTags     (MsgStream& log) const
{
    if (log.level() <= MSG::DEBUG) {
	log << MSG::DEBUG << "TagInfo tag: " << m_myTag << endreq;
	log << MSG::DEBUG << "Current tags: "  << endreq;
	for (unsigned int i = 0; i < m_tags.size(); ++i) {
	    log << MSG::DEBUG << "    " << m_tags[i].first << " " << m_tags[i].second << endreq;
	}
	log << MSG::DEBUG << "Input tags: "  << endreq;
	for (unsigned int i = 0; i < m_inputTags.size(); ++i) {
	    log << MSG::DEBUG << "    " << m_inputTags[i].first << " " << m_inputTags[i].second << endreq;
	}
    }
}

TagInfo::NameTagPairVec::iterator 
TagInfo::findTag(const std::string& name,
		 NameTagPairVec& tags) 
{
    // Use lower_bound to find tag
    NameTagPairComp comp;
    NameTagPair  pair;
    pair.first = name;

    NameTagPairVec::iterator it = std::lower_bound( tags.begin(),
						    tags.end(),
						    pair,
						    comp);

    // We are now just below the tag, check whether we have found it
    if (it != tags.end()) {
	if ((*it).first == name) {
	    return (it);
	}
    }
    return (tags.end());
}

TagInfo::NameTagPairVec::const_iterator 
TagInfo::findTag(const std::string& name,
		 const NameTagPairVec& tags) const
{
    // Use lower_bound to find tag
    NameTagPairComp comp;
    NameTagPair  pair;
    pair.first = name;
    NameTagPairVec::const_iterator it = std::lower_bound( tags.begin(),
							  tags.end(),
							  pair,
							  comp);
    // We are now just below the tag, check whether we have found it
    if (it != tags.end()) {
	if ((*it).first == name) {
	    return (it);
	}
    }
    return (tags.end());
}

