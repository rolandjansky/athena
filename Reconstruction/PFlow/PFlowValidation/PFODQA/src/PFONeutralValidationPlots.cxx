/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PFONeutralValidationPlots.h"

PFONeutralValidationPlots::PFONeutralValidationPlots(PlotBase* pParent, std::string sDir,  std::string sPFOContainerName) : PlotBase(pParent, sDir),
													     m_PFOPlots(this,"",sPFOContainerName),
													     m_PFOClusterMomentPlots(this,"",sPFOContainerName),
													     m_PFOCalibHitClusterMomentPlots(this,"",sPFOContainerName),
													     m_PFOAttributePlots(this,"",sPFOContainerName),
													     m_PFOEMPlots(this,"",sPFOContainerName)
{}

void PFONeutralValidationPlots::fill(const xAOD::PFO& thePFO){
  m_PFOPlots.fill(thePFO);
  m_PFOClusterMomentPlots.fill(thePFO);
  m_PFOCalibHitClusterMomentPlots.fill(thePFO);
  m_PFOAttributePlots.fill(thePFO);
  m_PFOEMPlots.fill(thePFO);
}


