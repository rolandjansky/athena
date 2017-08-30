/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopEventSelectionTools/RecoLevelSelector.h"

namespace top {
    bool RecoLevelSelector::apply(const top::Event&) const {
        return true;
    }

    bool RecoLevelSelector::applyParticleLevel(const top::ParticleLevelEvent&) const {
        return false;
    }

    std::string RecoLevelSelector::name() const {
        return "RECO_LEVEL";
    }
}
