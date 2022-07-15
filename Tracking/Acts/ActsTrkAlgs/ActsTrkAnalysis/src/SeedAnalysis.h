/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSTRKANALYSIS_SEEDANALYSIS_H
#define ACTSTRKANALYSIS_SEEDANALYSIS_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "TrkTruthData/PRD_MultiTruthCollection.h"

#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "xAODInDetMeasurement/PixelClusterContainer.h"
#include "xAODInDetMeasurement/StripClusterContainer.h"
#include "ActsTrkEvent/Seed.h"

#include "InDetPrepRawData/PixelClusterContainer.h"
#include "InDetPrepRawData/SCT_ClusterContainer.h"

#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/ReadHandleKey.h"
#include "ActsTrkEvent/Seed.h"

#include "BeamSpotConditionsData/BeamSpotData.h"
#include "MagFieldConditions/AtlasFieldCacheCondObj.h"
#include "MagFieldElements/AtlasFieldCache.h"

#include "GaudiKernel/ToolHandle.h"
#include "ActsTrkToolInterfaces/ITrackParamsEstimationTool.h"
#include "ActsGeometryInterfaces/IActsTrackingGeometryTool.h"
#include "ActsGeometryInterfaces/IActsATLASConverterTool.h"

#include "xAODEventInfo/EventInfo.h"

namespace ActsTrk {

  class SeedAnalysis final :
    public AthMonitorAlgorithm {
  public:
    SeedAnalysis(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~SeedAnalysis() override = default;

    virtual StatusCode initialize() override;
    virtual StatusCode fillHistograms(const EventContext& ctx) const override;

  private:
    StatusCode fillTruthHistograms(const EventContext& ctx,
				   const ActsTrk::SeedContainer& seed_container,
				   std::vector<int>&,
				   std::vector<double>&) const;

    const Identifier identify(const xAOD::PixelCluster&) const;
    const Identifier identify(const xAOD::StripCluster&) const;

  private:
    void matchParticleToSeedClusters(const PRD_MultiTruthCollection* prdTruth,
				     const Identifier& id,
				     std::map<int, int>& countMap) const;
    std::pair<int, double> findSeedMajorityTruthParticle(const std::map<int, int>& countMap) const;

    std::array<float, 7> estimateParameters(const ActsTrk::Seed& seed,
                                            float pTPerHelixRadius) const;
      
  private:
    ToolHandle< ActsTrk::ITrackParamsEstimationTool > m_paramEstimationTool {this, "TrackParamsEstimationTool", "", "Track Param Estimation from Seeds"};
    PublicToolHandle< IActsTrackingGeometryTool > m_trackingGeometryTool {this, "TrackingGeometryTool", "ActsTrackingGeometryTool"};
    ToolHandle< IActsATLASConverterTool > m_ATLASConverterTool{this, "ATLASConverterTool", "ActsATLASConverterTool"};

    SG::ReadHandleKey< ActsTrk::SeedContainer > m_inputSeedColletionKey {this,  "InputSeedCollection", "", ""}; 

    SG::ReadHandleKey< xAOD::PixelClusterContainer > m_pixelClusterContainerKey {this, "PixelClusterContainerKey", "ITkPixelClusters", "Key of input pixel clusters"};
    SG::ReadHandleKey< xAOD::StripClusterContainer > m_stripClusterContainerKey {this, "StripClusterContainerKey", "ITkStripClusters", "Key of input strip clusters"};
    SG::ReadHandleKey< PRD_MultiTruthCollection > m_prdTruth {this, "ITkClustersTruth", "", ""};
    SG::ReadCondHandleKey< InDetDD::SiDetectorElementCollection > m_detEleCollKey {this, "DetectorElements", "", "Key of input SiDetectorElementCollection"};

    SG::ReadCondHandleKey< InDet::BeamSpotData > m_beamSpotKey {this, "BeamSpotKey", "BeamSpotData", "SG key for beam spot"};
    SG::ReadCondHandleKey< AtlasFieldCacheCondObj > m_fieldCondObjInputKey {this, "AtlasFieldCacheCondObj", "fieldCondObj",
        "Name of the Magnetic Field conditions object key"};

    SG::ReadHandleKey<xAOD::EventInfo> m_evtKey {this, "EventInfoKey", "EventInfo"};

    Gaudi::Property< bool > m_usePixel {this, "UsePixel", true, ""};
  };

}

#endif
