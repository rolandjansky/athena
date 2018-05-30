
// this file is -*- C++ -*-


/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// TruthParticleExtractor.cxx

#include "JetSimTools/TruthParticleExtractor.h"
#include "JetRec/LineFormatter.h" // helper class for debug printing
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticle.h"
#include "JetRec/setAssocObjectsSummary.h"
#include <vector>
#include <string>

TruthParticleExtractor::TruthParticleExtractor(const xAOD::TruthParticleContainer* tps,
                                               const std::string & label,
                                               bool isGhost) : 
   m_originalParticles(tps), m_label(label), m_isGhost(isGhost){
}

TruthParticleExtractor::~TruthParticleExtractor(){}

TruthParticleExtractor* TruthParticleExtractor::clone() const {
  return new TruthParticleExtractor(*this);
}

TruthParticleExtractor* TruthParticleExtractor::ghostClone() const {
  // caller to delete 
  auto clone =  new TruthParticleExtractor(*this);
  (*clone).m_isGhost = true;
  return clone;
}

void TruthParticleExtractor::addToJet(xAOD::Jet& jet, 
                                  const std::vector<int>& indices) const{
  //Add a template specialisation for muon segments

  std::vector<const xAOD::IParticle*> constituents;
  constituents.reserve(indices.size());
  for(auto i : indices){constituents.push_back((*m_originalParticles)[i]);}

  if(m_isGhost) {
    jet.setAssociatedObjects(m_label, constituents);
    setAssocObjectsSummary(constituents, m_label, jet);
  } else {
    // these are constituents 
    for(auto c: constituents) {
      jet.addConstituent(c);
    }
  }
}


std::string TruthParticleExtractor::toString(int level) const {
  std::ostringstream oss{"", std::ios::ate};
  oss << "TruthParticleExtractor dump level (" << level << ")"

     << " label " << m_label 
     << " isGhost: " << std::boolalpha << m_isGhost 
     << " No of TruthParticles: " << m_originalParticles->size();

  if (level > 0){
    oss << "\n TruthParticle energies\n";
    std::vector<float> energies;
    energies.reserve(m_originalParticles->size());
    std::transform(m_originalParticles->begin(),
                   m_originalParticles->end(),
                   std::back_inserter(energies),
                   [](const xAOD::TruthParticle* p){return p->e();});

    LineFormatter formatter(10); // 10 numbers/line
    oss << formatter(energies);
  }

  return oss.str();
}


bool TruthParticleExtractor::checkIntegrity() const {
  for(const auto tp: *m_originalParticles){
    try{
      tp->e();
    } catch(...) {
      return false;
    }
  }
  return true;
}
