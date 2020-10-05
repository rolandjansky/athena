/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeneratorFilters/HtoVVFilter.h"

HtoVVFilter::HtoVVFilter(const std::string& name, ISvcLocator* pSvcLocator)
  : GenFilter(name,pSvcLocator)
{
  /// @todo
  static const int ch1[] =  { 11,12,13,14,15,16 };
  static const int ch2[] =  { 11,12,13,14,15,16 };
  static const std::vector<int> chv1(ch1, ch1 + sizeof(ch1) / sizeof(int));
  static const std::vector<int> chv2(ch2, ch2 + sizeof(ch2) / sizeof(int));

  declareProperty("PDGGrandParent",m_PDGGrandParent = 25);
  declareProperty("PDGParent",m_PDGParent = 24);
  declareProperty("PDGChild1",m_PDGChild1 = chv1);
  declareProperty("PDGChild2",m_PDGChild2 = chv2);

  m_nHtoVV = 0;
  m_nGoodHtoVV = 0;
}


StatusCode HtoVVFilter::filterInitialize() {
  ATH_MSG_INFO("PDGGrandParent(H) = " << m_PDGGrandParent);
  ATH_MSG_INFO("PDGParent(V)      = " << m_PDGParent);
  if (m_PDGChild1.size()==0)
    ATH_MSG_ERROR("PDGChild1[] not set ");
  if (m_PDGChild2.size()==0)
    ATH_MSG_ERROR("PDGChild2[] not set ");
  for (size_t i = 0; i < m_PDGChild1.size(); ++i) ATH_MSG_INFO("PDGChild1["<<i<<"] = " << m_PDGChild1[i]);
  for (size_t i = 0; i < m_PDGChild2.size(); ++i) ATH_MSG_INFO("PDGChild2["<<i<<"] = " << m_PDGChild2[i]);
  // init
  m_nHtoVV = 0;
  m_nGoodHtoVV = 0;
  return StatusCode::SUCCESS;
}


StatusCode HtoVVFilter::filterFinalize() {
  ATH_MSG_INFO("Statistics of H->VV, V->decay ");
  ATH_MSG_INFO("  ALL  H->VV " << m_nHtoVV);
  ATH_MSG_INFO("  Good H->VV " << m_nGoodHtoVV);
  if (m_nHtoVV != 0) ATH_MSG_INFO("  Fraction   " << double(m_nGoodHtoVV)/double(m_nHtoVV));
  return StatusCode::SUCCESS;
}


StatusCode HtoVVFilter::filterEvent() {
  McEventCollection::const_iterator itr;
  bool okPDGChild1 = false;
  bool okPDGChild2 = false;
  int nGoodParent = 0;
  for (itr = events()->begin(); itr!=events()->end(); ++itr) {
    const HepMC::GenEvent* genEvt = (*itr);
#ifdef HEPMC3
    for (auto pitr: genEvt->particles()) {
      if (std::abs(pitr->pdg_id()) != m_PDGParent || pitr->status() != 3)  continue;
        bool isGrandParentOK = false;
        for (auto thisMother: pitr->production_vertex()->particles_in()) {
          ATH_MSG_DEBUG(" Parent " << pitr->pdg_id() << " barcode = "   << HepMC::barcode(pitr) << " status = "  << pitr->status());
          ATH_MSG_DEBUG(" a Parent mother "  << thisMother->pdg_id()<< " barc = " << HepMC::barcode(thisMother));
          if ( thisMother->pdg_id() == m_PDGGrandParent ) isGrandParentOK = true;
        }
        ATH_MSG_DEBUG(" Grand Parent is OK? " << isGrandParentOK);
        if (!isGrandParentOK) continue;
        ++nGoodParent;
        for (auto thisChild: pitr->end_vertex()->particles_out()) {
          ATH_MSG_DEBUG(" child " << thisChild->pdg_id());
          if (!okPDGChild1) {
             if (find(m_PDGChild1.begin(),m_PDGChild1.end(),std::abs(thisChild->pdg_id()))!=m_PDGChild1.end()) okPDGChild1 = true;
          }
          if (!okPDGChild2) {
           if (find(m_PDGChild2.begin(),m_PDGChild2.end(),std::abs(thisChild->pdg_id()))!=m_PDGChild2.end()) okPDGChild2 = true;
          }
        }
    }
#else
    for (HepMC::GenEvent::particle_const_iterator pitr = genEvt->particles_begin(); pitr != genEvt->particles_end(); ++pitr) {
      if (std::abs((*pitr)->pdg_id()) == m_PDGParent && (*pitr)->status() == 3) {
        HepMC::GenVertex::particle_iterator firstMother = (*pitr)->production_vertex()->particles_begin(HepMC::parents);
        HepMC::GenVertex::particle_iterator endMother = (*pitr)->production_vertex()->particles_end(HepMC::parents);
        HepMC::GenVertex::particle_iterator thisMother = firstMother;
        bool isGrandParentOK = false;
        for (; thisMother != endMother; ++thisMother) {
          ATH_MSG_DEBUG(" Parent " << (*pitr)->pdg_id() << " barcode = "   << (*pitr)->barcode() << " status = "  << (*pitr)->status());
          ATH_MSG_DEBUG(" a Parent mother "  << (*thisMother)->pdg_id()<< " barc = " << (*thisMother)->barcode());
          if ( (*thisMother)->pdg_id() == m_PDGGrandParent ) isGrandParentOK = true;
        }
        ATH_MSG_DEBUG(" Grand Parent is OK? " << isGrandParentOK);
        if (!isGrandParentOK) continue;
        ++nGoodParent;

        HepMC::GenVertex::particle_iterator firstChild = (*pitr)->end_vertex()->particles_begin(HepMC::children);
        HepMC::GenVertex::particle_iterator endChild = (*pitr)->end_vertex()->particles_end(HepMC::children);
        HepMC::GenVertex::particle_iterator thisChild = firstChild;
        for (; thisChild != endChild; ++thisChild) {
          ATH_MSG_DEBUG(" child " << (*thisChild)->pdg_id());
          if (!okPDGChild1) {
            for (size_t i = 0; i < m_PDGChild1.size(); ++i)
              if (std::abs((*thisChild)->pdg_id()) == m_PDGChild1[i]) okPDGChild1 = true;
            if (okPDGChild1) break;
          }
          if (!okPDGChild2) {
            for (size_t i = 0; i < m_PDGChild2.size(); ++i)
              if (std::abs((*thisChild)->pdg_id()) == m_PDGChild2[i]) okPDGChild2 = true;
            if (okPDGChild2) break;
          }
        }
      }
    }
#endif
  }

  ATH_MSG_DEBUG("Result " << nGoodParent << " " << okPDGChild1 << " " << okPDGChild2);

  if (nGoodParent == 2) ++m_nHtoVV;
  if (nGoodParent == 2 && okPDGChild1 && okPDGChild2) {
    ++m_nGoodHtoVV;
    return StatusCode::SUCCESS;
  }

  setFilterPassed(false);
  return StatusCode::SUCCESS;
}
