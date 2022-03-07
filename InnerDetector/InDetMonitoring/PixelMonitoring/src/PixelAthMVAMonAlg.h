/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELATHMVAMONTOOL_H
#define PIXELATHMVAMONTOOL_H

#include "PixelAthMonitoringBase.h"

#include "InDetConditionsSummaryService/IInDetConditionsTool.h"
#include "PixelReadoutGeometry/IPixelReadoutManager.h"

#include "InDetRawData/PixelRDO_Container.h"
#include "InDetPrepRawData/PixelClusterContainer.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkToolInterfaces/ITrackHoleSearchTool.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"

#include "PathResolver/PathResolver.h"
#include "MVAUtils/BDT.h"
#include "TFile.h"
#include "TTree.h"

class PixelID;
class PixelRDORawData;

namespace InDet {
  class PixelCluster;
  class IInDetTrackSelectionTool;
}
namespace Trk {
  class ITrackHoleSearchTool;
}

class PixelAthMVAMonAlg : public PixelAthMonitoringBase {

 public:
  
  PixelAthMVAMonAlg( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~PixelAthMVAMonAlg();
  virtual StatusCode initialize() override;
  virtual StatusCode fillHistograms( const EventContext& ctx ) const override;
  std::string findComponentString(int bec, int ld) const;

 private:
  ServiceHandle<InDetDD::IPixelReadoutManager> m_pixelReadout
  {
    this, "PixelReadoutManager", "PixelReadoutManager", "Pixel readout manager"
  };

  ToolHandle<Trk::ITrackHoleSearchTool> m_holeSearchTool;
  ToolHandle<InDet::IInDetTrackSelectionTool> m_trackSelTool;
  ToolHandle<Trk::IExtrapolator> m_trkextrapolator;
  ToolHandle<IInDetConditionsTool> m_pixelCondSummaryTool{this, "PixelConditionsSummaryTool", "PixelConditionsSummaryTool", "Tool to retrieve Pixel Conditions summary"};

  const AtlasDetectorID* m_atlasid;  //tracks only
  const PixelID* m_pixelid;

  SG::ReadHandleKey<PixelRDO_Container> m_pixelRDOName{this, "RDOName", "PixelRDOs", "rdo data key"};
  SG::ReadHandleKey<InDet::PixelClusterContainer> m_clustersKey{this, "ClusterName", "PixelClusters", "pixel cluster data key" };
  SG::ReadHandleKey<TrackCollection> m_tracksKey{this, "TrackName", "CombinedInDetTracks", "track data key"};

  std::string m_calibFolder;
  std::map<std::string, std::unique_ptr<MVAUtils::BDT> > m_classBDT;

  bool m_dumpTree;
};
#endif
