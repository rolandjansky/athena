/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

#ifndef NSOFTMUONSELECTOR_H_
#define NSOFTMUONSELECTOR_H_

#include "TopEventSelectionTools/SignValueSelector.h"

namespace top {
/**
 * @brief A tool that selects events with a certain number of good muons.
 */
  class NSoftMuonSelector: public SignValueSelector {
  public:
    explicit NSoftMuonSelector(const std::string& params);

    bool apply(const top::Event& event) const override;

    bool applyParticleLevel(const top::ParticleLevelEvent& event) const override;
  };
}

#endif
