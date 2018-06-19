// this file is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MuonSegmentExtractor.cxx

#include "JetRecTools/TrackExtractor.h"
#include "JetRec/LineFormatter.h" // helper class for debug printing
#include "xAODJet/Jet.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "JetRec/setAssocObjectsSummary.h"
#include <vector>

TrackExtractor::TrackExtractor(const xAOD::TrackParticleContainer* tps,
                               const std::string & label,
                               bool isGhost) : 
   m_originalParticles(tps), m_label(label), m_isGhost(isGhost){
}

TrackExtractor::~TrackExtractor(){}

TrackExtractor* TrackExtractor::clone() const {
  // to be deleted by caller
  return  new TrackExtractor(*this);
}

TrackExtractor* TrackExtractor::ghostClone() const {
  // to be deleted by caller
  auto clone =  new TrackExtractor(*this);
  (*clone).m_isGhost = true;
  return clone;
}

void TrackExtractor::addToJet(xAOD::Jet& jet, 
                                  const std::vector<int>& indices) const{
  //Add a template specialisation for muon segments

  std::vector<const xAOD::TrackParticle*> constituents;
  constituents.reserve(indices.size());
  for(auto i : indices){constituents.push_back((*m_originalParticles)[i]);}

  if(m_isGhost) {

    std::vector<const xAOD::IParticle*> constituents;
    constituents.reserve(indices.size());
    for(auto i : indices){constituents.push_back((*m_originalParticles)[i]);}

    jet.setAssociatedObjects(m_label, constituents);
    setAssocObjectsSummary(constituents, m_label, jet);

  } else {
    // these are constituents
    std::vector<const xAOD::TrackParticle*> constituents;
    constituents.reserve(indices.size());
    for(auto i : indices){constituents.push_back((*m_originalParticles)[i]);}
    
    for(auto c: constituents) {
      jet.addConstituent(c);
    }
  }
}



bool TrackExtractor::checkIntegrity() const {
  for(const auto ip: *m_originalParticles){
    try{
      ip->e();
    } catch(...) {
      return false;
    }
  }
  return true;
}


std::string TrackExtractor::toString(int level) const {
  std::ostringstream oss{"", std::ios::ate};
  oss << "TrackExtractor dump level(" << level << ")"
     << " label " << m_label 
     << " isGhost: "  << std::boolalpha << m_isGhost 
     << " No of TrackParticles: " << m_originalParticles->size();

  if (level > 0){
    oss << "\n TrackParticle energies\n";
    std::vector<float> energies;
    energies.reserve(m_originalParticles->size());
    std::transform(m_originalParticles->begin(),
                   m_originalParticles->end(),
                   std::back_inserter(energies),
                   [](const xAOD::TrackParticle* p){return p->e();});

    LineFormatter formatter(10); // 10 numbers/line
    oss << formatter(energies);
  }

  return oss.str();
}

