/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ParticleJetTools/CopyTruthParticles.h"
#include "AthenaKernel/errorcheck.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticleAuxContainer.h"
#include "xAODTruth/TruthEventContainer.h"
#include "AthContainers/ConstDataVector.h"
#include "TruthUtils/PIDUtils.h"

using namespace std;


CopyTruthParticles::CopyTruthParticles(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
{
  declareProperty("OutputName", m_outputname="TagInputs", "Name of the resulting IParticle collection");
  declareProperty("ParticleType", m_ptype="BHadronsFinal", "Sort of particles to pick: BHadronsFinal | BHadronsInitial | BQuarksFinal | CHadronsFinal | CHadronsInitial | CQuarksFinal | TausFinal");
  declareProperty("PtMin", m_ptmin=0, "Minimum pT of particles to be accepted for tagging (in MeV)");
}


StatusCode CopyTruthParticles::execute() {
  // Retrieve the xAOD truth objects
  const xAOD::TruthEventContainer* xTruthEventContainer = NULL;
  CHECK( evtStore()->retrieve( xTruthEventContainer, "TruthEvent"));

  // Make a new TruthParticleContainer and link it to StoreGate
  if (evtStore()->contains<xAOD::TruthParticleContainer>(m_outputname))
    ATH_MSG_ERROR("Tag input TruthParticleContainer " << m_outputname << " already exists");
  ConstDataVector<xAOD::TruthParticleContainer> *ipc = new ConstDataVector<xAOD::TruthParticleContainer>(SG::VIEW_ELEMENTS);
  if (evtStore()->record(ipc, m_outputname).isFailure())
    ATH_MSG_ERROR("Failed to record a new TruthParticleContainer " << m_outputname);

  // Classify particles for tagging and add to the TruthParticleContainer
  const xAOD::TruthEvent* evt = *xTruthEventContainer->begin();
  size_t numCopied = 0;
  for (unsigned int ip = 0; ip < evt->nTruthParticles(); ++ip) {
    const xAOD::TruthParticle* tp = evt->truthParticle(ip);
    if (classify(tp)) {
      ipc->push_back(tp);
      numCopied += 1;
    }
  }

  ATH_MSG_DEBUG("Copied " << numCopied << " truth particles into " << m_outputname << " TruthParticle container");

  return StatusCode::SUCCESS;
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
      if (f(tp->decayVtx()->outgoingParticle(i))) return false;
    }
    return true;
  }

  template <typename FN>
  inline bool isInitialWith(const xAOD::TruthParticle* tp, const FN& f) {
    if (!f(tp)) return false;
    if (!tp->hasProdVtx()) return false;
    for (unsigned int i = 0; i < tp->prodVtx()->nIncomingParticles(); ++i) {
      if (f(tp->prodVtx()->incomingParticle(i))) return false;
    }
    return true;
  }
}


bool CopyTruthParticles::classify(const xAOD::TruthParticle* tp) {
  // Cut on pT
  if (tp->pt() < m_ptmin) return false;
  // Cut on particle type
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
