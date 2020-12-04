/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#include "GeneratorFilters/VHtoVVFilter.h"
#include "GaudiKernel/AlgFactory.h"


VHtoVVFilter::VHtoVVFilter(const std::string& name, ISvcLocator* pSvcLocator)
  : GenFilter(name, pSvcLocator)
{
  declareProperty("PDGGrandParent", m_PDGGrandParent);
  declareProperty("PDGParent", m_PDGParent);
  declareProperty("PDGHVChild1", m_PDGHVChild1);
  declareProperty("PDGHVChild2", m_PDGHVChild2);
  declareProperty("PDGAssocV", m_PDGAssocV=0);
  declareProperty("PDGAssocVChild", m_PDGAssocVChild);
  declareProperty("StatusIntermediateHardScatter", m_StatusIntermediateHardScatter=3); // Pythia would be 22....
  m_nVHtoVV = 0;
  m_nGoodVHtoVV = 0;
}


StatusCode VHtoVVFilter::filterInitialize() {
  ATH_MSG_INFO("PDGGrandParent(H)       = " << m_PDGGrandParent);
  ATH_MSG_INFO("PDGParent(V)            = " << m_PDGParent);
  if (m_PDGAssocV == 0) { m_PDGAssocV = m_PDGParent; }
  ATH_MSG_INFO("PDGAssoc[=V by default] = " << m_PDGAssocV);
  if (m_PDGHVChild1.empty()) { ATH_MSG_ERROR("PDGHVChild1[] not set "); }
  for (size_t i = 0; i < m_PDGHVChild1.size(); ++i) {
    ATH_MSG_INFO("PDGHVChild1[" << i << "]          = " << m_PDGHVChild1[i]);
  }
  if (m_PDGHVChild2.empty()) { ATH_MSG_ERROR("PDGHVChild2[] not set "); }
  for (size_t i = 0; i < m_PDGHVChild2.size(); ++i) {
    ATH_MSG_INFO("PDGHVChild2[" << i << "]          = " << m_PDGHVChild2[i]);
  }
  if (m_PDGAssocVChild.empty()) { ATH_MSG_ERROR("PDGAssocVChild[] not set "); }
  for (size_t i = 0; i < m_PDGAssocVChild.size(); ++i) {
    ATH_MSG_INFO("PDGAssocVChild[" << i << "]       = " << m_PDGAssocVChild[i]);
  }

  // Init counters
  m_nVHtoVV = 0;
  m_nGoodVHtoVV = 0;
  return StatusCode::SUCCESS;
}


StatusCode VHtoVVFilter::filterFinalize() {
  ATH_MSG_INFO("Statistics of WH->VV, W->decay V->decay ");
  ATH_MSG_INFO("  ALL  VH->VV " << m_nVHtoVV);
  ATH_MSG_INFO("  Good VH->VV " << m_nGoodVHtoVV);
  if (m_nVHtoVV != 0)
    ATH_MSG_INFO("  Fraction   " << double(m_nGoodVHtoVV)/double(m_nVHtoVV));
  return StatusCode::SUCCESS;
}


StatusCode VHtoVVFilter::filterEvent() {
  int nHiggsParent = 0;
  int nVParent = 0;
  int nHV1Like(0), nHV2Like(0), nHVVLike(0), nVassocLike(0);

  McEventCollection::const_iterator itr;
  for (itr = events_const()->begin(); itr != events_const()->end(); ++itr) {
    // Loop over all particles in the event
    const HepMC::GenEvent* genEvt = (*itr);
    for (HepMC::GenEvent::particle_const_iterator pitr = genEvt->particles_begin(); pitr != genEvt->particles_end(); ++pitr ) {
      // Loop over intermediate particles from the primary interaction
      if ( (*pitr)->status() == m_StatusIntermediateHardScatter) {
        HepMC::GenVertex::particle_iterator firstMother = (*pitr)->production_vertex()->particles_begin(HepMC::parents);
        HepMC::GenVertex::particle_iterator endMother = (*pitr)->production_vertex()->particles_end(HepMC::parents);
        HepMC::GenVertex::particle_iterator thisMother = firstMother;

        // Is this a V from a Higgs? Is it an associated V
        bool isGrandParentHiggs(false), isAssociatedV(false);
        if( (abs((*pitr)->pdg_id()) == m_PDGParent) || (abs((*pitr)->pdg_id()) == m_PDGAssocV) ) {
          ATH_MSG_DEBUG("Found hard-scatter V [" << (*pitr)->pdg_id() << "] with barcode = " << (*pitr)->barcode() << " status = "  << (*pitr)->status());
          for (; thisMother != endMother; ++thisMother) { // loop over chain of grandparents
            ATH_MSG_DEBUG("  ... with a mother ["  << (*thisMother)->pdg_id()<< "] with barcode = " << (*thisMother)->barcode());
            if ( (*thisMother)->pdg_id() == m_PDGGrandParent ) { isGrandParentHiggs = true; }
          }
          if (!isGrandParentHiggs & (abs((*pitr)->pdg_id()) == m_PDGAssocV)) { isAssociatedV = true; }
        }

        // Check H->VV decays
        if (isGrandParentHiggs) {
          ATH_MSG_DEBUG("  * from a Higgs decay *"); ++nHiggsParent;
          // ... proceed down the decay chain until this doesn't have a V as a child
          HepMC::GenParticle* lastVInChain = lastDecayWithSamePDGID(*pitr);
          ATH_MSG_DEBUG("  ... moved down to final V before decay with barcode " << lastVInChain->barcode() << " status = "  << lastVInChain->status() );
          // ... check decays
          bool decaysLikeChild1 = allChildrenInList(lastVInChain, m_PDGHVChild1);
          bool decaysLikeChild2 = allChildrenInList(lastVInChain, m_PDGHVChild2);
          ATH_MSG_DEBUG(" decays like child 1? " << (decaysLikeChild1 ? "YES" : "NO") << " and like child 2? " << (decaysLikeChild2 ? "YES" : "NO"));
          if (decaysLikeChild1) { ++nHV1Like; }
          if (decaysLikeChild2) { ++nHV2Like; }
          if (decaysLikeChild1 || decaysLikeChild2) { ++nHVVLike; }
        }

        // Check V decays for V not coming from Higgs
        if (isAssociatedV) {
          ATH_MSG_DEBUG("  * from associated production *"); ++nVParent;
          // ... proceed down the decay chain until this doesn't have a V as a child
          HepMC::GenParticle* lastVInChain = lastDecayWithSamePDGID(*pitr);
          ATH_MSG_DEBUG("  ... moved down to final V before decay with barcode " << lastVInChain->barcode() << " status = "  << lastVInChain->status() );
          // ... check decays
          if( allChildrenInList(lastVInChain, m_PDGAssocVChild) ) { ++nVassocLike; }
        }
      } //end good parent loop
    }
  }

  ATH_MSG_DEBUG("Found " << nHiggsParent << " H->V of which " << nHV1Like << " decay like V1 and " << nHV2Like << " decay like V2");
  ATH_MSG_DEBUG("Found " << nVParent << " V of which " << nVassocLike<< " decay like associated V");

  if (nHiggsParent == 2 && nVParent >= 1) { ++m_nVHtoVV; }

  if (nHiggsParent == 2 && (nHV1Like >= 1) && (nHV2Like >= 1) && (nHVVLike >= 2) && // at least one H->V decaying each way with at least 2 in total
      nVParent == 1 && (nVassocLike >= 1)) { // at least one V decaying like the associated one
    ++m_nGoodVHtoVV;
    return StatusCode::SUCCESS;
  }

  // If we get here we have failed
  setFilterPassed(false);
  return StatusCode::SUCCESS;
}

bool VHtoVVFilter::allChildrenInList(HepMC::GenParticle* particle, std::vector<int> childPDGIDs) {
  HepMC::GenVertex::particle_iterator firstChild = particle->end_vertex()->particles_begin(HepMC::children);
  HepMC::GenVertex::particle_iterator endChild = particle->end_vertex()->particles_end(HepMC::children);
  HepMC::GenVertex::particle_iterator thisChild = firstChild;
  int nGoodChildren(0), nBadChildren(0);
  ATH_MSG_DEBUG("  checking children against provided list");
  for (; thisChild != endChild; ++thisChild) {
    bool thisChildOK = false;
    for (unsigned int i=0;i<childPDGIDs.size();++i) {
      if (abs((*thisChild)->pdg_id()) == childPDGIDs[i]) { thisChildOK = true; }
    }
    ATH_MSG_DEBUG("    ... child with PDGID: " << (*thisChild)->pdg_id() << ", status: " << (*thisChild)->status() << "? " << (thisChildOK ? "PASS" : "FAIL"));
    if (thisChildOK) { ++nGoodChildren; }
    else { ++nBadChildren; }
  }
  ATH_MSG_DEBUG("  => found " << nGoodChildren << " good children and " << nBadChildren << " bad children");
  return (nGoodChildren > 0 && nBadChildren == 0);
}

HepMC::GenParticle* VHtoVVFilter::lastDecayWithSamePDGID(HepMC::GenParticle* particle) {
  HepMC::GenVertex::particle_iterator firstChild = particle->end_vertex()->particles_begin(HepMC::children);
  HepMC::GenVertex::particle_iterator endChild = particle->end_vertex()->particles_end(HepMC::children);
  HepMC::GenVertex::particle_iterator thisChild = firstChild;
  for (; thisChild != endChild; ++thisChild) {
    if ((*thisChild)->pdg_id() == particle->pdg_id() ) { return lastDecayWithSamePDGID(*thisChild); }
  }
  return particle;
}
