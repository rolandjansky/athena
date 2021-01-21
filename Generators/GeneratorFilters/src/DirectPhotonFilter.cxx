/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "GeneratorFilters/DirectPhotonFilter.h"
#include <limits>
#include <algorithm>

DirectPhotonFilter::DirectPhotonFilter(const std::string& name, ISvcLocator* pSvcLocator)
  : GenFilter(name, pSvcLocator)
{
  declareProperty("NPhotons", m_NPhotons = 1);
  declareProperty("OrderPhotons",m_OrderPhotons = true);
  declareProperty("Ptmin",m_Ptmin = std::vector<double>(m_NPhotons, 10000.));
  declareProperty("Ptmax",m_Ptmax = std::vector<double>(m_NPhotons, std::numeric_limits<double>::max()));
  declareProperty("Etacut", m_EtaRange = 2.50);
  declareProperty("AllowSUSYDecay",m_AllowSUSYDecay = false);

  // Backward compatibility aliases
  declareProperty("Ptcut", m_Ptmin);
}

StatusCode DirectPhotonFilter::filterInitialize() {

  ATH_MSG_INFO("Initialising DirectPhoton filter with OrderPhotons="<<m_OrderPhotons);

  if (m_Ptmin.size()>m_NPhotons || m_Ptmax.size()>m_NPhotons) {
    ATH_MSG_ERROR("Too many Ptmin/max for given NPhotons");
    return StatusCode::FAILURE;
  }

  if (!m_OrderPhotons && (m_Ptmin.size()>1 || m_Ptmax.size()>1)) {
    ATH_MSG_ERROR("Too many Ptmin/max requirements for OrderPhotons=false case.");
    return StatusCode::FAILURE;
  }

  // allow specifying only one pTmin/max to be applied to all (further) photons
  // for backward compatibility
  if (m_Ptmin.size()<m_NPhotons) {
    size_t origsize = m_Ptmin.size();
    double lastPt = m_Ptmin.back();
    m_Ptmin.resize(m_NPhotons);
    for (size_t i=origsize; i<m_NPhotons; ++i) m_Ptmin[i]=lastPt;
  }
  if (m_Ptmax.size()<m_NPhotons) {
    size_t origsize = m_Ptmax.size();
    double lastPt = m_Ptmax.back();
    m_Ptmax.resize(m_NPhotons);
    for (size_t i=origsize; i<m_NPhotons; ++i) m_Ptmax[i]=lastPt;
  }
  return StatusCode::SUCCESS;
}

bool DirectPhotonFilterCmpByPt(HepMC::GenParticle* p1, HepMC::GenParticle* p2) {
  return (p1->momentum().perp()>p2->momentum().perp());
}

StatusCode DirectPhotonFilter::filterEvent() {
  std::vector<HepMC::GenParticle*> promptPhotonsInEta;
  for (McEventCollection::const_iterator itr = events_const()->begin(); itr!=events_const()->end(); ++itr) {
    const HepMC::GenEvent* genEvt = (*itr);
    ATH_MSG_DEBUG("----->>> Process : " << genEvt->signal_process_id());

    // Find all prompt photons with within given eta range
    for (HepMC::GenEvent::particle_const_iterator pitr=genEvt->particles_begin(); pitr!=genEvt->particles_end(); ++pitr) {
      if ((*pitr)->pdg_id() == 22 &&
          (*pitr)->status() == 1 &&
          fabs((*pitr)->momentum().pseudoRapidity()) <= m_EtaRange) {
        
        // iterate over parent particles to exclude photons from hadron decays
        HepMC::GenVertex* prodVtx = (*pitr)->production_vertex();
        bool fromHadron(false);
        for (auto parent = prodVtx->particles_begin(HepMC::parents);
             parent != prodVtx->particles_end(HepMC::parents); ++parent) {
          int pdgindex =  abs((*parent)->pdg_id());
          ATH_MSG_DEBUG("Looping on Production (parents) vertex : " << (*parent)->pdg_id() << "  " << (*parent)->barcode());
          if (pdgindex > 100) {
            fromHadron = true;
            if (m_AllowSUSYDecay && ( (pdgindex > 1000000 && pdgindex < 1000040) || (pdgindex > 2000000 && pdgindex < 2000016) ) ) fromHadron = 
false;
          }
        }

        if (!fromHadron) promptPhotonsInEta.push_back((*pitr));
        else ATH_MSG_DEBUG("non-prompt photon ignored");
      }
    }
  }


  if (promptPhotonsInEta.size()<m_NPhotons) {
    setFilterPassed(false);
  }
  else {
    for (auto photon: promptPhotonsInEta) {
      ATH_MSG_DEBUG("Found prompt photon with pt="<<photon->momentum().perp());
    }
    if (m_OrderPhotons) { // apply cuts to leading/subleading/... photon as specified
      std::sort(promptPhotonsInEta.begin(), promptPhotonsInEta.end(), DirectPhotonFilterCmpByPt);

      bool pass = true;
      for (size_t i = 0; i < m_NPhotons; ++i) {
        double pt = promptPhotonsInEta[i]->momentum().perp();
        if (pt < m_Ptmin[i] || pt > m_Ptmax[i]) {
          ATH_MSG_DEBUG("  rejected pt="<<pt);
          pass = false;
        }
      }
      if (pass) ATH_MSG_DEBUG("Passed!");
      setFilterPassed(pass);
    }
    else { // just require NPhotons to pass m_Ptmin/max[0]
      size_t NPhotons=0;
      for (size_t i = 0; i < promptPhotonsInEta.size(); ++i) {
        double pt = promptPhotonsInEta[i]->momentum().perp();
        if (pt > m_Ptmin[0] && pt < m_Ptmax[0]) ++NPhotons;
      }
      if (NPhotons>=m_NPhotons) ATH_MSG_DEBUG("Passed!");
      setFilterPassed(NPhotons>=m_NPhotons);
    }
  }
  return StatusCode::SUCCESS;
}
