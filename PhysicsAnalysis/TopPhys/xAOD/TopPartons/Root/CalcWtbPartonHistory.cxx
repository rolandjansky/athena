/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#include "TopPartons/CalcWtbPartonHistory.h"
#include "TopConfiguration/TopConfig.h"

namespace top {
  CalcWtbPartonHistory::CalcWtbPartonHistory(const std::string& name) : CalcTopPartonHistory(name) {}

  void CalcWtbPartonHistory::wtbHistorySaver(const xAOD::TruthParticleContainer* truthParticles,
                                             xAOD::PartonHistory* wtbPartonHistory) {
    wtbPartonHistory->IniVarWtb();

    TLorentzVector t_before, t_after;
    TLorentzVector antit_before, antit_after;
    TLorentzVector WfromTop;
    TLorentzVector bFromTop;
    TLorentzVector WfromTopDecay1;
    TLorentzVector WfromTopDecay2;
    int WfromTopDecay1_pdgId;
    int WfromTopDecay2_pdgId;

    bool event_top = CalcTopPartonHistory::topWb(truthParticles, 6, t_before, t_after, WfromTop, bFromTop,
                                                 WfromTopDecay1, WfromTopDecay1_pdgId, WfromTopDecay2,
                                                 WfromTopDecay2_pdgId);

    TLorentzVector WfromAntiTop;
    TLorentzVector bFromAntiTop;
    TLorentzVector WfromAntiTopDecay1;
    TLorentzVector WfromAntiTopDecay2;
    int WfromAntiTopDecay1_pdgId;
    int WfromAntiTopDecay2_pdgId;

    bool event_antitop = CalcTopPartonHistory::topWb(truthParticles, -6, antit_before, antit_after, WfromAntiTop,
                                                     bFromAntiTop, WfromAntiTopDecay1, WfromAntiTopDecay1_pdgId,
                                                     WfromAntiTopDecay2, WfromAntiTopDecay2_pdgId);

    TLorentzVector WnotFromTop;
    TLorentzVector WnotFromTopDecay1;
    TLorentzVector WnotFromTopDecay2;
    int WnotFromTop_pdgId;
    int WnotFromTopDecay1_pdgId;
    int WnotFromTopDecay2_pdgId;

    bool event_WnotTop = CalcTopPartonHistory::Wt_W(truthParticles, WnotFromTop, WnotFromTop_pdgId, WnotFromTopDecay1,
                                                    WnotFromTopDecay1_pdgId, WnotFromTopDecay2,
                                                    WnotFromTopDecay2_pdgId);

    TLorentzVector bNotFromTop_before;
    TLorentzVector bNotFromTop_after;
    int bNotFromTop_pdgId;

    bool event_b = CalcTopPartonHistory::Wt_b(truthParticles, bNotFromTop_before, bNotFromTop_after, bNotFromTop_pdgId);

    if (event_top && !event_antitop) {
      wtbPartonHistory->auxdecor< float >("MC_top_beforeFSR_pt") = t_before.Pt();
      wtbPartonHistory->auxdecor< float >("MC_top_beforeFSR_eta") = t_before.Eta();
      wtbPartonHistory->auxdecor< float >("MC_top_beforeFSR_phi") = t_before.Phi();
      wtbPartonHistory->auxdecor< float >("MC_top_beforeFSR_m") = t_before.M();

      wtbPartonHistory->auxdecor< float >("MC_top_afterFSR_pt") = t_after.Pt();
      wtbPartonHistory->auxdecor< float >("MC_top_afterFSR_eta") = t_after.Eta();
      wtbPartonHistory->auxdecor< float >("MC_top_afterFSR_phi") = t_after.Phi();
      wtbPartonHistory->auxdecor< float >("MC_top_afterFSR_m") = t_after.M();

      wtbPartonHistory->auxdecor< float >("MC_b_from_top_pt") = bFromTop.Pt();
      wtbPartonHistory->auxdecor< float >("MC_b_from_top_eta") = bFromTop.Eta();
      wtbPartonHistory->auxdecor< float >("MC_b_from_top_phi") = bFromTop.Phi();
      wtbPartonHistory->auxdecor< float >("MC_b_from_top_m") = bFromTop.M();

      wtbPartonHistory->auxdecor< float >("MC_W_from_top_pt") = WfromTop.Pt();
      wtbPartonHistory->auxdecor< float >("MC_W_from_top_eta") = WfromTop.Eta();
      wtbPartonHistory->auxdecor< float >("MC_W_from_top_phi") = WfromTop.Phi();
      wtbPartonHistory->auxdecor< float >("MC_W_from_top_m") = WfromTop.M();

      wtbPartonHistory->auxdecor< float >("MC_Wdecay1_from_top_pt") = WfromTopDecay1.Pt();
      wtbPartonHistory->auxdecor< float >("MC_Wdecay1_from_top_eta") = WfromTopDecay1.Eta();
      wtbPartonHistory->auxdecor< float >("MC_Wdecay1_from_top_phi") = WfromTopDecay1.Phi();
      wtbPartonHistory->auxdecor< float >("MC_Wdecay1_from_top_m") = WfromTopDecay1.M();
      wtbPartonHistory->auxdecor< int >("MC_Wdecay1_from_top_pdgId") = WfromTopDecay1_pdgId;

      wtbPartonHistory->auxdecor< float >("MC_Wdecay2_from_top_pt") = WfromTopDecay2.Pt();
      wtbPartonHistory->auxdecor< float >("MC_Wdecay2_from_top_eta") = WfromTopDecay2.Eta();
      wtbPartonHistory->auxdecor< float >("MC_Wdecay2_from_top_phi") = WfromTopDecay2.Phi();
      wtbPartonHistory->auxdecor< float >("MC_Wdecay2_from_top_m") = WfromTopDecay2.M();
      wtbPartonHistory->auxdecor< int >("MC_Wdecay2_from_top_pdgId") = WfromTopDecay2_pdgId;
    } else if (!event_top && event_antitop) {
      wtbPartonHistory->auxdecor< float >("MC_top_beforeFSR_pt") = antit_before.Pt();
      wtbPartonHistory->auxdecor< float >("MC_top_beforeFSR_eta") = antit_before.Eta();
      wtbPartonHistory->auxdecor< float >("MC_top_beforeFSR_phi") = antit_before.Phi();
      wtbPartonHistory->auxdecor< float >("MC_top_beforeFSR_m") = antit_before.M();

      wtbPartonHistory->auxdecor< float >("MC_top_afterFSR_pt") = antit_after.Pt();
      wtbPartonHistory->auxdecor< float >("MC_top_afterFSR_eta") = antit_after.Eta();
      wtbPartonHistory->auxdecor< float >("MC_top_afterFSR_phi") = antit_after.Phi();
      wtbPartonHistory->auxdecor< float >("MC_top_afterFSR_m") = antit_after.M();

      wtbPartonHistory->auxdecor< float >("MC_b_from_top_pt") = bFromAntiTop.Pt();
      wtbPartonHistory->auxdecor< float >("MC_b_from_top_eta") = bFromAntiTop.Eta();
      wtbPartonHistory->auxdecor< float >("MC_b_from_top_phi") = bFromAntiTop.Phi();
      wtbPartonHistory->auxdecor< float >("MC_b_from_top_m") = bFromAntiTop.M();

      wtbPartonHistory->auxdecor< float >("MC_W_from_top_pt") = WfromAntiTop.Pt();
      wtbPartonHistory->auxdecor< float >("MC_W_from_top_eta") = WfromAntiTop.Eta();
      wtbPartonHistory->auxdecor< float >("MC_W_from_top_phi") = WfromAntiTop.Phi();
      wtbPartonHistory->auxdecor< float >("MC_W_from_top_m") = WfromAntiTop.M();

      wtbPartonHistory->auxdecor< float >("MC_Wdecay1_from_top_pt") = WfromAntiTopDecay1.Pt();
      wtbPartonHistory->auxdecor< float >("MC_Wdecay1_from_top_eta") = WfromAntiTopDecay1.Eta();
      wtbPartonHistory->auxdecor< float >("MC_Wdecay1_from_top_phi") = WfromAntiTopDecay1.Phi();
      wtbPartonHistory->auxdecor< float >("MC_Wdecay1_from_top_m") = WfromAntiTopDecay1.M();
      wtbPartonHistory->auxdecor< int >("MC_Wdecay1_from_top_pdgId") = WfromAntiTopDecay1_pdgId;

      wtbPartonHistory->auxdecor< float >("MC_Wdecay2_from_top_pt") = WfromAntiTopDecay2.Pt();
      wtbPartonHistory->auxdecor< float >("MC_Wdecay2_from_top_eta") = WfromAntiTopDecay2.Eta();
      wtbPartonHistory->auxdecor< float >("MC_Wdecay2_from_top_phi") = WfromAntiTopDecay2.Phi();
      wtbPartonHistory->auxdecor< float >("MC_Wdecay2_from_top_m") = WfromAntiTopDecay2.M();
      wtbPartonHistory->auxdecor< int >("MC_Wdecay2_from_top_pdgId") = WfromAntiTopDecay2_pdgId;
    }

    if (((event_top && !event_antitop) || (!event_top && event_antitop)) && event_WnotTop) {
      wtbPartonHistory->auxdecor< float >("MC_W_not_from_top_pt") = WnotFromTop.Pt();
      wtbPartonHistory->auxdecor< float >("MC_W_not_from_top_eta") = WnotFromTop.Eta();
      wtbPartonHistory->auxdecor< float >("MC_W_not_from_top_phi") = WnotFromTop.Phi();
      wtbPartonHistory->auxdecor< float >("MC_W_not_from_top_m") = WnotFromTop.M();

      wtbPartonHistory->auxdecor< float >("MC_Wdecay1_not_from_top_pt") = WnotFromTopDecay1.Pt();
      wtbPartonHistory->auxdecor< float >("MC_Wdecay1_not_from_top_eta") = WnotFromTopDecay1.Eta();
      wtbPartonHistory->auxdecor< float >("MC_Wdecay1_not_from_top_phi") = WnotFromTopDecay1.Phi();
      wtbPartonHistory->auxdecor< float >("MC_Wdecay1_not_from_top_m") = WnotFromTopDecay1.M();
      wtbPartonHistory->auxdecor< int >("MC_Wdecay1_not_from_top_pdgId") = WnotFromTopDecay1_pdgId;

      wtbPartonHistory->auxdecor< float >("MC_Wdecay2_not_from_top_pt") = WnotFromTopDecay2.Pt();
      wtbPartonHistory->auxdecor< float >("MC_Wdecay2_not_from_top_eta") = WnotFromTopDecay2.Eta();
      wtbPartonHistory->auxdecor< float >("MC_Wdecay2_not_from_top_phi") = WnotFromTopDecay2.Phi();
      wtbPartonHistory->auxdecor< float >("MC_Wdecay2_not_from_top_m") = WnotFromTopDecay2.M();
      wtbPartonHistory->auxdecor< int >("MC_Wdecay2_not_from_top_pdgId") = WnotFromTopDecay2_pdgId;
    }//if

    if (((event_top && !event_antitop) || (!event_top && event_antitop)) && event_b) {
      wtbPartonHistory->auxdecor< float >("MC_b_not_from_top_beforeFSR_pt") = bNotFromTop_before.Pt();
      wtbPartonHistory->auxdecor< float >("MC_b_not_from_top_beforeFSR_eta") = bNotFromTop_before.Eta();
      wtbPartonHistory->auxdecor< float >("MC_b_not_from_top_beforeFSR_phi") = bNotFromTop_before.Phi();
      wtbPartonHistory->auxdecor< float >("MC_b_not_from_top_beforeFSR_m") = bNotFromTop_before.M();
      wtbPartonHistory->auxdecor< int >("MC_b_not_from_top_pdgId") = bNotFromTop_pdgId;

      wtbPartonHistory->auxdecor< float >("MC_b_not_from_top_afterFSR_pt") = bNotFromTop_after.Pt();
      wtbPartonHistory->auxdecor< float >("MC_b_not_from_top_afterFSR_eta") = bNotFromTop_after.Eta();
      wtbPartonHistory->auxdecor< float >("MC_b_not_from_top_afterFSR_phi") = bNotFromTop_after.Phi();
      wtbPartonHistory->auxdecor< float >("MC_b_not_from_top_afterFSR_m") = bNotFromTop_after.M();
    }
  }

  StatusCode CalcWtbPartonHistory::execute() {
    // Get the Truth Particles
    const xAOD::TruthParticleContainer* truthParticles(nullptr);

    ATH_CHECK(evtStore()->retrieve(truthParticles, m_config->sgKeyMCParticle()));

    // Create the partonHistory xAOD object
    xAOD::PartonHistoryAuxContainer* partonAuxCont = new xAOD::PartonHistoryAuxContainer {};
    xAOD::PartonHistoryContainer* partonCont = new xAOD::PartonHistoryContainer {};
    partonCont->setStore(partonAuxCont);

    xAOD::PartonHistory* wtbPartonHistory = new xAOD::PartonHistory {};
    partonCont->push_back(wtbPartonHistory);

    // Recover the parton history for Wt SingleTop events
    wtbHistorySaver(truthParticles, wtbPartonHistory);

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
