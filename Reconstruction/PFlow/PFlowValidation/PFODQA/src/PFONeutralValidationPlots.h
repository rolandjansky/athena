/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PFONEUTRALVALIDATIONPLOTS_H
#define PFONEUTRALVALIDATIONPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "PFOHistUtils/PFOPlots.h"
#include "PFOHistUtils/PFOClusterMomentPlots.h"
#include "PFOHistUtils/PFOCalibHitClusterMomentPlots.h"
#include "PFOHistUtils/PFOAttributePlots.h"
#include "PFOHistUtils/PFOEMPlots.h"
#include "xAODPFlow/PFO.h"

class PFONeutralValidationPlots : public PlotBase {

 public:

  /** Standard Constructor */
  PFONeutralValidationPlots(PlotBase* pParent, std::string sDir, std::string sPFOContainerName);

  /** fill the histograms up */
  void fill(const xAOD::PFO& thePFO);

 private:
  /** 4-vector histograms */
  PFO::PFOPlots m_PFOPlots;
  /** Cluster Moment histograms */
  PFO::PFOClusterMomentPlots m_PFOClusterMomentPlots;
  /** CalibHit Cluster Moment histograms */
  PFO::PFOCalibHitClusterMomentPlots m_PFOCalibHitClusterMomentPlots;
  /** PFO attributes */
  PFO::PFOAttributePlots m_PFOAttributePlots;
  /** 4-vector histograms at EM scale */
  PFO::PFOEMPlots m_PFOEMPlots;

};
#endif
