/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// class header include
#include "ZeroLifetimePositioner.h"

// HepMC includes
#include "HepMC/GenEvent.h"

// CLHEP includes
#include "CLHEP/Vector/LorentzVector.h"

#include <limits>
#include <algorithm>

/** Constructor **/
Simulation::ZeroLifetimePositioner::ZeroLifetimePositioner( const std::string& name,
                                                            ISvcLocator* pSvcLocator )
  : base_class(name, pSvcLocator)
{
  declareProperty("ApplyPatch", m_applyPatch, "");
  declareProperty("RemovePatch", m_removePatch, "");
  declareProperty("PDGCodesToCheck", m_pdgCodesToCheck, "");
}


/** Athena algtool's Hooks */
StatusCode  Simulation::ZeroLifetimePositioner::initialize()
{
  ATH_MSG_VERBOSE("Initializing ...");
  for(auto& pdgcode : m_pdgCodesToCheck) {
    pdgcode = std::abs(pdgcode);
    ATH_MSG_DEBUG("Will look for verices where " << pdgcode <<
                  " oscillates into -" << pdgcode << " (and vice versa).");
  }
  return StatusCode::SUCCESS;
}


/** Athena algtool's Hooks */
StatusCode  Simulation::ZeroLifetimePositioner::finalize()
{
  ATH_MSG_VERBOSE("Finalizing ...");
  return StatusCode::SUCCESS;
}


StatusCode Simulation::ZeroLifetimePositioner::applyWorkaround(HepMC::GenEvent& ge) const
{
  ATH_MSG_DEBUG("applyWorkaround");
  return this->manipulate(ge, m_applyPatch, false);
}


StatusCode Simulation::ZeroLifetimePositioner::removeWorkaround(HepMC::GenEvent& ge) const
{
  ATH_MSG_DEBUG("removeWorkaround");
  return this->manipulate(ge, false, m_removePatch);
}


/** modifies (displaces) the given GenEvent */
StatusCode Simulation::ZeroLifetimePositioner::manipulate(HepMC::GenEvent& ge, bool applyPatch, bool removePatch) const
{
  // loop over the vertices in the event
  HepMC::GenEvent::vertex_iterator vtxIt = ge.vertices_begin();
  const HepMC::GenEvent::vertex_iterator vtxItEnd = ge.vertices_end();
  const auto pdgCodesBegin = m_pdgCodesToCheck.begin();
  const auto pdgCodesEnd = m_pdgCodesToCheck.end();
  for (; vtxIt != vtxItEnd; ++vtxIt) {
    // quick access:
    HepMC::GenVertex *curVtx = (*vtxIt);
    if (curVtx->particles_in_size()!=1 || curVtx->particles_out_size()!=1) { continue; }
    const int pdgIn=(*(curVtx->particles_in_const_begin()))->pdg_id();
    const int pdgOut=(*(curVtx->particles_out_const_begin()))->pdg_id();
    if (pdgIn!=-pdgOut ||
        std::find(pdgCodesBegin, pdgCodesEnd, std::abs(pdgIn))== pdgCodesEnd) {
      continue;
    }
    HepMC::GenVertex* nextVtx = (*(curVtx->particles_out_const_begin()))->end_vertex();
    if(!nextVtx) { continue; }
    ATH_MSG_DEBUG("Found a vertex to correct with incoming PDG code = " << pdgIn);
    ATH_MSG_VERBOSE("Next Vertex:");
    if (ATH_UNLIKELY(this->msgLvl (MSG::VERBOSE))) {
      nextVtx->print();
    }
    const HepMC::FourVector &nextVec = nextVtx->position();
    const CLHEP::HepLorentzVector nextPos( nextVec.x(), nextVec.y(), nextVec.z(), nextVec.t() );
    ATH_MSG_VERBOSE("Current Vertex:");
    if (ATH_UNLIKELY(this->msgLvl (MSG::VERBOSE))) {
      curVtx->print();
    }
    if (applyPatch) {
      HepMC::GenVertex* prevVtx = (*(curVtx->particles_in_const_begin()))->production_vertex();
      ATH_MSG_VERBOSE("Previous Vertex:");
      if (ATH_UNLIKELY(this->msgLvl (MSG::VERBOSE))) {
        prevVtx->print();
      }
      const HepMC::FourVector &prevVec = prevVtx->position();
      const CLHEP::HepLorentzVector prevPos( prevVec.x(), prevVec.y(), prevVec.z(), prevVec.t() );
      CLHEP::HepLorentzVector newPos = 0.5*(prevPos+nextPos);
      curVtx->set_position(newPos);
      ATH_MSG_DEBUG("Revised current Vertex");
      if (ATH_UNLIKELY(this->msgLvl (MSG::VERBOSE))) {
        curVtx->print();
      }
    }
    if (removePatch) {
      CLHEP::HepLorentzVector newPos = nextPos;
      curVtx->set_position(newPos);
      ATH_MSG_DEBUG("Revised current Vertex");
      if (ATH_UNLIKELY(this->msgLvl (MSG::VERBOSE))) {
        curVtx->print();
      }
    }
  }

  return StatusCode::SUCCESS;
}
