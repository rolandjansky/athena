/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#include "TopEventSelectionTools/PrintEventSelector.h"

#include "TopEvent/EventTools.h"

#include "TopCorrections/ScaleFactorRetriever.h"

#include "TopEventSelectionTools/MsgCategory.h"
using namespace TopEventSelectionTools;

namespace top {
  bool PrintEventSelector::apply(const top::Event& event) const {
    //print some stuff about the event
    ATH_MSG_INFO(event);
    return true;
  }

  bool PrintEventSelector::applyParticleLevel(const top::ParticleLevelEvent& plEvent) const {
    //print some stuff about the event
    ATH_MSG_INFO("Particle Level\n" << plEvent);
    return true;
  }

  std::string PrintEventSelector::name() const {
    return "PRINT";
  }
}
