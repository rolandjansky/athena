/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

#ifndef NFWDELECTRONSELECTOR_H_
#define NFWDELECTRONSELECTOR_H_

#include "TopEventSelectionTools/SignValueSelector.h"

#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"
#include "AsgTools/AnaToolHandle.h"

namespace top {
/**
 * @brief A tool that selects events with a certain number of good forward electrons.
 */
  class NFwdElectronSelector: public SignValueSelector {
  public:
    explicit NFwdElectronSelector(const std::string& params);

    bool apply(const top::Event& event) const override;

    bool applyParticleLevel(const top::ParticleLevelEvent& event) const override;
  };
}

#endif
