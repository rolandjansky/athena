/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ParticleJetTools/CopyBosonTopLabelTruthParticles.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticleAuxContainer.h"
#include "xAODTruth/TruthEventContainer.h"
#include "AthContainers/ConstDataVector.h"
#include "TruthUtils/PIDHelpers.h"

using namespace std;


CopyBosonTopLabelTruthParticles::CopyBosonTopLabelTruthParticles(const std::string& name)
  : CopyTruthParticles(name)
{
  declareProperty("ParticleType", m_ptype="None", "Sort of particles to pick: WBosons | ZBosons | HBosons | TQuarksFinal");
}


// Unnamed namespace for helpers: only visible to this compilation unit
namespace {
  inline bool isWBoson(const xAOD::TruthParticle* tp) { return (abs(tp->pdgId()) == MC::PID::WPLUSBOSON); }
  inline bool isZBoson(const xAOD::TruthParticle* tp) { return (abs(tp->pdgId()) == MC::PID::Z0BOSON); }
  inline bool isHBoson(const xAOD::TruthParticle* tp) { return (abs(tp->pdgId()) == MC::PID::HIGGSBOSON); }
  inline bool isTQuark(const xAOD::TruthParticle* tp) { return (abs(tp->pdgId()) == MC::PID::TQUARK); }

  template <typename FN>
  inline bool isFinalWith(const xAOD::TruthParticle* tp, const FN& f) {
    if (!f(tp)) return false;
    if (!tp->hasDecayVtx()) return false;
    for (unsigned int i = 0; i < tp->decayVtx()->nOutgoingParticles(); ++i) {
      if (f(tp->decayVtx()->outgoingParticle(i))) return false;
    }
    return true;
  }
}


bool CopyBosonTopLabelTruthParticles::classify(const xAOD::TruthParticle* tp) const {
  // Cut on particle type
  if (m_ptype == "WBosons") {
    return isFinalWith(tp, isWBoson);
  } else if (m_ptype == "ZBosons") {
    return isFinalWith(tp, isZBoson);
  } else if (m_ptype == "HBosons") {
    return isFinalWith(tp, isHBoson);
  } else if (m_ptype == "TQuarksFinal") {
    return isFinalWith(tp, isTQuark);
  }

  // not good.
  throw std::runtime_error("Requested unknown particle classification type: " + m_ptype);
  return false;
}
