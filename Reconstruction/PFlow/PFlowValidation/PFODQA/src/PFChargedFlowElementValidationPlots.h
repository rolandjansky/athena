/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PFCHARGEDFLOWELEMENTVALIDATIONPLOTS_H
#define PFCHARGEDFLOWELEMENTVALIDATIONPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
//#include "PFOHistUtils/PFOPlots.h"
#include "xAODPFlow/FlowElement.h"
#include "xAODTracking/Vertex.h" 

class PFChargedFlowElementValidationPlots : public PlotBase {

 public:

  /** Standard Constructor */
  PFChargedFlowElementValidationPlots(PlotBase* pParent, std::string sDir, std::string sPFOContainerName);

  /** fill the histograms up */
  void fill(const xAOD::FlowElement& theFE, const xAOD::Vertex* theVertex);

 private:
  /** 4-vector and charge histograms */
  //PFO::PFOPlots m_PFOPlots;

};
#endif
