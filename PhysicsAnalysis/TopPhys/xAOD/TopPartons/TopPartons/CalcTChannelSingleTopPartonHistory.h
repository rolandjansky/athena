/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#ifndef ANALYSISTOP_TOPPARTONS_CALCTCHANNELSINGLETOPPARTONHISTORY_H
#define ANALYSISTOP_TOPPARTONS_CALCTCHANNELSINGLETOPPARTONHISTORY_H

/**
 * @author Jordy Degens <jdegens@nikhef.nl>
 *
 * @brief CalcTChannelSingleTopPartonHistory
 *   Class derived from CalcTopPartonHistory, used to store t-channel SingleTop variables
 *
 **/


// Framework include(s):
#include "TopPartons/CalcTopPartonHistory.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "TopPartons/PartonHistory.h"

// forward declaration(s):
namespace top {
  class TopConfig;
}

namespace top {
  class CalcTChannelSingleTopPartonHistory: public CalcTopPartonHistory {
  public:
    explicit CalcTChannelSingleTopPartonHistory(const std::string& name);

    //Storing parton history for Wtb SingleTop analysis
    CalcTChannelSingleTopPartonHistory(const CalcTChannelSingleTopPartonHistory& rhs) = delete;
    CalcTChannelSingleTopPartonHistory(CalcTChannelSingleTopPartonHistory&& rhs) = delete;
    CalcTChannelSingleTopPartonHistory& operator = (const CalcTChannelSingleTopPartonHistory& rhs) = delete;

    void tChannelSingleTopHistorySaver(const xAOD::TruthParticleContainer* truthParticles, xAOD::PartonHistory* tChannelSingleTopPartonHistory);

    bool spectatorquark(const xAOD::TruthParticleContainer* truthParticles, TLorentzVector& top_quark, TLorentzVector& spectatorquark_beforeFSR,
              TLorentzVector& spectatorquark_afterFSR, int& spectatorquark_pdgId, int& spectatorquark_status, TLorentzVector& spectatorquark_method2_beforeFSR,
              TLorentzVector& spectatorquark_method2_afterFSR, int& spectatorquark_method2_pdgId, int& spectatorquark_method2_status);

    virtual StatusCode execute() override;


  };
}

#endif
