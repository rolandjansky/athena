/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

#include "TopPartons/CalcThqPartonHistory.h"
#include "TopPartons/CalcTopPartonHistory.h"
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
      tH.tauvis_decay1_p4 = higgs.tauvis_decay1_vector;
      tH.tauvis_decay2_p4 = higgs.tauvis_decay2_vector;
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
      tH.tauvis_decay1_from_decay1_p4 = higgs.tauvis_decay1_from_decay1_vector;
      tH.tauvis_decay2_from_decay1_p4 = higgs.tauvis_decay2_from_decay1_vector;
      tH.tauvis_decay1_from_decay2_p4 = higgs.tauvis_decay1_from_decay2_vector;
      tH.tauvis_decay2_from_decay2_p4 = higgs.tauvis_decay2_from_decay2_vector;
      

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
    TLorentzVector tauvis_decay_from_W_p4;

    const bool event_top = CalcTopPartonHistory::topWb(truthParticles, 6, t_before, t_after,
						       Wp, b, WpDecay1, WpDecay1_pdgId, WpDecay2, 
						       WpDecay2_pdgId, tau_decay_from_W_p4, tau_decay_from_W_isHadronic, tauvis_decay_from_W_p4);
    const bool event_top_SC = CalcTopPartonHistory::topAfterFSR_SC(truthParticles, 6, t_after_SC);
    const bool event_topbar = CalcTopPartonHistory::topWb(truthParticles, -6, t_before, t_after,
							  Wp, b, WpDecay1, WpDecay1_pdgId, WpDecay2, 
							  WpDecay2_pdgId, tau_decay_from_W_p4, tau_decay_from_W_isHadronic, tauvis_decay_from_W_p4);
    const bool event_topbar_SC = CalcTopPartonHistory::topAfterFSR_SC(truthParticles, -6, t_after_SC);
    const bool event_Higgs =  CalcThqPartonHistory::HiggsAndDecay(truthParticles);


    // find spectator quark:
    TLorentzVector spectatorquark_before;
    TLorentzVector spectatorquark_after;
    int spectatorquark_pdgId;
    int spectatorquark_status;
    bool event_sq = CalcThqPartonHistory::spectatorquark(truthParticles, spectatorquark_before, spectatorquark_after, spectatorquark_pdgId, spectatorquark_status);

    // second b-quark
    bool event_secondb = false;
    bool event_secondbbar = false;
    TLorentzVector secondb_beforeFSR_p4;
    int secondb_beforeFSR_pdgId;
    TLorentzVector secondb_afterFSR_p4;
    int secondb_afterFSR_pdgId;

    if (event_topbar) {
      event_secondb = CalcThqPartonHistory::secondb(truthParticles, 5, secondb_beforeFSR_p4, secondb_beforeFSR_pdgId, secondb_afterFSR_p4, secondb_afterFSR_pdgId);
    }
    if (event_top) {
      event_secondbbar = CalcThqPartonHistory::secondb(truthParticles, -5, secondb_beforeFSR_p4, secondb_beforeFSR_pdgId, secondb_afterFSR_p4, secondb_afterFSR_pdgId);
    }


    if (event_Higgs) {
      if ((event_top && event_secondbbar) || (event_topbar && event_secondb)) {
	//second b-quark
	ThqPartonHistory->auxdecor< float >("MC_secondb_beforeFSR_m") = secondb_beforeFSR_p4.M();
	ThqPartonHistory->auxdecor< float >("MC_secondb_beforeFSR_pt") = secondb_beforeFSR_p4.Pt();
	ThqPartonHistory->auxdecor< float >("MC_secondb_beforeFSR_phi") = secondb_beforeFSR_p4.Phi();
	ThqPartonHistory->auxdecor< int >("MC_secondb_beforeFSR_pdgId") = secondb_beforeFSR_pdgId;
	fillEtaBranch(ThqPartonHistory, "MC_secondb_beforeFSR_eta", secondb_beforeFSR_p4);
	
	ThqPartonHistory->auxdecor< float >("MC_secondb_afterFSR_m") = secondb_afterFSR_p4.M();
	ThqPartonHistory->auxdecor< float >("MC_secondb_afterFSR_pt") = secondb_afterFSR_p4.Pt();
	ThqPartonHistory->auxdecor< float >("MC_secondb_afterFSR_phi") = secondb_afterFSR_p4.Phi();
	ThqPartonHistory->auxdecor< int >("MC_secondb_afterFSR_pdgId") = secondb_afterFSR_pdgId;
	fillEtaBranch(ThqPartonHistory, "MC_secondb_afterFSR_eta", secondb_afterFSR_p4);      

	// top quark
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

	// spectator quark
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

	// tau decay system
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

	ThqPartonHistory->auxdecor< float >("MC_tauvis_from_W_from_t_m") = tauvis_decay_from_W_p4.M();
        ThqPartonHistory->auxdecor< float >("MC_tauvis_from_W_from_t_pt") = tauvis_decay_from_W_p4.Pt();
        ThqPartonHistory->auxdecor< float >("MC_tauvis_from_W_from_t_phi") = tauvis_decay_from_W_p4.Phi();
        fillEtaBranch(ThqPartonHistory, "MC_tauvis_from_W_from_t_eta", tauvis_decay_from_W_p4);

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

        ThqPartonHistory->auxdecor< float >("MC_Higgs_tauvis_decay1_m") = tH.tauvis_decay1_p4.M();
        ThqPartonHistory->auxdecor< float >("MC_Higgs_tauvis_decay1_pt") = tH.tauvis_decay1_p4.Pt();
        ThqPartonHistory->auxdecor< float >("MC_Higgs_tauvis_decay1_phi") = tH.tauvis_decay1_p4.Phi();
        fillEtaBranch(ThqPartonHistory, "MC_Higgs_tauvis_decay1_eta", tH.tauvis_decay1_p4);
		
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

        ThqPartonHistory->auxdecor< float >("MC_Higgs_tauvis_decay2_m") = tH.tauvis_decay2_p4.M();
        ThqPartonHistory->auxdecor< float >("MC_Higgs_tauvis_decay2_pt") = tH.tauvis_decay2_p4.Pt();
        ThqPartonHistory->auxdecor< float >("MC_Higgs_tauvis_decay2_phi") = tH.tauvis_decay2_p4.Phi();
        fillEtaBranch(ThqPartonHistory, "MC_Higgs_tauvis_decay2_eta", tH.tauvis_decay2_p4);
	
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

        ThqPartonHistory->auxdecor< float >("MC_Higgs_tauvis_decay1_from_decay1_m") = tH.tauvis_decay1_from_decay1_p4.M();
        ThqPartonHistory->auxdecor< float >("MC_Higgs_tauvis_decay1_from_decay1_pt") = tH.tauvis_decay1_from_decay1_p4.Pt();
        ThqPartonHistory->auxdecor< float >("MC_Higgs_tauvis_decay1_from_decay1_phi") = tH.tauvis_decay1_from_decay1_p4.Phi();
        fillEtaBranch(ThqPartonHistory, "MC_Higgs_tauvis_decay1_from_decay1_eta", tH.tauvis_decay1_from_decay1_p4);
	
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

        ThqPartonHistory->auxdecor< float >("MC_Higgs_tauvis_decay2_from_decay1_m") = tH.tauvis_decay2_from_decay1_p4.M();
        ThqPartonHistory->auxdecor< float >("MC_Higgs_tauvis_decay2_from_decay1_pt") = tH.tauvis_decay2_from_decay1_p4.Pt();
        ThqPartonHistory->auxdecor< float >("MC_Higgs_tauvis_decay2_from_decay1_phi") = tH.tauvis_decay2_from_decay1_p4.Phi();
        fillEtaBranch(ThqPartonHistory, "MC_Higgs_tauvis_decay2_from_decay1_eta", tH.tauvis_decay2_from_decay1_p4);


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

        ThqPartonHistory->auxdecor< float >("MC_Higgs_tauvis_decay1_from_decay2_m") = tH.tauvis_decay1_from_decay2_p4.M();
        ThqPartonHistory->auxdecor< float >("MC_Higgs_tauvis_decay1_from_decay2_pt") = tH.tauvis_decay1_from_decay2_p4.Pt();
        ThqPartonHistory->auxdecor< float >("MC_Higgs_tauvis_decay1_from_decay2_phi") = tH.tauvis_decay1_from_decay2_p4.Phi();
        fillEtaBranch(ThqPartonHistory, "MC_Higgs_tauvis_decay1_from_decay2_eta", tH.tauvis_decay1_from_decay2_p4);

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

        ThqPartonHistory->auxdecor< float >("MC_Higgs_tauvis_decay2_from_decay2_m") = tH.tauvis_decay2_from_decay2_p4.M();
        ThqPartonHistory->auxdecor< float >("MC_Higgs_tauvis_decay2_from_decay2_pt") = tH.tauvis_decay2_from_decay2_p4.Pt();
        ThqPartonHistory->auxdecor< float >("MC_Higgs_tauvis_decay2_from_decay2_phi") = tH.tauvis_decay2_from_decay2_p4.Phi();
        fillEtaBranch(ThqPartonHistory, "MC_Higgs_tauvis_decay2_from_decay2_eta", tH.tauvis_decay2_from_decay2_p4);

	
      }
    }
  }

  //,
  //                                TLorentzVector& spectatorquark_method2_beforeFSR, TLorentzVector& spectatorquark_method2_afterFSR,
  //                                int& spectatorquark_method2_pdgId, int& spectatorquark_method2_status

  bool CalcThqPartonHistory::spectatorquark(const xAOD::TruthParticleContainer* truthParticles,
                                  TLorentzVector& spectatorquark_beforeFSR, TLorentzVector& spectatorquark_afterFSR,
                                  int& spectatorquark_pdgId, int& spectatorquark_status) {
    bool hasSpectatorquark = false;

    //identify quark which does not originate from a decaying top quark -> W -> qq
    //should come from hard scattering process
    float min_pt =0;
    for (const xAOD::TruthParticle* particle : *truthParticles) { //loop over all truth partons
      if (particle == nullptr) continue;

      if (abs(particle->pdgId()) == 6){
        for (size_t iparent = 0; iparent < particle->nParents(); ++iparent) { // loop over parents
          if (particle->parent(iparent) == nullptr){
            continue;
          }
          if (std::abs(particle->parent(iparent)->pdgId()) == std::abs(particle->pdgId())) continue; 
            for (size_t ichildren = 0; ichildren < particle->parent(iparent)->nChildren(); ++ichildren) { 
              if (particle->parent(iparent)->child(ichildren) == nullptr) continue;
              if (abs(particle->parent(iparent)->child(ichildren)->pdgId()) >4) continue;

              auto partoncandidate = particle->parent(iparent)->child(ichildren);
              //Select highest PT light quark if more than 1 light quark
              if( partoncandidate->p4().Pt() > min_pt ) {
                min_pt= particle->parent(iparent)->child(ichildren)->p4().Pt();

                spectatorquark_beforeFSR =partoncandidate->p4();
                spectatorquark_pdgId = partoncandidate->pdgId();
                spectatorquark_status = partoncandidate->status();
                hasSpectatorquark = true;

                // find after FSR
                auto particle2 = PartonHistoryUtils::findAfterFSR(partoncandidate);
                spectatorquark_afterFSR = particle2->p4();
              } //if
          }//children of top parents loop
        }//parents loop of top
      }//top if statement
    }// particle loop
    if (hasSpectatorquark) return true;
    return false;
  }
  


  bool CalcThqPartonHistory::secondb(const xAOD::TruthParticleContainer* truthParticles, int start, 
				     TLorentzVector& secondb_beforeFSR_p4, int& secondb_beforeFSR_pdgId,
				     TLorentzVector& secondb_afterFSR_p4, int& secondb_afterFSR_pdgId) {
    for (const xAOD::TruthParticle* particle : *truthParticles) {
      if (particle->pdgId() != start) { continue; } // start = +- 6
      for (size_t i=0; i< particle->nParents(); i++) {
	const xAOD::TruthParticle* parent = particle->parent(i);
	if (parent->pdgId() == 21) { //second b is originated from gluon spliting
	  const xAOD::TruthParticle* secondb_beforeFSR = particle;
          secondb_beforeFSR_p4 = secondb_beforeFSR->p4();
	  secondb_beforeFSR_pdgId = secondb_beforeFSR->pdgId();

          const xAOD::TruthParticle* secondb_afterFSR = PartonHistoryUtils::findAfterFSR(particle);
          secondb_afterFSR_p4 = secondb_afterFSR->p4();
	  secondb_afterFSR_pdgId = secondb_afterFSR->pdgId();

	  return true;
	} // if parent is gluon
      } // for iparents
    }//for
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
