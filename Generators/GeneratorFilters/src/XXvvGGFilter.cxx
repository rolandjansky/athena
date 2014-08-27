/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeneratorFilters/XXvvGGFilter.h"

XXvvGGFilter::XXvvGGFilter(const std::string& name, ISvcLocator* pSvcLocator)
  : GenFilter(name, pSvcLocator)
{
  declareProperty("Ptcut",m_Ptmin = .0);
  declareProperty("Etacut",m_EtaRange = 2.7);
  declareProperty("GammaZ",m_gammaZ = 1);
  declareProperty("ZZ",m_ZZ = 0);
}


StatusCode XXvvGGFilter::filterEvent() {
  McEventCollection::const_iterator itr;
  for (itr = events()->begin(); itr!=events()->end(); ++itr) {
    int N_p = 0;
    int N_z = 0;
    int N_l = 0;
    int N_g = 0;
    const HepMC::GenEvent* genEvt = (*itr);
    HepMC::GenEvent::particle_const_iterator pitr=genEvt->particles_begin();
    HepMC::GenEvent::particle_const_iterator pitr_end=genEvt->particles_end();
    for (; pitr != pitr_end; ++pitr) {
      if ((*pitr)->pdg_id() == 1000022 && (*pitr)->status() == 155) { //< @todo How safe/portable is this?
        HepMC::GenVertex::particle_iterator firstChild, thisChild, lastChild;
        firstChild = (*pitr)->end_vertex()->particles_begin(HepMC::children);
        lastChild = (*pitr)->end_vertex()->particles_end(HepMC::children);
        thisChild = firstChild;
        for (; thisChild != lastChild; ++thisChild) {
          if ((*thisChild)->pdg_id() == 22) N_p++;
          if ((*thisChild)->pdg_id() == 1000039) N_g++;
          if ((*thisChild)->pdg_id() == 23) {
            N_z++;
            if (!((*thisChild)->end_vertex())) continue;
            HepMC::GenVertex::particle_iterator ZfirstChild, ZthisChild, ZlastChild;
            ZfirstChild = (*thisChild)->end_vertex()->particles_begin(HepMC::children);
            ZlastChild = (*thisChild)->end_vertex()->particles_end(HepMC::children);
            ZthisChild = ZfirstChild;
            for (; ZthisChild != ZlastChild; ++ZthisChild) {
              if ((*ZthisChild)->pdg_id() != 23) continue;
              if (!((*ZthisChild)->end_vertex())) continue;
              HepMC::GenVertex::particle_iterator ZZfirstChild,ZZthisChild, ZZlastChild;
              ZZfirstChild = (*ZthisChild)->end_vertex()->particles_begin(HepMC::children);
              ZZlastChild = (*ZthisChild)->end_vertex()->particles_end(HepMC::children);
              ZZthisChild = ZZfirstChild;
              for (; ZZthisChild != ZZlastChild; ++ZZthisChild) {
                if (abs((*ZZthisChild)->pdg_id()) == 11 || abs((*ZZthisChild)->pdg_id()) == 13) {
                  if (fabs((*ZZthisChild)->momentum().eta())<m_EtaRange) N_l++;
                }
              }
            }
          }
        }
        if (m_gammaZ) {
          if (N_p == 1 && N_z == 1 && N_l == 2 && N_g == 2) {
            ATH_MSG_INFO(" Keep XX-->Z(ll)gammaGG event ");
            return StatusCode::SUCCESS;
          }
        }
        if (m_ZZ) {
          if (N_z == 2 && N_l == 4 && N_g == 2) {
            ATH_MSG_INFO(" Keep XX-->Z(ll)Z(ll)GG event ");
            return StatusCode::SUCCESS;
          }
        }
      }
    }
  }
  setFilterPassed(false);
  return StatusCode::SUCCESS;
}
