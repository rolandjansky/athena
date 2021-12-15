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
    tChannelSingleTopPartonHistory->IniVarTChannelSingleTop();

    //find top and its decay particles:
    TLorentzVector t_before, t_after;
    TLorentzVector antit_before, antit_after;
    TLorentzVector WfromTop;
    TLorentzVector bFromTop;
    TLorentzVector WfromTopDecay1;
    TLorentzVector WfromTopDecay2;
    int WfromTopDecay1_pdgId;
    int WfromTopDecay2_pdgId;

    //look for the top
    bool event_top = CalcTopPartonHistory::topWb(truthParticles, 6, t_before, t_after, WfromTop, bFromTop,
                                                 WfromTopDecay1, WfromTopDecay1_pdgId, WfromTopDecay2,
                                                 WfromTopDecay2_pdgId);

    //find anti-top and its decay particles:
    TLorentzVector WfromAntiTop;
    TLorentzVector bFromAntiTop;
    TLorentzVector WfromAntiTopDecay1;
    TLorentzVector WfromAntiTopDecay2;
    int WfromAntiTopDecay1_pdgId;
    int WfromAntiTopDecay2_pdgId;

    //look for the anti top
    bool event_antitop = CalcTopPartonHistory::topWb(truthParticles, -6, antit_before, antit_after, WfromAntiTop,
                                                     bFromAntiTop, WfromAntiTopDecay1, WfromAntiTopDecay1_pdgId,
                                                     WfromAntiTopDecay2, WfromAntiTopDecay2_pdgId);

    //find spectatorquark:
    TLorentzVector spectatorquark_before;
    TLorentzVector spectatorquark_after;

    int spectatorquark_pdgId;
    int spectatorquark_status;

    TLorentzVector spectatorquark_method2_before;
    TLorentzVector spectatorquark_method2_after;

    int spectatorquark_method2_pdgId;
    int spectatorquark_method2_status;

    bool event_sq =false;

    if (event_top && !event_antitop){
      event_sq = CalcTChannelSingleTopPartonHistory::spectatorquark(truthParticles, t_before, spectatorquark_before, spectatorquark_after, spectatorquark_pdgId, spectatorquark_status, spectatorquark_method2_before, spectatorquark_method2_after, spectatorquark_method2_pdgId, spectatorquark_method2_status);
    }
    else if (!event_top && event_antitop){
      event_sq = CalcTChannelSingleTopPartonHistory::spectatorquark(truthParticles, antit_before, spectatorquark_before, spectatorquark_after, spectatorquark_pdgId, spectatorquark_status, spectatorquark_method2_before, spectatorquark_method2_after, spectatorquark_method2_pdgId, spectatorquark_method2_status);
    }

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
    } else if ( !event_top && event_antitop) {
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

    if(((event_top && !event_antitop) || (!event_top && event_antitop)) && event_sq){
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
      
      tChannelSingleTopPartonHistory->auxdecor< float >("MC_spectatorquark_method2_beforeFSR_pt") = spectatorquark_method2_before.Pt();
      tChannelSingleTopPartonHistory->auxdecor< float >("MC_spectatorquark_method2_beforeFSR_eta") = spectatorquark_method2_before.Eta();
      tChannelSingleTopPartonHistory->auxdecor< float >("MC_spectatorquark_method2_beforeFSR_phi") = spectatorquark_method2_before.Phi();
      tChannelSingleTopPartonHistory->auxdecor< float >("MC_spectatorquark_method2_beforeFSR_m") = spectatorquark_method2_before.M();
      tChannelSingleTopPartonHistory->auxdecor< int >("MC_spectatorquark_method2_pdgId") = spectatorquark_method2_pdgId;
      tChannelSingleTopPartonHistory->auxdecor< int >("MC_spectatorquark_method2_status") = spectatorquark_method2_status;

      tChannelSingleTopPartonHistory->auxdecor< float >("MC_spectatorquark_method2_afterFSR_pt") = spectatorquark_method2_after.Pt();
      tChannelSingleTopPartonHistory->auxdecor< float >("MC_spectatorquark_method2_afterFSR_eta") = spectatorquark_method2_after.Eta();
      tChannelSingleTopPartonHistory->auxdecor< float >("MC_spectatorquark_method2_afterFSR_phi") = spectatorquark_method2_after.Phi();
      tChannelSingleTopPartonHistory->auxdecor< float >("MC_spectatorquark_method2_afterFSR_m") = spectatorquark_method2_after.M();
    }
  }


  bool CalcTChannelSingleTopPartonHistory::spectatorquark(const xAOD::TruthParticleContainer* truthParticles,
                                  TLorentzVector& top_quark,
                                  TLorentzVector& spectatorquark_beforeFSR, TLorentzVector& spectatorquark_afterFSR,
                                  int& spectatorquark_pdgId, int& spectatorquark_status,
                                  TLorentzVector& spectatorquark_method2_beforeFSR, TLorentzVector& spectatorquark_method2_afterFSR,
                                  int& spectatorquark_method2_pdgId, int& spectatorquark_method2_status) {
    bool hasSpectatorquark = false;
    bool hasSpectatorquark_method2 = false;

    //identify quark which does not originate from a decaying top quark -> W -> qq
    //should come from hard scattering process
    float min_pt =0;
    float pt_balance = 9999999;

    for (const xAOD::TruthParticle* particle : *truthParticles) { //loop over all truth partons
      if (particle == nullptr) continue;

      if (abs(particle->pdgId()) == 6){
        for (size_t iparent = 0; iparent < particle->nParents(); ++iparent) { // loop over parents of top
          if (particle->parent(iparent) == nullptr){
            continue;
          }
          if (std::abs(particle->parent(iparent)->pdgId()) == std::abs(particle->pdgId())) continue; 
            for (size_t ichildren = 0; ichildren < particle->parent(iparent)->nChildren(); ++ichildren) { 
              if (particle->parent(iparent)->child(ichildren) == nullptr) continue;
              if (abs(particle->parent(iparent)->child(ichildren)->pdgId()) >4) continue;

              auto partoncandidate = particle->parent(iparent)->child(ichildren);

              //Method1 select highest PT light quark if more than 1 light quark
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

              TLorentzVector sum_top_spectatorquark = top_quark + partoncandidate->p4(); //sum of top quark and the light quark in question 
              //Method2 check if this top quark system has a smaller PT than other system. 0 pt expected because of feynman diagram
              if (sum_top_spectatorquark.Pt() < pt_balance){ 
                pt_balance = sum_top_spectatorquark.Pt();
                spectatorquark_method2_beforeFSR = partoncandidate->p4();
                spectatorquark_method2_pdgId = partoncandidate->pdgId();
                spectatorquark_method2_status = partoncandidate->status();
                hasSpectatorquark_method2 = true;
              
                auto particle2 = PartonHistoryUtils::findAfterFSR(partoncandidate);
                spectatorquark_method2_afterFSR = particle2->p4();
              } //if
            }//children of top parents loop
        }//parents loop of top
      }//top if statement
    } // particle loop
    
    if (hasSpectatorquark || hasSpectatorquark_method2) return true;

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
