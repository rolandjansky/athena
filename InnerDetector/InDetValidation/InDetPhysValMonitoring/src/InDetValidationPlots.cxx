/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetPhysValMonitoring/InDetValidationPlots.h"

// std includes


InDetValidationPlots::InDetValidationPlots(PlotBase* pParent, const std::string& dirName) : PlotBase(pParent, dirName),
  m_idPlots(this, "all/") {
  // nop
}

void
InDetValidationPlots::fill(const xAOD::TrackParticle& trkParticle) {
  m_idPlots.fill(trkParticle);
}
