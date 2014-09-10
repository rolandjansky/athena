/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRIGDKFTRACKMAKERTOOL_H__
#define __TRIGDKFTRACKMAKERTOOL_H__

#include "AthenaBaseComps/AthAlgTool.h"

#include "TrkDistributedKalmanFilter/TrkBaseNode.h"
#include "TrkDistributedKalmanFilter/TrkPlanarSurface.h"

#include "TrigInDetToolInterfaces/ITrigDkfTrackMakerTool.h"
#include <vector>

#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/PixelID.h" 
#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "InDetReadoutGeometry/SCT_DetectorManager.h"
#include "TrkTrack/Track.h"


class AtlasDetectorID;
class PixelID;
class SCT_ID;

class TrigDkfTrackMakerTool : virtual public ITrigDkfTrackMakerTool, public AthAlgTool {
 public:
      
  // standard AlgTool methods
  TrigDkfTrackMakerTool(const std::string&,const std::string&,const IInterface*);
  virtual ~TrigDkfTrackMakerTool();
		
  // standard Athena methods
  StatusCode initialize();
  StatusCode finalize();

  bool createDkfTrack(std::vector<const TrigSiSpacePoint*>&, std::vector<Trk::TrkBaseNode*>&, double);
	bool createDkfTrack(const Trk::Track& track, std::vector<Trk::TrkBaseNode*>& vpTrkNodes, double DChi2);

    
 private:
  const PixelID* m_pixelId;
  const SCT_ID* m_sctId;
  const AtlasDetectorID* m_idHelper;
  
  const InDetDD::PixelDetectorManager* m_pixelManager;
  const InDetDD::SCT_DetectorManager* m_SCT_Manager;

};

#endif 
