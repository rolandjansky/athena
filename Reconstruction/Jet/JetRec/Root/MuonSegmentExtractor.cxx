// this file is -*- C++ -*-

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "JetRec/MuonSegmentExtractor.h"
#include "xAODMuon/MuonSegment.h"
#include <vector>

MuonSegmentExtractor::MuonSegmentExtractor(const xAOD::MuonSegmentContainer* ms,
                                           const std::string & label) :
  m_originalParticles(ms), m_label(label){
}

MuonSegmentExtractor::~MuonSegmentExtractor(){}

MuonSegmentExtractor* MuonSegmentExtractor::clone() const {
  // who is going to delete this?
  return new MuonSegmentExtractor(*this);
}

MuonSegmentExtractor* MuonSegmentExtractor::ghostClone() const {
  // who is going to delete this?
  return new MuonSegmentExtractor(*this);
}

void MuonSegmentExtractor::addToJet(xAOD::Jet& jet, 
                                    const std::vector<int>& indices) const{

  std::vector<const xAOD::MuonSegment*> constituents;
  constituents.reserve(indices.size());
  for(auto i : indices){constituents.push_back((*m_originalParticles)[i]);}

  // MuonSegments are always ghosts

  jet.setAssociatedObjects(m_label, constituents);
  jet.setAttribute<int>(m_label+"Count", constituents.size());
}


std::string MuonSegmentExtractor::toString(int level) const {
  std::ostringstream oss{"", std::ios::ate};
  oss << "MuonSegmentExtractor dump level(" << level << ")" 
     << " label " << m_label 
     << " isGhost " << std::boolalpha << true  // muon segments ar always ghost
     << " No of MuonSegments: " << m_originalParticles->size();

  if (level > 0){
    oss << "\n MuonSegment momenta\n";
    std::for_each(m_originalParticles->begin(),
                  m_originalParticles->end(),
                  [&oss](const xAOD::MuonSegment* p){
                    oss << p->px() << " " << p->py() << " " << p->pz() << '\n';});
  }

  return oss.str();
}



bool  MuonSegmentExtractor::checkIntegrity() const {
  for(const auto ip: *m_originalParticles){
    try{
      ip->pz();
    } catch(...) {
      return false;
    }
  }
  return true;
}
