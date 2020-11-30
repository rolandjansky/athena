/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#include "TopPartons/CalcTChannelSingleTopPartonHistory.h"
#include "TopConfiguration/TopConfig.h"
#include "TopPartons/PartonHistoryUtils.h"

namespace top {
  CalcTChannelSingleTopPartonHistory::CalcTChannelSingleTopPartonHistory(const std::string& name) : CalcTopPartonHistory(name) {}

  void CalcTChannelSingleTopPartonHistory::tChannelSingleTopHistorySaver(const xAOD::TruthParticleContainer* truthParticles,
                                             xAOD::PartonHistory* tChannelSingleTopPartonHistory) {
    tChannelSingleTopPartonHistory->IniVarWtb();


    //this part is exactly the same as in Wtb
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




    //change this part
    TLorentzVector spectatorquark_before;
    TLorentzVector spectatorquark_after;

    int spectatorquark_pdgId;
    int spectatorquark_status;

    bool event_sq = CalcTChannelSingleTopPartonHistory::spectatorquark(truthParticles, spectatorquark_before, spectatorquark_before, spectatorquark_pdgId, spectatorquark_status);

    if (event_top && !event_antitop) {
      tChannelSingleTopPartonHistory->auxdecor< float >("MC_top_beforeFSR_pt") = t_before.Pt();
      tChannelSingleTopPartonHistory->auxdecor< float >("MC_top_beforeFSR_eta") = t_before.Eta();
      tChannelSingleTopPartonHistory->auxdecor< float >("MC_top_beforeFSR_phi") = t_before.Phi();
      tChannelSingleTopPartonHistory->auxdecor< float >("MC_top_beforeFSR_m") = t_before.M();

      tChannelSingleTopPartonHistory->auxdecor< float >("MC_top_afterFSR_pt") = t_after.Pt();
      tChannelSingleTopPartonHistory->auxdecor< float >("MC_top_afterFSR_eta") = t_after.Eta();
      tChannelSingleTopPartonHistory->auxdecor< float >("MC_top_afterFSR_phi") = t_after.Phi();
      tChannelSingleTopPartonHistory->auxdecor< float >("MC_top_afterFSR_m") = t_after.M();

      tChannelSingleTopPartonHistory->auxdecor< float >("MC_b_from_top_pt") = bFromTop.Pt();
      tChannelSingleTopPartonHistory->auxdecor< float >("MC_b_from_top_eta") = bFromTop.Eta();
      tChannelSingleTopPartonHistory->auxdecor< float >("MC_b_from_top_phi") = bFromTop.Phi();
      tChannelSingleTopPartonHistory->auxdecor< float >("MC_b_from_top_m") = bFromTop.M();

      tChannelSingleTopPartonHistory->auxdecor< float >("MC_W_from_top_pt") = WfromTop.Pt();
      tChannelSingleTopPartonHistory->auxdecor< float >("MC_W_from_top_eta") = WfromTop.Eta();
      tChannelSingleTopPartonHistory->auxdecor< float >("MC_W_from_top_phi") = WfromTop.Phi();
      tChannelSingleTopPartonHistory->auxdecor< float >("MC_W_from_top_m") = WfromTop.M();

      tChannelSingleTopPartonHistory->auxdecor< float >("MC_Wdecay1_from_top_pt") = WfromTopDecay1.Pt();
      tChannelSingleTopPartonHistory->auxdecor< float >("MC_Wdecay1_from_top_eta") = WfromTopDecay1.Eta();
      tChannelSingleTopPartonHistory->auxdecor< float >("MC_Wdecay1_from_top_phi") = WfromTopDecay1.Phi();
      tChannelSingleTopPartonHistory->auxdecor< float >("MC_Wdecay1_from_top_m") = WfromTopDecay1.M();
      tChannelSingleTopPartonHistory->auxdecor< int >("MC_Wdecay1_from_top_pdgId") = WfromTopDecay1_pdgId;

      tChannelSingleTopPartonHistory->auxdecor< float >("MC_Wdecay2_from_top_pt") = WfromTopDecay2.Pt();
      tChannelSingleTopPartonHistory->auxdecor< float >("MC_Wdecay2_from_top_eta") = WfromTopDecay2.Eta();
      tChannelSingleTopPartonHistory->auxdecor< float >("MC_Wdecay2_from_top_phi") = WfromTopDecay2.Phi();
      tChannelSingleTopPartonHistory->auxdecor< float >("MC_Wdecay2_from_top_m") = WfromTopDecay2.M();
      tChannelSingleTopPartonHistory->auxdecor< int >("MC_Wdecay2_from_top_pdgId") = WfromTopDecay2_pdgId;
    } else if (!event_top && event_antitop) {
      tChannelSingleTopPartonHistory->auxdecor< float >("MC_top_beforeFSR_pt") = antit_before.Pt();
      tChannelSingleTopPartonHistory->auxdecor< float >("MC_top_beforeFSR_eta") = antit_before.Eta();
      tChannelSingleTopPartonHistory->auxdecor< float >("MC_top_beforeFSR_phi") = antit_before.Phi();
      tChannelSingleTopPartonHistory->auxdecor< float >("MC_top_beforeFSR_m") = antit_before.M();

      tChannelSingleTopPartonHistory->auxdecor< float >("MC_top_afterFSR_pt") = antit_after.Pt();
      tChannelSingleTopPartonHistory->auxdecor< float >("MC_top_afterFSR_eta") = antit_after.Eta();
      tChannelSingleTopPartonHistory->auxdecor< float >("MC_top_afterFSR_phi") = antit_after.Phi();
      tChannelSingleTopPartonHistory->auxdecor< float >("MC_top_afterFSR_m") = antit_after.M();

      tChannelSingleTopPartonHistory->auxdecor< float >("MC_b_from_top_pt") = bFromAntiTop.Pt();
      tChannelSingleTopPartonHistory->auxdecor< float >("MC_b_from_top_eta") = bFromAntiTop.Eta();
      tChannelSingleTopPartonHistory->auxdecor< float >("MC_b_from_top_phi") = bFromAntiTop.Phi();
      tChannelSingleTopPartonHistory->auxdecor< float >("MC_b_from_top_m") = bFromAntiTop.M();

      tChannelSingleTopPartonHistory->auxdecor< float >("MC_W_from_top_pt") = WfromAntiTop.Pt();
      tChannelSingleTopPartonHistory->auxdecor< float >("MC_W_from_top_eta") = WfromAntiTop.Eta();
      tChannelSingleTopPartonHistory->auxdecor< float >("MC_W_from_top_phi") = WfromAntiTop.Phi();
      tChannelSingleTopPartonHistory->auxdecor< float >("MC_W_from_top_m") = WfromAntiTop.M();

      tChannelSingleTopPartonHistory->auxdecor< float >("MC_Wdecay1_from_top_pt") = WfromAntiTopDecay1.Pt();
      tChannelSingleTopPartonHistory->auxdecor< float >("MC_Wdecay1_from_top_eta") = WfromAntiTopDecay1.Eta();
      tChannelSingleTopPartonHistory->auxdecor< float >("MC_Wdecay1_from_top_phi") = WfromAntiTopDecay1.Phi();
      tChannelSingleTopPartonHistory->auxdecor< float >("MC_Wdecay1_from_top_m") = WfromAntiTopDecay1.M();
      tChannelSingleTopPartonHistory->auxdecor< int >("MC_Wdecay1_from_top_pdgId") = WfromAntiTopDecay1_pdgId;

      tChannelSingleTopPartonHistory->auxdecor< float >("MC_Wdecay2_from_top_pt") = WfromAntiTopDecay2.Pt();
      tChannelSingleTopPartonHistory->auxdecor< float >("MC_Wdecay2_from_top_eta") = WfromAntiTopDecay2.Eta();
      tChannelSingleTopPartonHistory->auxdecor< float >("MC_Wdecay2_from_top_phi") = WfromAntiTopDecay2.Phi();
      tChannelSingleTopPartonHistory->auxdecor< float >("MC_Wdecay2_from_top_m") = WfromAntiTopDecay2.M();
      tChannelSingleTopPartonHistory->auxdecor< int >("MC_Wdecay2_from_top_pdgId") = WfromAntiTopDecay2_pdgId;
    }

    if(event_sq){
      tChannelSingleTopPartonHistory->auxdecor< float >("MC_spectatorquark_beforeFSR_pt") = spectatorquark_before.Pt();
      tChannelSingleTopPartonHistory->auxdecor< float >("MC_spectatorquark_beforeFSR_eta") = spectatorquark_before.Eta();
      tChannelSingleTopPartonHistory->auxdecor< float >("MC_spectatorquark_beforeFSR_phi") = spectatorquark_before.Phi();
      tChannelSingleTopPartonHistory->auxdecor< float >("MC_spectatorquark_beforeFSR_m") = spectatorquark_before.M();
      tChannelSingleTopPartonHistory->auxdecor< int >("MC_spectatorquark_pdgId") = spectatorquark_pdgId;
      tChannelSingleTopPartonHistory->auxdecor< int >("MC_spectatorquark_status") = spectatorquark_status;


      tChannelSingleTopPartonHistory->auxdecor< float >("MC_spectatorquark_afterFSR_pt") = spectatorquark_after.Pt();
      tChannelSingleTopPartonHistory->auxdecor< float >("MC_spectatorquark_afterFSR_eta") = spectatorquark_after.Eta();
      tChannelSingleTopPartonHistory->auxdecor< float >("MC_spectatorquark_afterFSR_phi") = spectatorquark_after.Phi();
      tChannelSingleTopPartonHistory->auxdecor< float >("MC_spectatorquark_afterFSR_m") = spectatorquark_after.M();

    }
    /*
    if (((event_top && !event_antitop) || (!event_top && event_antitop)) && event_WnotTop) {
      tChannelSingleTopPartonHistory->auxdecor< float >("MC_W_not_from_top_pt") = WnotFromTop.Pt();
      tChannelSingleTopPartonHistory->auxdecor< float >("MC_W_not_from_top_eta") = WnotFromTop.Eta();
      tChannelSingleTopPartonHistory->auxdecor< float >("MC_W_not_from_top_phi") = WnotFromTop.Phi();
      tChannelSingleTopPartonHistory->auxdecor< float >("MC_W_not_from_top_m") = WnotFromTop.M();

      tChannelSingleTopPartonHistory->auxdecor< float >("MC_Wdecay1_not_from_top_pt") = WnotFromTopDecay1.Pt();
      tChannelSingleTopPartonHistory->auxdecor< float >("MC_Wdecay1_not_from_top_eta") = WnotFromTopDecay1.Eta();
      tChannelSingleTopPartonHistory->auxdecor< float >("MC_Wdecay1_not_from_top_phi") = WnotFromTopDecay1.Phi();
      tChannelSingleTopPartonHistory->auxdecor< float >("MC_Wdecay1_not_from_top_m") = WnotFromTopDecay1.M();
      tChannelSingleTopPartonHistory->auxdecor< int >("MC_Wdecay1_not_from_top_pdgId") = WnotFromTopDecay1_pdgId;

      tChannelSingleTopPartonHistory->auxdecor< float >("MC_Wdecay2_not_from_top_pt") = WnotFromTopDecay2.Pt();
      tChannelSingleTopPartonHistory->auxdecor< float >("MC_Wdecay2_not_from_top_eta") = WnotFromTopDecay2.Eta();
      tChannelSingleTopPartonHistory->auxdecor< float >("MC_Wdecay2_not_from_top_phi") = WnotFromTopDecay2.Phi();
      tChannelSingleTopPartonHistory->auxdecor< float >("MC_Wdecay2_not_from_top_m") = WnotFromTopDecay2.M();
      tChannelSingleTopPartonHistory->auxdecor< int >("MC_Wdecay2_not_from_top_pdgId") = WnotFromTopDecay2_pdgId;
    }//if

    if (((event_top && !event_antitop) || (!event_top && event_antitop)) && event_b) {
      tChannelSingleTopPartonHistory->auxdecor< float >("MC_b_not_from_top_beforeFSR_pt") = bNotFromTop_before.Pt();
      tChannelSingleTopPartonHistory->auxdecor< float >("MC_b_not_from_top_beforeFSR_eta") = bNotFromTop_before.Eta();
      tChannelSingleTopPartonHistory->auxdecor< float >("MC_b_not_from_top_beforeFSR_phi") = bNotFromTop_before.Phi();
      tChannelSingleTopPartonHistory->auxdecor< float >("MC_b_not_from_top_beforeFSR_m") = bNotFromTop_before.M();
      tChannelSingleTopPartonHistory->auxdecor< int >("MC_b_not_from_top_pdgId") = bNotFromTop_pdgId;

      tChannelSingleTopPartonHistory->auxdecor< float >("MC_b_not_from_top_afterFSR_pt") = bNotFromTop_after.Pt();
      tChannelSingleTopPartonHistory->auxdecor< float >("MC_b_not_from_top_afterFSR_eta") = bNotFromTop_after.Eta();
      tChannelSingleTopPartonHistory->auxdecor< float >("MC_b_not_from_top_afterFSR_phi") = bNotFromTop_after.Phi();
      tChannelSingleTopPartonHistory->auxdecor< float >("MC_b_not_from_top_afterFSR_m") = bNotFromTop_after.M();
    } */
  }


  bool CalcTChannelSingleTopPartonHistory::spectatorquark(const xAOD::TruthParticleContainer* truthParticles,
                                  TLorentzVector& spectatorquark_beforeFSR, TLorentzVector& spectatorquark_afterFSR,
                                  int& spectatorquark_pdgId, int& spectatorquark_status) {
    bool hasSpectatorquark = false;

    // identify "other" b quark that is not from radiation but from ME (Wtb)
    // logic is simple: search for b quark that doesn't have top, proton, or
    // nullptr as parent

    ANA_MSG_INFO ("new Event");


    for (const xAOD::TruthParticle* particle : *truthParticles) {
      if (particle == nullptr) continue;
      if (abs(particle->pdgId()) > 4) continue;
      if(particle->status() != 23) continue;


      //ANA_MSG_INFO ("particle ID: \t" <<particle->pdgId() << "\t particle status: \t" <<particle->status() << "\t particle PT: \t" << particle->p4().Pt());
      for (size_t iparent = 0; iparent < particle->nParents(); ++iparent) {
        if (particle->parent(iparent) == nullptr) continue;


        // we dont want quarks that have same pdgID as parent, I dont know man
        //if (abs(particle->parent(iparent)->pdgId()) == abs(particle->pdgId()) continue;
        // we dont want quarks that come from top
        if (abs(particle->parent(iparent)->pdgId()) == 6) continue;

        // we dont want quarks that come from W
        if (abs(particle->parent(iparent)->pdgId()) == 24) continue;

        // we dont want quarks that come from proton
        if (abs(particle->parent(iparent)->pdgId()) == 2212) continue;


        //ANA_MSG_INFO ("parent ID: \t" << particle->parent(iparent)->pdgId() << "\t parent status: \t" <<particle->parent(iparent)->status());



        hasSpectatorquark = true;
        spectatorquark_beforeFSR = particle->p4();
        spectatorquark_pdgId = particle->pdgId();
        spectatorquark_status = particle->status();

        // find after FSR
        particle = PartonHistoryUtils::findAfterFSR(particle);
        spectatorquark_afterFSR = particle->p4();
      }
    }


    if (hasSpectatorquark) return true;

    return false;
  }

  StatusCode CalcTChannelSingleTopPartonHistory::execute() {
    // Get the Truth Particles
    const xAOD::TruthParticleContainer* truthParticles(nullptr);

    ATH_CHECK(evtStore()->retrieve(truthParticles, m_config->sgKeyMCParticle()));

    // Create the partonHistory xAOD object
    xAOD::PartonHistoryAuxContainer* partonAuxCont = new xAOD::PartonHistoryAuxContainer {};
    xAOD::PartonHistoryContainer* partonCont = new xAOD::PartonHistoryContainer {};
    partonCont->setStore(partonAuxCont);

    xAOD::PartonHistory* tChannelSingleTopPartonHistory = new xAOD::PartonHistory {};
    partonCont->push_back(tChannelSingleTopPartonHistory);

    // Recover the parton history for Wt SingleTop events
    tChannelSingleTopHistorySaver(truthParticles, tChannelSingleTopPartonHistory);

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
