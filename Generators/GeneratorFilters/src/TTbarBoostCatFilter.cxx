/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeneratorFilters/TTbarBoostCatFilter.h"

TTbarBoostCatFilter::TTbarBoostCatFilter(const std::string& name, ISvcLocator* pSvcLocator)
  : GenFilter(name,pSvcLocator)
{
  declareProperty("Ptcut", m_Ptmin=200000.);
  declareProperty("NumLeptons", m_numLeptons=-1); // Negative for >0, positive integers for the specific number

  //Option 1: largest top pT (MeV)   // low, high   
  declareProperty("TopPtcutLowedge",  m_TopPtmin=-1.); // TopPtcutLowedge <= largest top pT 
  declareProperty("TopPtcutHighedge", m_TopPtmax=-1.); // largest top pT < TopPtcutHighedge  
  //Option 2: largest WpT      // low, high
  declareProperty("WPtcutLowedge",  m_WPtmin=-1.);  // WPtcutLowedge <= largest W pT 
  declareProperty("WPtcutHighedge", m_WPtmax=-1.);  // largest W pT < WPtcutHighedge  
  //Option 3: ttbar mass       // low, high
  declareProperty("TTMasscutLowedge", m_TTMassmin=-1.);  // TTMasscutLowedge <= ttbar mass
  declareProperty("TTMasscutHighedge", m_TTMassmax=-1.); // ttbar mass < TTMasscutHighedge 
  //Option 4: pT of leptons vector sum   // low, high 
  declareProperty("LepsPtcutLowedge",  m_LepsPtmin=-1.); // LepsPtcutLowedge <= pt of vector sum of Leptons from W 
  declareProperty("LepsPtcutHighedge", m_LepsPtmax=-1.); // pt of vector sum of Leptons from W < LepscutHighedge 
  //Option 5: pT of nuetrinos vector sum // low, high
  declareProperty("NusPtcutLowedge",  m_NusPtmin=-1.); // NusPtcutLowedge <= pt of vector sum of Neutrinos from W 
  declareProperty("NusPtcutHighedge", m_NusPtmax=-1.); // pt of vector sum of Neutrinos from W < NuscutHighedge
  //Option 6: largest lepton pT (in charged lepton and vector sum of neutrino)
  declareProperty("LepPtcutLowedge",  m_LepPtmin=-1.); // LepPtcutLowedge <= Largest pt of leptons from W 
  declareProperty("LepPtcutHighedge", m_LepPtmax=-1.); // Largest pt of leptons from W < LepcutHighedge

}


StatusCode TTbarBoostCatFilter::filterEvent() {
  int N_quark_t    = 0;
  int N_quark_tbar = 0;
  int N_quark_t_all    = 0;
  int N_quark_tbar_all = 0;
  int N_pt_above_cut = 0;

  double HighestTopPt = 0.;
  double HighestWPt = 0.;
  double LeptonsPt = 0.;
  double NeutrinosPt = 0.;
  double HighestLeptonPt = 0.;
  double TTMass = 0.;


  // in case cut value is not set, set 0 for min and 14 TeV for max.
  if(m_TopPtmin*m_TopPtmax<0 && m_TopPtmin < 0 ) m_TopPtmin = 0.;  
  if(m_TopPtmin*m_TopPtmax<0 && m_TopPtmax < 0 ) m_TopPtmax = 14000000.; // 14 TeV
  if(m_WPtmin*m_WPtmax<0 && m_WPtmin   < 0 ) m_WPtmin = 0.;
  if(m_WPtmin*m_WPtmax<0 && m_WPtmax   < 0 ) m_WPtmax = 14000000.; // 14 TeV
  if(m_TTMassmin*m_TTMassmax < 0 && m_TTMassmin< 0 ) m_TTMassmin = 0.;
  if(m_TTMassmin*m_TTMassmax < 0 && m_TTMassmax< 0 ) m_TTMassmax = 14000000.; // 14 TeV
  if(m_LepsPtmin* m_LepsPtmax < 0 && m_LepsPtmin< 0 ) m_LepsPtmin = 0.;
  if(m_LepsPtmin* m_LepsPtmax < 0 && m_LepsPtmax< 0 ) m_LepsPtmax = 14000000.; // 14 TeV
  if(m_NusPtmin*m_NusPtmax < 0 && m_NusPtmin < 0 ) m_NusPtmin = 0.;
  if(m_NusPtmin*m_NusPtmax < 0 && m_NusPtmax < 0 ) m_NusPtmax = 14000000.; // 14 TeV
  if(m_LepPtmin*m_LepPtmax <0 && m_LepPtmin < 0 ) m_LepPtmin = 0.;
  if(m_LepPtmin*m_LepPtmax <0 && m_LepPtmax < 0 ) m_LepPtmax = 14000000.; // 14 TeV

  std::vector<HepMC::ConstGenParticlePtr> tops;
  std::vector<HepMC::ConstGenParticlePtr> ws;   // W from top decay (from tops)
  std::vector<HepMC::ConstGenParticlePtr> leps; // e, mu, tau from W decay (from ws)
  std::vector<HepMC::ConstGenParticlePtr> nus;  // nutrino from W decay (from ws)

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
          for (auto child_mcpart:  *decayVtx ) {
            //  Implicitly assume that tops always decay to W X
            if (std::abs(child_mcpart->pdg_id()) == 24) {
              if ( pitr->pdg_id() ==  6 ){
		N_quark_t++;
		tops.push_back(pitr);			  
		ws.push_back(child_mcpart);
	      }
              if ( pitr->pdg_id() == -6 ){
		N_quark_tbar++;
		tops.push_back(pitr);	
		ws.push_back(child_mcpart);		  
	      }
              bool  useNextVertex = false;
              auto  w_decayVtx = child_mcpart->end_vertex();

              while (w_decayVtx) {
                useNextVertex = false;
                for (auto  grandchild_mcpart: *w_decayVtx) {
			      int grandchild_pid = grandchild_mcpart->pdg_id();
			      ATH_MSG_DEBUG("W (t/tbar) has " << w_decayVtx->particles_out().size() << " children and the pdg_id of the next is " << grandchild_pid);
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
			      if (std::abs(grandchild_pid) == 11 ||  std::abs(grandchild_pid) == 13 || std::abs(grandchild_pid) == 15) {
				leps.push_back(grandchild_mcpart);
					if (grandchild_mcpart->momentum().perp() >= m_Ptmin) N_pt_above_cut++;
					// W decay lepton is found. Break loop over the decay product particles
					// break;
			      }
			      if (std::abs(grandchild_pid) == 12 ||  std::abs(grandchild_pid) == 14 || std::abs(grandchild_pid) == 16) {
				nus.push_back(grandchild_mcpart);
			      }
                }
                // If investigation of W's next decay vertex is not required then finish looking for leptons
                if (!useNextVertex) break;
              }
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
              if ( (*pitr)->pdg_id() ==  6 ){
		N_quark_t++;
		tops.push_back(mcpart);			  
		ws.push_back(child_mcpart);
	      }
              if ( (*pitr)->pdg_id() == -6 ){
		N_quark_tbar++;
		tops.push_back(mcpart);	
		ws.push_back(child_mcpart);		  
	      }

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

			      if (std::abs(grandchild_pid) == 11 ||  std::abs(grandchild_pid) == 13 || std::abs(grandchild_pid) == 15) {
				leps.push_back(grandchild_mcpart);
					if (grandchild_mcpart->momentum().perp() >= m_Ptmin) N_pt_above_cut++;
					// W decay lepton is found. Break loop over the decay product particles
					// break;
			      }
			      if (std::abs(grandchild_pid) == 12 ||  std::abs(grandchild_pid) == 14 || std::abs(grandchild_pid) == 16) {
				nus.push_back(grandchild_mcpart);
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

  if(tops.size()==2){
    HepMC::FourVector topSumMomentum(0.,0.,0.,0.);
    topSumMomentum.set(
		       tops[0]->momentum().px()+tops[1]->momentum().px(),
		       tops[0]->momentum().py()+tops[1]->momentum().py(),
		       tops[0]->momentum().pz()+tops[1]->momentum().pz(),
		       tops[0]->momentum().e() +tops[1]->momentum().e()
		       );   
    if(tops[0]->momentum().perp() > tops[1]->momentum().perp()) HighestTopPt = tops[0]->momentum().perp();
    else  HighestTopPt = tops[1]->momentum().perp();
    TTMass = topSumMomentum.m();
    ATH_MSG_INFO("  tops[0] p = " << tops[0]->momentum().perp()  << "  tops[1] p = "<< tops[1]->momentum().perp() );
  }

  if(ws.size()==2){ // assume both top decay into W+b...
    if(ws[0]->momentum().perp() > ws[1]->momentum().perp()) HighestWPt = ws[0]->momentum().perp();
    else  HighestWPt = ws[1]->momentum().perp();    
    ATH_MSG_INFO("  w[0] p = " << ws[0]->momentum().perp()  << "  w[1] p = "<< ws[1]->momentum().perp() );
  }

  HepMC::FourVector LepsSumMomentum(0.,0.,0.,0.);
  if(leps.size()>0){ 
    for (unsigned int i=0; i<leps.size(); i++) {
      LepsSumMomentum.set(
			 LepsSumMomentum.px()+leps[i]->momentum().px(),
			 LepsSumMomentum.py()+leps[i]->momentum().py(),
			 LepsSumMomentum.pz()+leps[i]->momentum().pz(),
			 LepsSumMomentum.e() +leps[i]->momentum().e()
			 );   
      if(leps[i]->momentum().perp() > HighestLeptonPt) HighestLeptonPt = leps[i]->momentum().perp();
    }  
  }  
  LeptonsPt = LepsSumMomentum.perp();

  HepMC::FourVector NusSumMomentum(0.,0.,0.,0.);
  if(nus.size()>0){ 
    for (unsigned int i=0; i<nus.size(); i++) {
      NusSumMomentum.set(
			 NusSumMomentum.px()+nus[i]->momentum().px(),
			 NusSumMomentum.py()+nus[i]->momentum().py(),
			 NusSumMomentum.pz()+nus[i]->momentum().pz(),
			 NusSumMomentum.e() +nus[i]->momentum().e()
			 );   
    }      
  }  
  NeutrinosPt = NusSumMomentum.perp();
  if(NeutrinosPt > HighestLeptonPt) HighestLeptonPt = NeutrinosPt;

  ATH_MSG_INFO("Found " << N_quark_t_all    << " t    quarks in event record");
  ATH_MSG_INFO("Found " << N_quark_tbar_all << " tbar quarks in event record");
  ATH_MSG_INFO("Found " << N_quark_t    << " t    -> W X decays");
  ATH_MSG_INFO("Found " << N_quark_tbar << " tbar -> W X decays");
  ATH_MSG_INFO("Num leptons from W decays from tops with lepton pt above " << m_Ptmin/1000.0 << " GeV/c = " << N_pt_above_cut);

  ATH_MSG_INFO("N leps = " << leps.size()  << " N nus = " << nus.size() );
  ATH_MSG_INFO("TopPt  :: " << m_TopPtmin  << " < " << HighestTopPt    << " < " << m_TopPtmax );
  ATH_MSG_INFO("WPt    :: " << m_WPtmin    << " < " << HighestWPt      << " < " << m_WPtmax    );
  ATH_MSG_INFO("LepsPt :: " << m_LepsPtmin << " < " << LeptonsPt       << " < " << m_LepsPtmax );
  ATH_MSG_INFO("NusPt  :: " << m_NusPtmin  << " < " << NeutrinosPt     << " < " << m_NusPtmax  ); 
  ATH_MSG_INFO("LepPt  :: " << m_LepPtmin  << " < " << HighestLeptonPt << " < " << m_LepPtmax  );
  ATH_MSG_INFO("TTMass :: " << m_TTMassmin << " < " << TTMass          << " < " << m_TTMassmax );


  if (N_quark_t_all < 1 || N_quark_tbar_all < 1) {
    ATH_MSG_ERROR("No t or tbar quarks were found in a (presumably) ttbar event! Event is rejected.");
    setFilterPassed(false);
    return StatusCode::SUCCESS;
  }

  if (N_quark_t < 1 || N_quark_tbar < 1) {
    ATH_MSG_ERROR("No t or tbar quarks were found decaying to W in a (presumably) ttbar event! Event is rejected. Event dump follows.");
    int event = 0;
    for (McEventCollection::const_iterator itr = events()->begin(); itr!=events()->end(); ++itr) {
      event++;
      const HepMC::GenEvent* genEvt = (*itr);
      int part=0;
      for (auto mcpart: *genEvt ) {
        part++;
        int pid = mcpart->pdg_id();
        ATH_MSG_ERROR("In event (from MC collection) " << event << " particle number " << part << " has pdg_id = " << pid);

        // retrieve decay vertex
        auto decayVtx = mcpart->end_vertex();
        // verify if we got a valid pointer
        if ( !decayVtx)  continue;
        int part_child =0 ;
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

  bool event_flag = false;
  if(m_TopPtmin <= HighestTopPt && HighestTopPt < m_TopPtmax ) event_flag = true;
  
  if( m_WPtmin <= HighestWPt && HighestWPt < m_WPtmax ) event_flag = true;
  if( m_LepsPtmin <= LeptonsPt && LeptonsPt < m_LepsPtmax ) event_flag = true;
  if( m_NusPtmin <= NeutrinosPt && NeutrinosPt < m_NusPtmax ) event_flag = true; 
  if( m_LepPtmin <= HighestLeptonPt && HighestLeptonPt < m_LepPtmax ) event_flag = true;
  if( m_TTMassmin <= TTMass && TTMass < m_TTMassmax ) event_flag = true;

  setFilterPassed(event_flag);

  return StatusCode::SUCCESS;
}

TTbarBoostCatFilter::~TTbarBoostCatFilter(){
  ;
}

//---------------------------------------------------------------------------
StatusCode TTbarBoostCatFilter::filterInitialize() {
  //---------------------------------------------------------------------------
  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------
StatusCode TTbarBoostCatFilter::filterFinalize() {
  //---------------------------------------------------------------------------
  return StatusCode::SUCCESS;
}

