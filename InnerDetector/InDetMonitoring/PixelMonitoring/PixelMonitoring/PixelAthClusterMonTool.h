/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELATHCLUSTERMONTOOL_H
#define PIXELATHCLUSTERMONTOOL_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoring/Monitored.h"
#include "PixelAthMonitoringBase.h"

#include "AtlasDetDescr/AtlasDetectorID.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetConditionsSummaryService/IInDetConditionsTool.h"
#include "StoreGate/ReadHandleKey.h"

#include "InDetPrepRawData/PixelClusterContainer.h"

//------------TrackMon------------
#include <algorithm>
#include "TrkToolInterfaces/ITrackHoleSearchTool.h"
#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"

#include "TrkTrack/TrackCollection.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetRIO_OnTrack/SiClusterOnTrack.h"


//for Amg::error helper function:
#include "EventPrimitives/EventPrimitivesHelpers.h"
//------------------------------

class PixelID;

namespace InDet {
  class PixelCluster;
  class IInDetTrackSelectionTool;
}
namespace Trk {
  class ITrackHoleSearchTool;
}


class PixelAthClusterMonTool : public PixelAthMonitoringBase {

 public:
  
  PixelAthClusterMonTool( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~PixelAthClusterMonTool();
  virtual StatusCode initialize() override;
  virtual StatusCode fillHistograms( const EventContext& ctx ) const override;
  std::string findComponentString(int bec, int ld) const;

 private:

  ToolHandle<Trk::ITrackHoleSearchTool> m_holeSearchTool;
  ToolHandle<InDet::IInDetTrackSelectionTool> m_trackSelTool;
  ToolHandle<IInDetConditionsTool> m_pixelCondSummaryTool{this, "PixelConditionsSummaryTool", "PixelConditionsSummaryTool", "Tool to retrieve Pixel Conditions summary"};

  const AtlasDetectorID* m_atlasid;  //tracks only
  const PixelID* m_pixelid;

  SG::ReadHandleKey<InDet::PixelClusterContainer> m_clustersKey{this, "ClusterName", "PixelClusters", "pixel cluster data key" };
  SG::ReadHandleKey<TrackCollection> m_tracksKey{this, "TrackName", "CombinedInDetTracks", "track data key"};
 
  bool m_doOnline;
  bool m_doModules;
  bool m_doLumiBlock;
  bool m_doLowOccupancy;
  bool m_doHighOccupancy;
  bool m_doHeavyIonMon; 

};
#endif
