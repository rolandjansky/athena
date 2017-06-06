/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SSTIGHTSELECTOR_H_
#define SSTIGHTSELECTOR_H_

#include "TopEventSelectionTools/EventSelectorBase.h"

namespace top {

/**
 * @brief Keep only events that have at least one same sign tight lepton pair.
 * This is identical to SSLeptonSelector in the case of tight selection, so only relevant for loose selection.
 *
 */
class SSLeptonTightSelector : public EventSelectorBase{
public:
    bool apply(const top::Event& event) const override;

    bool applyParticleLevel(const top::ParticleLevelEvent& event) const override;

    std::string name() const override;
};

}

#endif
