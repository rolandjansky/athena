/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */

#include "TopPartons/CalcTtttPartonHistory.h"
#include "TopPartons/CalcTopPartonHistory.h"
#include "TopConfiguration/TopConfig.h"
#include <iostream>


namespace top {

  CalcTtttPartonHistory::CalcTtttPartonHistory(const std::string& name) : CalcTopPartonHistory(name) {}

  void CalcTtttPartonHistory::TtttHistorySaver(const xAOD::TruthParticleContainer* truthParticles,
					     xAOD::PartonHistory* TtttPartonHistory){
    
    TtttPartonHistory->IniVarTttt();

    // Extract the 4-top parton history
    bool event_tttt = CalcTopPartonHistory::tttt(truthParticles, top_pdgId, top_beforeFSR_p4, top_afterFSR_p4, 
						 b_p4, W_p4, Wdecay1_pdgId, Wdecay2_pdgId, Wdecay1_p4, Wdecay2_p4);


    if(!event_tttt){
      ATH_MSG_WARNING("WARNING:\t Fail to extract the 4-top parton history.");
      return;
    }

    // Find the index of the top quarks in decreasing pt order after FSR
    int index_ordered[4] = {0, 1, 2, 3};
    std::sort(index_ordered, index_ordered+4,
	      [&](int a, int b){
		return( top_afterFSR_p4[a].Pt() > top_afterFSR_p4[b].Pt() );
	      });

    //top1 variables
    TtttPartonHistory->auxdecor< int >("MC_top1_pdgId") = top_pdgId[index_ordered[0]];
    TtttPartonHistory->auxdecor< float >("MC_top1_beforeFSR_m") = top_beforeFSR_p4[index_ordered[0]].M();
    TtttPartonHistory->auxdecor< float >("MC_top1_beforeFSR_pt") = top_beforeFSR_p4[index_ordered[0]].Pt();
    fillEtaBranch(TtttPartonHistory, "MC_top1_beforeFSR_eta", top_beforeFSR_p4[index_ordered[0]]);
    TtttPartonHistory->auxdecor< float >("MC_top1_beforeFSR_phi") = top_beforeFSR_p4[index_ordered[0]].Phi();
    TtttPartonHistory->auxdecor< float >("MC_top1_afterFSR_m") = top_afterFSR_p4[index_ordered[0]].M();
    TtttPartonHistory->auxdecor< float >("MC_top1_afterFSR_pt") = top_afterFSR_p4[index_ordered[0]].Pt();
    fillEtaBranch(TtttPartonHistory, "MC_top1_afterFSR_eta", top_afterFSR_p4[index_ordered[0]]);
    TtttPartonHistory->auxdecor< float >("MC_top1_afterFSR_phi") = top_afterFSR_p4[index_ordered[0]].Phi();
    TtttPartonHistory->auxdecor< float >("MC_top1_b_m") = b_p4[index_ordered[0]].M();
    TtttPartonHistory->auxdecor< float >("MC_top1_b_pt") = b_p4[index_ordered[0]].Pt();
    fillEtaBranch(TtttPartonHistory, "MC_top1_b_eta", b_p4[index_ordered[0]]);
    TtttPartonHistory->auxdecor< float >("MC_top1_b_phi") = b_p4[index_ordered[0]].Phi();
    TtttPartonHistory->auxdecor< float >("MC_top1_W_m") = W_p4[index_ordered[0]].M();
    TtttPartonHistory->auxdecor< float >("MC_top1_W_pt") = W_p4[index_ordered[0]].Pt();
    fillEtaBranch(TtttPartonHistory, "MC_top1_W_eta", W_p4[index_ordered[0]]);
    TtttPartonHistory->auxdecor< float >("MC_top1_W_phi") = W_p4[index_ordered[0]].Phi();
    TtttPartonHistory->auxdecor< float >("MC_top1_Wdecay1_m") = Wdecay1_p4[index_ordered[0]].M();
    TtttPartonHistory->auxdecor< float >("MC_top1_Wdecay1_pt") = Wdecay1_p4[index_ordered[0]].Pt();
    fillEtaBranch(TtttPartonHistory, "MC_top1_Wdecay1_eta", Wdecay1_p4[index_ordered[0]]);
    TtttPartonHistory->auxdecor< float >("MC_top1_Wdecay1_phi") = Wdecay1_p4[index_ordered[0]].Phi();
    TtttPartonHistory->auxdecor< int >("MC_top1_Wdecay1_pdgId") = Wdecay1_pdgId[index_ordered[0]];
    TtttPartonHistory->auxdecor< float >("MC_top1_Wdecay2_m") = Wdecay2_p4[index_ordered[0]].M();
    TtttPartonHistory->auxdecor< float >("MC_top1_Wdecay2_pt") = Wdecay2_p4[index_ordered[0]].Pt();
    fillEtaBranch(TtttPartonHistory, "MC_top1_Wdecay2_eta", Wdecay2_p4[index_ordered[0]]);
    TtttPartonHistory->auxdecor< float >("MC_top1_Wdecay2_phi") = Wdecay2_p4[index_ordered[0]].Phi();
    TtttPartonHistory->auxdecor< int >("MC_top1_Wdecay2_pdgId") = Wdecay2_pdgId[index_ordered[0]];

    //top2 variables
    TtttPartonHistory->auxdecor< int >("MC_top2_pdgId") = top_pdgId[index_ordered[1]];
    TtttPartonHistory->auxdecor< float >("MC_top2_beforeFSR_m") = top_beforeFSR_p4[index_ordered[1]].M();
    TtttPartonHistory->auxdecor< float >("MC_top2_beforeFSR_pt") = top_beforeFSR_p4[index_ordered[1]].Pt();
    fillEtaBranch(TtttPartonHistory, "MC_top2_beforeFSR_eta", top_beforeFSR_p4[index_ordered[1]]);
    TtttPartonHistory->auxdecor< float >("MC_top2_beforeFSR_phi") = top_beforeFSR_p4[index_ordered[1]].Phi();
    TtttPartonHistory->auxdecor< float >("MC_top2_afterFSR_m") = top_afterFSR_p4[index_ordered[1]].M();
    TtttPartonHistory->auxdecor< float >("MC_top2_afterFSR_pt") = top_afterFSR_p4[index_ordered[1]].Pt();
    fillEtaBranch(TtttPartonHistory, "MC_top2_afterFSR_eta", top_afterFSR_p4[index_ordered[1]]);
    TtttPartonHistory->auxdecor< float >("MC_top2_afterFSR_phi") = top_afterFSR_p4[index_ordered[1]].Phi();
    TtttPartonHistory->auxdecor< float >("MC_top2_b_m") = b_p4[index_ordered[1]].M();
    TtttPartonHistory->auxdecor< float >("MC_top2_b_pt") = b_p4[index_ordered[1]].Pt();
    fillEtaBranch(TtttPartonHistory, "MC_top2_b_eta", b_p4[index_ordered[1]]);
    TtttPartonHistory->auxdecor< float >("MC_top2_b_phi") = b_p4[index_ordered[1]].Phi();
    TtttPartonHistory->auxdecor< float >("MC_top2_W_m") = W_p4[index_ordered[1]].M();
    TtttPartonHistory->auxdecor< float >("MC_top2_W_pt") = W_p4[index_ordered[1]].Pt();
    fillEtaBranch(TtttPartonHistory, "MC_top2_W_eta", W_p4[index_ordered[1]]);
    TtttPartonHistory->auxdecor< float >("MC_top2_W_phi") = W_p4[index_ordered[1]].Phi();
    TtttPartonHistory->auxdecor< float >("MC_top2_Wdecay1_m") = Wdecay1_p4[index_ordered[1]].M();
    TtttPartonHistory->auxdecor< float >("MC_top2_Wdecay1_pt") = Wdecay1_p4[index_ordered[1]].Pt();
    fillEtaBranch(TtttPartonHistory, "MC_top2_Wdecay1_eta", Wdecay1_p4[index_ordered[1]]);
    TtttPartonHistory->auxdecor< float >("MC_top2_Wdecay1_phi") = Wdecay1_p4[index_ordered[1]].Phi();
    TtttPartonHistory->auxdecor< int >("MC_top2_Wdecay1_pdgId") = Wdecay1_pdgId[index_ordered[1]];
    TtttPartonHistory->auxdecor< float >("MC_top2_Wdecay2_m") = Wdecay2_p4[index_ordered[1]].M();
    TtttPartonHistory->auxdecor< float >("MC_top2_Wdecay2_pt") = Wdecay2_p4[index_ordered[1]].Pt();
    fillEtaBranch(TtttPartonHistory, "MC_top2_Wdecay2_eta", Wdecay2_p4[index_ordered[1]]);
    TtttPartonHistory->auxdecor< float >("MC_top2_Wdecay2_phi") = Wdecay2_p4[index_ordered[1]].Phi();
    TtttPartonHistory->auxdecor< int >("MC_top2_Wdecay2_pdgId") = Wdecay2_pdgId[index_ordered[1]];

    //top3 variables
    TtttPartonHistory->auxdecor< int >("MC_top3_pdgId") = top_pdgId[index_ordered[2]];
    TtttPartonHistory->auxdecor< float >("MC_top3_beforeFSR_m") = top_beforeFSR_p4[index_ordered[2]].M();
    TtttPartonHistory->auxdecor< float >("MC_top3_beforeFSR_pt") = top_beforeFSR_p4[index_ordered[2]].Pt();
    fillEtaBranch(TtttPartonHistory, "MC_top3_beforeFSR_eta", top_beforeFSR_p4[index_ordered[2]]);
    TtttPartonHistory->auxdecor< float >("MC_top3_beforeFSR_phi") = top_beforeFSR_p4[index_ordered[2]].Phi();
    TtttPartonHistory->auxdecor< float >("MC_top3_afterFSR_m") = top_afterFSR_p4[index_ordered[2]].M();
    TtttPartonHistory->auxdecor< float >("MC_top3_afterFSR_pt") = top_afterFSR_p4[index_ordered[2]].Pt();
    fillEtaBranch(TtttPartonHistory, "MC_top3_afterFSR_eta", top_afterFSR_p4[index_ordered[2]]);
    TtttPartonHistory->auxdecor< float >("MC_top3_afterFSR_phi") = top_afterFSR_p4[index_ordered[2]].Phi();
    TtttPartonHistory->auxdecor< float >("MC_top3_b_m") = b_p4[index_ordered[2]].M();
    TtttPartonHistory->auxdecor< float >("MC_top3_b_pt") = b_p4[index_ordered[2]].Pt();
    fillEtaBranch(TtttPartonHistory, "MC_top3_b_eta", b_p4[index_ordered[2]]);
    TtttPartonHistory->auxdecor< float >("MC_top3_b_phi") = b_p4[index_ordered[2]].Phi();
    TtttPartonHistory->auxdecor< float >("MC_top3_W_m") = W_p4[index_ordered[2]].M();
    TtttPartonHistory->auxdecor< float >("MC_top3_W_pt") = W_p4[index_ordered[2]].Pt();
    fillEtaBranch(TtttPartonHistory, "MC_top3_W_eta", W_p4[index_ordered[2]]);
    TtttPartonHistory->auxdecor< float >("MC_top3_W_phi") = W_p4[index_ordered[2]].Phi();
    TtttPartonHistory->auxdecor< float >("MC_top3_Wdecay1_m") = Wdecay1_p4[index_ordered[2]].M();
    TtttPartonHistory->auxdecor< float >("MC_top3_Wdecay1_pt") = Wdecay1_p4[index_ordered[2]].Pt();
    fillEtaBranch(TtttPartonHistory, "MC_top3_Wdecay1_eta", Wdecay1_p4[index_ordered[2]]);
    TtttPartonHistory->auxdecor< float >("MC_top3_Wdecay1_phi") = Wdecay1_p4[index_ordered[2]].Phi();
    TtttPartonHistory->auxdecor< int >("MC_top3_Wdecay1_pdgId") = Wdecay1_pdgId[index_ordered[2]];
    TtttPartonHistory->auxdecor< float >("MC_top3_Wdecay2_m") = Wdecay2_p4[index_ordered[2]].M();
    TtttPartonHistory->auxdecor< float >("MC_top3_Wdecay2_pt") = Wdecay2_p4[index_ordered[2]].Pt();
    fillEtaBranch(TtttPartonHistory, "MC_top3_Wdecay2_eta", Wdecay2_p4[index_ordered[2]]);
    TtttPartonHistory->auxdecor< float >("MC_top3_Wdecay2_phi") = Wdecay2_p4[index_ordered[2]].Phi();
    TtttPartonHistory->auxdecor< int >("MC_top3_Wdecay2_pdgId") = Wdecay2_pdgId[index_ordered[2]];

    //top4 variables
    TtttPartonHistory->auxdecor< int >("MC_top4_pdgId") = top_pdgId[index_ordered[3]];
    TtttPartonHistory->auxdecor< float >("MC_top4_beforeFSR_m") = top_beforeFSR_p4[index_ordered[3]].M();
    TtttPartonHistory->auxdecor< float >("MC_top4_beforeFSR_pt") = top_beforeFSR_p4[index_ordered[3]].Pt();
    fillEtaBranch(TtttPartonHistory, "MC_top4_beforeFSR_eta", top_beforeFSR_p4[index_ordered[3]]);
    TtttPartonHistory->auxdecor< float >("MC_top4_beforeFSR_phi") = top_beforeFSR_p4[index_ordered[3]].Phi();
    TtttPartonHistory->auxdecor< float >("MC_top4_afterFSR_m") = top_afterFSR_p4[index_ordered[3]].M();
    TtttPartonHistory->auxdecor< float >("MC_top4_afterFSR_pt") = top_afterFSR_p4[index_ordered[3]].Pt();
    fillEtaBranch(TtttPartonHistory, "MC_top4_afterFSR_eta", top_afterFSR_p4[index_ordered[3]]);
    TtttPartonHistory->auxdecor< float >("MC_top4_afterFSR_phi") = top_afterFSR_p4[index_ordered[3]].Phi();
    TtttPartonHistory->auxdecor< float >("MC_top4_b_m") = b_p4[index_ordered[3]].M();
    TtttPartonHistory->auxdecor< float >("MC_top4_b_pt") = b_p4[index_ordered[3]].Pt();
    fillEtaBranch(TtttPartonHistory, "MC_top4_b_eta", b_p4[index_ordered[3]]);
    TtttPartonHistory->auxdecor< float >("MC_top4_b_phi") = b_p4[index_ordered[3]].Phi();
    TtttPartonHistory->auxdecor< float >("MC_top4_W_m") = W_p4[index_ordered[3]].M();
    TtttPartonHistory->auxdecor< float >("MC_top4_W_pt") = W_p4[index_ordered[3]].Pt();
    fillEtaBranch(TtttPartonHistory, "MC_top4_W_eta", W_p4[index_ordered[3]]);
    TtttPartonHistory->auxdecor< float >("MC_top4_W_phi") = W_p4[index_ordered[3]].Phi();
    TtttPartonHistory->auxdecor< float >("MC_top4_Wdecay1_m") = Wdecay1_p4[index_ordered[3]].M();
    TtttPartonHistory->auxdecor< float >("MC_top4_Wdecay1_pt") = Wdecay1_p4[index_ordered[3]].Pt();
    fillEtaBranch(TtttPartonHistory, "MC_top4_Wdecay1_eta", Wdecay1_p4[index_ordered[3]]);
    TtttPartonHistory->auxdecor< float >("MC_top4_Wdecay1_phi") = Wdecay1_p4[index_ordered[3]].Phi();
    TtttPartonHistory->auxdecor< int >("MC_top4_Wdecay1_pdgId") = Wdecay1_pdgId[index_ordered[3]];
    TtttPartonHistory->auxdecor< float >("MC_top4_Wdecay2_m") = Wdecay2_p4[index_ordered[3]].M();
    TtttPartonHistory->auxdecor< float >("MC_top4_Wdecay2_pt") = Wdecay2_p4[index_ordered[3]].Pt();
    fillEtaBranch(TtttPartonHistory, "MC_top4_Wdecay2_eta", Wdecay2_p4[index_ordered[3]]);
    TtttPartonHistory->auxdecor< float >("MC_top4_Wdecay2_phi") = Wdecay2_p4[index_ordered[3]].Phi();
    TtttPartonHistory->auxdecor< int >("MC_top4_Wdecay2_pdgId") = Wdecay2_pdgId[index_ordered[3]];

    // Combined mass of the 4-top system
    TtttPartonHistory->auxdecor< float >("MC_mtttt") = (top_afterFSR_p4[0]+top_afterFSR_p4[1]+top_afterFSR_p4[2]+top_afterFSR_p4[3]).M();

    // Sum of the transvers mass
    TtttPartonHistory->auxdecor< float >("MC_HT") = top_afterFSR_p4[0].Mt() + top_afterFSR_p4[1].Mt() + top_afterFSR_p4[2].Mt() + top_afterFSR_p4[3].Mt();

  }

  StatusCode CalcTtttPartonHistory::execute() {
    //Get the Truth Particles
    const xAOD::TruthParticleContainer* truthParticles(nullptr);

    ATH_CHECK(evtStore()->retrieve(truthParticles, m_config->sgKeyMCParticle()));

    // Create the partonHistory xAOD object
    xAOD::PartonHistoryAuxContainer* partonAuxCont = new xAOD::PartonHistoryAuxContainer {};
    xAOD::PartonHistoryContainer* partonCont = new xAOD::PartonHistoryContainer {};
    partonCont->setStore(partonAuxCont);

    xAOD::PartonHistory* TtttPartonHistory = new xAOD::PartonHistory {};
    partonCont->push_back(TtttPartonHistory);

    // Recover the parton history for TZ events
    TtttHistorySaver(truthParticles, TtttPartonHistory);

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
