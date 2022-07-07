/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSTRKANALYSIS_STRIPCLUSTERANALYSIS_H
#define ACTSTRKANALYSIS_STRIPCLUSTERANALYSIS_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "xAODInDetMeasurement/StripClusterContainer.h"
#include "InDetIdentifier/SCT_ID.h"

namespace ActsTrk {

  class StripClusterAnalysis final : 
    public AthMonitorAlgorithm {
  public:
    StripClusterAnalysis(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~StripClusterAnalysis() override = default;

    virtual StatusCode initialize() override;
    virtual StatusCode fillHistograms(const EventContext& ctx) const override;

  private:
    SG::ReadHandleKey<xAOD::StripClusterContainer> m_stripClusterContainerKey{this, "ClusterContainerKey", "ITkStripClusters", "Key of input pixel clusters"};

    const SCT_ID *m_stripID {};
  };

}

#endif // ACTSTRKANALYSIS_STRIPCLUSTERANALYSIS_H
