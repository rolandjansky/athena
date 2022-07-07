/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSTRKANALYSIS_PIXELCLUSTERANALYSIS_H
#define ACTSTRKANALYSIS_PIXELCLUSTERANALYSIS_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "xAODInDetMeasurement/PixelClusterContainer.h"
#include "InDetIdentifier/PixelID.h"

namespace ActsTrk {

  class PixelClusterAnalysis final: 
    public AthMonitorAlgorithm {
  public:
    PixelClusterAnalysis(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~PixelClusterAnalysis() override = default;

    virtual StatusCode initialize() override;
    virtual StatusCode fillHistograms(const EventContext& ctx) const override;

  private:
    SG::ReadHandleKey< xAOD::PixelClusterContainer > m_pixelClusterContainerKey{this, "ClusterContainerKey", "ITkPixelClusters", "Key of input pixel clusters"};    

    const PixelID *m_pixelID {};
  };

}

#endif // ACTSTRKANALYSIS_PIXELCLUSTERANALYSIS_H
