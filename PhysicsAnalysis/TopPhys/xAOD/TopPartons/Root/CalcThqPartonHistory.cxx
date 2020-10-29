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
    int WpDecay1_pdgId;
    int WpDecay2_pdgId;

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
        ThqPartonHistory->auxdecor< int >("MC_Higgs_tau_decay1_isHadronic") = tH.tau_decay1_isHadronic;
        fillEtaBranch(ThqPartonHistory, "MC_Higgs_decay1_eta", tH.decay1_p4);

        //Higgs-decay2-Variables
        ThqPartonHistory->auxdecor< float >("MC_Higgs_decay2_m") = tH.decay2_p4.M();
        ThqPartonHistory->auxdecor< float >("MC_Higgs_decay2_pt") = tH.decay2_p4.Pt();
        ThqPartonHistory->auxdecor< float >("MC_Higgs_decay2_phi") = tH.decay2_p4.Phi();
        ThqPartonHistory->auxdecor< int >("MC_Higgs_decay2_pdgId") = tH.decay2_pdgId;
        ThqPartonHistory->auxdecor< int >("MC_Higgs_tau_decay2_isHadronic") = tH.tau_decay1_isHadronic;
        fillEtaBranch(ThqPartonHistory, "MC_Higgs_decay2_eta", tH.decay2_p4);

        //Higgs-decay1- from decay1-Variables
        ThqPartonHistory->auxdecor< float >("MC_Higgs_decay1_from_decay1_m") = tH.decay1_from_decay1_p4.M();
        ThqPartonHistory->auxdecor< float >("MC_Higgs_decay1_from_decay1_pt") = tH.decay1_from_decay1_p4.Pt();
        ThqPartonHistory->auxdecor< float >("MC_Higgs_decay1_from_decay1_phi") = tH.decay1_from_decay1_p4.Phi();
        ThqPartonHistory->auxdecor< int >("MC_Higgs_decay1_from_decay1_pdgId") = tH.decay1_from_decay1_pdgId;
        ThqPartonHistory->auxdecor< int >("MC_Higgs_tau_decay1_from_decay1_isHadronic") = tH.tau_decay1_from_decay1_isHadronic;
        fillEtaBranch(ThqPartonHistory, "MC_Higgs_decay1_from_decay1_eta", tH.decay1_from_decay1_p4);

        //Higgs-decay2- from decay1-Variables
        ThqPartonHistory->auxdecor< float >("MC_Higgs_decay2_from_decay1_m") = tH.decay2_from_decay1_p4.M();
        ThqPartonHistory->auxdecor< float >("MC_Higgs_decay2_from_decay1_pt") = tH.decay2_from_decay1_p4.Pt();
        ThqPartonHistory->auxdecor< float >("MC_Higgs_decay2_from_decay1_phi") = tH.decay2_from_decay1_p4.Phi();
        ThqPartonHistory->auxdecor< int >("MC_Higgs_decay2_from_decay1_pdgId") = tH.decay2_from_decay1_pdgId;
        ThqPartonHistory->auxdecor< int >("MC_Higgs_tau_decay2_from_decay1_isHadronic") = tH.tau_decay2_from_decay1_isHadronic;
        fillEtaBranch(ThqPartonHistory, "MC_Higgs_decay2_from_decay1_eta", tH.decay2_from_decay1_p4);

        //Higgs-decay1- from decay2-Variables
        ThqPartonHistory->auxdecor< float >("MC_Higgs_decay1_from_decay2_m") = tH.decay1_from_decay2_p4.M();
        ThqPartonHistory->auxdecor< float >("MC_Higgs_decay1_from_decay2_pt") = tH.decay1_from_decay2_p4.Pt();
        ThqPartonHistory->auxdecor< float >("MC_Higgs_decay1_from_decay2_phi") = tH.decay1_from_decay2_p4.Phi();
        ThqPartonHistory->auxdecor< int >("MC_Higgs_decay1_from_decay2_pdgId") = tH.decay1_from_decay2_pdgId;
        ThqPartonHistory->auxdecor< int >("MC_Higgs_tau_decay1_from_decay2_isHadronic") = tH.tau_decay1_from_decay2_isHadronic;
        fillEtaBranch(ThqPartonHistory, "MC_Higgs_decay1_from_decay2_eta", tH.decay1_from_decay2_p4);

        //Higgs-decay2- from decay2-Variables
        ThqPartonHistory->auxdecor< float >("MC_Higgs_decay2_from_decay2_m") = tH.decay2_from_decay2_p4.M();
        ThqPartonHistory->auxdecor< float >("MC_Higgs_decay2_from_decay2_pt") = tH.decay2_from_decay2_p4.Pt();
        ThqPartonHistory->auxdecor< float >("MC_Higgs_decay2_from_decay2_phi") = tH.decay2_from_decay2_p4.Phi();
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
