/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Filename: ParticleLevelSelector.cxx
// Description:
// Author: Fabian Wilk
// Created: Tue Oct 27 13:29:16 2015
//
// (c) by Fabian Wilk
//
// This file is licensed under a Creative Commons Attribution-ShareAlike 4.0
// International License.
//
// You should have received a copy of the license along with this work.
// If not, see <http://creativecommons.org/licenses/by-sa/4.0/>.

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
