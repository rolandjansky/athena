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
      TLorentzVector Z_p4;
      TLorentzVector El1_from_Z_p4;
      int El1_from_Z_pdgId;
      int El1_from_Z_status;
      char El1_from_Z_energyunit;
      TLorentzVector El2_from_Z_p4;
      int El2_from_Z_pdgId;
      int El2_from_Z_status;
      TLorentzVector mu1_from_Z_p4;
      int mu1_from_Z_pdgId;
      TLorentzVector mu2_from_Z_p4;
      int mu2_from_Z_pdgId;
      
      TLorentzVector Tau1_from_Z_p4;
      int Tau1_from_Z_pdgId;
      TLorentzVector Tau2_from_Z_p4;
      int Tau2_from_Z_pdgId;
      TLorentzVector nu_from_Tau1_p4;
      int nu_from_Tau1_pdgId;
      TLorentzVector nu_from_Tau2_p4;
      int nu_from_Tau2_pdgId;
      TLorentzVector W_decay1_from_Tau1_p4;
      int W_decay1_from_Tau1_pdgId;
      TLorentzVector W_decay2_from_Tau1_p4;
      int W_decay2_from_Tau1_pdgId;
      TLorentzVector W_decay1_from_Tau2_p4;
      int W_decay1_from_Tau2_pdgId;
      TLorentzVector W_decay2_from_Tau2_p4;
      int W_decay2_from_Tau2_pdgId;

      //Bools
      int TauJets1;
      int TauJets2;



      //b
      TLorentzVector b_p4;
      int b_pdgId;
    } tZ;
    //Storing parton history for ttbar resonance analysis
    CalcTzqPartonHistory(const CalcTzqPartonHistory& rhs) = delete;
    CalcTzqPartonHistory(CalcTzqPartonHistory&& rhs) = delete;
    CalcTzqPartonHistory& operator = (const CalcTzqPartonHistory& rhs) = delete;

    void TZHistorySaver(const xAOD::TruthParticleContainer* truthParticles, xAOD::PartonHistory* TzqPartonHistory);

    //handle gamma radiation of taus
    const xAOD::TruthParticle* findAfterGamma(const xAOD::TruthParticle* particle);

    ///Store the four-momentum of several particles in the Z decay chain
    bool Zee(const xAOD::TruthParticleContainer* truthParticles, int start);

    //Store four-momentum of bottom quark
    bool bottom(const xAOD::TruthParticleContainer* truthParticles, int start);

    int sign(int a);

    virtual StatusCode execute();
  };
}

#endif
