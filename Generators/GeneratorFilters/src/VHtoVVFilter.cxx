/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeneratorFilters/VHtoVVFilter.h"


VHtoVVFilter::VHtoVVFilter(const std::string& name, ISvcLocator* pSvcLocator)
  : GenFilter(name, pSvcLocator)
{
  declareProperty("PDGGrandParent", m_PDGGrandParent);
  declareProperty("PDGParent", m_PDGParent);
  declareProperty("PDGHVChild1", m_PDGHVChild1);
  declareProperty("PDGHVChild2", m_PDGHVChild2);
  declareProperty("PDGAssocVChild", m_PDGAssocVChild);
  
  m_nVHtoVV = 0;
  m_nGoodVHtoVV = 0;
}


StatusCode VHtoVVFilter::filterInitialize() {
  ATH_MSG_INFO("PDGGrandParent(H) = " << m_PDGGrandParent);
  ATH_MSG_INFO("PDGParent(V)      = " << m_PDGParent);
  if (m_PDGHVChild1.empty()) ATH_MSG_ERROR("PDGHVChild1[] not set ");
  if (m_PDGHVChild2.empty()) ATH_MSG_ERROR("PDGHVChild2[] not set ");
  if (m_PDGAssocVChild.empty()) ATH_MSG_ERROR("PDGAssocVChild[] not set ");
  for (size_t i = 0; i < m_PDGHVChild1.size(); ++i)
    ATH_MSG_INFO("PDGHVChild1[" << i << "] = " << m_PDGHVChild1[i]);
  for (size_t i = 0; i < m_PDGHVChild2.size(); ++i)
    ATH_MSG_INFO("PDGHVChild2[" << i << "] = " << m_PDGHVChild2[i]);
  for (size_t i = 0; i < m_PDGAssocVChild.size(); ++i)
    ATH_MSG_INFO("PDGAssocVChild[" << i << "] = " << m_PDGAssocVChild[i]);

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
  bool okPDGHVChild1 = false;
  bool okPDGHVChild2 = false;
  bool okPDGAssocVChild = false;
  int nHiggsParent = 0;
  int nVParent = 0;

  McEventCollection::const_iterator itr;
  for (itr = events()->begin(); itr != events()->end(); ++itr) {
    // Loop over all particles in the event
    const HepMC::GenEvent* genEvt = (*itr);
#ifdef HEPMC3
    for (auto pitr: genEvt->particles()) {
      // Loop over particles from the primary interaction that match the PDG Parent
      if ( std::abs(pitr->pdg_id()) != m_PDGParent || pitr->status() != 3) continue;
        bool isGrandParentHiggs = false;
        bool isGrandParentV = false;
        for ( auto thisMother: pitr->production_vertex()->particles_in()) { //loop over chain of grandparents
          ATH_MSG_DEBUG(" Parent " << pitr->pdg_id() << " barcode = "   << HepMC::barcode(pitr) << " status = "  << pitr->status());
          ATH_MSG_DEBUG(" a Parent mother "  << thisMother->pdg_id()<< " barc = " << HepMC::barcode(thisMother));
          if ( thisMother->pdg_id() == m_PDGGrandParent ) isGrandParentHiggs = true; else isGrandParentV = true;
        }
        ATH_MSG_DEBUG(" Grand Parent is Higgs? " << isGrandParentHiggs);
        ATH_MSG_DEBUG(" Grand Parent is V? " << isGrandParentV);

        if (!isGrandParentHiggs && !isGrandParentV) continue;

        if (isGrandParentHiggs) {
          ++nHiggsParent;
          for (auto thisChild:pitr->end_vertex()->particles_out()) {
            ATH_MSG_DEBUG(" child " << thisChild->pdg_id());
            if (!okPDGHVChild1) {
              for (size_t i = 0; i < m_PDGHVChild1.size(); ++i)
                if (abs(thisChild->pdg_id()) == m_PDGHVChild1[i]) okPDGHVChild1 = true;
              if (okPDGHVChild1) break;
            }
            if (!okPDGHVChild2) {
              for (size_t i = 0; i < m_PDGHVChild2.size(); ++i)
                if (std::abs(thisChild->pdg_id()) == m_PDGHVChild2[i]) okPDGHVChild2 = true;
              if (okPDGHVChild2) break;
            }
          }
        } //end of higgs grandparent loop

        if (isGrandParentV) {
          ++nVParent;
          for (auto thisChild: pitr->end_vertex()->particles_out()) {
            ATH_MSG_DEBUG(" child " << thisChild->pdg_id());
            if (!okPDGAssocVChild) {
              for (unsigned int i=0;i<m_PDGAssocVChild.size();++i)
                if (std::abs(thisChild->pdg_id()) == m_PDGAssocVChild[i]) okPDGAssocVChild = true;
              if (okPDGAssocVChild) break;
            }

          }
      }     
   }
#else
    for (HepMC::GenEvent::particle_const_iterator pitr = genEvt->particles_begin(); pitr != genEvt->particles_end(); ++pitr ) {
      // Loop over particles from the primary interaction that match the PDG Parent
      if ( abs((*pitr)->pdg_id()) == m_PDGParent && (*pitr)->status() == 3) {
        HepMC::GenVertex::particle_iterator firstMother = (*pitr)->production_vertex()->particles_begin(HepMC::parents);
        HepMC::GenVertex::particle_iterator endMother = (*pitr)->production_vertex()->particles_end(HepMC::parents);
        HepMC::GenVertex::particle_iterator thisMother = firstMother;
        bool isGrandParentHiggs = false;
        bool isGrandParentV = false;
        for (; thisMother != endMother; ++thisMother) { //loop over chain of grandparents
          ATH_MSG_DEBUG(" Parent " << (*pitr)->pdg_id() << " barcode = "   << (*pitr)->barcode() << " status = "  << (*pitr)->status());
          ATH_MSG_DEBUG(" a Parent mother "  << (*thisMother)->pdg_id()<< " barc = " << (*thisMother)->barcode());
          if ( (*thisMother)->pdg_id() == m_PDGGrandParent ) isGrandParentHiggs = true; else isGrandParentV = true;
        }
        ATH_MSG_DEBUG(" Grand Parent is Higgs? " << isGrandParentHiggs);
        ATH_MSG_DEBUG(" Grand Parent is V? " << isGrandParentV);

        if (!isGrandParentHiggs && !isGrandParentV) continue;

        if (isGrandParentHiggs) {
          ++nHiggsParent;
          HepMC::GenVertex::particle_iterator firstChild = (*pitr)->end_vertex()->particles_begin(HepMC::children);
          HepMC::GenVertex::particle_iterator endChild = (*pitr)->end_vertex()->particles_end(HepMC::children);
          HepMC::GenVertex::particle_iterator thisChild = firstChild;
          for (; thisChild != endChild; ++thisChild) {
            ATH_MSG_DEBUG(" child " << (*thisChild)->pdg_id());
            if (!okPDGHVChild1) {
              for (size_t i = 0; i < m_PDGHVChild1.size(); ++i)
                if (abs((*thisChild)->pdg_id()) == m_PDGHVChild1[i]) okPDGHVChild1 = true;
              if (okPDGHVChild1) break;
            }
            if (!okPDGHVChild2) {
              for (size_t i = 0; i < m_PDGHVChild2.size(); ++i)
                if (abs((*thisChild)->pdg_id()) == m_PDGHVChild2[i]) okPDGHVChild2 = true;
              if (okPDGHVChild2) break;
            }
          }
        } //end of higgs grandparent loop

        if (isGrandParentV) {
          ++nVParent;
          HepMC::GenVertex::particle_iterator firstChild = (*pitr)->end_vertex()->particles_begin(HepMC::children);
          HepMC::GenVertex::particle_iterator endChild = (*pitr)->end_vertex()->particles_end(HepMC::children);
          HepMC::GenVertex::particle_iterator thisChild = firstChild;
          for (; thisChild != endChild; ++thisChild) {
            ATH_MSG_DEBUG(" child " << (*thisChild)->pdg_id());
            if (!okPDGAssocVChild) {
              for (unsigned int i=0;i<m_PDGAssocVChild.size();++i)
                if (abs((*thisChild)->pdg_id()) == m_PDGAssocVChild[i]) okPDGAssocVChild = true;
              if (okPDGAssocVChild) break;
            }

          }
        }  //end of v grandparent loop


      } //end good parent loop
    }
#endif    
  }

  ATH_MSG_DEBUG("Result " << nHiggsParent << " " << okPDGHVChild1 << " " << okPDGHVChild2);
  ATH_MSG_DEBUG("Result " << nVParent << " " << okPDGAssocVChild);

  if (nHiggsParent == 2 && nVParent >= 1) ++m_nVHtoVV;

  if (nHiggsParent == 2 && okPDGHVChild1 && okPDGHVChild2 && nVParent==1 && okPDGAssocVChild) {
    ++m_nGoodVHtoVV;
    return StatusCode::SUCCESS;
  }

  // If we get here we have failed
  setFilterPassed(false);
  return StatusCode::SUCCESS;
}
