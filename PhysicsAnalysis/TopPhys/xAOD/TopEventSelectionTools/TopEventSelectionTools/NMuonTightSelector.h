/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef NMUONTIGHTSELECTOR_H_
#define NMUONTIGHTSELECTOR_H_

#include "TopEventSelectionTools/SignValueSelector.h"

namespace top {

/**
 * @brief A tool that selects events with a certain number of good tight muons.
 * This is identical to NMuonSelector in the case of tight selection, so only relevant for loose selection.
 */
class NMuonTightSelector : public SignValueSelector {
public:
    explicit NMuonTightSelector(const std::string& params);

    bool apply(const top::Event& event) const override;

    bool applyParticleLevel(const top::ParticleLevelEvent& event) const override;
};

}

#endif
