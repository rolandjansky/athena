/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSTRKANALYSIS_SPACEPOINTANALYSIS_H
#define ACTSTRKANALYSIS_SPACEPOINTANALYSIS_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "GeoPrimitives/GeoPrimitives.h"

#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"

#include "ActsTrkEvent/SpacePoint.h"
#include "xAODInDetMeasurement/PixelClusterContainer.h"
#include "xAODInDetMeasurement/StripClusterContainer.h"

namespace ActsTrk {

  class SpacePointAnalysis final : 
    public AthMonitorAlgorithm {

  public:
    SpacePointAnalysis(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~SpacePointAnalysis() override = default;

    virtual StatusCode initialize() override;
    virtual StatusCode fillHistograms(const EventContext& ctx) const override;

  private:
    SG::ReadHandleKey< ActsTrk::SpacePointContainer > m_spacePointContainerKey {this, "SpacePointContainerKey", "ITkPixelSpacePoints", "Key of input space points"};
    SG::ReadHandleKey< xAOD::PixelClusterContainer > m_pixelClusterContainerKey {this, "PixelClusterContainerKey", "ITkPixelClusters", "Key of input pixel clusters"};
    SG::ReadHandleKey< xAOD::StripClusterContainer > m_stripClusterContainerKey {this, "StripClusterContainerKey", "ITkStripClusters", "Key of input strip clusters"};

    Gaudi::Property< bool > m_usePixel {this, "UsePixel", true, "enable use of pixel ID or SCT ID"};
    Gaudi::Property< bool > m_useOverlap {this, "UseOverlap", false, "looking at strip strip space points"};
  };
}

#endif // ACTSTRKANALYSIS_SPACEPOINTANALYSIS_H
