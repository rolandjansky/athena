/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PFOCHARGEDVALIDATIONPLOTS_H
#define PFOCHARGEDVALIDATIONPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "PFOHistUtils/PFOPlots.h"
#include "PFOHistUtils/PFOPVMatchedPlots.h"
#include "PFOHistUtils/PFOAlgPropertyPlots.h"
#include "xAODPFlow/PFO.h"
#include "xAODTracking/Vertex.h" 

class PFOChargedValidationPlots : public PlotBase {

 public:

  /** Standard Constructor */
  PFOChargedValidationPlots(PlotBase* pParent, std::string sDir, std::string sPFOContainerName);

  /** fill the histograms up */
  void fill(const xAOD::PFO& thePFO, const xAOD::Vertex* theVertex);

 private:
  /** 4-vector and charge histograms */
  PFO::PFOPlots m_PFOPlots;
  /** 4-vector and charge histograms with PV match cut applied */
  PFO::PFOPVMatchedPlots m_PFOPVMatchedPlots;
  /** Algorithm property plots */
  PFO::PFOAlgPropertyPlots m_PFOAlgPropertyPlots;
};
#endif
