/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#include "TopPartons/CalcTtbarPartonHistory.h"
#include "TopConfiguration/TopConfig.h"
#include "TopPartons/PartonHistoryUtils.h"

namespace top {
  using PartonHistoryUtils::decorateWithMPtPhi;
  CalcTtbarPartonHistory::CalcTtbarPartonHistory(const std::string& name) : CalcTopPartonHistory(name) {}

  void CalcTtbarPartonHistory::ttbarHistorySaver(const xAOD::TruthParticleContainer* truthParticles,
                                                 xAOD::PartonHistory* ttbarPartonHistory) {
    ttbarPartonHistory->IniVarTtbar();

    TLorentzVector t_before, t_after, t_after_SC;
    TLorentzVector Wp;
    TLorentzVector b;
    TLorentzVector WpDecay1;
    TLorentzVector WpDecay2;
    int WpDecay1_pdgId{};
    int WpDecay2_pdgId{};

    bool event_top = CalcTopPartonHistory::topWb(truthParticles, 6, t_before, t_after, Wp, b, WpDecay1, WpDecay1_pdgId,
                                                 WpDecay2, WpDecay2_pdgId);
    bool event_top_SC = CalcTopPartonHistory::topAfterFSR_SC(truthParticles, 6, t_after_SC);

    TLorentzVector tbar_before, tbar_after, tbar_after_SC;
    TLorentzVector Wm;
    TLorentzVector bbar;
    TLorentzVector WmDecay1;
    TLorentzVector WmDecay2;
    int WmDecay1_pdgId{};
    int WmDecay2_pdgId{};

    bool event_topbar = CalcTopPartonHistory::topWb(truthParticles, -6, tbar_before, tbar_after, Wm, bbar, WmDecay1,
                                                    WmDecay1_pdgId, WmDecay2, WmDecay2_pdgId);
    bool event_topbar_SC = CalcTopPartonHistory::topAfterFSR_SC(truthParticles, -6, tbar_after_SC);

    if (event_top && event_topbar) {
      TLorentzVector temp = t_before + tbar_before;
      decorateWithMPtPhi(ttbarPartonHistory, "MC_ttbar_beforeFSR", temp);
      fillEtaBranch(ttbarPartonHistory, "MC_ttbar_beforeFSR_eta", temp);

      temp = t_after + tbar_after;
      decorateWithMPtPhi(ttbarPartonHistory, "MC_ttbar_afterFSR_beforeDecay", temp);
      fillEtaBranch(ttbarPartonHistory, "MC_ttbar_afterFSR_beforeDecay_eta", temp);

      temp = WmDecay1 + WmDecay2 + b + WpDecay1 + WpDecay2 + bbar;
      decorateWithMPtPhi(ttbarPartonHistory, "MC_ttbar_afterFSR", temp);
      fillEtaBranch(ttbarPartonHistory, "MC_ttbar_afterFSR_eta", temp);
    }//if
    if (event_top) {
      decorateWithMPtPhi(ttbarPartonHistory, "MC_t_beforeFSR", t_before);
      fillEtaBranch(ttbarPartonHistory, "MC_t_beforeFSR_eta", t_before);

      decorateWithMPtPhi(ttbarPartonHistory, "MC_t_afterFSR", t_after);
      fillEtaBranch(ttbarPartonHistory, "MC_t_afterFSR_eta", t_after);

      if (event_top_SC) {
        decorateWithMPtPhi(ttbarPartonHistory, "MC_t_afterFSR_SC", t_after_SC);
        fillEtaBranch(ttbarPartonHistory, "MC_t_afterFSR_SC_eta", t_after_SC);
      }
      decorateWithMPtPhi(ttbarPartonHistory, "MC_W_from_t", Wp);
      fillEtaBranch(ttbarPartonHistory, "MC_W_from_t_eta", Wp);

      decorateWithMPtPhi(ttbarPartonHistory, "MC_b_from_t", b);
      fillEtaBranch(ttbarPartonHistory, "MC_b_from_t_eta", b);

      decorateWithMPtPhi(ttbarPartonHistory, "MC_Wdecay1_from_t", WpDecay1);
      ttbarPartonHistory->auxdecor< int >("MC_Wdecay1_from_t_pdgId") = WpDecay1_pdgId;
      fillEtaBranch(ttbarPartonHistory, "MC_Wdecay1_from_t_eta", WpDecay1);

      decorateWithMPtPhi(ttbarPartonHistory, "MC_Wdecay2_from_t", WpDecay2);
      ttbarPartonHistory->auxdecor< int >("MC_Wdecay2_from_t_pdgId") = WpDecay2_pdgId;
      fillEtaBranch(ttbarPartonHistory, "MC_Wdecay2_from_t_eta", WpDecay2);
    }//if
    if (event_topbar) {
      decorateWithMPtPhi(ttbarPartonHistory, "MC_tbar_beforeFSR", tbar_before);
      fillEtaBranch(ttbarPartonHistory, "MC_tbar_beforeFSR_eta", tbar_before);

      decorateWithMPtPhi(ttbarPartonHistory, "MC_tbar_afterFSR", tbar_after);
      fillEtaBranch(ttbarPartonHistory, "MC_tbar_afterFSR_eta", tbar_after);

      if (event_topbar_SC) {
        decorateWithMPtPhi(ttbarPartonHistory, "MC_tbar_afterFSR_SC", tbar_after_SC);
        fillEtaBranch(ttbarPartonHistory, "MC_tbar_afterFSR_SC_eta", tbar_after_SC);
      }
      
      decorateWithMPtPhi(ttbarPartonHistory, "MC_W_from_tbar", Wm);
      fillEtaBranch(ttbarPartonHistory, "MC_W_from_tbar_eta", Wm);
      
      decorateWithMPtPhi(ttbarPartonHistory, "MC_b_from_tbar", bbar);
      fillEtaBranch(ttbarPartonHistory, "MC_b_from_tbar_eta", bbar);
      
      decorateWithMPtPhi(ttbarPartonHistory, "MC_Wdecay1_from_tbar", WmDecay1);
      ttbarPartonHistory->auxdecor< int >("MC_Wdecay1_from_tbar_pdgId") = WmDecay1_pdgId;
      fillEtaBranch(ttbarPartonHistory, "MC_Wdecay1_from_tbar_eta", WmDecay1);
      
      decorateWithMPtPhi(ttbarPartonHistory, "MC_Wdecay2_from_tbar", WmDecay2);
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
    //cppcheck-suppress uninitvar
    xAOD::PartonHistoryAuxContainer* partonAuxCont = new xAOD::PartonHistoryAuxContainer {};
    //cppcheck-suppress uninitvar
    xAOD::PartonHistoryContainer* partonCont = new xAOD::PartonHistoryContainer {};
    partonCont->setStore(partonAuxCont);
    //cppcheck-suppress uninitvar
    xAOD::PartonHistory* ttbarPartonHistory = new xAOD::PartonHistory {};
    partonCont->push_back(ttbarPartonHistory);

    // Recover the parton history for ttbar events
    ttbarHistorySaver(truthParticles, ttbarPartonHistory);
    
    // Save to StoreGate / TStore
    std::string outputSGKey = m_config->sgKeyTopPartonHistory();
    std::string outputSGKeyAux = outputSGKey + "Aux.";

    StatusCode save = evtStore()->tds()->record(partonCont, outputSGKey);
    StatusCode saveAux = evtStore()->tds()->record(partonAuxCont, outputSGKeyAux);
    if (!save || !saveAux) {
      return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
  }
}
