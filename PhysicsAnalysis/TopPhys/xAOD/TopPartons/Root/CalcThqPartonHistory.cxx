/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TopPartons/CalcThqPartonHistory.h"
#include "TopPartons/CalcTopPartonHistory.h"
#include "TopConfiguration/TopConfig.h"


namespace top {

  CalcThqPartonHistory::CalcThqPartonHistory(const std::string& name) : CalcTopPartonHistory(name) {}
  const xAOD::TruthParticle* CalcThqPartonHistory::findAfterGamma(const xAOD::TruthParticle* particle) {
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

      if (forLoop == 0)       isAfterGamma = true;
    }//while
    return particle;
  }
  int CalcThqPartonHistory::sign(int a) {
    if (a < 0) { return -1; }
    else return 1;
  }
  bool CalcThqPartonHistory::bottom(const xAOD::TruthParticleContainer* truthParticles, int start) {
    for (const xAOD::TruthParticle* particle : *truthParticles) {
      if (particle->pdgId() != start) { continue; }
      tH.b_p4 = particle->p4();
      tH.b_pdgId = particle->pdgId();
      return true;
    }
    return false;
  }
  bool CalcThqPartonHistory::Higgstautau(const xAOD::TruthParticleContainer* truthParticles, int start) {
    bool has_Higgs = false;
    bool has_tau1_neutrino = false;
    bool has_tau2_neutrino = false;
    bool hadr_tau1 = false;
    bool hadr_tau2 = false;
    for (const xAOD::TruthParticle* particle : *truthParticles) {
      if (particle->pdgId() != start || particle->nChildren() != 2) { continue; }
      tH.Higgs_p4 = particle->p4();
      has_Higgs = true;
      for (size_t k = 0; k < particle->nChildren(); k++) {
        const xAOD::TruthParticle* HiggsChildren = particle->child(k);
        if (fabs(HiggsChildren->pdgId()) == 15) {// demanding tau-leptons as childen
          const xAOD::TruthParticle* tau = CalcThqPartonHistory::findAfterGamma(HiggsChildren);
          if (k == 0) {
            tH.Tau1_from_Higgs_p4 = tau->p4();
            tH.Tau1_from_Higgs_pdgId = tau->pdgId();
          }
          else {
            tH.Tau2_from_Higgs_p4 = tau->p4();
            tH.Tau2_from_Higgs_pdgId = tau->pdgId();
          }
          for (size_t q = 0; q < tau->nChildren(); ++q) {
            const xAOD::TruthParticle* tauChildren = tau->child(q);
            if (fabs(tauChildren->pdgId()) == 16) {// tau neutrino
              if (k == 0) {
                tH.nu_from_Tau1_p4 = tauChildren->p4();
                tH.nu_from_Tau1_pdgId = tauChildren->pdgId();
                has_tau1_neutrino = true;
              }
              else {
                tH.nu_from_Tau2_p4 = tauChildren->p4();
                tH.nu_from_Tau2_pdgId = tauChildren->pdgId();
                has_tau2_neutrino = true;
              }
            }
            else if (fabs(tauChildren->pdgId()) >= 11 && fabs(tauChildren->pdgId()) <= 14) { //light leptons
              if (fabs(tauChildren->pdgId()) == 11 || fabs(tauChildren->pdgId()) == 13) { //electron or muon
                if (k == 0) {
                  tH.W_decay1_from_Tau1_p4 = tauChildren->p4();
                  tH.W_decay1_from_Tau1_pdgId = tauChildren->pdgId();
                }
                else {
                  tH.W_decay1_from_Tau2_p4 = tauChildren->p4();
                  tH.W_decay1_from_Tau2_pdgId = tauChildren->pdgId();
                }
              }
              else if (fabs(tauChildren->pdgId()) == 12 || fabs(tauChildren->pdgId()) == 14) { // electron or muon neutrino
                if (k == 0) {
                  tH.W_decay2_from_Tau1_p4 = tauChildren->p4();
                  tH.W_decay2_from_Tau1_pdgId = tauChildren->pdgId();
                }
                else {
                  tH.W_decay2_from_Tau2_p4 = tauChildren->p4();
                  tH.W_decay2_from_Tau2_pdgId = tauChildren->pdgId();
                }
              }
            }
            else { // if a particle passes the criteria above, it has to be a hadron.
              if (k == 0) { hadr_tau1 = true; }
              else { hadr_tau2 = true; }
            }// else
          } // for
        }//if 
      } //for
    }
    if (has_Higgs && has_tau1_neutrino && has_tau2_neutrino) {
      if (hadr_tau1) { //convention: store hadr. decaying W-Boson as Wdecay1, set all parameters of Wdecay2 to 0.
        tH.W_decay1_from_Tau1_p4 = tH.Tau1_from_Higgs_p4 - tH.nu_from_Tau1_p4;
        tH.W_decay1_from_Tau1_pdgId = -24 * sign(tH.nu_from_Tau1_pdgId);
        tH.W_decay2_from_Tau1_p4 = { 0, 0, 0, 0 };
        tH.W_decay2_from_Tau1_pdgId = 0;
        tH.TauJets1 = 1;
      }
      else if (hadr_tau2) {
        tH.W_decay1_from_Tau2_p4 = tH.Tau2_from_Higgs_p4 - tH.nu_from_Tau2_p4;
        tH.W_decay1_from_Tau2_pdgId = -24 * sign(tH.nu_from_Tau2_pdgId);
        tH.W_decay2_from_Tau2_p4 = { 0, 0, 0, 0 };
        tH.W_decay2_from_Tau2_pdgId = 0;
        tH.TauJets2 = 1;
      }
      return true;
    }
    tH.TauJets1 = 0;
    tH.TauJets2 = 0;
    return false;
  }
  void CalcThqPartonHistory::THHistorySaver(const xAOD::TruthParticleContainer* truthParticles, xAOD::PartonHistory* ThqPartonHistory) {

    ThqPartonHistory->IniVarThqtautau();
    TLorentzVector t_before, t_after, t_after_SC;
    TLorentzVector Wp;
    TLorentzVector b;
    TLorentzVector WpDecay1;
    TLorentzVector WpDecay2;
    int WpDecay1_pdgId;
    int WpDecay2_pdgId;

    bool event_top = CalcTopPartonHistory::topWb(truthParticles, 6, t_before, t_after, Wp, b, WpDecay1, WpDecay1_pdgId, WpDecay2, WpDecay2_pdgId);
    bool event_top_SC = CalcTopPartonHistory::topAfterFSR_SC(truthParticles, 6, t_after_SC);
    bool event_topbar = CalcTopPartonHistory::topWb(truthParticles, -6, t_before, t_after, Wp, b, WpDecay1, WpDecay1_pdgId, WpDecay2, WpDecay2_pdgId);
    bool event_topbar_SC = CalcTopPartonHistory::topAfterFSR_SC(truthParticles, -6, t_after_SC);
    bool event_Higgs = CalcThqPartonHistory::Higgstautau(truthParticles, 25);
    bool event_bottom = CalcThqPartonHistory::bottom(truthParticles, 5);
    bool event_bottombar = CalcThqPartonHistory::bottom(truthParticles, -5);


    if (event_Higgs ) {
      if ((event_top && event_bottombar) || (event_topbar && event_bottom))
      {
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

        //First Tau (Tau1)
        ThqPartonHistory->auxdecor< float >("MC_Tau1_from_Higgs_m") = tH.Tau1_from_Higgs_p4.M();
        ThqPartonHistory->auxdecor< float >("MC_Tau1_from_Higgs_pt") = tH.Tau1_from_Higgs_p4.Pt();
        ThqPartonHistory->auxdecor< float >("MC_Tau1_from_Higgs_phi") = tH.Tau1_from_Higgs_p4.Phi();
        ThqPartonHistory->auxdecor< int >("MC_Tau1_from_Higgs_pdgId") = tH.Tau1_from_Higgs_pdgId;
        fillEtaBranch(ThqPartonHistory, "MC_Tau1_from_Higgs_eta", tH.Tau1_from_Higgs_p4);

        //Second Tau (Tau2)
        ThqPartonHistory->auxdecor< float >("MC_Tau2_from_Higgs_m") = tH.Tau2_from_Higgs_p4.M();
        ThqPartonHistory->auxdecor< float >("MC_Tau2_from_Higgs_pt") = tH.Tau2_from_Higgs_p4.Pt();
        ThqPartonHistory->auxdecor< float >("MC_Tau2_from_Higgs_phi") = tH.Tau2_from_Higgs_p4.Phi();
        ThqPartonHistory->auxdecor< int >("MC_Tau2_from_Higgs_pdgId") = tH.Tau2_from_Higgs_pdgId;
        fillEtaBranch(ThqPartonHistory, "MC_Tau2_from_Higgs_eta", tH.Tau2_from_Higgs_p4);

        //neutrino from first Tau (Tau1)
        ThqPartonHistory->auxdecor< float >("MC_nu_from_Tau1_m") = tH.nu_from_Tau1_p4.M();
        ThqPartonHistory->auxdecor< float >("MC_nu_from_Tau1_pt") = tH.nu_from_Tau1_p4.Pt();
        ThqPartonHistory->auxdecor< float >("MC_nu_from_Tau1_phi") = tH.nu_from_Tau1_p4.Phi();
        ThqPartonHistory->auxdecor< int >("MC_nu_from_Tau1_pdgId") = tH.nu_from_Tau1_pdgId;
        fillEtaBranch(ThqPartonHistory, "MC_nu_from_Tau1_eta", tH.nu_from_Tau1_p4);

        //neutrino from second Tau (Tau2)
        ThqPartonHistory->auxdecor< float >("MC_nu_from_Tau2_m") = tH.nu_from_Tau2_p4.M();
        ThqPartonHistory->auxdecor< float >("MC_nu_from_Tau2_pt") = tH.nu_from_Tau2_p4.Pt();
        ThqPartonHistory->auxdecor< float >("MC_nu_from_Tau2_phi") = tH.nu_from_Tau2_p4.Phi();
        ThqPartonHistory->auxdecor< int >("MC_nu_from_Tau2_pdgId") = tH.nu_from_Tau2_pdgId;
        fillEtaBranch(ThqPartonHistory, "MC_nu_from_Tau2_eta", tH.nu_from_Tau2_p4);

        //First hadronic decay product from the W-Boson from Tau1
        ThqPartonHistory->auxdecor< float >("MC_Wdecay1_from_Tau1_m") = tH.W_decay1_from_Tau1_p4.M();
        ThqPartonHistory->auxdecor< float >("MC_Wdecay1_from_Tau1_pt") = tH.W_decay1_from_Tau1_p4.Pt();
        ThqPartonHistory->auxdecor< float >("MC_Wdecay1_from_Tau1_phi") = tH.W_decay1_from_Tau1_p4.Phi();
        ThqPartonHistory->auxdecor< int >("MC_Wdecay1_from_Tau1_pdgId") = tH.W_decay1_from_Tau1_pdgId;
        fillEtaBranch(ThqPartonHistory, "MC_Wdecay1_from_Tau1_eta", tH.W_decay1_from_Tau1_p4);

        //Second hadronic decay product from the W-Boson from Tau1
        ThqPartonHistory->auxdecor< float >("MC_Wdecay2_from_Tau1_m") = tH.W_decay2_from_Tau1_p4.M();
        ThqPartonHistory->auxdecor< float >("MC_Wdecay2_from_Tau1_pt") = tH.W_decay2_from_Tau1_p4.Pt();
        ThqPartonHistory->auxdecor< float >("MC_Wdecay2_from_Tau1_phi") = tH.W_decay2_from_Tau1_p4.Phi();
        ThqPartonHistory->auxdecor< int >("MC_Wdecay2_from_Tau1_pdgId") = tH.W_decay2_from_Tau1_pdgId;
        fillEtaBranch(ThqPartonHistory, "MC_Wdecay2_from_Tau1_eta", tH.W_decay2_from_Tau1_p4);

        //First leptonic decay product from the W-Boson from Tau2
        ThqPartonHistory->auxdecor< float >("MC_Wdecay1_from_Tau2_m") = tH.W_decay1_from_Tau2_p4.M();
        ThqPartonHistory->auxdecor< float >("MC_Wdecay1_from_Tau2_pt") = tH.W_decay1_from_Tau2_p4.Pt();
        ThqPartonHistory->auxdecor< float >("MC_Wdecay1_from_Tau2_phi") = tH.W_decay1_from_Tau2_p4.Phi();
        ThqPartonHistory->auxdecor< int >("MC_Wdecay1_from_Tau2_pdgId") = tH.W_decay1_from_Tau2_pdgId;
        fillEtaBranch(ThqPartonHistory, "MC_Wdecay1_from_Tau2_eta", tH.W_decay1_from_Tau2_p4);

        //Second leptonic decay product from the W-Boson from Tau2
        ThqPartonHistory->auxdecor< float >("MC_Wdecay2_from_Tau2_m") = tH.W_decay2_from_Tau2_p4.M();
        ThqPartonHistory->auxdecor< float >("MC_Wdecay2_from_Tau2_pt") = tH.W_decay2_from_Tau2_p4.Pt();
        ThqPartonHistory->auxdecor< float >("MC_Wdecay2_from_Tau2_phi") = tH.W_decay2_from_Tau2_p4.Phi();
        ThqPartonHistory->auxdecor< int >("MC_Wdecay2_from_Tau2_pdgId") = tH.W_decay2_from_Tau2_pdgId;
        fillEtaBranch(ThqPartonHistory, "MC_Wdecay2_from_Tau2_eta", tH.W_decay2_from_Tau2_p4);

        //b quark
        ThqPartonHistory->auxdecor< float >("MC_b_m") = tH.b_p4.M();
        ThqPartonHistory->auxdecor< float >("MC_b_pt") = tH.b_p4.Pt();
        ThqPartonHistory->auxdecor< float >("MC_b_phi") = tH.b_p4.Phi();
        ThqPartonHistory->auxdecor< int >("MC_b_pdgId") = tH.b_pdgId;
        fillEtaBranch(ThqPartonHistory, "MC_b_eta", tH.b_p4);

        //Hadr. Variables
        ThqPartonHistory->auxdecor< int >("MC_hadr_Tau_Jet1") = tH.TauJets1;
        ThqPartonHistory->auxdecor< int >("MC_hadr_Tau_Jet2") = tH.TauJets2;
      }
    }
  }
  StatusCode CalcThqPartonHistory::execute()
  {
    //Get the Truth Particles
    const xAOD::TruthParticleContainer* truthParticles(nullptr);
    ATH_CHECK(evtStore()->retrieve(truthParticles, m_config->sgKeyMCParticle()));

    // Create the partonHistory xAOD object
    xAOD::PartonHistoryAuxContainer* partonAuxCont = new xAOD::PartonHistoryAuxContainer{};
    xAOD::PartonHistoryContainer* partonCont = new xAOD::PartonHistoryContainer{};
    partonCont->setStore(partonAuxCont);

    xAOD::PartonHistory* ThqPartonHistory = new xAOD::PartonHistory{};
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
