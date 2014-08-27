/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __ITRIG_L2_DUP_TRACK_REMOVAL_TOOL_H__
#define __ITRIG_L2_DUP_TRACK_REMOVAL_TOOL_H__

#include<vector>
#include<algorithm>
#include<iterator>

#include "GaudiKernel/IAlgTool.h"
//#include "TrigInDetPattRecoEvent/TrigFTK_TrackSeed.h"
//#include "TrigSteeringEvent/Enums.h"

class TrigInDetTrackCollection;

static const InterfaceID IID_ITrigL2DupTrackRemovalTool("ITrigL2DupTrackRemovalTool",1,0);  
  
class ITrigL2DupTrackRemovalTool: virtual public IAlgTool { 
 public: 
  static const InterfaceID& interfaceID() { 
    return IID_ITrigL2DupTrackRemovalTool; 
  } 
  virtual void removeDupTracks(TrigInDetTrackCollection* trackColl, std::vector<unsigned int> placeHolder) = 0;

  //protected:

  //std::vector<float> m_zVertices;// vector of z-vertex coordinates for monitoring
};

#endif


 
