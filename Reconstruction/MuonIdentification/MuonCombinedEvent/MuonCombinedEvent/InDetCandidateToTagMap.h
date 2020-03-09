/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
    std::map<const InDetCandidate*,std::unique_ptr<TagBase> >::iterator mapBegin() {return m_tagMap.begin();}
    std::map<const InDetCandidate*,std::unique_ptr<TagBase> >::iterator mapEnd() {return m_tagMap.end();}

  private:
    std::map<const InDetCandidate*,std::unique_ptr<TagBase> > m_tagMap;

  };
}

#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF(MuonCombined::InDetCandidateToTagMap,68903047,1)

#endif
