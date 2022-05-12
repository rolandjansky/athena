/*
   Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
 */

#ifndef PIXELATHCLUSTERMONTOOL_H
#define PIXELATHCLUSTERMONTOOL_H

#include "PixelAthMonitoringBase.h"


#include "TrkToolInterfaces/ITrackHoleSearchTool.h"
#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"

#include "InDetPrepRawData/PixelClusterContainer.h"
#include "TrkTrack/TrackCollection.h"



//------------------------------

class PixelID;
class AtlasDetectorID;

namespace InDet {
  class PixelCluster;
  class IInDetTrackSelectionTool;
}
namespace Trk {
  class ITrackHoleSearchTool;
}


class PixelAthClusterMonAlg: public PixelAthMonitoringBase {
public:
  PixelAthClusterMonAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~PixelAthClusterMonAlg();
  virtual StatusCode initialize() override;
  virtual StatusCode fillHistograms(const EventContext& ctx) const override;
  std::string findComponentString(int bec, int ld) const;
private:

  ToolHandle<Trk::ITrackHoleSearchTool> m_holeSearchTool;
  ToolHandle<InDet::IInDetTrackSelectionTool> m_trackSelTool;

  const AtlasDetectorID* m_atlasid {};

  SG::ReadHandleKey<InDet::PixelClusterContainer> m_clustersKey {
    this, "ClusterName", "PixelClusters", "pixel cluster data key"
  };
  SG::ReadHandleKey<TrackCollection> m_tracksKey {
    this, "TrackName", "CombinedInDetTracks", "track data key"
  };

  bool m_doOnline {};
  bool m_doLumiBlock {};
  bool m_doLowOccupancy {};
  bool m_doHighOccupancy {};
  bool m_doHeavyIonMon {};
  bool m_doFEPlots {};
};
#endif
