/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCombinedEvent/InDetCandidateToTagMap.h"

namespace MuonCombined{

  InDetCandidateToTagMap::InDetCandidateToTagMap(){
    m_tagMap.clear();
  }

  InDetCandidateToTagMap::InDetCandidateToTagMap(InDetCandidateToTagMap& oldMap){
    m_tagMap=oldMap.getMap();
  }
  
  InDetCandidateToTagMap::~InDetCandidateToTagMap(){
    m_tagMap.clear();
  }
  
  void InDetCandidateToTagMap::addEntry(const InDetCandidate* idcand,TagBase* tag){
    m_tagMap.insert(std::pair<const InDetCandidate*,TagBase*>(idcand,tag));
  }
  
  const TagBase* InDetCandidateToTagMap::getTag(const InDetCandidate* idcand) const{
    auto pos=m_tagMap.find(idcand);
    if(pos==m_tagMap.end()) return 0;
    else return pos->second;
  }
  
  unsigned int InDetCandidateToTagMap::size() const{
    return m_tagMap.size();
  }

  std::map<const InDetCandidate*,TagBase*> InDetCandidateToTagMap::getMap() const{
    return m_tagMap;
  }
}
