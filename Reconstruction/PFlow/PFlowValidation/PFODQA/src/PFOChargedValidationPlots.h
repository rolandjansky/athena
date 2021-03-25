/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PFOCHARGEDVALIDATIONPLOTS_H
#define PFOCHARGEDVALIDATIONPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "PFOHistUtils/PFOPlots.h"
#include "PFOHistUtils/PFOPVMatchedPlots.h"
#include "PFOHistUtils/PFOAlgPropertyPlots.h"
#include "PFOHistUtils/FlowElement_LinkerPlots.h"
#include "xAODPFlow/PFO.h"
#include "xAODPFlow/FlowElement.h"
#include "xAODTracking/Vertex.h" 
#include "xAODEventInfo/EventInfo.h"

class PFOChargedValidationPlots : public PlotBase {

 public:

  /** Standard Constructor */
  PFOChargedValidationPlots(PlotBase* pParent, std::string sDir, std::string sPFOContainerName,std::string sFEContainerName);

  /** fill the histograms up */
  void fill(const xAOD::PFO& thePFO, const xAOD::Vertex* theVertex, const xAOD::EventInfo& eventInfo);
  void fill(const xAOD::FlowElement& theFE, const xAOD::Vertex* theVertex, const xAOD::EventInfo& eventInfo);
 private:
  // PFO plots
  /** 4-vector and charge histograms */
  PFO::PFOPlots m_PFOPlots;
  /** 4-vector and charge histograms with PV match cut applied */
  PFO::PFOPVMatchedPlots m_PFOPVMatchedPlots;
  /** Algorithm property plots */
  PFO::PFOAlgPropertyPlots m_PFOAlgPropertyPlots;

  //Flow Element Plots
  /** 4-vector and charge histograms */
  PFO::PFOPlots m_FEPlots;
  /** 4-vector and charge histograms with PV match cut applied */
  PFO::PFOPVMatchedPlots m_FEPVMatchedPlots;
  /** Algorithm property plots */
  PFO::PFOAlgPropertyPlots m_FEAlgPropertyPlots;

  //Flow Element specific plots (linker algs)
  PFO::FlowElement_LinkerPlots m_FELinkerPlots;
};
#endif
