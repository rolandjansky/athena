/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeneratorFilters/TTbarFilter.h"
#include "CLHEP/Units/SystemOfUnits.h"

TTbarFilter::TTbarFilter(const std::string& name, ISvcLocator* pSvcLocator)
  : GenFilter(name,pSvcLocator)
{
  declareProperty("Ptcut",m_Ptmin = 200000.);
}


StatusCode TTbarFilter::filterEvent() {
  int N_quark_t    = 0;
  int N_quark_tbar = 0;
  int N_quark_t_all    = 0;
  int N_quark_tbar_all = 0;
  int N_pt_above_cut = 0;

  McEventCollection::const_iterator itr;
  for (itr = events()->begin(); itr!=events()->end(); ++itr) {
    const HepMC::GenEvent* genEvt = (*itr);
    HepMC::GenEvent::particle_const_iterator pitr = genEvt->particles_begin();
    for (; pitr!=genEvt->particles_end(); ++pitr) {
      if (fabs((*pitr)->pdg_id()) == 6) {
        if ( (*pitr)->pdg_id() ==  6 ) N_quark_t_all++;
        if ( (*pitr)->pdg_id() == -6 ) N_quark_tbar_all++;
        int n_daughters = 0;
        HepMC::GenParticle * mcpart = (*pitr);
        const HepMC::GenVertex * decayVtx = mcpart->end_vertex();
        if (decayVtx != 0) n_daughters = decayVtx->particles_out_size();

        // For this analysis we are not interested in t->t MC structures, only in decays
        if (n_daughters >= 2) {
          HepMC::GenVertex::particles_in_const_iterator child_mcpartItr  = decayVtx->particles_out_const_begin();
          HepMC::GenVertex::particles_in_const_iterator child_mcpartItrE = decayVtx->particles_out_const_end();
          for (; child_mcpartItr != child_mcpartItrE; ++child_mcpartItr) {
            HepMC::GenParticle * child_mcpart = (*child_mcpartItr);
            //  Implicitly, I assume that tops always decay to W X
            if (abs(child_mcpart->pdg_id()) == 24) {
              if ( (*pitr)->pdg_id() ==  6 ) N_quark_t++;
              if ( (*pitr)->pdg_id() == -6 ) N_quark_tbar++;
              if ((*pitr)->momentum().perp() >= m_Ptmin) N_pt_above_cut++;
            }
          }
        }
      }
    }
  }

  ATH_MSG_INFO("Found " << N_quark_t_all    << " t    quarks in event record");
  ATH_MSG_INFO("Found " << N_quark_tbar_all << " tbar quarks in event record");
  ATH_MSG_INFO("Found " << N_quark_t    << " t    -> W X decays");
  ATH_MSG_INFO("Found " << N_quark_tbar << " tbar -> W X decays");
  ATH_MSG_INFO("The number of tops with pt above " << m_Ptmin/CLHEP::GeV << " CLHEP::GeV/c is " << N_pt_above_cut);

  if (N_quark_t_all <= 0 || N_quark_tbar_all <= 0) ATH_MSG_ERROR("No t or tbar quarks were found in a (presumably) ttbar event!");
  if (N_quark_t >= 2 || N_quark_tbar >= 2) ATH_MSG_WARNING("More than one t -> W X or tbar -> W X decays found.");

  setFilterPassed(N_pt_above_cut >= 1 && N_quark_t >= 1 && N_quark_tbar >= 1);
  return StatusCode::SUCCESS;
}
