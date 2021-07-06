/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "EgammaPhysValHistUtilities.h"

namespace Egamma{
  
  uint8_t RetrieveHitInfo(const xAOD::Electron& el, xAOD::SummaryType info, uint8_t defaultval){
    uint8_t val = defaultval;
    if (!el.trackParticleSummaryValue(val,info)) return defaultval;
    return val;

  }

}
