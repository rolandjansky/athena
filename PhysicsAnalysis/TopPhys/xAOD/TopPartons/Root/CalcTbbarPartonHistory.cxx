/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#include "TopPartons/CalcTbbarPartonHistory.h"
#include "TopConfiguration/TopConfig.h"
#include "TopPartons/PartonHistoryUtils.h"


namespace top {
  using PartonHistoryUtils::decorateWithMPtPhi;

  CalcTbbarPartonHistory::CalcTbbarPartonHistory(const std::string& name) : CalcTopPartonHistory(name) {}

  void CalcTbbarPartonHistory::tbbarHistorySaver(const xAOD::TruthParticleContainer* truthParticles,
                                                 xAOD::PartonHistory* tbbarPartonHistory) {
    tbbarPartonHistory->IniVarTbbar();

    TLorentzVector t_before, t_after;
    TLorentzVector Wp;
    TLorentzVector b_from_top;
    TLorentzVector WpDecay1;
    TLorentzVector WpDecay2;
    int WpDecay1_pdgId;
    int WpDecay2_pdgId;
    bool event_top = CalcTopPartonHistory::topWb(truthParticles, 6, t_before, t_after, Wp, b_from_top, WpDecay1,
                                                 WpDecay1_pdgId, WpDecay2, WpDecay2_pdgId);
    if (!event_top) event_top = CalcTopPartonHistory::topWb(truthParticles, -6, t_before, t_after, Wp, b_from_top,
                                                            WpDecay1, WpDecay1_pdgId, WpDecay2, WpDecay2_pdgId);

    TLorentzVector b_beforeFSR;
    TLorentzVector b_afterFSR;
    bool event_b = CalcTopPartonHistory::b(truthParticles, b_beforeFSR, b_afterFSR);

    if (event_top && event_b) {
      TLorentzVector temp = t_before + b_beforeFSR;
      decorateWithMPtPhi(tbbarPartonHistory, "MC_tbbar_beforeFSR", temp);
      fillEtaBranch(tbbarPartonHistory, "MC_tbbar_beforeFSR_eta", temp);

      temp = WpDecay1 + WpDecay2 + b_from_top + b_afterFSR;
      decorateWithMPtPhi(tbbarPartonHistory, "MC_tbbar_afterFSR", temp);
      fillEtaBranch(tbbarPartonHistory, "MC_tbbar_afterFSR_eta", temp);
    }//if
    if (event_top) {
      decorateWithMPtPhi(tbbarPartonHistory, "MC_t_beforeFSR", t_before);
      fillEtaBranch(tbbarPartonHistory, "MC_t_beforeFSR_eta", t_before);

      decorateWithMPtPhi(tbbarPartonHistory, "MC_W_from_t", Wp);
      fillEtaBranch(tbbarPartonHistory, "MC_W_from_t_eta", Wp);

      decorateWithMPtPhi(tbbarPartonHistory, "MC_b_from_t", b_from_top);
      fillEtaBranch(tbbarPartonHistory, "MC_b_from_t_eta", b_from_top);

      decorateWithMPtPhi(tbbarPartonHistory, "MC_Wdecay1_from_t", WpDecay1);
      tbbarPartonHistory->auxdecor< int >("MC_Wdecay1_from_t_pdgId") = WpDecay1_pdgId;
      fillEtaBranch(tbbarPartonHistory, "MC_Wdecay1_from_t_eta", WpDecay1);

      decorateWithMPtPhi(tbbarPartonHistory, "MC_Wdecay2_from_t", WpDecay2);
      tbbarPartonHistory->auxdecor< int >("MC_Wdecay2_from_t_pdgId") = WpDecay2_pdgId;
      fillEtaBranch(tbbarPartonHistory, "MC_Wdecay2_from_t_eta", WpDecay2);
    }//if
    if (event_b) {
      decorateWithMPtPhi(tbbarPartonHistory, "MC_b_beforeFSR", b_beforeFSR);
      fillEtaBranch(tbbarPartonHistory, "MC_b_beforeFSR_eta", b_beforeFSR);

      decorateWithMPtPhi(tbbarPartonHistory, "MC_b_afterFSR", b_afterFSR);
      fillEtaBranch(tbbarPartonHistory, "MC_b_afterFSR_eta", b_afterFSR);
    }//if
  }

  StatusCode CalcTbbarPartonHistory::execute() {
    // Get the Truth Particles
    const xAOD::TruthParticleContainer* truthParticles(nullptr);

    ATH_CHECK(evtStore()->retrieve(truthParticles, m_config->sgKeyMCParticle()));

    // Create the partonHistory xAOD object
    //cppcheck-suppress uninitvar
    xAOD::PartonHistoryAuxContainer* partonAuxCont = new xAOD::PartonHistoryAuxContainer {};
    //cppcheck-suppress uninitvar
    xAOD::PartonHistoryContainer* partonCont = new xAOD::PartonHistoryContainer {};
    partonCont->setStore(partonAuxCont);
    //cppcheck-suppress uninitvar
    xAOD::PartonHistory* tbbarPartonHistory = new xAOD::PartonHistory {};
    partonCont->push_back(tbbarPartonHistory);

    // Recover the parton history for tbbar events
    tbbarHistorySaver(truthParticles, tbbarPartonHistory);

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
