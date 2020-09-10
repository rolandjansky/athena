/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeneratorFilters/ZtoLeptonFilter.h"

ZtoLeptonFilter::ZtoLeptonFilter(const std::string& name, ISvcLocator* pSvcLocator)
  : GenFilter(name, pSvcLocator)
{
  declareProperty("Ptcut",m_Ptmin = .0);
  declareProperty("Etacut",m_EtaRange = 10.0);
}


StatusCode ZtoLeptonFilter::filterEvent() {
  McEventCollection::const_iterator itr;
  for (itr = events()->begin(); itr!=events()->end(); ++itr) {
    const HepMC::GenEvent* genEvt = (*itr);
#ifdef HEPMC3
    for ( auto pitr: genEvt->particles()) {
      if (pitr->pdg_id() == 23) {
        if ( !pitr->end_vertex() && pitr->status()==3) continue; // Allow status 3 Zs with no end vertex
        else if (!pitr->end_vertex() ){
          // Found a Z boson with no end vertex and status!=3 .  Something is sick about this event
          break;
        }
        // Z children
        for (auto thisChild: pitr->end_vertex()->particles_out()) {
          if (std::abs(thisChild->pdg_id()) == 11 || std::abs(thisChild->pdg_id()) == 13 || std::abs(thisChild->pdg_id()) == 15) {
            return StatusCode::SUCCESS;
          }
        }
      }
    }
#else
    for (HepMC::GenEvent::particle_const_iterator pitr = genEvt->particles_begin();	pitr != genEvt->particles_end(); ++pitr) {
      if (((*pitr)->pdg_id()) == 23) {
        if ( !(*pitr)->end_vertex() && (*pitr)->status()==3) continue; // Allow status 3 Zs with no end vertex
        else if ( !(*pitr)->end_vertex() ){
          // Found a Z boson with no end vertex and status!=3 .  Something is sick about this event
          break;
        }
        // Z children
        HepMC::GenVertex::particle_iterator firstChild = (*pitr)->end_vertex()->particles_begin(HepMC::children);
        HepMC::GenVertex::particle_iterator endChild = (*pitr)->end_vertex()->particles_end(HepMC::children);
        HepMC::GenVertex::particle_iterator thisChild = firstChild;
        for (; thisChild != endChild; ++thisChild) {
          if (std::abs((*thisChild)->pdg_id()) == 11 || std::abs((*thisChild)->pdg_id()) == 13 || std::abs((*thisChild)->pdg_id()) == 15) {
            return StatusCode::SUCCESS;
          }
        }
      }
    }
#endif
  }
  setFilterPassed(false);
  return StatusCode::SUCCESS;
}
