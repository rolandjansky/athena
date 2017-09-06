/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SSSELECTOR_H_
#define SSSELECTOR_H_

#include "TopEventSelectionTools/EventSelectorBase.h"

namespace top {

/**
 * @brief Keep only events that have at least one same sign lepton pair.
 *
 */
class SSLeptonSelector : public EventSelectorBase{
public:
    bool apply(const top::Event& event) const override;

    bool applyParticleLevel(const top::ParticleLevelEvent& event) const override;

    std::string name() const override;
};

}

#endif
