/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration 
*/
#include "GeneratorFilters/SplitPhotonFilter.h"


SplitPhotonFilter::SplitPhotonFilter(const std::string& name, ISvcLocator* pSvcLocator)
  : GenFilter(name, pSvcLocator)
{
  declareProperty("Ptcut", m_Ptmin = 15000.);
  declareProperty("Etacut", m_EtaRange = 2.50);
  declareProperty("NPhotons", m_NPhotons = 1);
  declareProperty("AcceptedSplit", m_dauPdg);
}


StatusCode SplitPhotonFilter::filterEvent() {
  int NPhotons = 0;
  bool GoodFlav = m_dauPdg.size() == 0 ? true : false;
  McEventCollection::const_iterator itr;
  for (itr = events_const()->begin(); itr!=events_const()->end(); ++itr) {
    const HepMC::GenEvent* genEvt = (*itr);
    ATH_MSG_VERBOSE("----->>> Process : " << genEvt->signal_process_id());

    for (HepMC::GenEvent::particle_const_iterator pitr=genEvt->particles_begin(); pitr!=genEvt->particles_end(); ++pitr) {

      // Check for a photon with desired kinematics
      if ( ((*pitr)->pdg_id() == 22) ) {
        if ( ((*pitr)->momentum().perp() >= m_Ptmin) &&
             fabs((*pitr)->momentum().pseudoRapidity()) <= m_EtaRange) {

	  // First find a direct photon (not from hadron decay)
          bool fromHadron(false);
          HepMC::GenVertex::particle_iterator firstChild1 = (*pitr)->production_vertex()->particles_begin(HepMC::parents);
          HepMC::GenVertex::particle_iterator endChild1   = (*pitr)->production_vertex()->particles_end(HepMC::parents);
          HepMC::GenVertex::particle_iterator thisChild1 = firstChild1;
          for (; thisChild1 != endChild1; ++thisChild1) {
            int pdgindex =  abs((*thisChild1)->pdg_id());
            ATH_MSG_VERBOSE("Looping on Production (parents) vertex : " << (*thisChild1)->pdg_id() << "  " << (*thisChild1)->barcode());
            if (pdgindex > 100) {
              fromHadron = true;
	      break;
            }
          }
          if (fromHadron)
	    continue;

	  if ( (*pitr)->end_vertex() && (*pitr)->end_vertex()->particles_out_size() > 1 ) {
	    ATH_MSG_DEBUG("A split photon");
	    HepMC::GenVertex::particle_iterator dauBegin = (*pitr)->end_vertex()->particles_begin(HepMC::children);
	    HepMC::GenVertex::particle_iterator dauEnd   = (*pitr)->end_vertex()->particles_end(HepMC::children);
	    HepMC::GenVertex::particle_iterator dau = dauBegin;
	    for (; dau != dauEnd; dau++) {
	      int pdgid = (*dau)->pdg_id();
	      if (std::find(m_dauPdg.begin(),m_dauPdg.end(),abs(pdgid)) != m_dauPdg.end())
		//Argh I should break here... anyway should not waste too much time to continue the loop even when a good daughter is found
		GoodFlav = true; 
	      ATH_MSG_DEBUG("Daughter : pdg = " << pdgid);
	    }
	    NPhotons++;
	  }

        } // kinematic requirements
      }   // photon
    }
  }

  if (NPhotons >= m_NPhotons && GoodFlav) return StatusCode::SUCCESS;
  setFilterPassed(false);
  return StatusCode::SUCCESS;
}
