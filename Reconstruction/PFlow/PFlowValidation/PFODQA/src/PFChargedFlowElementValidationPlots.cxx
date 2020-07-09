/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PFChargedFlowElementValidationPlots.h"

PFChargedFlowElementValidationPlots::PFChargedFlowElementValidationPlots(PlotBase* pParent, std::string sDir,  std::string sFlowElementContainerName) : PlotBase(pParent, sDir)
															    //m_PFOPlots(this,"",sPFOContainerName),
{}

void PFChargedFlowElementValidationPlots::fill(const xAOD::FlowElement& theFE, const xAOD::Vertex* theVertex ){
  //m_PFOPlots.fill(thePFO);
}


