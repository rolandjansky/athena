/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

#ifndef NFWDELECTRONTIGHTSELECTOR_H_
#define NFWDELECTRONTIGHTSELECTOR_H_

#include "TopEventSelectionTools/SignValueSelector.h"

namespace top {
/**
 * @brief A tool that selects events with a certain number of good tight electrons.
 * This is identical to NElectronSelector in the case of tight selection, so only relevant for loose selection.
 */
  class NFwdElectronTightSelector: public SignValueSelector {
  public:
    explicit NFwdElectronTightSelector(const std::string& params);

    bool apply(const top::Event& event) const override;

    bool applyParticleLevel(const top::ParticleLevelEvent& event) const override;
  };
}

#endif
