/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCOMBINEDEVENT_INDETCANDIDATETOTAGMAP_H
#define MUONCOMBINEDEVENT_INDETCANDIDATETOTAGMAP_H

#include "MuonCombinedEvent/TagBase.h"
#include "MuonCombinedEvent/InDetCandidate.h"

#include <map>

namespace MuonCombined{

  class InDetCandidateToTagMap{
  public:
    
    InDetCandidateToTagMap();
    InDetCandidateToTagMap(InDetCandidateToTagMap& oldMap);
    ~InDetCandidateToTagMap();
    
    void addEntry(const InDetCandidate* idcand, TagBase* tag);
    const TagBase* getTag(const InDetCandidate* idcand) const;
    unsigned int size() const;
    std::map<const InDetCandidate*,TagBase*> getMap() const;

  private:
    std::map<const InDetCandidate*,TagBase*> m_tagMap;

  };
}

#include "SGTools/CLASS_DEF.h"
CLASS_DEF(MuonCombined::InDetCandidateToTagMap,68903047,1)

#endif
