/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Collection of tags for the database

#ifndef COMPOUNDTAG_H
#define COMPOUNDTAG_H

#include <string>
#include <iostream>

namespace CoralDB {

  class CompoundTag {
    std::string m_objectDictionaryTag;
    std::string m_connectivityTag;
    std::string m_dataTag;
    std::string m_aliasTag;
  public:
    CompoundTag(const std::string& objectDictionaryTag,
		const std::string& connectivityTag,
		const std::string& dataTag,
		const std::string& aliasTag);

    CompoundTag();

    //----------------
    // An easy way to create CompoundTag with just IdTag set
    struct OnlyIdTag {
      std::string m_idtag;
      explicit OnlyIdTag(const std::string& idtag) : m_idtag(idtag) {}
    };

    CompoundTag(const OnlyIdTag& oidt);

    //----------------
    std::string objectDictionaryTag() const { return m_objectDictionaryTag; }
    std::string connectivityTag() const { return m_connectivityTag; }
    std::string dataTag() const { return m_dataTag; }
    std::string aliasTag() const { return m_aliasTag; }

    void setObjectDictionaryTag(const std::string& tag) { m_objectDictionaryTag = tag; }
    void setConnectivityTag(const std::string& tag) { m_connectivityTag = tag; }
    void setDataTag(const std::string& tag) { m_dataTag = tag; }
    void setAliasTag(const std::string& tag) { m_aliasTag = tag; }

    // "Not valid" means empty idTag with a non-empty tag. This combination 
    // never makes sense.  All empty, or just idTag set, are OK.
    // (This call does not check the database for the existence of tags.)
    bool isValid() const;

    bool empty() const;

    // No C,A,D tags are set.
    bool fullIdTag() const;

    std::string toString() const;
  };

  std::ostream& operator<<(std::ostream& os, const CompoundTag& t);
  bool operator==(const CompoundTag& a, const CompoundTag& b);

}

#endif/*COMPOUNDTAG_H*/
