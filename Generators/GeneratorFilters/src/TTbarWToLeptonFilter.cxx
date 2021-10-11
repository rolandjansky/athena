/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "GeneratorFilters/TTbarWToLeptonFilter.h"

TTbarWToLeptonFilter::TTbarWToLeptonFilter(const std::string& name, ISvcLocator* pSvcLocator)
  : GenFilter(name,pSvcLocator)
{
  declareProperty("Ptcut", m_Ptmin=200000.);
  declareProperty("NumLeptons", m_numLeptons=-1); // Negative for >0, positive integers for the specific number
  declareProperty("fourTopsFilter", m_fourTopsFilter=false);//four top filter or not
  declareProperty("SSMLFilter", m_SSMLFilter=false);//Same sign multilepton filter or not

}


StatusCode TTbarWToLeptonFilter::filterEvent() {
  int N_quark_t    = 0;
  int N_quark_tbar = 0;
  int N_quark_t_all    = 0;
  int N_quark_tbar_all = 0;
  int N_pt_above_cut = 0;
  int N_pt_above_cut_plus = 0;
  int N_pt_above_cut_minus = 0;

  int foundlepton[6] = {0, 0, 0, 0, 0, 0}; // e+, e-, mu+, mu-, tau+, tau-
  int  count_found_leptons = 1; // In ttbar each charged lepton flavour is counted at most once
  if(m_fourTopsFilter) count_found_leptons = 2; // In four tops, one can have the same charged lepton flavour twice

  for (McEventCollection::const_iterator itr = events()->begin(); itr!=events()->end(); ++itr) {
    const HepMC::GenEvent* genEvt = (*itr);
#ifdef HEPMC3
    for (auto  pitr: *genEvt) {
        if (std::abs(pitr->pdg_id()) != 6) continue;
        if ( pitr->pdg_id() ==  6 ) N_quark_t_all++;
        if ( pitr->pdg_id() == -6 ) N_quark_tbar_all++;
        auto decayVtx = pitr->end_vertex();
        // Verify if we got a valid pointer and retrieve the number of daughters
        if (!decayVtx) continue; 
        // For this analysis we are not interested in t->t MC structures, only in decays
        if (decayVtx->particles_out().size() < 2) continue;
        for (auto child_mcpart:  decayVtx->particles_out()) {
            //  Implicitly assume that tops always decay to W X
            if (std::abs(child_mcpart->pdg_id()) != 24) continue;
              if ( pitr->pdg_id() ==  6 ) N_quark_t++;
              if ( pitr->pdg_id() == -6 ) N_quark_tbar++;

              bool  useNextVertex = false;
              auto w_decayVtx = child_mcpart->end_vertex();
              while (w_decayVtx) {
                useNextVertex = false;
                for (auto grandchild_mcpart: *w_decayVtx) {
			      int grandchild_pid = grandchild_mcpart->pdg_id();
			      ATH_MSG_DEBUG("W (t/tbar) has " <<  w_decayVtx->particles_out().size() << " children and the pdg_id of the next is " << grandchild_pid);
			      // Check if the W's child is W again. If yes, then move to its next decay vertex in a decay tree
			      if (std::abs(grandchild_pid) == 24) {
			         w_decayVtx = grandchild_mcpart->end_vertex();
				 // If something wrong comes from truth...
				 if (!w_decayVtx) {
                                   ATH_MSG_ERROR("A stable W is found... ");
                                   break;
			         }
				 useNextVertex = true;
				break;
			      }

		  // use brute force to use only leptons that have not been found already 
		  if (grandchild_pid == -11 && foundlepton[0] < count_found_leptons) {
		    if (grandchild_mcpart->momentum().perp() >= m_Ptmin) { 
		      foundlepton[0]++;
		      N_pt_above_cut++;
                      N_pt_above_cut_minus++;
		    }
		  }
		  if (grandchild_pid == 11 && foundlepton[1] < count_found_leptons) {
		    if (grandchild_mcpart->momentum().perp() >= m_Ptmin) { 
		      foundlepton[1]++;
		      N_pt_above_cut++;
                      N_pt_above_cut_plus++;

		    }
		  }
		  if (grandchild_pid == -13 && foundlepton[2] < count_found_leptons) {
		    if (grandchild_mcpart->momentum().perp() >= m_Ptmin) { 
		      foundlepton[2]++;
		      N_pt_above_cut++;
                      N_pt_above_cut_minus++;
		    }
		  }
		  if (grandchild_pid == 13 && foundlepton[3] < count_found_leptons) {
		    if (grandchild_mcpart->momentum().perp() >= m_Ptmin) { 
		      foundlepton[3]++;
		      N_pt_above_cut++;
                      N_pt_above_cut_plus++;
		    }
		  }
		  if (grandchild_pid == -15 && foundlepton[4] < count_found_leptons) {
		    if (grandchild_mcpart->momentum().perp() >= m_Ptmin) { 
		      foundlepton[4]++;
		      N_pt_above_cut++;
                      N_pt_above_cut_minus++;
		    }
		  }
		  if (grandchild_pid == 15 && foundlepton[5] < count_found_leptons) {
		    if (grandchild_mcpart->momentum().perp() >= m_Ptmin) { 
		      foundlepton[5]++;
		      N_pt_above_cut++;
                      N_pt_above_cut_plus++;
		    }
		  }
                }
                // If investigation of W's next decay vertex is not required then finish looking for leptons
                if (!useNextVertex) break;
         }
      }
   }
#else
    for (HepMC::GenEvent::particle_const_iterator pitr = genEvt->particles_begin(); pitr != genEvt->particles_end(); ++pitr) {
      if (std::abs((*pitr)->pdg_id()) == 6) {
        if ( (*pitr)->pdg_id() ==  6 ) N_quark_t_all++;
        if ( (*pitr)->pdg_id() == -6 ) N_quark_tbar_all++;

        int n_daughters = 0;

        HepMC::GenParticle * mcpart = (*pitr);
        const HepMC::GenVertex * decayVtx = mcpart->end_vertex();

        // Verify if we got a valid pointer and retrieve the number of daughters
        if (decayVtx != 0) n_daughters = decayVtx->particles_out_size();

        // For this analysis we are not interested in t->t MC structures, only in decays
        if (n_daughters >= 2) {
          HepMC::GenVertex::particles_in_const_iterator child_mcpartItr  = decayVtx->particles_out_const_begin();
          HepMC::GenVertex::particles_in_const_iterator child_mcpartItrE = decayVtx->particles_out_const_end();
          for (; child_mcpartItr != child_mcpartItrE; ++child_mcpartItr) {
            HepMC::GenParticle * child_mcpart = (*child_mcpartItr);

            //  Implicitly assume that tops always decay to W X
            if (std::abs(child_mcpart->pdg_id()) == 24) {
              if ( (*pitr)->pdg_id() ==  6 ) N_quark_t++;
              if ( (*pitr)->pdg_id() == -6 ) N_quark_tbar++;

              bool  useNextVertex = false;
              const HepMC::GenVertex * w_decayVtx = child_mcpart->end_vertex();

              while (w_decayVtx) {

                useNextVertex = false;
                int mcpart_n_particles_out = w_decayVtx->particles_out_size();

                HepMC::GenVertex::particles_out_const_iterator grandchild_mcpartItr  = w_decayVtx->particles_out_const_begin();
                HepMC::GenVertex::particles_out_const_iterator grandchild_mcpartItrE = w_decayVtx->particles_out_const_end();

                for (; grandchild_mcpartItr != grandchild_mcpartItrE; ++grandchild_mcpartItr) {
      HepMC::GenParticle * grandchild_mcpart = (*grandchild_mcpartItr);
		  int grandchild_pid = grandchild_mcpart->pdg_id();

                  // cppcheck-suppress shiftNegative; false positive!
		  ATH_MSG_DEBUG("W (t/tbar) has " << mcpart_n_particles_out << " children and the pdg_id of the next is " << grandchild_pid);

		  // Check if the W's child is W again. If yes, then move to its next decay vertex in a decay tree
		  if (std::abs(grandchild_pid) == 24) {
		    w_decayVtx = grandchild_mcpart->end_vertex();

		    // If something wrong comes from truth...
		    if (!w_decayVtx) {
		      ATH_MSG_ERROR("A stable W is found... ");
		      break;
		    }

		    useNextVertex = true;
		    break;
		  }

		  // use brute force to use only leptons that have not been found already 
		  if (grandchild_pid == -11 && foundlepton[0] < count_found_leptons) {
		    if (grandchild_mcpart->momentum().perp() >= m_Ptmin) { 
		      foundlepton[0]++;
		      N_pt_above_cut++;
                      N_pt_above_cut_minus++;
		    }
		  }
		  if (grandchild_pid == 11 && foundlepton[1] < count_found_leptons) {
		    if (grandchild_mcpart->momentum().perp() >= m_Ptmin) { 
		      foundlepton[1]++;
		      N_pt_above_cut++;
                      N_pt_above_cut_plus++;

		    }
		  }
		  if (grandchild_pid == -13 && foundlepton[2] < count_found_leptons) {
		    if (grandchild_mcpart->momentum().perp() >= m_Ptmin) { 
		      foundlepton[2]++;
		      N_pt_above_cut++;
                      N_pt_above_cut_minus++;
		    }
		  }
		  if (grandchild_pid == 13 && foundlepton[3] < count_found_leptons) {
		    if (grandchild_mcpart->momentum().perp() >= m_Ptmin) { 
		      foundlepton[3]++;
		      N_pt_above_cut++;
                      N_pt_above_cut_plus++;
		    }
		  }
		  if (grandchild_pid == -15 && foundlepton[4] < count_found_leptons) {
		    if (grandchild_mcpart->momentum().perp() >= m_Ptmin) { 
		      foundlepton[4]++;
		      N_pt_above_cut++;
                      N_pt_above_cut_minus++;
		    }
		  }
		  if (grandchild_pid == 15 && foundlepton[5] < count_found_leptons) {
		    if (grandchild_mcpart->momentum().perp() >= m_Ptmin) { 
		      foundlepton[5]++;
		      N_pt_above_cut++;
                      N_pt_above_cut_plus++;
		    }
		  }
			      
		}
		
                // If investigation of W's next decay vertex is not required then finish looking for leptons
                if (!useNextVertex) break;
              }
            }
          }
        }
      }
    }
#endif
  }

  ATH_MSG_INFO("Found " << N_quark_t_all    << " t    quarks in event record");
  ATH_MSG_INFO("Found " << N_quark_tbar_all << " tbar quarks in event record");
  ATH_MSG_INFO("Found " << N_quark_t    << " t    -> W X decays");
  ATH_MSG_INFO("Found " << N_quark_tbar << " tbar -> W X decays");
  ATH_MSG_INFO("Num leptons from W decays from tops with lepton pt above " << m_Ptmin/1000.0 << " CLHEP::GeV/c = " << N_pt_above_cut);

  int count_tops = 1;
  if(m_fourTopsFilter) count_tops = 2;
  if (N_quark_t_all < count_tops || N_quark_tbar_all < count_tops) {

    ATH_MSG_ERROR("No t or tbar quarks were found in a (presumably) ttbar event! Event is rejected.");
    setFilterPassed(false);
    return StatusCode::SUCCESS;
  }

    if (N_quark_t < count_tops || N_quark_tbar < count_tops) {

    ATH_MSG_ERROR("No t or tbar quarks were found decaying to W in a (presumably) ttbar event! Event is rejected. Event dump follows.");
    int event = 0;
    for (McEventCollection::const_iterator itr = events()->begin(); itr!=events()->end(); ++itr) {
      event++;
      const HepMC::GenEvent* genEvt = (*itr);
      int part=0 ;
      for (auto  mcpart: *genEvt) {
        part++;
        int pid = mcpart->pdg_id();
        ATH_MSG_ERROR("In event (from MC collection) " << event << " particle number " << part << " has pdg_id = " << pid);
        // retrieve decay vertex
        auto  decayVtx = mcpart->end_vertex();
        // verify if we got a valid pointer
        if ( !decayVtx ) continue;
          int part_child=0;
          for ( auto  child_mcpart: *decayVtx) {
            part_child++;
            int child_pid = child_mcpart->pdg_id();
            ATH_MSG_ERROR("          child " << part_child << " with pdg_id = " << child_pid);
          }
      }
    }
    setFilterPassed(false);
    return StatusCode::SUCCESS;
  }

  // If NumLeptons is negative (default), accept if Nlep > 0, otherwise only accept an exact match
  if (m_numLeptons < 0) {
    if ( (N_quark_t >= 2 || N_quark_tbar >= 2) && !m_fourTopsFilter) {
      ATH_MSG_WARNING("More than one t -> W X or tbar -> W X decays found. Event is accepted anyway.");
    }
    if ( (N_quark_t > 2 || N_quark_tbar > 2) && m_fourTopsFilter) {
      ATH_MSG_WARNING("More than one t -> W X or tbar -> W X decays found. Event is accepted anyway.");
    }
    setFilterPassed(N_pt_above_cut > 0);
  } else {
    if(m_fourTopsFilter){
      if(m_SSMLFilter) setFilterPassed( (N_pt_above_cut >= m_numLeptons) && (N_pt_above_cut_plus >= 2 || N_pt_above_cut_minus >= 2));
      else setFilterPassed(N_pt_above_cut >= m_numLeptons);}
    else setFilterPassed(N_pt_above_cut == m_numLeptons);
  }

  return StatusCode::SUCCESS;
}
