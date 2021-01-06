/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PFONeutralValidationPlots.h"

PFONeutralValidationPlots::PFONeutralValidationPlots(PlotBase* pParent, std::string sDir,  std::string sPFOContainerName, std::string sFEContainerName) : PlotBase(pParent, sDir),
																			  m_PFOPlots(this,"",sPFOContainerName,""),
																			  m_PFOClusterMomentPlots(this,"",sPFOContainerName,""),
																			  m_PFOCalibHitClusterMomentPlots(this,"",sPFOContainerName,""),
																			  m_PFOAttributePlots(this,"",sPFOContainerName,""),
																			  m_PFOEMPlots(this,"",sPFOContainerName,""),	        
																			  m_FEPlots(this,"","",sFEContainerName),
																			  m_FEClusterMomentPlots(this,"","",sFEContainerName),
																			  m_FEAttributePlots(this,"","",sFEContainerName),
																			  m_FEEMPlots(this,"","",sFEContainerName),
                                                                                                                                                          m_FELinkerPlots(this,"",sFEContainerName,true)
{}

void PFONeutralValidationPlots::fill(const xAOD::PFO& thePFO){
  m_PFOPlots.fill(thePFO);
  m_PFOClusterMomentPlots.fill(thePFO);
  m_PFOCalibHitClusterMomentPlots.fill(thePFO);
  m_PFOAttributePlots.fill(thePFO);
  m_PFOEMPlots.fill(thePFO);
}

void PFONeutralValidationPlots::fill(const xAOD::FlowElement& theFE){
  //overload of previous function using FlowElement
  m_FEPlots.fill(theFE);
  m_FEClusterMomentPlots.fill(theFE);
  //m_FECalibHitClusterMomentPlots.fill(theFE); // MC doesn't generally have the relevant calibhits saved. To add at a later date if needed
  m_FEAttributePlots.fill(theFE);
  //m_FEEMPlots.fill(theFE);
  m_FELinkerPlots.fill(theFE);
}
