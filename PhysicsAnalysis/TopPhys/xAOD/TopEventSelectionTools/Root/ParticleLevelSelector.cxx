/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopEventSelectionTools/ParticleLevelSelector.h"

namespace top {
    bool ParticleLevelSelector::apply(const top::Event&) const {
        return false;
    }

    bool ParticleLevelSelector::applyParticleLevel(const top::ParticleLevelEvent&) const {
        return true;
    }

    std::string ParticleLevelSelector::name() const {
        return "PARTICLE_LEVEL";
    }
}
