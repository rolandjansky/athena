/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "GeneratorFilters/TtHtoVVDecayFilter.h"



TtHtoVVDecayFilter::TtHtoVVDecayFilter(const std::string& name, ISvcLocator* pSvcLocator)
  : GenFilter(name, pSvcLocator)
{
  declareProperty("PDGGrandParent", m_PDGGrandParent);
  declareProperty("PDGParent", m_PDGParent);
  declareProperty("StatusParent", m_StatusParent);
  declareProperty("TotalParent", m_TotalParent);
  declareProperty("SameChargeParent", m_SameChargeParent);
  declareProperty("PDGChild", m_PDGChild);

    // initialize all members 
  m_nHtoVV = 0;
  m_nGoodHtoVV = 0;
  m_nGoodHtoVVSameCharge = 0;
}


StatusCode TtHtoVVDecayFilter::filterInitialize() {
  if (m_PDGGrandParent.empty()) ATH_MSG_ERROR("m_PDGGrandParent[] not set ");
  if (m_PDGParent.empty()) ATH_MSG_ERROR("PDGParent[] not set ");
  if (m_StatusParent.empty()) ATH_MSG_ERROR("StatusParent[] not set ");
  if (m_PDGChild.empty()) ATH_MSG_ERROR("PDGChild[] not set ");

  for (size_t i = 0; i < m_PDGGrandParent.size(); ++i)
    ATH_MSG_INFO("PDGGrandParent[" << i << "] = " << m_PDGGrandParent[i]);
  for (size_t i = 0; i < m_PDGParent.size(); ++i)
    ATH_MSG_INFO("PDGParent(V)      = " << m_PDGParent[i] << " " << "StatusParent(V)      = " << m_StatusParent[i]);
  for (size_t i = 0; i < m_PDGChild.size(); ++i)
    ATH_MSG_INFO("PDGChild[" << i << "] = " << m_PDGChild[i]);

  ATH_MSG_INFO("TotalParent = " << m_TotalParent);
  ATH_MSG_INFO("SameChargeParent = " << m_SameChargeParent);

  m_nHtoVV = 0;
  m_nGoodHtoVV = 0;
  return StatusCode::SUCCESS;
}


StatusCode TtHtoVVDecayFilter::filterFinalize() {
  ATH_MSG_INFO("Statistics of X->VV, V->decay ");
  ATH_MSG_INFO("  ALL  X->VV  " << m_nHtoVV);
  ATH_MSG_INFO("  Good X->VV  " << m_nGoodHtoVV);
  if (m_nHtoVV != 0) ATH_MSG_INFO("  V->child Fraction   " << double(m_nGoodHtoVV)/double(m_nHtoVV));
  if (m_nHtoVV != 0 && m_SameChargeParent) ATH_MSG_INFO("  V->child and same charge V Fraction   " << double(m_nGoodHtoVVSameCharge)/double(m_nHtoVV));
  return StatusCode::SUCCESS;
}


StatusCode TtHtoVVDecayFilter::filterEvent() {
  int  nGoodParent = 0;
  int  nGoodDecayedParent = 0;
  bool sameCharge = true;
  int  charge = -2;

  for (McEventCollection::const_iterator itr = events()->begin(); itr != events()->end(); ++itr) {
    // Loop over all particles in the event
    const HepMC::GenEvent* genEvt = (*itr);
#ifdef HEPMC3
    for (auto pitr: *genEvt) {
      for (size_t iParent=0; iParent < m_PDGParent.size(); iParent++) {
        if ( std::abs(pitr->pdg_id()) == m_PDGParent[iParent] && pitr->status() == m_StatusParent[iParent]) { // if find a parent here
          bool isGrandParentOK = false;
          for (auto thisMother: pitr->production_vertex()->particles_in()) { // Loop over all grandparent of this founded parent
            ATH_MSG_DEBUG("Parent " << pitr->pdg_id() << " barcode = "   << HepMC::barcode(pitr) << " status = "  << pitr->status());
            ATH_MSG_DEBUG("A parent mother "  << thisMother->pdg_id()<< " barcode = " << HepMC::barcode(thisMother));
            for (size_t iGrandParent = 0; iGrandParent < m_PDGGrandParent.size(); iGrandParent++) { // Loop over all Grandparents we want to check
              if ( thisMother->pdg_id() == m_PDGGrandParent[iGrandParent] ) isGrandParentOK = true;
            }
          }
          ATH_MSG_DEBUG(" Grandparent is OK? " << isGrandParentOK);
          if (!isGrandParentOK) continue;

          ++nGoodParent;  // good parent from the indicated GrandParents
          const bool okPDGChild = findAncestor(pitr->end_vertex(), m_PDGParent[iParent]);
          ATH_MSG_DEBUG("Result " << nGoodParent << " " << okPDGChild );
          if (!(okPDGChild)) continue;

          ++nGoodDecayedParent;  // good parent has the indicated child and child2
          if (nGoodDecayedParent == 1) {
            if ( pitr->pdg_id() < 0) charge = -1;
            else if ( pitr->pdg_id() > 0) charge = 1;
          } else {
            if (charge == -1 && pitr->pdg_id() > 0) sameCharge = false;
            if (charge ==  1 && pitr->pdg_id() < 0) sameCharge = false;
          }
        }
      } // End loop over all Parents we want to check
    } // End loop over all particles in the event
#else
    for (HepMC::GenEvent::particle_const_iterator pitr = genEvt->particles_begin(); pitr != genEvt->particles_end(); ++pitr) {

      for (size_t iParent=0; iParent < m_PDGParent.size(); iParent++) {
        if ( std::abs((*pitr)->pdg_id()) == m_PDGParent[iParent] && (*pitr)->status() == m_StatusParent[iParent]) { // if find a parent here
          HepMC::GenVertex::particle_iterator firstMother = (*pitr)->production_vertex()->particles_begin(HepMC::parents);
          HepMC::GenVertex::particle_iterator endMother = (*pitr)->production_vertex()->particles_end(HepMC::parents);
          HepMC::GenVertex::particle_iterator thisMother = firstMother;
          bool isGrandParentOK = false;
          for (; thisMother != endMother; ++thisMother) { // Loop over all grandparent of this founded parent
            ATH_MSG_DEBUG("Parent " << (*pitr)->pdg_id() << " barcode = "   << (*pitr)->barcode() << " status = "  << (*pitr)->status());
            ATH_MSG_DEBUG("A parent mother "  << (*thisMother)->pdg_id()<< " barcode = " << (*thisMother)->barcode());
            for (size_t iGrandParent = 0; iGrandParent < m_PDGGrandParent.size(); iGrandParent++) { // Loop over all Grandparents we want to check
              if ( (*thisMother)->pdg_id() == m_PDGGrandParent[iGrandParent] ) isGrandParentOK = true;
            }
          }
          ATH_MSG_DEBUG(" Grandparent is OK? " << isGrandParentOK);
          if (!isGrandParentOK) continue;

          ++nGoodParent;  // good parent from the indicated GrandParents
          const bool okPDGChild = findAncestor((*pitr)->end_vertex(), m_PDGParent[iParent]);
          ATH_MSG_DEBUG("Result " << nGoodParent << " " << okPDGChild );
          if (!(okPDGChild)) continue;

          ++nGoodDecayedParent;  // good parent has the indicated child and child2
          if (nGoodDecayedParent == 1) {
            if ( (*pitr)->pdg_id() < 0) charge = -1;
            else if ( (*pitr)->pdg_id() > 0) charge = 1;
          } else {
            if (charge == -1 && (*pitr)->pdg_id() > 0) sameCharge = false;
            if (charge ==  1 && (*pitr)->pdg_id() < 0) sameCharge = false;
          }

        }
      } // End loop over all Parents we want to check

    } // End loop over all particles in the event
#endif
  }

  ATH_MSG_DEBUG("Selected # good parents = " << nGoodParent << "; # good decayed parents = " << nGoodDecayedParent);

  ++m_nHtoVV;
  if (nGoodDecayedParent == m_TotalParent) {
    ++m_nGoodHtoVV;
    if (m_SameChargeParent) { // if require same charge, check same charge flag
      if (sameCharge) {
        ++m_nGoodHtoVVSameCharge;
        return StatusCode::SUCCESS;
      }
    } else { // if not require same charge, just accept this event
      return StatusCode::SUCCESS;
    }
  }

  // If we get here we have failed
  setFilterPassed(false);
  return StatusCode::SUCCESS;
}


bool TtHtoVVDecayFilter::findAncestor(HepMC::ConstGenVertexPtr searchvertex,
                                      int targetPDGID) {
  if (!searchvertex) return false;
  for (HepMC::ConstGenParticlePtr anc:  *searchvertex) {
    if (std::abs(anc->pdg_id()) == targetPDGID) { // same particle as parent
      return findAncestor(anc->end_vertex(),
                          targetPDGID);
    } else {
      for (size_t i = 0; i < m_PDGChild.size(); ++i) {
        if (std::abs(anc->pdg_id()) == m_PDGChild[i]) return true;
      }
    }
  }
  return false;
}
