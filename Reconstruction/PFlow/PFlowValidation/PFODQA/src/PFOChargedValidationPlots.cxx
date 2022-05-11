/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "PFOChargedValidationPlots.h"

PFOChargedValidationPlots::PFOChargedValidationPlots(
  PlotBase* pParent,
  const std::string& sDir,
  const std::string& sFEContainerName)
  : PlotBase(pParent, sDir)
  , m_FEPlots(this, "", sFEContainerName)
  , m_FEPVMatchedPlots(this, "", sFEContainerName)
  , m_FEAlgPropertyPlots(this, "", sFEContainerName)
  , m_FELinkerPlots(this, "", sFEContainerName, false)
{
}

void PFOChargedValidationPlots::fill(const xAOD::FlowElement& theFE, const xAOD::Vertex* theVertex, const xAOD::EventInfo& eventInfo ){
  m_FEPlots.fill(theFE,eventInfo);
  m_FEAlgPropertyPlots.fill(theFE,eventInfo);
  if (theVertex) m_FEPVMatchedPlots.fill(theFE, *theVertex,eventInfo);
  m_FELinkerPlots.fill(theFE,eventInfo);
}


