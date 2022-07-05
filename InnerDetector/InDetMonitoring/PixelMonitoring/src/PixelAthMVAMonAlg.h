/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELATHMVAMONTOOL_H
#define PIXELATHMVAMONTOOL_H

#include "PixelAthMonitoringBase.h"


#include "InDetRawData/PixelRDO_Container.h"
#include "InDetPrepRawData/PixelClusterContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "TrkToolInterfaces/ITrackHoleSearchTool.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"

#include "PathResolver/PathResolver.h"
#include "MVAUtils/BDT.h"


#include <map>
#include <memory>

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

  ToolHandle<Trk::ITrackHoleSearchTool> m_holeSearchTool;
  ToolHandle<InDet::IInDetTrackSelectionTool> m_trackSelTool;
  ToolHandle<Trk::IExtrapolator> m_trkextrapolator;

  const AtlasDetectorID* m_atlasid{};

  SG::ReadHandleKey<PixelRDO_Container> m_pixelRDOName{this, "RDOName", "PixelRDOs", "rdo data key"};
  SG::ReadHandleKey<InDet::PixelClusterContainer> m_clustersKey{this, "ClusterName", "PixelClusters", "pixel cluster data key" };
  SG::ReadHandleKey<xAOD::TrackParticleContainer> m_trackParticlesKey{this, "TrackParticleContainerName", "InDetTrackParticles", "track particle data key"};

  std::string m_calibFolder;
  std::map<std::string, std::unique_ptr<MVAUtils::BDT> > m_classBDT;

  bool m_dumpTree;
};
#endif
