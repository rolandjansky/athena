/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#include "TopPartons/CalcTtbarPartonHistory.h"
#include "TopConfiguration/TopConfig.h"

namespace top {
  CalcTtbarPartonHistory::CalcTtbarPartonHistory(const std::string& name) : CalcTopPartonHistory(name) {}

  void CalcTtbarPartonHistory::ttbarHistorySaver(const xAOD::TruthParticleContainer* truthParticles,
                                                 xAOD::PartonHistory* ttbarPartonHistory) {
    ttbarPartonHistory->IniVarTtbar();

    TLorentzVector t_before, t_after, t_after_SC;
    TLorentzVector Wp;
    TLorentzVector b;
    TLorentzVector WpDecay1;
    TLorentzVector WpDecay2;
    int WpDecay1_pdgId;
    int WpDecay2_pdgId;

    bool event_top = CalcTopPartonHistory::topWb(truthParticles, 6, t_before, t_after, Wp, b, WpDecay1, WpDecay1_pdgId,
                                                 WpDecay2, WpDecay2_pdgId);
    bool event_top_SC = CalcTopPartonHistory::topAfterFSR_SC(truthParticles, 6, t_after_SC);

    TLorentzVector tbar_before, tbar_after, tbar_after_SC;
    TLorentzVector Wm;
    TLorentzVector bbar;
    TLorentzVector WmDecay1;
    TLorentzVector WmDecay2;
    int WmDecay1_pdgId;
    int WmDecay2_pdgId;

    bool event_topbar = CalcTopPartonHistory::topWb(truthParticles, -6, tbar_before, tbar_after, Wm, bbar, WmDecay1,
                                                    WmDecay1_pdgId, WmDecay2, WmDecay2_pdgId);
    bool event_topbar_SC = CalcTopPartonHistory::topAfterFSR_SC(truthParticles, -6, tbar_after_SC);

    if (event_top && event_topbar) {
      TLorentzVector temp = t_before + tbar_before;
      ttbarPartonHistory->auxdecor< float >("MC_ttbar_beforeFSR_m") = temp.M();
      ttbarPartonHistory->auxdecor< float >("MC_ttbar_beforeFSR_pt") = temp.Pt();
      ttbarPartonHistory->auxdecor< float >("MC_ttbar_beforeFSR_phi") = temp.Phi();
      fillEtaBranch(ttbarPartonHistory, "MC_ttbar_beforeFSR_eta", temp);

      temp = t_after + tbar_after;
      ttbarPartonHistory->auxdecor< float >("MC_ttbar_afterFSR_beforeDecay_m") = temp.M();
      ttbarPartonHistory->auxdecor< float >("MC_ttbar_afterFSR_beforeDecay_pt") = temp.Pt();
      ttbarPartonHistory->auxdecor< float >("MC_ttbar_afterFSR_beforeDecay_phi") = temp.Phi();
      fillEtaBranch(ttbarPartonHistory, "MC_ttbar_afterFSR_beforeDecay_eta", temp);

      temp = WmDecay1 + WmDecay2 + b + WpDecay1 + WpDecay2 + bbar;
      ttbarPartonHistory->auxdecor< float >("MC_ttbar_afterFSR_m") = temp.M();
      ttbarPartonHistory->auxdecor< float >("MC_ttbar_afterFSR_pt") = temp.Pt();
      ttbarPartonHistory->auxdecor< float >("MC_ttbar_afterFSR_phi") = temp.Phi();
      fillEtaBranch(ttbarPartonHistory, "MC_ttbar_afterFSR_eta", temp);
    }//if
    if (event_top) {
      ttbarPartonHistory->auxdecor< float >("MC_t_beforeFSR_m") = t_before.M();
      ttbarPartonHistory->auxdecor< float >("MC_t_beforeFSR_pt") = t_before.Pt();
      ttbarPartonHistory->auxdecor< float >("MC_t_beforeFSR_phi") = t_before.Phi();
      fillEtaBranch(ttbarPartonHistory, "MC_t_beforeFSR_eta", t_before);

      ttbarPartonHistory->auxdecor< float >("MC_t_afterFSR_m") = t_after.M();
      ttbarPartonHistory->auxdecor< float >("MC_t_afterFSR_pt") = t_after.Pt();
      ttbarPartonHistory->auxdecor< float >("MC_t_afterFSR_phi") = t_after.Phi();
      fillEtaBranch(ttbarPartonHistory, "MC_t_afterFSR_eta", t_after);

      if (event_top_SC) {
        ttbarPartonHistory->auxdecor< float >("MC_t_afterFSR_SC_m") = t_after_SC.M();
        ttbarPartonHistory->auxdecor< float >("MC_t_afterFSR_SC_pt") = t_after_SC.Pt();
        ttbarPartonHistory->auxdecor< float >("MC_t_afterFSR_SC_phi") = t_after_SC.Phi();
        fillEtaBranch(ttbarPartonHistory, "MC_t_afterFSR_SC_eta", t_after_SC);
      }

      ttbarPartonHistory->auxdecor< float >("MC_W_from_t_m") = Wp.M();
      ttbarPartonHistory->auxdecor< float >("MC_W_from_t_pt") = Wp.Pt();
      ttbarPartonHistory->auxdecor< float >("MC_W_from_t_phi") = Wp.Phi();
      fillEtaBranch(ttbarPartonHistory, "MC_W_from_t_eta", Wp);

      ttbarPartonHistory->auxdecor< float >("MC_b_from_t_m") = b.M();
      ttbarPartonHistory->auxdecor< float >("MC_b_from_t_pt") = b.Pt();
      ttbarPartonHistory->auxdecor< float >("MC_b_from_t_phi") = b.Phi();
      fillEtaBranch(ttbarPartonHistory, "MC_b_from_t_eta", b);

      ttbarPartonHistory->auxdecor< float >("MC_Wdecay1_from_t_m") = WpDecay1.M();
      ttbarPartonHistory->auxdecor< float >("MC_Wdecay1_from_t_pt") = WpDecay1.Pt();
      ttbarPartonHistory->auxdecor< float >("MC_Wdecay1_from_t_phi") = WpDecay1.Phi();
      ttbarPartonHistory->auxdecor< int >("MC_Wdecay1_from_t_pdgId") = WpDecay1_pdgId;
      fillEtaBranch(ttbarPartonHistory, "MC_Wdecay1_from_t_eta", WpDecay1);

      ttbarPartonHistory->auxdecor< float >("MC_Wdecay2_from_t_m") = WpDecay2.M();
      ttbarPartonHistory->auxdecor< float >("MC_Wdecay2_from_t_pt") = WpDecay2.Pt();
      ttbarPartonHistory->auxdecor< float >("MC_Wdecay2_from_t_phi") = WpDecay2.Phi();
      ttbarPartonHistory->auxdecor< int >("MC_Wdecay2_from_t_pdgId") = WpDecay2_pdgId;
      fillEtaBranch(ttbarPartonHistory, "MC_Wdecay2_from_t_eta", WpDecay2);
    }//if
    if (event_topbar) {
      ttbarPartonHistory->auxdecor< float >("MC_tbar_beforeFSR_m") = tbar_before.M();
      ttbarPartonHistory->auxdecor< float >("MC_tbar_beforeFSR_pt") = tbar_before.Pt();
      ttbarPartonHistory->auxdecor< float >("MC_tbar_beforeFSR_phi") = tbar_before.Phi();
      fillEtaBranch(ttbarPartonHistory, "MC_tbar_beforeFSR_eta", tbar_before);

      ttbarPartonHistory->auxdecor< float >("MC_tbar_afterFSR_m") = tbar_after.M();
      ttbarPartonHistory->auxdecor< float >("MC_tbar_afterFSR_pt") = tbar_after.Pt();
      ttbarPartonHistory->auxdecor< float >("MC_tbar_afterFSR_phi") = tbar_after.Phi();
      fillEtaBranch(ttbarPartonHistory, "MC_tbar_afterFSR_eta", tbar_after);

      if (event_topbar_SC) {
        ttbarPartonHistory->auxdecor< float >("MC_tbar_afterFSR_SC_m") = tbar_after_SC.M();
        ttbarPartonHistory->auxdecor< float >("MC_tbar_afterFSR_SC_pt") = tbar_after_SC.Pt();
        ttbarPartonHistory->auxdecor< float >("MC_tbar_afterFSR_SC_phi") = tbar_after_SC.Phi();
        fillEtaBranch(ttbarPartonHistory, "MC_tbar_afterFSR_SC_eta", tbar_after_SC);
      }

      ttbarPartonHistory->auxdecor< float >("MC_W_from_tbar_m") = Wm.M();
      ttbarPartonHistory->auxdecor< float >("MC_W_from_tbar_pt") = Wm.Pt();
      ttbarPartonHistory->auxdecor< float >("MC_W_from_tbar_phi") = Wm.Phi();
      fillEtaBranch(ttbarPartonHistory, "MC_W_from_tbar_eta", Wm);

      ttbarPartonHistory->auxdecor< float >("MC_b_from_tbar_m") = bbar.M();
      ttbarPartonHistory->auxdecor< float >("MC_b_from_tbar_pt") = bbar.Pt();
      ttbarPartonHistory->auxdecor< float >("MC_b_from_tbar_phi") = bbar.Phi();
      fillEtaBranch(ttbarPartonHistory, "MC_b_from_tbar_eta", bbar);

      ttbarPartonHistory->auxdecor< float >("MC_Wdecay1_from_tbar_m") = WmDecay1.M();
      ttbarPartonHistory->auxdecor< float >("MC_Wdecay1_from_tbar_pt") = WmDecay1.Pt();
      ttbarPartonHistory->auxdecor< float >("MC_Wdecay1_from_tbar_phi") = WmDecay1.Phi();
      ttbarPartonHistory->auxdecor< int >("MC_Wdecay1_from_tbar_pdgId") = WmDecay1_pdgId;
      fillEtaBranch(ttbarPartonHistory, "MC_Wdecay1_from_tbar_eta", WmDecay1);

      ttbarPartonHistory->auxdecor< float >("MC_Wdecay2_from_tbar_m") = WmDecay2.M();
      ttbarPartonHistory->auxdecor< float >("MC_Wdecay2_from_tbar_pt") = WmDecay2.Pt();
      ttbarPartonHistory->auxdecor< float >("MC_Wdecay2_from_tbar_phi") = WmDecay2.Phi();
      ttbarPartonHistory->auxdecor< int >("MC_Wdecay2_from_tbar_pdgId") = WmDecay2_pdgId;
      fillEtaBranch(ttbarPartonHistory, "MC_Wdecay2_from_tbar_eta", WmDecay2);
    }//if
  }

  StatusCode CalcTtbarPartonHistory::execute() {
    // Get the Truth Particles

    const xAOD::TruthParticleContainer* truthParticles(nullptr);
    
    if(m_config->getDerivationStream() == "PHYS") //in DAOD_PHYS we don't have the truth particles container
    {
      //the functions ued in this class always start from the top, so it's enough to do the following
      std::vector<std::string> collections = {"TruthTop"};
      ATH_CHECK(buildContainerFromMultipleCollections(collections,"AT_TTbarPartonHistory_TruthParticles"));
      ATH_CHECK(evtStore()->retrieve(truthParticles, "AT_TTbarPartonHistory_TruthParticles"));
      
      //we need to be able to navigate from the Ws to their decayProducts, see CalcTopPartonHistory.h for details
      ATH_CHECK(linkBosonCollections());
      
    }
    else  //otherwise we retrieve the container as usual
    {
      ATH_CHECK(evtStore()->retrieve(truthParticles, m_config->sgKeyMCParticle()));
    }
    
    // Create the partonHistory xAOD object
    xAOD::PartonHistoryAuxContainer* partonAuxCont = new xAOD::PartonHistoryAuxContainer {};
    xAOD::PartonHistoryContainer* partonCont = new xAOD::PartonHistoryContainer {};
    partonCont->setStore(partonAuxCont);

    xAOD::PartonHistory* ttbarPartonHistory = new xAOD::PartonHistory {};
    partonCont->push_back(ttbarPartonHistory);

    // Recover the parton history for ttbar events
    ttbarHistorySaver(truthParticles, ttbarPartonHistory);
    
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
