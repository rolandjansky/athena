/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */

#ifndef ANALYSISTOP_TOPPARTONS_CALCTtttPARTONHISTORY_H
#define ANALYSISTOP_TOPPARTONS_CALCTtttPARTONHISTORY_H


// Framework include(s):
#include "TopPartons/CalcTopPartonHistory.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "TopPartons/PartonHistory.h"

// forward declaration(s):
namespace top {
  class TopConfig;
}

namespace top {
  class CalcTtttPartonHistory: public CalcTopPartonHistory {
  public:
    explicit CalcTtttPartonHistory(const std::string& name);
    virtual ~CalcTtttPartonHistory() {}

    //Storing parton history for ttbar resonance analysis
    CalcTtttPartonHistory(const CalcTtttPartonHistory& rhs) = delete;
    CalcTtttPartonHistory(CalcTtttPartonHistory&& rhs) = delete;
    CalcTtttPartonHistory& operator = (const CalcTtttPartonHistory& rhs) = delete;

    void TtttHistorySaver(const xAOD::TruthParticleContainer* truthParticles, xAOD::PartonHistory* TtttPartonHistory);

    virtual StatusCode execute();

  private:
    
    std::array<int,4> top_pdgId;
    std::array<TLorentzVector,4> top_beforeFSR_p4;
    std::array<TLorentzVector,4> top_afterFSR_p4;
    std::array<TLorentzVector,4> b_p4;
    std::array<TLorentzVector,4> W_p4;
    std::array<int,4> Wdecay1_pdgId;
    std::array<int,4> Wdecay2_pdgId;
    std::array<TLorentzVector,4> Wdecay1_p4;
    std::array<TLorentzVector,4> Wdecay2_p4;

  };
}

#endif
