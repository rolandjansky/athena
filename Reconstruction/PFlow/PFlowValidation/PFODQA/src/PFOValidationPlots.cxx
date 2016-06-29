/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PFOValidationPlots.h"

PFOValidationPlots::PFOValidationPlots(PlotBase* pParent, std::string sDir,  std::string sPFOContainerName) : PlotBase(pParent, sDir),
													     m_PFOPlots(this,"PFO_JetETMiss/",sPFOContainerName),
													     m_PFOClusterMomentPlots(this,"PFO_JetETMiss/",sPFOContainerName),
													     m_PFOAttributePlots(this,"PFO_JetETMiss/",sPFOContainerName),
													     m_PFOAlgPropertyPlots(this,"PFO_JetETMiss/",sPFOContainerName),
													     m_PFOEMPlots(this,"PFO_JetETMiss/",sPFOContainerName)
{}

void PFOValidationPlots::fill(const xAOD::PFO& thePFO, bool fillEMHistograms){
  m_PFOPlots.fill(thePFO);
  if (0.0 == thePFO.charge()) m_PFOClusterMomentPlots.fill(thePFO);
  if (0.0 == thePFO.charge()) m_PFOAttributePlots.fill(thePFO);
  else  m_PFOAlgPropertyPlots.fill(thePFO);
  if (true == fillEMHistograms) m_PFOEMPlots.fill(thePFO);
}


