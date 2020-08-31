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
      TLorentzVector Zdecay1_p4;
      int Zdecay1_pdgId;
      int Zdecay1_status;
      TLorentzVector Zdecay2_p4;
      int Zdecay2_pdgId;
      int Zdecay2_status;
      



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
    bool Zllqq(const xAOD::TruthParticleContainer* truthParticles, int start);

    //Store four-momentum of bottom quark
    bool bottom(const xAOD::TruthParticleContainer* truthParticles, int start);

    int sign(int a);

    virtual StatusCode execute();
  };
}

#endif
