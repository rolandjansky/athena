/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PFONeutralValidationPlots.h"

PFONeutralValidationPlots::PFONeutralValidationPlots(PlotBase* pParent, std::string sDir, std::string sFEContainerName) : PlotBase(pParent, sDir),																			 
																			  m_FEPlots(this,"",sFEContainerName),
																			  m_FEClusterMomentPlots(this,"",sFEContainerName),
																			  m_FEAttributePlots(this,"",sFEContainerName),
                                        m_FELinkerPlots(this,"",sFEContainerName,true)
{}

void PFONeutralValidationPlots::fill(const xAOD::FlowElement& theFE, const xAOD::EventInfo& eventInfo){
  //overload of previous function using FlowElement
  m_FEPlots.fill(theFE,eventInfo);
  m_FEClusterMomentPlots.fill(theFE,eventInfo);
  m_FEAttributePlots.fill(theFE,eventInfo);
  m_FELinkerPlots.fill(theFE,eventInfo);
}
