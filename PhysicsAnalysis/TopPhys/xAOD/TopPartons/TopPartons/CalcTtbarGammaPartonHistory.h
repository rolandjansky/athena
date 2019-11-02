/*
   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 */

#ifndef ANALYSISTOP_TOPPARTONS_CalcTtbarGammaPartonHistory_H
#define ANALYSISTOP_TOPPARTONS_CalcTtbarGammaPartonHistory_H

/**
 * @author John Morris <john.morris@cern.ch>
 * @author Silvestre Marino Romano <sromanos@cern.ch>
 * @author Samuel Calvet <scalvet@cern.ch>
 *
 * @brief CalcTtbarGammaPartonHistory
 *   Class derived from CalcTopPartonHistory, used to store ttbargamma variables
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
  class CalcTtbarGammaPartonHistory: public CalcTopPartonHistory {
  public:
    explicit CalcTtbarGammaPartonHistory(const std::string& name);
    virtual ~CalcTtbarGammaPartonHistory() {}

    //Storing parton history for ttbargamma analysis
    CalcTtbarGammaPartonHistory(const CalcTtbarGammaPartonHistory& rhs) = delete;
    CalcTtbarGammaPartonHistory(CalcTtbarGammaPartonHistory&& rhs) = delete;
    CalcTtbarGammaPartonHistory& operator = (const CalcTtbarGammaPartonHistory& rhs) = delete;

    void ttbarGammaHistorySaver(const xAOD::TruthParticleContainer* truthParticles,
                                xAOD::PartonHistory* ttbarGammaPartonHistory);

    virtual StatusCode execute();
  };
}

#endif
