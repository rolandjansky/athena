// this file is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetRec/IParticleExtractor.h"
#include "JetRec/LineFormatter.h" // helper class for debug printing
#include "xAODBase/IParticle.h"
#include <vector>

IParticleExtractor::IParticleExtractor(const xAOD::IParticleContainer* ips,
                                       const std::string & label, 
                                       bool isGhost,
                                       bool isTrigger) : 
  m_iParticles(ips), m_label(label),
  m_isGhost(isGhost),
  m_isTrigger(isTrigger)// does not add constits or assoc particles if true
{
}

IParticleExtractor::~IParticleExtractor(){}

IParticleExtractor* IParticleExtractor::clone() const {
  return new IParticleExtractor(*this);
}
  
IParticleExtractor* IParticleExtractor::ghostClone() const {
  // user responsible for deletion.
  auto clone =  new IParticleExtractor(*this);
  (*clone).m_isGhost = true;
  return clone;
}

void IParticleExtractor::addToJet(xAOD::Jet& jet, 
                                  const std::vector<int>& indices) const{
  //Add a template specialisation for muon segments

  std::vector<const xAOD::IParticle*> constituents;
  constituents.reserve(indices.size());
  for(auto i : indices){
    if(m_debug){
      constituents.push_back(m_iParticles->at(i));
    } else {
      constituents.push_back((*m_iParticles)[i]);
    }
  }

  if(m_isGhost) {
    // ghosts (in this context) are objects to be stored with the
    // jet, but with a scaled momentum so they have no kinimatical effect.
    double ptSum = std::accumulate(constituents.begin(),
                                   constituents.end(),
                                   0.0,
                                   [](double sumPt, const xAOD::IParticle* p){
                                     return sumPt + p->pt();});

    if ( (!m_isTrigger) || (m_label == "GhostTrack") ){jet.setAssociatedObjects(m_label, constituents);}
    jet.setAttribute<float>(m_label + "Pt", ptSum);
    jet.setAttribute<int>(m_label+"Count", constituents.size());
  } else {
    // these are constituents
    for(auto c: constituents) {
      jet.addConstituent(c);
    }
  }
}



std::string IParticleExtractor::toString(int level) const {
  std::ostringstream oss{"", std::ios::ate};
  oss << "IParticleExtractor dump level (" << level << ")"
      << " label " << m_label 
      << " isGhost: " << std::boolalpha << m_isGhost 
      << " isTrigger: " << std::boolalpha << m_isTrigger 
      << " No of IParticles: " << m_iParticles->size();

  if (level > 0){

    oss << "\n IParticle energies\n";
    std::vector<float> energies;
    energies.reserve(m_iParticles->size());
    std::transform(m_iParticles->begin(),
                   m_iParticles->end(),
                   std::back_inserter(energies),
                   [](const xAOD::IParticle* p){return p->e();});

    LineFormatter formatter(10); // 10 numbers/line
    oss << formatter(energies) << '\n';

    std::vector<const xAOD::IParticle*> adds(m_iParticles->begin(),
                                             m_iParticles->end());
    oss << "\n IParticle addresses\n"
        << formatter(adds)
        << '\n';
  }
  return oss.str();
}



bool IParticleExtractor::checkIntegrity() const {
  for(const auto ip: (*m_iParticles)){
    try{
      ip->e();
    } catch(...) {
      return false;
    }
  }
  return true;
}
