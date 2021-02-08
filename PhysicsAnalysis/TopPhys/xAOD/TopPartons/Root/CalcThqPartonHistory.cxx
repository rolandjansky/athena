/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

#include "TopPartons/CalcThqPartonHistory.h"
#include "TopPartons/CalcTopPartonHistory.h"
#include "TopPartons/CalcTChannelSingleTopPartonHistory.h"
#include "TopPartons/PartonHistoryUtils.h"
#include "TopConfiguration/TopConfig.h"

namespace top {
  CalcThqPartonHistory::CalcThqPartonHistory(const std::string& name) : CalcTopPartonHistory(name) {}

  bool CalcThqPartonHistory::HiggsAndDecay(const xAOD::TruthParticleContainer* truthParticles) {

    for (const xAOD::TruthParticle* particle : *truthParticles) {
      if (particle->pdgId() != 25 || particle->nChildren() != 2) {
        continue;
      }
      tH.Higgs_p4 = particle->p4();

      const top::PartonHistoryUtils::HiggsDecay& higgs = top::PartonHistoryUtils::AnalyzeHiggsDecay(particle);
      
      tH.decay1_p4 = higgs.decay1_vector;
      tH.decay2_p4 = higgs.decay2_vector;
      tH.decay1_pdgId = higgs.decay1_pdgId;
      tH.decay2_pdgId = higgs.decay2_pdgId;
      tH.tau_decay1_isHadronic = higgs.tau_decay1_isHadronic;
      tH.tau_decay2_isHadronic = higgs.tau_decay2_isHadronic;
      tH.tau_decay1_p4 = higgs.tau_decay1_vector;
      tH.tau_decay2_p4 = higgs.tau_decay2_vector;
      tH.decay1_from_decay1_p4 = higgs.decay1_from_decay1_vector;
      tH.decay2_from_decay1_p4 = higgs.decay2_from_decay1_vector;
      tH.decay1_from_decay1_pdgId = higgs.decay1_from_decay1_pdgId;
      tH.decay2_from_decay1_pdgId = higgs.decay2_from_decay1_pdgId;
      tH.decay1_from_decay2_p4 = higgs.decay1_from_decay2_vector;
      tH.decay2_from_decay2_p4 = higgs.decay2_from_decay2_vector;
      tH.decay1_from_decay2_pdgId = higgs.decay1_from_decay2_pdgId;
      tH.decay2_from_decay2_pdgId = higgs.decay2_from_decay2_pdgId;
      tH.tau_decay1_from_decay1_isHadronic = higgs.tau_decay1_from_decay1_isHadronic;
      tH.tau_decay2_from_decay1_isHadronic = higgs.tau_decay2_from_decay1_isHadronic;
      tH.tau_decay1_from_decay2_isHadronic = higgs.tau_decay1_from_decay2_isHadronic;
      tH.tau_decay2_from_decay2_isHadronic = higgs.tau_decay2_from_decay2_isHadronic;
      tH.tau_decay1_from_decay1_p4 = higgs.tau_decay1_from_decay1_vector;
      tH.tau_decay2_from_decay1_p4 = higgs.tau_decay2_from_decay1_vector;
      tH.tau_decay1_from_decay2_p4 = higgs.tau_decay1_from_decay2_vector;
      tH.tau_decay2_from_decay2_p4 = higgs.tau_decay2_from_decay2_vector;
      

      return true;
    }
    return false;
  }

  void CalcThqPartonHistory::THHistorySaver(const xAOD::TruthParticleContainer* truthParticles,
                                            xAOD::PartonHistory* ThqPartonHistory) {
    ThqPartonHistory->IniVarThq();
    TLorentzVector t_before, t_after, t_after_SC;
    TLorentzVector Wp;
    TLorentzVector b;
    TLorentzVector WpDecay1;
    TLorentzVector WpDecay2;
    int WpDecay1_pdgId;
    int WpDecay2_pdgId;
    TLorentzVector tau_decay_from_W_p4;
    int tau_decay_from_W_isHadronic;

    const bool event_top = CalcTopPartonHistory::topWb(truthParticles, 6, t_before, t_after,
								Wp, b, WpDecay1, WpDecay1_pdgId, WpDecay2, 
								WpDecay2_pdgId, tau_decay_from_W_p4, tau_decay_from_W_isHadronic);
    const bool event_top_SC = CalcTopPartonHistory::topAfterFSR_SC(truthParticles, 6, t_after_SC);
    const bool event_topbar = CalcTopPartonHistory::topWb(truthParticles, -6, t_before, t_after,
								   Wp, b, WpDecay1, WpDecay1_pdgId, WpDecay2, 
								   WpDecay2_pdgId, tau_decay_from_W_p4, tau_decay_from_W_isHadronic);
    const bool event_topbar_SC = CalcTopPartonHistory::topAfterFSR_SC(truthParticles, -6, t_after_SC);
    const bool event_Higgs =  CalcThqPartonHistory::HiggsAndDecay(truthParticles);


    // find spectator quark:
    TLorentzVector spectatorquark_before;
    TLorentzVector spectatorquark_after;

    int spectatorquark_pdgId;
    int spectatorquark_status;

    //const bool event_sq = CalcTChannelSingleTopPartonHistory::spectatorquark(truthParticles, spectatorquark_before, spectatorquark_after, spectatorquark_pdgId, spectatorquark_status);
    bool event_sq = CalcThqPartonHistory::spectatorquark(truthParticles, spectatorquark_before, spectatorquark_after, spectatorquark_pdgId, spectatorquark_status);

    if (event_Higgs) {
      if (event_top || event_topbar) {
        ThqPartonHistory->auxdecor< float >("MC_t_beforeFSR_m") = t_before.M();
        ThqPartonHistory->auxdecor< float >("MC_t_beforeFSR_pt") = t_before.Pt();
        ThqPartonHistory->auxdecor< float >("MC_t_beforeFSR_phi") = t_before.Phi();
        fillEtaBranch(ThqPartonHistory, "MC_t_beforeFSR_eta", t_before);

        ThqPartonHistory->auxdecor< float >("MC_t_afterFSR_m") = t_after.M();
        ThqPartonHistory->auxdecor< float >("MC_t_afterFSR_pt") = t_after.Pt();
        ThqPartonHistory->auxdecor< float >("MC_t_afterFSR_phi") = t_after.Phi();
        fillEtaBranch(ThqPartonHistory, "MC_t_afterFSR_eta", t_after);

        if (event_top_SC || event_topbar_SC) {
          ThqPartonHistory->auxdecor< float >("MC_t_afterFSR_SC_m") = t_after_SC.M();
          ThqPartonHistory->auxdecor< float >("MC_t_afterFSR_SC_pt") = t_after_SC.Pt();
          ThqPartonHistory->auxdecor< float >("MC_t_afterFSR_SC_phi") = t_after_SC.Phi();
          fillEtaBranch(ThqPartonHistory, "MC_t_afterFSR_SC_eta", t_after_SC);
        }

	if(((event_top && !event_topbar) || (!event_top && event_topbar)) && event_sq){
	  ThqPartonHistory->auxdecor< float >("MC_spectatorquark_beforeFSR_pt") = spectatorquark_before.Pt();
	  ThqPartonHistory->auxdecor< float >("MC_spectatorquark_beforeFSR_phi") = spectatorquark_before.Phi();
	  ThqPartonHistory->auxdecor< float >("MC_spectatorquark_beforeFSR_m") = spectatorquark_before.M();
	  ThqPartonHistory->auxdecor< int >("MC_spectatorquark_pdgId") = spectatorquark_pdgId;
	  ThqPartonHistory->auxdecor< int >("MC_spectatorquark_status") = spectatorquark_status;
	  fillEtaBranch(ThqPartonHistory,"MC_spectatorquark_beforeFSR_eta", spectatorquark_before);

	  ThqPartonHistory->auxdecor< float >("MC_spectatorquark_afterFSR_pt") = spectatorquark_after.Pt();
	  ThqPartonHistory->auxdecor< float >("MC_spectatorquark_afterFSR_phi") = spectatorquark_after.Phi();
	  ThqPartonHistory->auxdecor< float >("MC_spectatorquark_afterFSR_m") = spectatorquark_after.M();
	  fillEtaBranch(ThqPartonHistory,"MC_spectatorquark_afterFSR_eta", spectatorquark_after);
	}


        ThqPartonHistory->auxdecor< float >("MC_W_from_t_m") = Wp.M();
        ThqPartonHistory->auxdecor< float >("MC_W_from_t_pt") = Wp.Pt();
        ThqPartonHistory->auxdecor< float >("MC_W_from_t_phi") = Wp.Phi();
        fillEtaBranch(ThqPartonHistory, "MC_W_from_t_eta", Wp);

        ThqPartonHistory->auxdecor< float >("MC_b_from_t_m") = b.M();
        ThqPartonHistory->auxdecor< float >("MC_b_from_t_pt") = b.Pt();
        ThqPartonHistory->auxdecor< float >("MC_b_from_t_phi") = b.Phi();
        fillEtaBranch(ThqPartonHistory, "MC_b_from_t_eta", b);

        ThqPartonHistory->auxdecor< float >("MC_Wdecay1_from_t_m") = WpDecay1.M();
        ThqPartonHistory->auxdecor< float >("MC_Wdecay1_from_t_pt") = WpDecay1.Pt();
        ThqPartonHistory->auxdecor< float >("MC_Wdecay1_from_t_phi") = WpDecay1.Phi();
        ThqPartonHistory->auxdecor< int >("MC_Wdecay1_from_t_pdgId") = WpDecay1_pdgId;
        fillEtaBranch(ThqPartonHistory, "MC_Wdecay1_from_t_eta", WpDecay1);

        ThqPartonHistory->auxdecor< float >("MC_Wdecay2_from_t_m") = WpDecay2.M();
        ThqPartonHistory->auxdecor< float >("MC_Wdecay2_from_t_pt") = WpDecay2.Pt();
        ThqPartonHistory->auxdecor< float >("MC_Wdecay2_from_t_phi") = WpDecay2.Phi();
        ThqPartonHistory->auxdecor< int >("MC_Wdecay2_from_t_pdgId") = WpDecay2_pdgId;
        fillEtaBranch(ThqPartonHistory, "MC_Wdecay2_from_t_eta", WpDecay2);

	ThqPartonHistory->auxdecor< float >("MC_tau_from_W_from_t_m") = tau_decay_from_W_p4.M();
        ThqPartonHistory->auxdecor< float >("MC_tau_from_W_from_t_pt") = tau_decay_from_W_p4.Pt();
        ThqPartonHistory->auxdecor< float >("MC_tau_from_W_from_t_phi") = tau_decay_from_W_p4.Phi();
        ThqPartonHistory->auxdecor< int >("MC_tau_from_W_from_t_isHadronic") = tau_decay_from_W_isHadronic;
        fillEtaBranch(ThqPartonHistory, "MC_tau_from_W_from_t_eta", tau_decay_from_W_p4);

        //Higgs-Variables
        ThqPartonHistory->auxdecor< float >("MC_Higgs_m") = tH.Higgs_p4.M();
        ThqPartonHistory->auxdecor< float >("MC_Higgs_pt") = tH.Higgs_p4.Pt();
        ThqPartonHistory->auxdecor< float >("MC_Higgs_phi") = tH.Higgs_p4.Phi();
        fillEtaBranch(ThqPartonHistory, "MC_Higgs_eta", tH.Higgs_p4);

        //Higgs-decay1-Variables
        ThqPartonHistory->auxdecor< float >("MC_Higgs_decay1_m") = tH.decay1_p4.M();
        ThqPartonHistory->auxdecor< float >("MC_Higgs_decay1_pt") = tH.decay1_p4.Pt();
        ThqPartonHistory->auxdecor< float >("MC_Higgs_decay1_phi") = tH.decay1_p4.Phi();
        ThqPartonHistory->auxdecor< int >("MC_Higgs_decay1_pdgId") = tH.decay1_pdgId;
        fillEtaBranch(ThqPartonHistory, "MC_Higgs_decay1_eta", tH.decay1_p4);

	ThqPartonHistory->auxdecor< int >("MC_Higgs_tau_decay1_isHadronic") = tH.tau_decay1_isHadronic;
        ThqPartonHistory->auxdecor< float >("MC_Higgs_tau_decay1_m") = tH.tau_decay1_p4.M();
        ThqPartonHistory->auxdecor< float >("MC_Higgs_tau_decay1_pt") = tH.tau_decay1_p4.Pt();
        ThqPartonHistory->auxdecor< float >("MC_Higgs_tau_decay1_phi") = tH.tau_decay1_p4.Phi();
        fillEtaBranch(ThqPartonHistory, "MC_Higgs_tau_decay1_eta", tH.tau_decay1_p4);
		
        //Higgs-decay2-Variables
        ThqPartonHistory->auxdecor< float >("MC_Higgs_decay2_m") = tH.decay2_p4.M();
        ThqPartonHistory->auxdecor< float >("MC_Higgs_decay2_pt") = tH.decay2_p4.Pt();
        ThqPartonHistory->auxdecor< float >("MC_Higgs_decay2_phi") = tH.decay2_p4.Phi();
        ThqPartonHistory->auxdecor< int >("MC_Higgs_decay2_pdgId") = tH.decay2_pdgId;
        fillEtaBranch(ThqPartonHistory, "MC_Higgs_decay2_eta", tH.decay2_p4);

	ThqPartonHistory->auxdecor< int >("MC_Higgs_tau_decay2_isHadronic") = tH.tau_decay2_isHadronic;
        ThqPartonHistory->auxdecor< float >("MC_Higgs_tau_decay2_m") = tH.tau_decay2_p4.M();
        ThqPartonHistory->auxdecor< float >("MC_Higgs_tau_decay2_pt") = tH.tau_decay2_p4.Pt();
        ThqPartonHistory->auxdecor< float >("MC_Higgs_tau_decay2_phi") = tH.tau_decay2_p4.Phi();
        fillEtaBranch(ThqPartonHistory, "MC_Higgs_tau_decay2_eta", tH.tau_decay2_p4);	
	
        //Higgs-decay1- from decay1-Variables
        ThqPartonHistory->auxdecor< float >("MC_Higgs_decay1_from_decay1_m") = tH.decay1_from_decay1_p4.M();
        ThqPartonHistory->auxdecor< float >("MC_Higgs_decay1_from_decay1_pt") = tH.decay1_from_decay1_p4.Pt();
        ThqPartonHistory->auxdecor< float >("MC_Higgs_decay1_from_decay1_phi") = tH.decay1_from_decay1_p4.Phi();
        ThqPartonHistory->auxdecor< int >("MC_Higgs_decay1_from_decay1_pdgId") = tH.decay1_from_decay1_pdgId;
        fillEtaBranch(ThqPartonHistory, "MC_Higgs_decay1_from_decay1_eta", tH.decay1_from_decay1_p4);

        ThqPartonHistory->auxdecor< int >("MC_Higgs_tau_decay1_from_decay1_isHadronic") = tH.tau_decay1_from_decay1_isHadronic;
        ThqPartonHistory->auxdecor< float >("MC_Higgs_tau_decay1_from_decay1_m") = tH.tau_decay1_from_decay1_p4.M();
        ThqPartonHistory->auxdecor< float >("MC_Higgs_tau_decay1_from_decay1_pt") = tH.tau_decay1_from_decay1_p4.Pt();
        ThqPartonHistory->auxdecor< float >("MC_Higgs_tau_decay1_from_decay1_phi") = tH.tau_decay1_from_decay1_p4.Phi();
        fillEtaBranch(ThqPartonHistory, "MC_Higgs_tau_decay1_from_decay1_eta", tH.tau_decay1_from_decay1_p4);
	
        //Higgs-decay2- from decay1-Variables
        ThqPartonHistory->auxdecor< float >("MC_Higgs_decay2_from_decay1_m") = tH.decay2_from_decay1_p4.M();
        ThqPartonHistory->auxdecor< float >("MC_Higgs_decay2_from_decay1_pt") = tH.decay2_from_decay1_p4.Pt();
        ThqPartonHistory->auxdecor< float >("MC_Higgs_decay2_from_decay1_phi") = tH.decay2_from_decay1_p4.Phi();
        ThqPartonHistory->auxdecor< int >("MC_Higgs_decay2_from_decay1_pdgId") = tH.decay2_from_decay1_pdgId;
        fillEtaBranch(ThqPartonHistory, "MC_Higgs_decay2_from_decay1_eta", tH.decay2_from_decay1_p4);

	ThqPartonHistory->auxdecor< int >("MC_Higgs_tau_decay2_from_decay1_isHadronic") = tH.tau_decay2_from_decay1_isHadronic;
        ThqPartonHistory->auxdecor< float >("MC_Higgs_tau_decay2_from_decay1_m") = tH.tau_decay2_from_decay1_p4.M();
        ThqPartonHistory->auxdecor< float >("MC_Higgs_tau_decay2_from_decay1_pt") = tH.tau_decay2_from_decay1_p4.Pt();
        ThqPartonHistory->auxdecor< float >("MC_Higgs_tau_decay2_from_decay1_phi") = tH.tau_decay2_from_decay1_p4.Phi();
        fillEtaBranch(ThqPartonHistory, "MC_Higgs_tau_decay2_from_decay1_eta", tH.tau_decay2_from_decay1_p4);	


        //Higgs-decay1- from decay2-Variables
        ThqPartonHistory->auxdecor< float >("MC_Higgs_decay1_from_decay2_m") = tH.decay1_from_decay2_p4.M();
        ThqPartonHistory->auxdecor< float >("MC_Higgs_decay1_from_decay2_pt") = tH.decay1_from_decay2_p4.Pt();
        ThqPartonHistory->auxdecor< float >("MC_Higgs_decay1_from_decay2_phi") = tH.decay1_from_decay2_p4.Phi();
        ThqPartonHistory->auxdecor< int >("MC_Higgs_decay1_from_decay2_pdgId") = tH.decay1_from_decay2_pdgId;
        fillEtaBranch(ThqPartonHistory, "MC_Higgs_decay1_from_decay2_eta", tH.decay1_from_decay2_p4);


	ThqPartonHistory->auxdecor< int >("MC_Higgs_tau_decay1_from_decay2_isHadronic") = tH.tau_decay1_from_decay2_isHadronic;
        ThqPartonHistory->auxdecor< float >("MC_Higgs_tau_decay1_from_decay2_m") = tH.tau_decay1_from_decay2_p4.M();
        ThqPartonHistory->auxdecor< float >("MC_Higgs_tau_decay1_from_decay2_pt") = tH.tau_decay1_from_decay2_p4.Pt();
        ThqPartonHistory->auxdecor< float >("MC_Higgs_tau_decay1_from_decay2_phi") = tH.tau_decay1_from_decay2_p4.Phi();
        fillEtaBranch(ThqPartonHistory, "MC_Higgs_tau_decay1_from_decay2_eta", tH.tau_decay1_from_decay2_p4);

	
        //Higgs-decay2- from decay2-Variables
        ThqPartonHistory->auxdecor< float >("MC_Higgs_decay2_from_decay2_m") = tH.decay2_from_decay2_p4.M();
        ThqPartonHistory->auxdecor< float >("MC_Higgs_decay2_from_decay2_pt") = tH.decay2_from_decay2_p4.Pt();
        ThqPartonHistory->auxdecor< float >("MC_Higgs_decay2_from_decay2_phi") = tH.decay2_from_decay2_p4.Phi();
        ThqPartonHistory->auxdecor< int >("MC_Higgs_decay2_from_decay2_pdgId") = tH.decay2_from_decay2_pdgId;
        fillEtaBranch(ThqPartonHistory, "MC_Higgs_decay2_from_decay2_eta", tH.decay2_from_decay2_p4);

	ThqPartonHistory->auxdecor< int >("MC_Higgs_tau_decay2_from_decay2_isHadronic") = tH.tau_decay2_from_decay2_isHadronic;
        ThqPartonHistory->auxdecor< float >("MC_Higgs_tau_decay2_from_decay2_m") = tH.tau_decay2_from_decay2_p4.M();
        ThqPartonHistory->auxdecor< float >("MC_Higgs_tau_decay2_from_decay2_pt") = tH.tau_decay2_from_decay2_p4.Pt();
        ThqPartonHistory->auxdecor< float >("MC_Higgs_tau_decay2_from_decay2_phi") = tH.tau_decay2_from_decay2_p4.Phi();
        fillEtaBranch(ThqPartonHistory, "MC_Higgs_tau_decay2_from_decay2_eta", tH.tau_decay2_from_decay2_p4);

	
      }
    }
  }


  bool CalcThqPartonHistory::spectatorquark(const xAOD::TruthParticleContainer* truthParticles,
							  TLorentzVector& spectatorquark_beforeFSR, TLorentzVector& spectatorquark_afterFSR,
							  int& spectatorquark_pdgId, int& spectatorquark_status) {
    bool hasSpectatorquark = false;

    //identify quark which does not originate from a decaying top quark -> W -> qq
    //should come from hard scattering process
    float min_pt =0;
    for (const xAOD::TruthParticle* particle : *truthParticles) {
      if (particle == nullptr) continue;
      if (abs(particle->pdgId()) > 4) continue; //only light quarks

      for (size_t iparent = 0; iparent < particle->nParents(); ++iparent) {
        if (particle->parent(iparent) == nullptr){
          continue;
        }

        // we dont want quarks that have same pdgID as parent, since its a W interaction it should change sign
        if (std::abs(particle->parent(iparent)->pdgId()) == std::abs(particle->pdgId())) continue;

        // we dont want quarks that come from top
        if (std::abs(particle->parent(iparent)->pdgId()) == 6) continue;

        // we dont want quarks that come from W
        if (std::abs(particle->parent(iparent)->pdgId()) == 24) continue;

        // we dont want quarks that come from proton
        if (std::abs(particle->parent(iparent)->pdgId()) == 2212) continue;

        if( particle->p4().Pt() > min_pt ) {
          min_pt= particle->p4().Pt();

          spectatorquark_beforeFSR = particle->p4();
          spectatorquark_pdgId = particle->pdgId();
          spectatorquark_status = particle->status();
          hasSpectatorquark = true;

          // find after FSR
          particle = PartonHistoryUtils::findAfterFSR(particle);
          spectatorquark_afterFSR = particle->p4();
        } //if
      } // parent loop
    } // particle loop

    if (hasSpectatorquark) return true;

    return false;
  }



  StatusCode CalcThqPartonHistory::execute() {
    //Get the Truth Particles
    const xAOD::TruthParticleContainer* truthParticles(nullptr);

    ATH_CHECK(evtStore()->retrieve(truthParticles, m_config->sgKeyMCParticle()));

    // Create the partonHistory xAOD object
    xAOD::PartonHistoryAuxContainer* partonAuxCont = new xAOD::PartonHistoryAuxContainer {};
    xAOD::PartonHistoryContainer* partonCont = new xAOD::PartonHistoryContainer {};
    partonCont->setStore(partonAuxCont);

    xAOD::PartonHistory* ThqPartonHistory = new xAOD::PartonHistory {};
    partonCont->push_back(ThqPartonHistory);

    // Recover the parton history for TH events
    THHistorySaver(truthParticles, ThqPartonHistory);

    // Save to StoreGate / TStore
    std::string outputSGKey = m_config->sgKeyTopPartonHistory();
    std::string outputSGKeyAux = outputSGKey + "Aux.";

    xAOD::TReturnCode save = evtStore()->tds()->record(partonCont, outputSGKey);
    xAOD::TReturnCode saveAux = evtStore()->tds()->record(partonAuxCont, outputSGKeyAux);
    if (!save || !saveAux) {
      return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
  }
}
