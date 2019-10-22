/*
   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 */

#ifndef ANALYSISTOP_TOPPARTONS_CALCTTBARLIGHTPARTONHISTORY_H
#define ANALYSISTOP_TOPPARTONS_CALCTTBARLIGHTPARTONHISTORY_H

/**
 * @author John Morris <john.morris@cern.ch>
 * @author Silvestre Marino Romano <sromanos@cern.ch>
 * @author Samuel Calvet <scalvet@cern.ch>
 * @author Sonja Zeissner <sonja.zeissner@cern.ch>
 *
 * @brief CalcTtbarLightPartonHistory
 *   Class derived from CalcTopPartonHistory, used to store ttbar variables
 *
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
  class CalcTtbarLightPartonHistory: public CalcTopPartonHistory {
  public:
    explicit CalcTtbarLightPartonHistory(const std::string& name);
    virtual ~CalcTtbarLightPartonHistory() {}

    //Storing parton history for ttbar resonance analysis
    CalcTtbarLightPartonHistory(const CalcTtbarLightPartonHistory& rhs) = delete;
    CalcTtbarLightPartonHistory(CalcTtbarLightPartonHistory&& rhs) = delete;
    CalcTtbarLightPartonHistory& operator = (const CalcTtbarLightPartonHistory& rhs) = delete;

    void ttbarHistorySaver(const xAOD::TruthParticleContainer* truthParticles, xAOD::PartonHistory* ttbarPartonHistory);

    virtual StatusCode execute();
  };
}

#endif
