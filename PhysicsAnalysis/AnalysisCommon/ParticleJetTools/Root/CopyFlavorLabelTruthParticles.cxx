/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ParticleJetTools/CopyFlavorLabelTruthParticles.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticleAuxContainer.h"
#include "xAODTruth/TruthEventContainer.h"
#include "AthContainers/ConstDataVector.h"
#include "TruthUtils/PIDHelpers.h"

using namespace std;


CopyFlavorLabelTruthParticles::CopyFlavorLabelTruthParticles(const std::string& name)
  : CopyTruthParticles(name)
{
  declareProperty("ParticleType", m_ptype="BHadronsFinal", "Sort of particles to pick: BHadronsFinal | BHadronsInitial | BQuarksFinal | CHadronsFinal | CHadronsInitial | CQuarksFinal | TausFinal");
}


// Unnamed namespace for helpers: only visible to this compilation unit
namespace {
  inline bool isBQuark(const xAOD::TruthParticle* tp) { return (abs(tp->pdgId()) == MC::PID::BQUARK); }
  inline bool isCQuark(const xAOD::TruthParticle* tp) { return (abs(tp->pdgId()) == MC::PID::CQUARK); }
  inline bool isBHadron(const xAOD::TruthParticle* tp) { return MC::PID::isBottomHadron(tp->pdgId()); }
  inline bool isCHadron(const xAOD::TruthParticle* tp) { return MC::PID::isCharmHadron(tp->pdgId()); }
  inline bool isTau(const xAOD::TruthParticle* tp) { return MC::PID::isTau(tp->pdgId()); }

  template <typename FN>
  inline bool isFinalWith(const xAOD::TruthParticle* tp, const FN& f) {
    if (!f(tp)) return false;
    if (!tp->hasDecayVtx()) return false;
    for (unsigned int i = 0; i < tp->decayVtx()->nOutgoingParticles(); ++i) {
      if (tp->decayVtx()->outgoingParticle(i)==nullptr) continue;
      if (f(tp->decayVtx()->outgoingParticle(i))) return false;
    }
    return true;
  }

  template <typename FN>
  inline bool isInitialWith(const xAOD::TruthParticle* tp, const FN& f) {
    if (!f(tp)) return false;
    if (!tp->hasProdVtx()) return false;
    for (unsigned int i = 0; i < tp->prodVtx()->nIncomingParticles(); ++i) {
      if (tp->prodVtx()->incomingParticle(i)==nullptr) continue;
      if (f(tp->prodVtx()->incomingParticle(i))) return false;
    }
    return true;
  }
}


bool CopyFlavorLabelTruthParticles::classify(const xAOD::TruthParticle* tp) const {
  // Cut on particle type
  if (tp == nullptr){
    return false;
  }
  if (m_ptype == "BHadronsFinal") {
    //ATH_MSG_DEBUG("Selecting in BHadronsFinal mode");
    return isFinalWith(tp, isBHadron);
  } else if (m_ptype == "BHadronsInitial") {
    //ATH_MSG_DEBUG("Selecting in BHadronsInitial mode");
    return isInitialWith(tp, isBHadron);
  } else if (m_ptype == "BQuarksFinal") {
    //ATH_MSG_DEBUG("Selecting in BQuarksFinal mode");
    return isFinalWith(tp, isBQuark);
  } else if (m_ptype == "CHadronsFinal") {
    //ATH_MSG_DEBUG("Selecting in CHadronsFinal mode");
    return isFinalWith(tp, isCHadron);
  } else if (m_ptype == "CHadronsInitial") {
    //ATH_MSG_DEBUG("Selecting in CHadronsInitial mode");
    return isInitialWith(tp, isCHadron);
  } else if (m_ptype == "CQuarksFinal") {
    //ATH_MSG_DEBUG("Selecting in CQuarksFinal mode");
    return isFinalWith(tp, isCQuark);
  } else if (m_ptype == "TausFinal") {
    //ATH_MSG_DEBUG("Selecting in TausFinal mode");
    return isFinalWith(tp, isTau);
  }
  throw std::runtime_error("Requested unknown particle classification type: " + m_ptype);
  return false;
}
