/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

#ifndef ANALYSISTOP_TOPPARTONS_CALCTzqPARTONHISTORY_H
#define ANALYSISTOP_TOPPARTONS_CALCTzqPARTONHISTORY_H


// Framework include(s):
#include "TopPartons/CalcTopPartonHistory.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "TopPartons/PartonHistory.h"

// forward declaration(s):
namespace top {
  class TopConfig;
}

namespace top {
  class CalcTzqPartonHistory: public CalcTopPartonHistory {
  public:
    explicit CalcTzqPartonHistory(const std::string& name);
    virtual ~CalcTzqPartonHistory() {}

    struct tZ_values {
      //Z boson
      // All momenta for the chain Z -> tau tau -> [taudecay11 taudecay21] + [taudecay12 taudecay22]
      TLorentzVector Z_p4;
      TLorentzVector Zdecay1_p4;
      TLorentzVector Zdecay2_p4;
      int Zdecay1_pdgId;
      int Zdecay2_pdgId;
      int Zdecay1_status;
      int Zdecay2_status;
      // decay1/2_tau_isHadronic is 
      //    +24 for a hadronically decaying tau+ 
      //    -24 for a hadronically decaying tau- 
      //    the pdgID in case of leptons 
      int Zdecay1_tau_isHadronic;
      int Zdecay2_tau_isHadronic;
      TLorentzVector Zdecay1_tauvis_p4;
      TLorentzVector Zdecay2_tauvis_p4;    

      //b
      TLorentzVector b_p4;
      int b_pdgId;

      //q
      TLorentzVector q_p4;
      int q_pdgId;
      bool isThq;

      void reset(){
        Z_p4.SetPtEtaPhiE(0,0,0,0);
        Zdecay1_p4.SetPtEtaPhiE(0,0,0,0);
        Zdecay2_p4.SetPtEtaPhiE(0,0,0,0);
        b_p4.SetPtEtaPhiE(0,0,0,0);
        q_p4.SetPtEtaPhiE(0,0,0,0);
        Zdecay1_pdgId = -9999;
        Zdecay2_pdgId = -9999;
        Zdecay1_status = -9999;
        Zdecay2_status = -9999;
        Zdecay1_tau_isHadronic = -9999;
        Zdecay2_tau_isHadronic = -9999;
        Zdecay1_tauvis_p4.SetPtEtaPhiE(0,0,0,0);
        Zdecay2_tauvis_p4.SetPtEtaPhiE(0,0,0,0);

        b_pdgId = -9999;
        q_pdgId = -9999;
        isThq = false;
      }

    } tZ;
    //Storing parton history for ttbar resonance analysis
    CalcTzqPartonHistory(const CalcTzqPartonHistory& rhs) = delete;
    CalcTzqPartonHistory(CalcTzqPartonHistory&& rhs) = delete;
    CalcTzqPartonHistory& operator = (const CalcTzqPartonHistory& rhs) = delete;

    void TZHistorySaver(const xAOD::TruthParticleContainer* truthParticles, xAOD::PartonHistory* TzqPartonHistory);

    //handle gamma radiation of taus
    const xAOD::TruthParticle* findAfterGamma(const xAOD::TruthParticle* particle);

    const xAOD::TruthParticle* getFlavourSibling(const xAOD::TruthParticle* particle);
    bool FindTZQVertex(const xAOD::TruthParticleContainer* truthParticles);
    void FillZ(const xAOD::TruthParticle* particle);
    bool FindLostZ(const xAOD::TruthParticleContainer* truthParticles);
    bool FindLostTop(const xAOD::TruthParticleContainer *truthParticles,
                     TLorentzVector &t_beforeFSR_p4, TLorentzVector &t_afterFSR_p4,
                     TLorentzVector &W_p4, TLorentzVector &b_p4, TLorentzVector &Wdecay1_p4,
                     int &Wdecay1_pdgId, TLorentzVector &Wdecay2_p4, int &Wdecay2_pdgId);
    bool FindLostW(const xAOD::TruthParticleContainer *truthParticles,
                   TLorentzVector &Wdecay1_p4, int &Wdecay1_pdgId,
                   TLorentzVector &Wdecay2_p4, int &Wdecay2_pdgId);

    virtual StatusCode execute();
  };
}

#endif
