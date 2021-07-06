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

void PFONeutralValidationPlots::fill(const xAOD::PFO& thePFO,const xAOD::EventInfo& eventInfo){
  m_PFOPlots.fill(thePFO,eventInfo);
  m_PFOClusterMomentPlots.fill(thePFO,eventInfo);
  m_PFOCalibHitClusterMomentPlots.fill(thePFO,eventInfo);
  m_PFOAttributePlots.fill(thePFO,eventInfo);
  m_PFOEMPlots.fill(thePFO,eventInfo);
}

void PFONeutralValidationPlots::fill(const xAOD::FlowElement& theFE, const xAOD::EventInfo& eventInfo){
  //overload of previous function using FlowElement
  m_FEPlots.fill(theFE,eventInfo);
  m_FEClusterMomentPlots.fill(theFE,eventInfo);
  m_FEAttributePlots.fill(theFE,eventInfo);
  m_FELinkerPlots.fill(theFE,eventInfo);
}
