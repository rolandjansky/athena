/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PFOChargedValidationPlots.h"

PFOChargedValidationPlots::PFOChargedValidationPlots(PlotBase* pParent, std::string sDir,  std::string sPFOContainerName) : PlotBase(pParent, sDir),
															    m_PFOPlots(this,"PFO_JetETMiss/",sPFOContainerName),
															    m_PFOPVMatchedPlots(this,"PFO_JetETMiss/",sPFOContainerName),
															    m_PFOAlgPropertyPlots(this,"PFO_JetETMiss/",sPFOContainerName)
{}

void PFOChargedValidationPlots::fill(const xAOD::PFO& thePFO, const xAOD::Vertex* theVertex ){
  m_PFOPlots.fill(thePFO);
  m_PFOAlgPropertyPlots.fill(thePFO);
  if (theVertex) m_PFOPVMatchedPlots.fill(thePFO, *theVertex);
}


