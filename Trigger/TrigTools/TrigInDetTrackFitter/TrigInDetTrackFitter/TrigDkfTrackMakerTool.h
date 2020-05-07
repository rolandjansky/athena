/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRIGDKFTRACKMAKERTOOL_H__
#define __TRIGDKFTRACKMAKERTOOL_H__

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigInDetToolInterfaces/ITrigDkfTrackMakerTool.h"

#include "StoreGate/ReadCondHandleKey.h"
#include "TrkDistributedKalmanFilter/TrkBaseNode.h"
#include "TrkDistributedKalmanFilter/TrkPlanarSurface.h"
#include "TrkTrack/Track.h"

#include <vector>

class AtlasDetectorID;
class PixelID;
class SCT_ID;

class TrigDkfTrackMakerTool : virtual public ITrigDkfTrackMakerTool, public AthAlgTool {
 public:
      
  // standard AlgTool methods
  TrigDkfTrackMakerTool(const std::string&,const std::string&,const IInterface*);
		
  // standard Athena methods
  StatusCode initialize();

	bool createDkfTrack(const Trk::Track& track, std::vector<Trk::TrkBaseNode*>& vpTrkNodes, double DChi2) const;

    
 private:
  const PixelID* m_pixelId;
  const SCT_ID* m_sctId;
  const AtlasDetectorID* m_idHelper;
  
};

#endif 
