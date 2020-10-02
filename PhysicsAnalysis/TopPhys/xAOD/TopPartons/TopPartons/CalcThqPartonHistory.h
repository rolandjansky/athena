/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

#ifndef ANALYSISTOP_TOPPARTONS_CALCThqPARTONHISTORY_H
#define ANALYSISTOP_TOPPARTONS_CALCThqPARTONHISTORY_H


// Framework include(s):
#include "TopPartons/CalcTopPartonHistory.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "TopPartons/PartonHistory.h"

// forward declaration(s):
namespace top {
  class TopConfig;
}

namespace top {
  class CalcThqPartonHistory: public CalcTopPartonHistory {
  public:
    explicit CalcThqPartonHistory(const std::string& name);
    virtual ~CalcThqPartonHistory() {}

    struct tH_values {
      //Higgs
      TLorentzVector Higgs_p4;
      TLorentzVector decay1_p4;
      TLorentzVector decay2_p4;
      int decay1_pdgId;
      int decay2_pdgId;
      int tau_decay1_isHadronic;
      int tau_decay2_isHadronic;
      TLorentzVector decay1_from_decay1_p4;
      TLorentzVector decay2_from_decay1_p4;
      int decay1_from_decay1_pdgId;
      int decay2_from_decay1_pdgId;
      TLorentzVector decay1_from_decay2_p4;
      TLorentzVector decay2_from_decay2_p4;
      int decay1_from_decay2_pdgId;
      int decay2_from_decay2_pdgId;
      int tau_decay1_from_decay1_isHadronic;
      int tau_decay2_from_decay1_isHadronic;
      int tau_decay1_from_decay2_isHadronic;
      int tau_decay2_from_decay2_isHadronic;
    } tH;
    //Storing parton history for ttbar resonance analysis
    CalcThqPartonHistory(const CalcThqPartonHistory& rhs) = delete;
    CalcThqPartonHistory(CalcThqPartonHistory&& rhs) = delete;
    CalcThqPartonHistory& operator = (const CalcThqPartonHistory& rhs) = delete;

    void THHistorySaver(const xAOD::TruthParticleContainer* truthParticles, xAOD::PartonHistory* ThqPartonHistory);

    ///Store the four-momentum of several particles in the Higgs decay chain
    bool HiggsAndDecay(const xAOD::TruthParticleContainer* truthParticles);

    virtual StatusCode execute();
  };
}

#endif
