/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

#include "TopPartons/CalcTzqPartonHistory.h"
#include "TopPartons/CalcTopPartonHistory.h"
#include "TopConfiguration/TopConfig.h"
#include <iostream>


namespace top {
  CalcTzqPartonHistory::CalcTzqPartonHistory(const std::string& name) : CalcTopPartonHistory(name) {}
  const xAOD::TruthParticle* CalcTzqPartonHistory::findAfterGamma(const xAOD::TruthParticle* particle) {
    bool isAfterGamma(false);
    const int particle_ID = particle->pdgId();
    const int status = particle->status();
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

  bool CalcTzqPartonHistory::Zee(const xAOD::TruthParticleContainer* truthParticles, int start) {
    bool has_Z = false;
    bool has_tau1_neutrino = false;
    bool has_tau2_neutrino = false;
    bool hadr_tau1 = false;
    bool hadr_tau2 = false;
    Int_t nZChildren = 0;

    for (const xAOD::TruthParticle* particle : *truthParticles) {
      if (particle->pdgId() != start || particle->nChildren() != 2) {
        continue;
      }
      tZ.Z_p4 = particle->p4();
      has_Z = true;
      for (size_t k = 0; k < particle->nChildren(); k++) {
		
        const xAOD::TruthParticle* ZChildren = particle->child(k);
        nZChildren++;
        if (fabs(ZChildren->pdgId()) == 11) {// demanding electrons as childen
          const xAOD::TruthParticle* electron = CalcTzqPartonHistory::findAfterGamma(ZChildren);
          if (k == 0) {
            tZ.El1_from_Z_p4 = electron->p4();
            tZ.El1_from_Z_pdgId = electron->pdgId();
            tZ.El1_from_Z_status = electron->status();
            
                        
          } else {
            tZ.El2_from_Z_p4 = electron->p4();
            tZ.El2_from_Z_pdgId = electron->pdgId();
            tZ.El2_from_Z_status = electron->status();
            
          }
        } else if (fabs(ZChildren->pdgId()) == 13) {// demanding muons as childen
          const xAOD::TruthParticle* muon = CalcTzqPartonHistory::findAfterGamma(ZChildren);
          if (k == 0) {
            tZ.mu1_from_Z_p4 = muon->p4();
            tZ.mu1_from_Z_pdgId = muon->pdgId();
          } else {
            tZ.mu2_from_Z_p4 = muon->p4();
            tZ.mu2_from_Z_pdgId = muon->pdgId();
          }
        } else if (fabs(ZChildren->pdgId()) == 15) {// demanding tau-leptons as childen
          const xAOD::TruthParticle* tau = CalcTzqPartonHistory::findAfterGamma(ZChildren);
          if (k == 0) {
            tZ.Tau1_from_Z_p4 = tau->p4();
            tZ.Tau1_from_Z_pdgId = tau->pdgId();
          } else {
            tZ.Tau2_from_Z_p4 = tau->p4();
            tZ.Tau2_from_Z_pdgId = tau->pdgId();
          }
          for (size_t q = 0; q < tau->nChildren(); ++q) {
            const xAOD::TruthParticle* tauChildren = tau->child(q);
            if (fabs(tauChildren->pdgId()) == 16) {// tau neutrino
              if (k == 0) {
                tZ.nu_from_Tau1_p4 = tauChildren->p4();
                tZ.nu_from_Tau1_pdgId = tauChildren->pdgId();
                has_tau1_neutrino = true;
              } else {
                tZ.nu_from_Tau2_p4 = tauChildren->p4();
                tZ.nu_from_Tau2_pdgId = tauChildren->pdgId();
                has_tau2_neutrino = true;
              }
            } else if (fabs(tauChildren->pdgId()) >= 11 && fabs(tauChildren->pdgId()) <= 14) { //light leptons
              if (fabs(tauChildren->pdgId()) == 11 || fabs(tauChildren->pdgId()) == 13) { //electron or muon
                if (k == 0) {
                  tZ.W_decay1_from_Tau1_p4 = tauChildren->p4();
                  tZ.W_decay1_from_Tau1_pdgId = tauChildren->pdgId();
                } else {
                  tZ.W_decay1_from_Tau2_p4 = tauChildren->p4();
                  tZ.W_decay1_from_Tau2_pdgId = tauChildren->pdgId();
                }
              } else if (fabs(tauChildren->pdgId()) == 12 || fabs(tauChildren->pdgId()) == 14) { // electron or muon
                                                                                                 // neutrino
                if (k == 0) {
                  tZ.W_decay2_from_Tau1_p4 = tauChildren->p4();
                  tZ.W_decay2_from_Tau1_pdgId = tauChildren->pdgId();
                } else {
                  tZ.W_decay2_from_Tau2_p4 = tauChildren->p4();
                  tZ.W_decay2_from_Tau2_pdgId = tauChildren->pdgId();
                }
              }
            } else { // if a particle passes the criteria above, it has to be a hadron.
              if (k == 0) {
                hadr_tau1 = true;
              } else {
                hadr_tau2 = true;
              }
            }// else
          } // for
        }//else if
      } //for
    }
    std::cout << "Z has exactly ..." << nZChildren << std::endl;
    if (has_Z && has_tau1_neutrino && has_tau2_neutrino) {
      if (hadr_tau1) { //convention: store hadr. decaying W-Boson as Wdecay1, set all parameters of Wdecay2 to 0.
        tZ.W_decay1_from_Tau1_p4 = tZ.Tau1_from_Z_p4 - tZ.nu_from_Tau1_p4;
        tZ.W_decay1_from_Tau1_pdgId = -24 * sign(tZ.nu_from_Tau1_pdgId);
        tZ.W_decay2_from_Tau1_p4 = {
          0, 0, 0, 0
        };
        tZ.W_decay2_from_Tau1_pdgId = 0;
        tZ.TauJets1 = 1;
      } else if (hadr_tau2) {
        tZ.W_decay1_from_Tau2_p4 = tZ.Tau2_from_Z_p4 - tZ.nu_from_Tau2_p4;
        tZ.W_decay1_from_Tau2_pdgId = -24 * sign(tZ.nu_from_Tau2_pdgId);
        tZ.W_decay2_from_Tau2_p4 = {
          0, 0, 0, 0
        };
        tZ.W_decay2_from_Tau2_pdgId = 0;
        tZ.TauJets2 = 1;
      }
      return true;
    }
    tZ.TauJets1 = 0;
    tZ.TauJets2 = 0;
    return false;
  }

  void CalcTzqPartonHistory::TZHistorySaver(const xAOD::TruthParticleContainer* truthParticles,
                                            xAOD::PartonHistory* TzqPartonHistory) {
    //std::cout << "Parton code running..." << std::endl;
    TzqPartonHistory->IniVarTzqee();
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
    bool event_topbar = CalcTopPartonHistory::topWb(truthParticles, -6, t_before, t_after, Wp, b, WpDecay1,
                                                    WpDecay1_pdgId, WpDecay2, WpDecay2_pdgId);
    bool event_topbar_SC = CalcTopPartonHistory::topAfterFSR_SC(truthParticles, -6, t_after_SC);
    bool event_Z = CalcTzqPartonHistory::Zee(truthParticles, 23);
    bool event_bottom = CalcTzqPartonHistory::bottom(truthParticles, 5);
    bool event_bottombar = CalcTzqPartonHistory::bottom(truthParticles, -5);


    if (event_Z) {
      if ((event_top && event_bottombar) || (event_topbar && event_bottom)) {
        TzqPartonHistory->auxdecor< float >("MC_t_beforeFSR_m") = t_before.M();
        TzqPartonHistory->auxdecor< float >("MC_t_beforeFSR_pt") = t_before.Pt();
        TzqPartonHistory->auxdecor< float >("MC_t_beforeFSR_phi") = t_before.Phi();
        fillEtaBranch(TzqPartonHistory, "MC_t_beforeFSR_eta", t_before);

        TzqPartonHistory->auxdecor< float >("MC_t_afterFSR_m") = t_after.M();
        TzqPartonHistory->auxdecor< float >("MC_t_afterFSR_pt") = t_after.Pt();
        TzqPartonHistory->auxdecor< float >("MC_t_afterFSR_phi") = t_after.Phi();
        fillEtaBranch(TzqPartonHistory, "MC_t_afterFSR_eta", t_after);

        if (event_top_SC || event_topbar_SC) {
          TzqPartonHistory->auxdecor< float >("MC_t_afterFSR_SC_m") = t_after_SC.M();
          TzqPartonHistory->auxdecor< float >("MC_t_afterFSR_SC_pt") = t_after_SC.Pt();
          TzqPartonHistory->auxdecor< float >("MC_t_afterFSR_SC_phi") = t_after_SC.Phi();
          fillEtaBranch(TzqPartonHistory, "MC_t_afterFSR_SC_eta", t_after_SC);
        }

        TzqPartonHistory->auxdecor< float >("MC_W_from_t_m") = Wp.M();
        TzqPartonHistory->auxdecor< float >("MC_W_from_t_pt") = Wp.Pt();
        TzqPartonHistory->auxdecor< float >("MC_W_from_t_phi") = Wp.Phi();
        fillEtaBranch(TzqPartonHistory, "MC_W_from_t_eta", Wp);

        TzqPartonHistory->auxdecor< float >("MC_b_from_t_m") = b.M();
        TzqPartonHistory->auxdecor< float >("MC_b_from_t_pt") = b.Pt();
        TzqPartonHistory->auxdecor< float >("MC_b_from_t_phi") = b.Phi();
        fillEtaBranch(TzqPartonHistory, "MC_b_from_t_eta", b);

        TzqPartonHistory->auxdecor< float >("MC_Wdecay1_from_t_m") = WpDecay1.M();
        TzqPartonHistory->auxdecor< float >("MC_Wdecay1_from_t_pt") = WpDecay1.Pt();
        TzqPartonHistory->auxdecor< float >("MC_Wdecay1_from_t_phi") = WpDecay1.Phi();
        TzqPartonHistory->auxdecor< int >("MC_Wdecay1_from_t_pdgId") = WpDecay1_pdgId;
        fillEtaBranch(TzqPartonHistory, "MC_Wdecay1_from_t_eta", WpDecay1);

        TzqPartonHistory->auxdecor< float >("MC_Wdecay2_from_t_m") = WpDecay2.M();
        TzqPartonHistory->auxdecor< float >("MC_Wdecay2_from_t_pt") = WpDecay2.Pt();
        TzqPartonHistory->auxdecor< float >("MC_Wdecay2_from_t_phi") = WpDecay2.Phi();
        TzqPartonHistory->auxdecor< int >("MC_Wdecay2_from_t_pdgId") = WpDecay2_pdgId;
        fillEtaBranch(TzqPartonHistory, "MC_Wdecay2_from_t_eta", WpDecay2);

        //Z-Variables
        TzqPartonHistory->auxdecor< float >("MC_Z_m") = tZ.Z_p4.M();
        TzqPartonHistory->auxdecor< float >("MC_Z_pt") = tZ.Z_p4.Pt();
        TzqPartonHistory->auxdecor< float >("MC_Z_phi") = tZ.Z_p4.Phi();
        fillEtaBranch(TzqPartonHistory, "MC_Z_eta", tZ.Z_p4);

        //First Tau (Tau1)
        TzqPartonHistory->auxdecor< float >("MC_Tau1_from_Z_m") = tZ.Tau1_from_Z_p4.M();
        TzqPartonHistory->auxdecor< float >("MC_Tau1_from_Z_pt") = tZ.Tau1_from_Z_p4.Pt();
        TzqPartonHistory->auxdecor< float >("MC_Tau1_from_Z_phi") = tZ.Tau1_from_Z_p4.Phi();
        TzqPartonHistory->auxdecor< int >("MC_Tau1_from_Z_pdgId") = tZ.Tau1_from_Z_pdgId;
        fillEtaBranch(TzqPartonHistory, "MC_Tau1_from_Z_eta", tZ.Tau1_from_Z_p4);

        //Second Tau (Tau2)
        TzqPartonHistory->auxdecor< float >("MC_Tau2_from_Z_m") = tZ.Tau2_from_Z_p4.M();
        TzqPartonHistory->auxdecor< float >("MC_Tau2_from_Z_pt") = tZ.Tau2_from_Z_p4.Pt();
        TzqPartonHistory->auxdecor< float >("MC_Tau2_from_Z_phi") = tZ.Tau2_from_Z_p4.Phi();
        TzqPartonHistory->auxdecor< int >("MC_Tau2_from_Z_pdgId") = tZ.Tau2_from_Z_pdgId;
        fillEtaBranch(TzqPartonHistory, "MC_Tau2_from_Z_eta", tZ.Tau2_from_Z_p4);
        
        //First Electron (El1)
        TzqPartonHistory->auxdecor< float >("MC_El1_from_Z_m") = tZ.El1_from_Z_p4.M();
        TzqPartonHistory->auxdecor< float >("MC_El1_from_Z_pt") = tZ.El1_from_Z_p4.Pt();
        TzqPartonHistory->auxdecor< float >("MC_El1_from_Z_phi") = tZ.El1_from_Z_p4.Phi();
        TzqPartonHistory->auxdecor< int >("MC_El1_from_Z_pdgId") = tZ.El1_from_Z_pdgId;
        TzqPartonHistory->auxdecor< int >("MC_El1_from_Z_status") = tZ.El1_from_Z_status;
        fillEtaBranch(TzqPartonHistory, "MC_El1_from_Z_eta", tZ.El1_from_Z_p4);
        

        //Second Electron (El2)
        TzqPartonHistory->auxdecor< float >("MC_El2_from_Z_m") = tZ.El2_from_Z_p4.M();
        TzqPartonHistory->auxdecor< float >("MC_El2_from_Z_pt") = tZ.El2_from_Z_p4.Pt();
        TzqPartonHistory->auxdecor< float >("MC_El2_from_Z_phi") = tZ.El2_from_Z_p4.Phi();
        TzqPartonHistory->auxdecor< int >("MC_El2_from_Z_pdgId") = tZ.El2_from_Z_pdgId;
        fillEtaBranch(TzqPartonHistory, "MC_El2_from_Z_eta", tZ.El2_from_Z_p4);
        TzqPartonHistory->auxdecor< int >("MC_El2_from_Z_status") = tZ.El2_from_Z_status;
        
        //First muon (mu1)
        TzqPartonHistory->auxdecor< float >("MC_mu1_from_Z_m") = tZ.mu1_from_Z_p4.M();
        TzqPartonHistory->auxdecor< float >("MC_mu1_from_Z_pt") = tZ.mu1_from_Z_p4.Pt();
        TzqPartonHistory->auxdecor< float >("MC_mu1_from_Z_phi") = tZ.mu1_from_Z_p4.Phi();
        TzqPartonHistory->auxdecor< int >("MC_mu1_from_Z_pdgId") = tZ.mu1_from_Z_pdgId;
        fillEtaBranch(TzqPartonHistory, "MC_mu1_from_Z_eta", tZ.mu1_from_Z_p4);

        //Second muon (mu2)
        TzqPartonHistory->auxdecor< float >("MC_mu2_from_Z_m") = tZ.mu2_from_Z_p4.M();
        TzqPartonHistory->auxdecor< float >("MC_mu2_from_Z_pt") = tZ.mu2_from_Z_p4.Pt();
        TzqPartonHistory->auxdecor< float >("MC_mu2_from_Z_phi") = tZ.mu2_from_Z_p4.Phi();
        TzqPartonHistory->auxdecor< int >("MC_mu2_from_Z_pdgId") = tZ.mu2_from_Z_pdgId;
        fillEtaBranch(TzqPartonHistory, "MC_mu2_from_Z_eta", tZ.mu2_from_Z_p4);

        //neutrino from first Tau (Tau1)
        TzqPartonHistory->auxdecor< float >("MC_nu_from_Tau1_m") = tZ.nu_from_Tau1_p4.M();
        TzqPartonHistory->auxdecor< float >("MC_nu_from_Tau1_pt") = tZ.nu_from_Tau1_p4.Pt();
        TzqPartonHistory->auxdecor< float >("MC_nu_from_Tau1_phi") = tZ.nu_from_Tau1_p4.Phi();
        TzqPartonHistory->auxdecor< int >("MC_nu_from_Tau1_pdgId") = tZ.nu_from_Tau1_pdgId;
        fillEtaBranch(TzqPartonHistory, "MC_nu_from_Tau1_eta", tZ.nu_from_Tau1_p4);

        //neutrino from second Tau (Tau2)
        TzqPartonHistory->auxdecor< float >("MC_nu_from_Tau2_m") = tZ.nu_from_Tau2_p4.M();
        TzqPartonHistory->auxdecor< float >("MC_nu_from_Tau2_pt") = tZ.nu_from_Tau2_p4.Pt();
        TzqPartonHistory->auxdecor< float >("MC_nu_from_Tau2_phi") = tZ.nu_from_Tau2_p4.Phi();
        TzqPartonHistory->auxdecor< int >("MC_nu_from_Tau2_pdgId") = tZ.nu_from_Tau2_pdgId;
        fillEtaBranch(TzqPartonHistory, "MC_nu_from_Tau2_eta", tZ.nu_from_Tau2_p4);

        //First hadronic decay product from the W-Boson from Tau1
        TzqPartonHistory->auxdecor< float >("MC_Wdecay1_from_Tau1_m") = tZ.W_decay1_from_Tau1_p4.M();
        TzqPartonHistory->auxdecor< float >("MC_Wdecay1_from_Tau1_pt") = tZ.W_decay1_from_Tau1_p4.Pt();
        TzqPartonHistory->auxdecor< float >("MC_Wdecay1_from_Tau1_phi") = tZ.W_decay1_from_Tau1_p4.Phi();
        TzqPartonHistory->auxdecor< int >("MC_Wdecay1_from_Tau1_pdgId") = tZ.W_decay1_from_Tau1_pdgId;
        fillEtaBranch(TzqPartonHistory, "MC_Wdecay1_from_Tau1_eta", tZ.W_decay1_from_Tau1_p4);

        //Second hadronic decay product from the W-Boson from Tau1
        TzqPartonHistory->auxdecor< float >("MC_Wdecay2_from_Tau1_m") = tZ.W_decay2_from_Tau1_p4.M();
        TzqPartonHistory->auxdecor< float >("MC_Wdecay2_from_Tau1_pt") = tZ.W_decay2_from_Tau1_p4.Pt();
        TzqPartonHistory->auxdecor< float >("MC_Wdecay2_from_Tau1_phi") = tZ.W_decay2_from_Tau1_p4.Phi();
        TzqPartonHistory->auxdecor< int >("MC_Wdecay2_from_Tau1_pdgId") = tZ.W_decay2_from_Tau1_pdgId;
        fillEtaBranch(TzqPartonHistory, "MC_Wdecay2_from_Tau1_eta", tZ.W_decay2_from_Tau1_p4);

        //First leptonic decay product from the W-Boson from Tau2
        TzqPartonHistory->auxdecor< float >("MC_Wdecay1_from_Tau2_m") = tZ.W_decay1_from_Tau2_p4.M();
        TzqPartonHistory->auxdecor< float >("MC_Wdecay1_from_Tau2_pt") = tZ.W_decay1_from_Tau2_p4.Pt();
        TzqPartonHistory->auxdecor< float >("MC_Wdecay1_from_Tau2_phi") = tZ.W_decay1_from_Tau2_p4.Phi();
        TzqPartonHistory->auxdecor< int >("MC_Wdecay1_from_Tau2_pdgId") = tZ.W_decay1_from_Tau2_pdgId;
        fillEtaBranch(TzqPartonHistory, "MC_Wdecay1_from_Tau2_eta", tZ.W_decay1_from_Tau2_p4);

        //Second leptonic decay product from the W-Boson from Tau2
        TzqPartonHistory->auxdecor< float >("MC_Wdecay2_from_Tau2_m") = tZ.W_decay2_from_Tau2_p4.M();
        TzqPartonHistory->auxdecor< float >("MC_Wdecay2_from_Tau2_pt") = tZ.W_decay2_from_Tau2_p4.Pt();
        TzqPartonHistory->auxdecor< float >("MC_Wdecay2_from_Tau2_phi") = tZ.W_decay2_from_Tau2_p4.Phi();
        TzqPartonHistory->auxdecor< int >("MC_Wdecay2_from_Tau2_pdgId") = tZ.W_decay2_from_Tau2_pdgId;
        fillEtaBranch(TzqPartonHistory, "MC_Wdecay2_from_Tau2_eta", tZ.W_decay2_from_Tau2_p4);

        //b quark
        TzqPartonHistory->auxdecor< float >("MC_b_m") = tZ.b_p4.M();
        TzqPartonHistory->auxdecor< float >("MC_b_pt") = tZ.b_p4.Pt();
        TzqPartonHistory->auxdecor< float >("MC_b_phi") = tZ.b_p4.Phi();
        TzqPartonHistory->auxdecor< int >("MC_b_pdgId") = tZ.b_pdgId;
        fillEtaBranch(TzqPartonHistory, "MC_b_eta", tZ.b_p4);

        //Hadr. Variables
        TzqPartonHistory->auxdecor< int >("MC_hadr_Tau_Jet1") = tZ.TauJets1;
        TzqPartonHistory->auxdecor< int >("MC_hadr_Tau_Jet2") = tZ.TauJets2;
      }
    }
  }

  StatusCode CalcTzqPartonHistory::execute() {
    //Get the Truth Particles
    const xAOD::TruthParticleContainer* truthParticles(nullptr);

    ATH_CHECK(evtStore()->retrieve(truthParticles, m_config->sgKeyMCParticle()));

    // Create the partonHistory xAOD object
    xAOD::PartonHistoryAuxContainer* partonAuxCont = new xAOD::PartonHistoryAuxContainer {};
    xAOD::PartonHistoryContainer* partonCont = new xAOD::PartonHistoryContainer {};
    partonCont->setStore(partonAuxCont);

    xAOD::PartonHistory* TzqPartonHistory = new xAOD::PartonHistory {};
    partonCont->push_back(TzqPartonHistory);

    // Recover the parton history for TZ events
    TZHistorySaver(truthParticles, TzqPartonHistory);

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
