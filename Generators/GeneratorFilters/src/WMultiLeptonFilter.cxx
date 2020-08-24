/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeneratorFilters/WMultiLeptonFilter.h"

WMultiLeptonFilter::WMultiLeptonFilter(const std::string& name, ISvcLocator* pSvcLocator)
  : GenFilter(name,pSvcLocator),
    m_H_WPlus_e(0), m_H_WMinus_e(0), 
    m_H_WPlus_m(0), m_H_WMinus_m(0), 
    m_T_WPlus_e(0), m_T_WMinus_e(0), 
    m_T_WPlus_m(0), m_T_WMinus_m(0)
{
  declareProperty("NLeptonsH",m_NLeptonsH = 2);
  declareProperty("NLeptonsT",m_NLeptonsT = 1);
}


StatusCode WMultiLeptonFilter::filterInitialize() {
  // 8 counters for branch ratio check
  m_H_WPlus_e = 0, m_H_WMinus_e = 0;
  m_H_WPlus_m = 0, m_H_WMinus_m = 0;
  m_T_WPlus_e = 0, m_T_WMinus_e = 0;
  m_T_WPlus_m = 0, m_T_WMinus_m = 0;
  return StatusCode::SUCCESS;
}


StatusCode WMultiLeptonFilter::filterFinalize() {
  ATH_MSG_INFO("statistics of H->W->lep and Top->W->lep ");
  ATH_MSG_INFO(" m_H_WPlus_e:\t " << m_H_WPlus_e << "\t m_H_WMinus_e:\t " << m_H_WMinus_e);
  ATH_MSG_INFO(" m_H_WPlus_m:\t " << m_H_WPlus_m << "\t m_H_WMinus_m:\t " << m_H_WMinus_m);
  ATH_MSG_INFO(" m_T_WPlus_e:\t " << m_T_WPlus_e << "\t m_T_WMinus_e:\t " << m_T_WMinus_e);
  ATH_MSG_INFO(" m_T_WPlus_m:\t " << m_T_WPlus_m << "\t m_T_WMinus_m:\t " << m_T_WMinus_m);
  return StatusCode::SUCCESS;
}


StatusCode WMultiLeptonFilter::filterEvent() {
  McEventCollection::const_iterator itr;
  int NLeptonsH = 0, NLeptonsT = 0;
  for (itr = events()->begin(); itr!=events()->end(); ++itr) {
    // Loop over all particles in the event
    const HepMC::GenEvent* genEvt = (*itr);
#ifdef HEPMC3
    for (auto pitr: genEvt->particles()) {
      if ( std::abs(pitr->pdg_id())!= 24 || pitr->status() != 3) continue;
        bool fromhiggs = false;
        bool fromtop   = false;
        for( auto thisMother: pitr->production_vertex()->particles_in()) {
          ATH_MSG_DEBUG(" W barcode "   << HepMC::barcode(pitr) << "  status = "  << pitr->status());
          ATH_MSG_DEBUG(" a W mother "  << thisMother->pdg_id()<< " barcode = " << HepMC::barcode(thisMother));
          if ( thisMother->pdg_id() == 25 )     fromhiggs  = true;
          if ( std::abs(thisMother->pdg_id()) == 6 ) fromtop    = true;
          ATH_MSG_DEBUG(" W from " << fromhiggs << "  "  << fromtop);
        }
        for (auto thisChild: pitr->end_vertex()->particles_out()) {
          if   (std::abs(thisChild->pdg_id()) == 11)  {
            if (fromhiggs) NLeptonsH++;
            if (fromtop)   NLeptonsT++;
            if (fromhiggs && (thisChild->pdg_id() == -11) )  m_H_WPlus_e++;
            if (fromhiggs && (thisChild->pdg_id() ==  11) )  m_H_WMinus_e++;
            if (fromtop   && (thisChild->pdg_id() == -11) )  m_T_WPlus_e++;
            if (fromtop   && (thisChild->pdg_id() ==  11) )  m_T_WMinus_e++;
            ATH_MSG_DEBUG(" electron from W ");
          }

          if   (std::abs(thisChild->pdg_id()) == 13)  {
            if (fromhiggs) NLeptonsH++;
            if (fromtop)   NLeptonsT++;
            if (fromhiggs && (thisChild->pdg_id() == -13) )  m_H_WPlus_m++;
            if (fromhiggs && (thisChild->pdg_id() ==  13) )  m_H_WMinus_m++;
            if (fromtop   && (thisChild->pdg_id() == -13) )  m_T_WPlus_m++;
            if (fromtop   && (thisChild->pdg_id() ==  13) )  m_T_WMinus_m++;
            ATH_MSG_DEBUG(" muon from W ");
          }
        }
	}
#else
    for (HepMC::GenEvent::particle_const_iterator pitr = genEvt->particles_begin(); pitr != genEvt->particles_end(); ++pitr) {
      if ( abs((*pitr)->pdg_id()) == 24 && (*pitr)->status() == 3) {
        HepMC::GenVertex::particle_iterator firstMother = (*pitr)->production_vertex()->particles_begin(HepMC::parents);
        HepMC::GenVertex::particle_iterator endMother = (*pitr)->production_vertex()->particles_end(HepMC::parents);
        HepMC::GenVertex::particle_iterator thisMother = firstMother;
        bool fromhiggs = false;
        bool fromtop   = false;
        for(; thisMother != endMother; ++thisMother) {
          ATH_MSG_DEBUG(" W barcode "   << (*pitr)->barcode() << "  status = "  << (*pitr)->status());
          ATH_MSG_DEBUG(" a W mother "  << (*thisMother)->pdg_id()<< " barc = " << (*thisMother)->barcode());
          if ( (*thisMother)->pdg_id() == 25 )     fromhiggs  = true;
          if ( abs((*thisMother)->pdg_id()) == 6 ) fromtop    = true;
          ATH_MSG_DEBUG(" W from " << fromhiggs << "  "  << fromtop);
        }
        HepMC::GenVertex::particle_iterator firstChild = (*pitr)->end_vertex()->particles_begin(HepMC::children);
        HepMC::GenVertex::particle_iterator endChild = (*pitr)->end_vertex()->particles_end(HepMC::children);
        HepMC::GenVertex::particle_iterator thisChild = firstChild;
        for (; thisChild != endChild; ++thisChild) {
          if ( (abs((*thisChild)->pdg_id()) == 11) ) {
            if (fromhiggs) NLeptonsH++;
            if (fromtop)   NLeptonsT++;
            if (fromhiggs && ((*thisChild)->pdg_id() == -11) )  m_H_WPlus_e++;
            if (fromhiggs && ((*thisChild)->pdg_id() ==  11) )  m_H_WMinus_e++;
            if (fromtop   && ((*thisChild)->pdg_id() == -11) )  m_T_WPlus_e++;
            if (fromtop   && ((*thisChild)->pdg_id() ==  11) )  m_T_WMinus_e++;
            ATH_MSG_DEBUG(" electron from W ");
          }

          if ( (abs((*thisChild)->pdg_id()) == 13) ) {
            if (fromhiggs) NLeptonsH++;
            if (fromtop)   NLeptonsT++;
            if (fromhiggs && ((*thisChild)->pdg_id() == -13) )  m_H_WPlus_m++;
            if (fromhiggs && ((*thisChild)->pdg_id() ==  13) )  m_H_WMinus_m++;
            if (fromtop   && ((*thisChild)->pdg_id() == -13) )  m_T_WPlus_m++;
            if (fromtop   && ((*thisChild)->pdg_id() ==  13) )  m_T_WMinus_m++;
            ATH_MSG_DEBUG(" muon from W ");
          }
        }
      }
    }
#endif
  }
  // NB. no taus!

  ATH_MSG_DEBUG("Found " << NLeptonsH << " Leptons from W from Higgs");
  ATH_MSG_DEBUG("Found " << NLeptonsT << " Leptons from W from Top");
  setFilterPassed(NLeptonsH == m_NLeptonsH && NLeptonsT == m_NLeptonsT);
  return StatusCode::SUCCESS;
}
