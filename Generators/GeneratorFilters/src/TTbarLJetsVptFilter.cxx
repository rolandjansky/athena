/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "GeneratorFilters/TTbarLJetsVptFilter.h"

TTbarLJetsVptFilter::TTbarLJetsVptFilter(const std::string& name, ISvcLocator* pSvcLocator)
  : GenFilter(name,pSvcLocator)
{
  declareProperty("Ptcut"     , m_Ptmin     = 200000. );
  declareProperty("Etacut"    , m_Etamax    =      6. );
  declareProperty("NumLeptons", m_numLeptons=     -1  ); // positive integers for the specific number
  declareProperty("FollowTaus", m_followTaus= false   ); 

  // FLS - for Vpt filtering
  declareProperty("VptMin"    , m_VptMin    =      0. );
  declareProperty("VptMax"    , m_VptMax    =13000000.);
  //---
}


StatusCode TTbarLJetsVptFilter::filterEvent() {
  int N_quark_t        = 0;
  int N_quark_tbar     = 0;
  int N_quark_t_all    = 0;
  int N_quark_tbar_all = 0;
  int N_pt_above_cut   = 0;
  
  // FLS
  int N_W_in_pt_range = 0;
  int N_W_lep         = 0;
  float tmp_Wpt =0;
  float W_lep_pt  =-1;
  //  float W_lep_eta = 0; // for debugging: no requirement in filter
  //---

  for (McEventCollection::const_iterator itr = events_const()->begin(); itr!=events_const()->end(); ++itr) {
    const HepMC::GenEvent* genEvt = (*itr);
    for (HepMC::GenEvent::particle_const_iterator pitr = genEvt->particles_begin(); pitr != genEvt->particles_end(); ++pitr) {
      // get the top
      if (fabs((*pitr)->pdg_id()) == 6) {
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
            if (fabs(child_mcpart->pdg_id()) == 24) {
	      tmp_Wpt=child_mcpart->momentum().perp();
	      
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
		  
		  ATH_MSG_DEBUG("W (t/tbar) has " << mcpart_n_particles_out << " children and the pdg_id of the next is " << grandchild_pid);
		  
		  // Check if the W's child is W again. If yes, then move to its next decay vertex in a decay tree
		  if (fabs(grandchild_pid) == 24) {
		    tmp_Wpt=grandchild_mcpart->momentum().perp();
		    w_decayVtx = grandchild_mcpart->end_vertex();
		    
		    // If something wrong comes from truth...
		    if (!w_decayVtx) {
                      ATH_MSG_ERROR("A stable W is found... ");
                      break;
		    }
		    
		    useNextVertex = true;
		    break;
		  }
		  
		  if (abs(grandchild_pid) == 11 ||  abs(grandchild_pid) == 13 || abs(grandchild_pid) == 15) {
		    // save the number of leptonic W to cut on l+jets 
		    // overwrite the leptonic W pt since in any case the filter will always request always 1 of them
		    W_lep_pt  = tmp_Wpt;
		    //W_lep_eta = child_mcpart->momentum().eta(); // for debugging
		    N_W_lep++;

		    // check the lepton:
		    // - if mu of el take it as it is
		    // - if tau: check its decay and take the lepton (if tau->lep)
		    HepMC::GenParticle *FS_lep = 0;
		    if (abs(grandchild_pid) == 11 || abs(grandchild_pid) == 13) FS_lep = grandchild_mcpart;
		    else {
		      if (not m_followTaus) {
			FS_lep = grandchild_mcpart;
		      } else { // followTau

					const HepMC::GenVertex * TauDecayVtx = grandchild_mcpart->end_vertex();

					bool decaysToTau = true;
					while (decaysToTau) {
						decaysToTau = false;
						HepMC::GenVertex::particles_out_const_iterator tauDecay_itr = TauDecayVtx->particles_out_const_begin();
						HepMC::GenVertex::particles_out_const_iterator tauDecay_itrE = TauDecayVtx->particles_out_const_end();
						for (; tauDecay_itr != tauDecay_itrE; ++tauDecay_itr) {
							HepMC::GenParticle *tauChild = (*tauDecay_itr);
							if (abs(tauChild->pdg_id()) == 15) {
								decaysToTau = true;
								TauDecayVtx = tauChild->end_vertex();
							}
							else if (abs(tauChild->pdg_id()) == 11 || abs(tauChild->pdg_id()) == 13) {
								FS_lep = tauChild;
							}
						}
					}
		      } // followTau
		    }
		    // cut on the final state lepton (if any)
		    if (FS_lep) {
		      if (FS_lep->momentum().perp() >= m_Ptmin && fabs(FS_lep->momentum().eta()) <= m_Etamax) N_pt_above_cut++;
		    }
		    break;
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
  }

  ATH_MSG_INFO("Found " << N_quark_t_all    << " t    quarks in event record");
  ATH_MSG_INFO("Found " << N_quark_tbar_all << " tbar quarks in event record");
  ATH_MSG_INFO("Found " << N_quark_t    << " t    -> W X decays");
  ATH_MSG_INFO("Found " << N_quark_tbar << " tbar -> W X decays");
  ATH_MSG_INFO("Num leptons from W decays from tops with lepton pt above " << m_Ptmin/1000.0 << " CLHEP::GeV/c = " << N_pt_above_cut);
  // FLS
  ATH_MSG_INFO("Num W->lep with " << m_VptMin/1000.0 << " < pt < " << m_VptMax/1000.0 << " GeV/c = " << N_W_in_pt_range);
  // ---
  
  if (N_quark_t_all < 1 || N_quark_tbar_all < 1) {
    ATH_MSG_ERROR("No t or tbar quarks were found in a (presumably) ttbar event! Event is rejected.");
    setFilterPassed(false);
    return StatusCode::SUCCESS;
  }
  
  if (N_quark_t < 1 || N_quark_tbar < 1) {
    ATH_MSG_ERROR("No t or tbar quarks were found decaying to W in a (presumably) ttbar event! Event is rejected. Event dump follows.");
    int event = 0;
    for (McEventCollection::const_iterator itr = events_const()->begin(); itr!=events_const()->end(); ++itr) {
      event++;
      const HepMC::GenEvent* genEvt = (*itr);
      HepMC::GenEvent::particle_const_iterator mcpartItr  = genEvt->particles_begin();
      HepMC::GenEvent::particle_const_iterator mcpartItrE = genEvt->particles_end();
      int part ( 0 );
      for (; mcpartItr != mcpartItrE; ++mcpartItr) {
        part++;
        HepMC::GenParticle * mcpart = (*mcpartItr);
        int pid = mcpart->pdg_id();
        ATH_MSG_ERROR("In event (from MC collection) " << event << " particle number " << part << " has pdg_id = " << pid);

        // retrieve decay vertex
        const HepMC::GenVertex * decayVtx = mcpart->end_vertex();

        // verify if we got a valid pointer
        if ( decayVtx != 0 ) {
          HepMC::GenVertex::particles_in_const_iterator child_mcpartItr  = decayVtx->particles_out_const_begin();
          HepMC::GenVertex::particles_in_const_iterator child_mcpartItrE = decayVtx->particles_out_const_end();
          int part_child ( 0 );
          for (; child_mcpartItr != child_mcpartItrE; ++child_mcpartItr) {
            part_child++;
            HepMC::GenParticle * child_mcpart = (*child_mcpartItr);
            int child_pid = child_mcpart->pdg_id();
            ATH_MSG_ERROR("          child " << part_child << " with pdg_id = " << child_pid);
          }
        }
      }
    }
    setFilterPassed(false);
    return StatusCode::SUCCESS;
  }
  
  /*
  std::cout << "Number of W leptons: " <<  N_W_lep 
	    << " in pt range: " << N_pt_above_cut 
	    << " Vpt: " << W_lep_pt 
	    << " Vets: " << W_lep_eta << std::endl;
  */
  // VD: requring only l+jets events:
  if ( N_W_lep!=1 ) {
    //////std::cout << "rejecting event!!!" << std::endl;
    setFilterPassed(false);
  } else {
    //at this point I can easily cut on Vpt
    bool passVptCut= (W_lep_pt>m_VptMin && W_lep_pt<m_VptMax);

    // If NumLeptons is negative (default), accept if Nlep > 0, otherwise only accept an exact match
    if (m_numLeptons < 0) {
      setFilterPassed( (N_pt_above_cut>0) && passVptCut);
    } else {
      setFilterPassed( (N_pt_above_cut==m_numLeptons)  && passVptCut);
      /////std::cout << "KEEP: " <<  ((N_pt_above_cut==m_numLeptons)  && passVptCut) << std::endl;
    }
  }

  return StatusCode::SUCCESS;
}
