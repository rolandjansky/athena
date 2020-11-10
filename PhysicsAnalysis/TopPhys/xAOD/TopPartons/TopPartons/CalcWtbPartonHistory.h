/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#ifndef ANALYSISTOP_TOPPARTONS_CALCWTBPARTONHISTORY_H
#define ANALYSISTOP_TOPPARTONS_CALCWTBPARTONHISTORY_H

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
  class CalcWtbPartonHistory: public CalcTopPartonHistory {
  public:
    explicit CalcWtbPartonHistory(const std::string& name);

    //Storing parton history for Wtb SingleTop analysis
    CalcWtbPartonHistory(const CalcWtbPartonHistory& rhs) = delete;
    CalcWtbPartonHistory(CalcWtbPartonHistory&& rhs) = delete;
    CalcWtbPartonHistory& operator = (const CalcWtbPartonHistory& rhs) = delete;

    void wtbHistorySaver(const xAOD::TruthParticleContainer* truthParticles, xAOD::PartonHistory* wtSTPartonHistory);

    virtual StatusCode execute() override;
  };
}

#endif
