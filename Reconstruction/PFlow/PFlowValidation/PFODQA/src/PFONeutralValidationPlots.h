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
#include "PFOHistUtils/FlowElement_LinkerPlots.h"
#include "xAODPFlow/PFO.h"
#include "xAODPFlow/FlowElement.h"
#include "xAODEventInfo/EventInfo.h"

class PFONeutralValidationPlots : public PlotBase {

 public:

  /** Standard Constructor */
  PFONeutralValidationPlots(PlotBase* pParent, std::string sDir, std::string sPFOContainerName, std::string sFEContainerName);

  /** fill the histograms up */
  void fill(const xAOD::PFO& thePFO,const xAOD::EventInfo& eventInfo);
  void fill(const xAOD::FlowElement& theFE, const xAOD::EventInfo& eventInfo);

 private:
  // objects for PFOs
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

  // equivalent classes for FlowElements - separate so we can book-keep them separately
  /** 4-vector histograms */
  PFO::PFOPlots m_FEPlots;
  /** Cluster Moment histograms */
  PFO::PFOClusterMomentPlots m_FEClusterMomentPlots;
  /** CalibHit Cluster Moment histograms */
  /**  PFO::PFOCalibHitClusterMomentPlots m_FECalibHitClusterMomentPlots; // MC doesn't generally have the relevant calibhits saved. To add at a later date if needed */
  /** FE attributes */
  PFO::PFOAttributePlots m_FEAttributePlots;
  /** 4-vector histograms at EM scale */
  PFO::PFOEMPlots m_FEEMPlots;
  
  /** Flow element linkers to leptons/photons */
  PFO::FlowElement_LinkerPlots m_FELinkerPlots;
  
};
#endif
