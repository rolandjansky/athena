/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

#include "TopPartons/CalcTzqPartonHistory.h"
#include "TopPartons/CalcTopPartonHistory.h"
#include "TopConfiguration/TopConfig.h"
#include "TopPartons/PartonHistoryUtils.h"


namespace top {
  using PartonHistoryUtils::decorateWithMPtPhi;

  CalcTzqPartonHistory::CalcTzqPartonHistory(const std::string& name) : CalcTopPartonHistory(name) {}
  const xAOD::TruthParticle* CalcTzqPartonHistory::findAfterGamma(const xAOD::TruthParticle* particle) {
    bool isAfterGamma(false);
    const int particle_ID = particle->pdgId();
    int forLoop;

    while (!isAfterGamma) {
      forLoop = 0;
      for (size_t j = 0; j < particle->nChildren(); j++) {
        const xAOD::TruthParticle* tmp_children = particle->child(j);
        if (tmp_children && tmp_children->pdgId() == particle_ID && tmp_children->pdgId() != 22) {
          particle = particle->child(j);
          forLoop++;
          break;
        }//if
      }//for

      if (forLoop == 0) isAfterGamma = true;
    }//while
    return particle;
  }

  int CalcTzqPartonHistory::sign(int a) {
    if (a < 0) {
      return -1;
    } else return 1;
  }

  bool CalcTzqPartonHistory::bottom(const xAOD::TruthParticleContainer* truthParticles, int start) {
    for (const xAOD::TruthParticle* particle : *truthParticles) {
      if (particle->pdgId() != start) {
        continue;
      }
      tZ.b_p4 = particle->p4();
      tZ.b_pdgId = particle->pdgId();
      return true;
    }
    return false;
  }

  bool CalcTzqPartonHistory::Zllqq(const xAOD::TruthParticleContainer* truthParticles, int start) {

    for (const xAOD::TruthParticle* particle : *truthParticles) {
      if (particle->pdgId() != start || particle->nChildren() != 2) {
        continue;
      }
      tZ.Z_p4 = particle->p4();
      
      for (size_t k = 0; k < particle->nChildren(); k++) {
		
        const xAOD::TruthParticle* ZChildren = particle->child(k);
        if (ZChildren->pdgId() > 0) {
           tZ.Zdecay1_p4 = findAfterGamma(ZChildren)->p4();
           tZ.Zdecay1_pdgId = ZChildren->pdgId();
           tZ.Zdecay1_status = ZChildren->status();
        } else {
		   tZ.Zdecay2_p4 = findAfterGamma(ZChildren)->p4();
           tZ.Zdecay2_pdgId = ZChildren->pdgId();
           tZ.Zdecay1_status = ZChildren->status();
        }
       } //for  
       // here we ask to return true if it identifies the particle and its children correctly.
      return true; 
    }
    return false;
  }

  void CalcTzqPartonHistory::TZHistorySaver(const xAOD::TruthParticleContainer* truthParticles,
                                            xAOD::PartonHistory* TzqPartonHistory) {
    TzqPartonHistory->IniVarTzq();
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
    bool event_topbar = CalcTopPartonHistory::topWb(truthParticles, -6, t_before, t_after, Wp, b, WpDecay1,
                                                    WpDecay1_pdgId, WpDecay2, WpDecay2_pdgId);
    bool event_topbar_SC = CalcTopPartonHistory::topAfterFSR_SC(truthParticles, -6, t_after_SC);
    bool event_Z = CalcTzqPartonHistory::Zllqq(truthParticles, 23);
    bool event_bottom = CalcTzqPartonHistory::bottom(truthParticles, 5);
    bool event_bottombar = CalcTzqPartonHistory::bottom(truthParticles, -5);


    if (event_Z) {
      if ((event_top && event_bottombar) || (event_topbar && event_bottom)) {
        decorateWithMPtPhi(TzqPartonHistory, "MC_t_beforeFSR", t_before);
        fillEtaBranch(TzqPartonHistory, "MC_t_beforeFSR_eta", t_before);

        decorateWithMPtPhi(TzqPartonHistory, "MC_t_afterFSR", t_after);
        fillEtaBranch(TzqPartonHistory, "MC_t_afterFSR_eta", t_after);

        if (event_top_SC || event_topbar_SC) {
          decorateWithMPtPhi(TzqPartonHistory, "MC_t_afterFSR_SC", t_after_SC);
          fillEtaBranch(TzqPartonHistory, "MC_t_afterFSR_SC_eta", t_after_SC);
        }
        decorateWithMPtPhi(TzqPartonHistory, "MC_W_from_t", Wp);
        fillEtaBranch(TzqPartonHistory, "MC_W_from_t_eta", Wp);

        decorateWithMPtPhi(TzqPartonHistory, "MC_b_from_t", b);
        fillEtaBranch(TzqPartonHistory, "MC_b_from_t_eta", b);

        decorateWithMPtPhi(TzqPartonHistory, "MC_Wdecay1_from_t", WpDecay1);
        TzqPartonHistory->auxdecor< int >("MC_Wdecay1_from_t_pdgId") = WpDecay1_pdgId;
        fillEtaBranch(TzqPartonHistory, "MC_Wdecay1_from_t_eta", WpDecay1);

        decorateWithMPtPhi(TzqPartonHistory, "MC_Wdecay2_from_t", WpDecay2);
        TzqPartonHistory->auxdecor< int >("MC_Wdecay2_from_t_pdgId") = WpDecay2_pdgId;
        fillEtaBranch(TzqPartonHistory, "MC_Wdecay2_from_t_eta", WpDecay2);

        //Z-Variables
        decorateWithMPtPhi(TzqPartonHistory, "MC_Z", tZ.Z_p4);
        fillEtaBranch(TzqPartonHistory, "MC_Z_eta", tZ.Z_p4);

        
        //First Decay (Zdecay1)
        decorateWithMPtPhi(TzqPartonHistory, "MC_Zdecay1", tZ.Zdecay1_p4);
        TzqPartonHistory->auxdecor< int >("MC_Zdecay1_pdgId") = tZ.Zdecay1_pdgId;
        TzqPartonHistory->auxdecor< int >("MC_Zdecay1_status") = tZ.Zdecay1_status;
        fillEtaBranch(TzqPartonHistory, "MC_Zdecay1_eta", tZ.Zdecay1_p4);
        
        //Second Decay (Zdecay2)
        decorateWithMPtPhi(TzqPartonHistory, "MC_Zdecay2", tZ.Zdecay2_p4);
        TzqPartonHistory->auxdecor< int >("MC_Zdecay2_pdgId") = tZ.Zdecay2_pdgId;
        TzqPartonHistory->auxdecor< int >("MC_Zdecay2_status") = tZ.Zdecay2_status;
        fillEtaBranch(TzqPartonHistory, "MC_Zdecay2_eta", tZ.Zdecay2_p4);
        
        

        //b quark
        decorateWithMPtPhi(TzqPartonHistory, "MC_b",  tZ.b_p4);
        TzqPartonHistory->auxdecor< int >("MC_b_pdgId") = tZ.b_pdgId;
        fillEtaBranch(TzqPartonHistory, "MC_b_eta", tZ.b_p4);

        
      }
    }
  }

  StatusCode CalcTzqPartonHistory::execute() {
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
    xAOD::PartonHistory* TzqPartonHistory = new xAOD::PartonHistory {};
    partonCont->push_back(TzqPartonHistory);

    // Recover the parton history for TZ events
    TZHistorySaver(truthParticles, TzqPartonHistory);

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
