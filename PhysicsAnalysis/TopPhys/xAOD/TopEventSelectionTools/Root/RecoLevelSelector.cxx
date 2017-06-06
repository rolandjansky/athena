/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Filename: RecoLevelSelector.cxx
// Description:
// Author: Fabian Wilk
// Created: Tue Oct 27 13:28:22 2015
//
// (c) by Fabian Wilk
//
// This file is licensed under a Creative Commons Attribution-ShareAlike 4.0
// International License.
//
// You should have received a copy of the license along with this work.
// If not, see <http://creativecommons.org/licenses/by-sa/4.0/>.

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
