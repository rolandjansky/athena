/*
   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 */

#ifndef NRCJETSELECTOR_H_
#define NRCJETSELECTOR_H_

#include "TopEventSelectionTools/SignValueSelector.h"


namespace top {
  class NRCJetSelector: public top::SignValueSelector {
  public:
    explicit NRCJetSelector(const std::string& params);

    bool apply(const top::Event& event) const override;
    bool applyParticleLevel(const top::ParticleLevelEvent&) const override;
  };
}

#endif
