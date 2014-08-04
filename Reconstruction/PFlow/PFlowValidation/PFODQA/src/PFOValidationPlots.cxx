/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PFOValidationPlots.h"

PFOValidationPlots::PFOValidationPlots(PlotBase* pParent, std::string sDir,  std::string sPFOContainerName) : PlotBase(pParent, sDir),
													     m_PFOPlots(this,"PFO_JetETMiss/",sPFOContainerName),
													     m_PFOClusterMomentPlots(this,"PFO_JetETMiss/",sPFOContainerName),
													     m_PFOEMPlots(this,"PFO_JetETMiss/",sPFOContainerName)
{}

void PFOValidationPlots::fill(const xAOD::PFO& thePFO, bool fillEMHistograms){
  m_PFOPlots.fill(thePFO);
  m_PFOClusterMomentPlots.fill(thePFO);
  if (true == fillEMHistograms) m_PFOEMPlots.fill(thePFO);
}


