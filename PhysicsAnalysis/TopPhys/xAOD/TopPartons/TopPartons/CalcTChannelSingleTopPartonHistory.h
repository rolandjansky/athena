/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#ifndef ANALYSISTOP_TOPPARTONS_CALCTCHANNELSINGLETOPPARTONHISTORY_H
#define ANALYSISTOP_TOPPARTONS_CALCTCHANNELSINGLETOPPARTONHISTORY_H

/**
 * @author Tomas Dado <tomas.dado@cern.ch>
 *
 * @brief CalcWtbPartonHistory
 *   Class derived from CalcTopPartonHistory, used to store Wt SingleTop variables
 *
 * $Revision: 658996 $
 * $Date: 2015-04-04 16:57:15 +0100 (Sat, 04 Apr 2015) $
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

    bool spectatorquark(const xAOD::TruthParticleContainer* truthParticles, TLorentzVector& spectatorquark_beforeFSR,
              TLorentzVector& spectatorquark_afterFSR, int& spectatorquark_pdgId, int& spectatorquark_status);

    virtual StatusCode execute() override;


  };
}

#endif
