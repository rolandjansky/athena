/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CoralDB/CompoundTag.h"
#include <sstream>

namespace CoralDB {
#if 0
} // this brace is for better Emacs indentation
#endif

//================================================================
CompoundTag::CompoundTag(const std::string& objectDictionaryTag,
			 const std::string& connectivityTag,
			 const std::string& dataTag,
			 const std::string& aliasTag)
  : m_objectDictionaryTag(objectDictionaryTag), 
    m_connectivityTag(connectivityTag), 
    m_dataTag(dataTag),
    m_aliasTag(aliasTag)
{}

//================================================================
CompoundTag::CompoundTag() {}

//================================================================
CompoundTag::CompoundTag(const OnlyIdTag& oidt)
  : m_objectDictionaryTag(oidt.m_idtag)
{}

//================================================================
bool CompoundTag::isValid() const {
  return !m_objectDictionaryTag.empty() || ( CompoundTag() == *this );
}

//================================================================
bool CompoundTag::empty() const {
  return m_objectDictionaryTag.empty()
    && m_connectivityTag.empty()
    && m_dataTag.empty()
    && m_aliasTag.empty();
}

//================================================================
bool CompoundTag::fullIdTag() const {
  return m_connectivityTag.empty()
    && m_dataTag.empty()
    && m_aliasTag.empty();
}

//================================================================
std::string CompoundTag::toString() const {
  std::ostringstream os;
  os<<*this;
  return os.str();
}

//================================================================
std::ostream& operator<<(std::ostream& os, const CompoundTag& tags) {
  return os<<"CompoundTag(objectDictionaryTag="<<tags.objectDictionaryTag()
	   <<", connectivityTag="<<tags.connectivityTag()
	   <<", dataTag="<<tags.dataTag()
	   <<", aliasTag="<<tags.aliasTag()
	   <<")";
}

//================================================================
bool operator==(const CompoundTag& a, const CompoundTag& b) {
  return 
    a.objectDictionaryTag() == b.objectDictionaryTag() &&
    a.connectivityTag() == b.connectivityTag() &&
    a.dataTag() == b.dataTag() &&
    a.aliasTag() == b.aliasTag() ;
}

//================================================================

#if 0
{ // to make num braces even
#endif
} // namespace CoralDB
