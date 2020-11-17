/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PFOChargedValidationPlots.h"

PFOChargedValidationPlots::PFOChargedValidationPlots(PlotBase* pParent, std::string sDir,  std::string sPFOContainerName,std::string sFEContainerName) : PlotBase(pParent, sDir),
																			 m_PFOPlots(this,"",sPFOContainerName,""),
																			 m_PFOPVMatchedPlots(this,"",sPFOContainerName,""),
																			 m_PFOAlgPropertyPlots(this,"",sPFOContainerName,""),
																			 m_FEPlots(this,"","",sFEContainerName),
																			 m_FEPVMatchedPlots(this,"","",sFEContainerName),
																			 m_FEAlgPropertyPlots(this,"","",sFEContainerName),
                                                                                                                                                         m_FELinkerPlots(this,"",sFEContainerName,false)
{}

void PFOChargedValidationPlots::fill(const xAOD::PFO& thePFO, const xAOD::Vertex* theVertex ){
  m_PFOPlots.fill(thePFO);
  m_PFOAlgPropertyPlots.fill(thePFO);
  if (theVertex) m_PFOPVMatchedPlots.fill(thePFO, *theVertex);
}
void PFOChargedValidationPlots::fill(const xAOD::FlowElement& theFE, const xAOD::Vertex* theVertex ){
  m_FEPlots.fill(theFE);
  m_FEAlgPropertyPlots.fill(theFE);
  if (theVertex) m_FEPVMatchedPlots.fill(theFE, *theVertex);
  m_FELinkerPlots.fill(theFE);
}


