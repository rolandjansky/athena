/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCOMBINEDEVENT_INDETCANDIDATETOTAGMAP_H
#define MUONCOMBINEDEVENT_INDETCANDIDATETOTAGMAP_H

#include "MuonCombinedEvent/TagBase.h"
#include "MuonCombinedEvent/InDetCandidate.h"
#include <map>

namespace MuonCombined{

  class InDetCandidateToTagMap{
  public:
    using tagMap = std::map<const InDetCandidate*,std::unique_ptr<TagBase> >;
    
    InDetCandidateToTagMap() = default;
    InDetCandidateToTagMap(InDetCandidateToTagMap&& oldMap) = default;
    ~InDetCandidateToTagMap() = default;
    
    void addEntry(const InDetCandidate* idcand, TagBase* tag);
    const TagBase* getTag(const InDetCandidate* idcand) const;
    unsigned int size() const;
    bool empty() const;
    
    tagMap::iterator begin();
    tagMap::iterator end();
    tagMap::const_iterator begin() const;
    tagMap::const_iterator end() const;

  private:
    tagMap m_tagMap;

  };
}

#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF(MuonCombined::InDetCandidateToTagMap,68903047,1)

#endif
