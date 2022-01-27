/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCombinedEvent/InDetCandidateToTagMap.h"

namespace MuonCombined{
  void InDetCandidateToTagMap::addEntry(const InDetCandidate* idcand,TagBase* tag){
    m_tagMap.insert(std::pair<const InDetCandidate*,std::unique_ptr<TagBase> >(idcand,std::unique_ptr<TagBase>(tag)));
  }
  
  const TagBase* InDetCandidateToTagMap::getTag(const InDetCandidate* idcand) const{
    tagMap::const_iterator  pos=m_tagMap.find(idcand);
    if(pos==end()) return nullptr;
    return pos->second.get();
  }
  
  unsigned int InDetCandidateToTagMap::size() const{ return m_tagMap.size(); }
  bool InDetCandidateToTagMap::empty() const {return m_tagMap.empty();}
  InDetCandidateToTagMap::tagMap::iterator InDetCandidateToTagMap::begin() { return m_tagMap.begin();}
  InDetCandidateToTagMap::tagMap::iterator InDetCandidateToTagMap::end() { return m_tagMap.end();}
  InDetCandidateToTagMap::tagMap::const_iterator InDetCandidateToTagMap::begin() const { return m_tagMap.begin();}
  InDetCandidateToTagMap::tagMap::const_iterator InDetCandidateToTagMap::end() const { return m_tagMap.end();}
}
