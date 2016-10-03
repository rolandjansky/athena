/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PFOVALIDATIONPLOTS_H
#define PFOVALIDATIONPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "PFOHistUtils/PFOPlots.h"
#include "PFOHistUtils/PFOClusterMomentPlots.h"
#include "PFOHistUtils/PFOAttributePlots.h"
#include "PFOHistUtils/PFOAlgPropertyPlots.h"
#include "PFOHistUtils/PFOEMPlots.h"
#include "xAODPFlow/PFO.h"

class PFOValidationPlots : public PlotBase {

 public:

  /** Standard Constructor */
  PFOValidationPlots(PlotBase* pParent, std::string sDir, std::string sPFOContainerName);

  /** fill the histograms up */
  void fill(const xAOD::PFO& thePFO, bool fillEMHistograms);

 private:
  /** 4-vector histograms */
  PFO::PFOPlots m_PFOPlots;
  /** Cluster Moment histograms */
  PFO::PFOClusterMomentPlots m_PFOClusterMomentPlots;
  /** PFO attributes */
  PFO::PFOAttributePlots m_PFOAttributePlots;
  /** Algorithm property plots */
  PFO::PFOAlgPropertyPlots m_PFOAlgPropertyPlots;
  /** 4-vector histograms at EM scale */
  PFO::PFOEMPlots m_PFOEMPlots;

};
#endif
