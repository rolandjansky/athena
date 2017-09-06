/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef OSTIGHTSELECTOR_H_
#define OSTIGHTSELECTOR_H_

#include "TopEventSelectionTools/EventSelectorBase.h"

namespace top {

/**
 * @brief Keep only events that have at least one opposite sign tight lepton pair.
 * This is identical to OSLeptonSelector in the case of tight selection, so only relevant for loose selection.
 *
 * Try to find at least one lepton (el, mu) with positive charge,
 * and at least one with negative charge.
 */
class OSLeptonTightSelector : public EventSelectorBase  {
public:
    bool apply(const top::Event& event) const override;

    bool applyParticleLevel(const top::ParticleLevelEvent& event) const override;

    std::string name() const override;
};

}

#endif
