/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef NELECTRONTIGHTSELECTOR_H_
#define NELECTRONTIGHTSELECTOR_H_

#include "TopEventSelectionTools/SignValueSelector.h"

namespace top {

/**
 * @brief A tool that selects events with a certain number of good tight electrons.
 * This is identical to NElectronSelector in the case of tight selection, so only relevant for loose selection.
 */
class NElectronTightSelector : public SignValueSelector {
public:
    explicit NElectronTightSelector(const std::string& params);

    bool apply(const top::Event& event) const override;

    bool applyParticleLevel(const top::ParticleLevelEvent& event) const override;
};

}

#endif
