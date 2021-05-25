/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCombinedEvent/InDetCandidateToTagMap.h"

namespace MuonCombined{

  InDetCandidateToTagMap::InDetCandidateToTagMap(){}

  InDetCandidateToTagMap::InDetCandidateToTagMap(InDetCandidateToTagMap& oldMap){
    m_tagMap.clear();
    for(std::map<const InDetCandidate*,std::unique_ptr<TagBase> >::iterator it=oldMap.mapBegin(); it!=oldMap.mapEnd(); ++it){
      m_tagMap.insert(std::pair<const InDetCandidate*,std::unique_ptr<TagBase> >((*it).first,std::move((*it).second)));
    }
  }

  InDetCandidateToTagMap::~InDetCandidateToTagMap(){
    m_tagMap.clear();
  }
  
  void InDetCandidateToTagMap::addEntry(const InDetCandidate* idcand,TagBase* tag){
    m_tagMap.insert(std::pair<const InDetCandidate*,std::unique_ptr<TagBase> >(idcand,std::unique_ptr<TagBase>(tag)));
  }
  
  const TagBase* InDetCandidateToTagMap::getTag(const InDetCandidate* idcand) const{
    auto pos=m_tagMap.find(idcand);
    if(pos==m_tagMap.end()) return nullptr;
    else return pos->second.get();
  }
  
  unsigned int InDetCandidateToTagMap::size() const{
    return m_tagMap.size();
  }

}
