/*
   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 */

#ifndef ANALYSISTOP_TOPPARTONS_CALCTTZPARTONHISTORY_H
#define ANALYSISTOP_TOPPARTONS_CALCTTZPARTONHISTORY_H

/**
 * @author Knut Zoch <kzoch@cern.ch>
 * @brief CalcTTZPartonHistory
 *   Class derived from CalcTopPartonHistory, used to store ttZ variables
 *
 * Class definition follows that of class CalcTtbarHistory.
 **/


// Framework include(s):
#include "TopPartons/CalcTtbarPartonHistory.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "TopPartons/PartonHistory.h"

// forward declaration(s):
namespace top {
  class TopConfig;
}

namespace top {
  class CalcTTZPartonHistory: public CalcTtbarPartonHistory {
  public:
    explicit CalcTTZPartonHistory(const std::string& name);
    virtual ~CalcTTZPartonHistory() {}

    CalcTTZPartonHistory(const CalcTTZPartonHistory& rhs) = delete;
    CalcTTZPartonHistory(CalcTTZPartonHistory&& rhs) = delete;
    CalcTTZPartonHistory& operator = (const CalcTTZPartonHistory& rhs) = delete;

    void zHistorySaver(const xAOD::TruthParticleContainer* truthParticles, xAOD::PartonHistory* ttbarPartonHistory);

    virtual StatusCode execute();

    const xAOD::TruthParticle* getFlavourSibling(const xAOD::TruthParticle* particle);
    bool getZ(const xAOD::TruthParticleContainer* truthParticles,
              TLorentzVector* Z,
              TLorentzVector* ZDecay1,
              int* ZDecay1_pdgId,
              TLorentzVector* ZDecay2,
              int* ZDecay2_pdgId,
              bool* is_on_shell);
  private:
    bool m_ancestry_corrupted;
  };
}

#endif
