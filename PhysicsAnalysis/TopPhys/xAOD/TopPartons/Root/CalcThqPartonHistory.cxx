/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

#include "TopPartons/CalcThqPartonHistory.h"
#include "TopPartons/CalcTopPartonHistory.h"
#include "TopPartons/PartonHistoryUtils.h"
#include "TopConfiguration/TopConfig.h"

namespace top {
  using PartonHistoryUtils::decorateWithMPtPhi;
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
    int WpDecay1_pdgId{};
    int WpDecay2_pdgId{};

    const bool event_top = CalcTopPartonHistory::topWb(truthParticles, 6, t_before, t_after,
                                                       Wp, b, WpDecay1, WpDecay1_pdgId,
                                                       WpDecay2, WpDecay2_pdgId);
    const bool event_top_SC = CalcTopPartonHistory::topAfterFSR_SC(truthParticles, 6, t_after_SC);
    const bool event_topbar = CalcTopPartonHistory::topWb(truthParticles, -6, t_before, t_after,
                                                          Wp, b, WpDecay1,
                                                          WpDecay1_pdgId, WpDecay2, WpDecay2_pdgId);
    const bool event_topbar_SC = CalcTopPartonHistory::topAfterFSR_SC(truthParticles, -6, t_after_SC);
    const bool event_Higgs =  CalcThqPartonHistory::HiggsAndDecay(truthParticles);

    if (event_Higgs) {
      if (event_top || event_topbar) {
      
        decorateWithMPtPhi(ThqPartonHistory,"MC_t_beforeFSR", t_before);    
        fillEtaBranch(ThqPartonHistory, "MC_t_beforeFSR_eta", t_before);

        decorateWithMPtPhi(ThqPartonHistory,"MC_t_afterFSR", t_after);    
        fillEtaBranch(ThqPartonHistory, "MC_t_afterFSR_eta", t_after);

        if (event_top_SC || event_topbar_SC) {
          decorateWithMPtPhi(ThqPartonHistory,"MC_t_afterFSR_SC", t_after_SC);    
          fillEtaBranch(ThqPartonHistory, "MC_t_afterFSR_SC_eta", t_after_SC);
        }

        decorateWithMPtPhi(ThqPartonHistory,"MC_W_from_t", Wp);
        fillEtaBranch(ThqPartonHistory, "MC_W_from_t_eta", Wp);

        decorateWithMPtPhi(ThqPartonHistory,"MC_b_from_t", b);
        fillEtaBranch(ThqPartonHistory, "MC_b_from_t_eta", b);

        decorateWithMPtPhi(ThqPartonHistory,"MC_Wdecay1_from_t", WpDecay1);
        ThqPartonHistory->auxdecor< int >("MC_Wdecay1_from_t_pdgId") = WpDecay1_pdgId;
        fillEtaBranch(ThqPartonHistory, "MC_Wdecay1_from_t_eta", WpDecay1);

        decorateWithMPtPhi(ThqPartonHistory,"MC_Wdecay2_from_t", WpDecay2);
        ThqPartonHistory->auxdecor< int >("MC_Wdecay2_from_t_pdgId") = WpDecay2_pdgId;
        fillEtaBranch(ThqPartonHistory, "MC_Wdecay2_from_t_eta", WpDecay2);

        //Higgs-Variables
        decorateWithMPtPhi(ThqPartonHistory,"MC_Higgs", tH.Higgs_p4);
        fillEtaBranch(ThqPartonHistory, "MC_Higgs_eta", tH.Higgs_p4);

        //Higgs-decay1-Variables
        decorateWithMPtPhi(ThqPartonHistory,"MC_Higgs_decay1", tH.decay1_p4);
        ThqPartonHistory->auxdecor< int >("MC_Higgs_decay1_pdgId") = tH.decay1_pdgId;
        ThqPartonHistory->auxdecor< int >("MC_Higgs_tau_decay1_isHadronic") = tH.tau_decay1_isHadronic;
        fillEtaBranch(ThqPartonHistory, "MC_Higgs_decay1_eta", tH.decay1_p4);

        //Higgs-decay2-Variables
        decorateWithMPtPhi(ThqPartonHistory,"MC_Higgs_decay2", tH.decay2_p4);
        ThqPartonHistory->auxdecor< int >("MC_Higgs_decay2_pdgId") = tH.decay2_pdgId;
        ThqPartonHistory->auxdecor< int >("MC_Higgs_tau_decay2_isHadronic") = tH.tau_decay2_isHadronic;
        fillEtaBranch(ThqPartonHistory, "MC_Higgs_decay2_eta", tH.decay2_p4);

        //Higgs-decay1- from decay1-Variables
        decorateWithMPtPhi(ThqPartonHistory,"MC_Higgs_decay1_from_decay1", tH.decay1_from_decay1_p4);
        ThqPartonHistory->auxdecor< int >("MC_Higgs_decay1_from_decay1_pdgId") = tH.decay1_from_decay1_pdgId;
        ThqPartonHistory->auxdecor< int >("MC_Higgs_tau_decay1_from_decay1_isHadronic") = tH.tau_decay1_from_decay1_isHadronic;
        fillEtaBranch(ThqPartonHistory, "MC_Higgs_decay1_from_decay1_eta", tH.decay1_from_decay1_p4);

        //Higgs-decay2- from decay1-Variables
        decorateWithMPtPhi(ThqPartonHistory,"MC_Higgs_decay2_from_decay1", tH.decay2_from_decay1_p4);
        ThqPartonHistory->auxdecor< int >("MC_Higgs_decay2_from_decay1_pdgId") = tH.decay2_from_decay1_pdgId;
        ThqPartonHistory->auxdecor< int >("MC_Higgs_tau_decay2_from_decay1_isHadronic") = tH.tau_decay2_from_decay1_isHadronic;
        fillEtaBranch(ThqPartonHistory, "MC_Higgs_decay2_from_decay1_eta", tH.decay2_from_decay1_p4);

        //Higgs-decay1- from decay2-Variables
        decorateWithMPtPhi(ThqPartonHistory,"MC_Higgs_decay1_from_decay2", tH.decay1_from_decay2_p4);
        ThqPartonHistory->auxdecor< int >("MC_Higgs_decay1_from_decay2_pdgId") = tH.decay1_from_decay2_pdgId;
        ThqPartonHistory->auxdecor< int >("MC_Higgs_tau_decay1_from_decay2_isHadronic") = tH.tau_decay1_from_decay2_isHadronic;
        fillEtaBranch(ThqPartonHistory, "MC_Higgs_decay1_from_decay2_eta", tH.decay1_from_decay2_p4);

        //Higgs-decay2- from decay2-Variables
        decorateWithMPtPhi(ThqPartonHistory,"MC_Higgs_decay2_from_decay2", tH.decay2_from_decay2_p4);
        ThqPartonHistory->auxdecor< int >("MC_Higgs_decay2_from_decay2_pdgId") = tH.decay2_from_decay2_pdgId;
        ThqPartonHistory->auxdecor< int >("MC_Higgs_tau_decay2_from_decay2_isHadronic") = tH.tau_decay2_from_decay2_isHadronic;
        fillEtaBranch(ThqPartonHistory, "MC_Higgs_decay2_from_decay2_eta", tH.decay2_from_decay2_p4);

      }
    }
  }

  StatusCode CalcThqPartonHistory::execute() {
    //Get the Truth Particles
    const xAOD::TruthParticleContainer* truthParticles(nullptr);

    ATH_CHECK(evtStore()->retrieve(truthParticles, m_config->sgKeyMCParticle()));

    // Create the partonHistory xAOD object
    //cppcheck-suppress uninitvar
    xAOD::PartonHistoryAuxContainer* partonAuxCont = new xAOD::PartonHistoryAuxContainer {};
    //cppcheck-suppress uninitvar
    xAOD::PartonHistoryContainer* partonCont = new xAOD::PartonHistoryContainer {};
    partonCont->setStore(partonAuxCont);
    //cppcheck-suppress uninitvar
    xAOD::PartonHistory* ThqPartonHistory = new xAOD::PartonHistory {};
    partonCont->push_back(ThqPartonHistory);

    // Recover the parton history for TH events
    THHistorySaver(truthParticles, ThqPartonHistory);

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
