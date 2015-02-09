/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRIG_L2_DUP_TRACK_REMOVAL_TOOL_H__
#define __TRIG_L2_DUP_TRACK_REMOVAL_TOOL_H__

#include<vector>

#include "GaudiKernel/IAlgTool.h"
//#include "TrigSteeringEvent/Enums.h"
#include "TrigInDetRecoTools/ITrigL2DupTrackRemovalTool.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

class TrigInDetTrackCollection;
/*
class TrigSiSpacePoint;

class SpacePointCollection;  
class SpacePointContainer;  

class SCT_ID;
class PixelID;

#include "TrigInDetEvent/TrigSiSpacePointCollection.h"
#include "TrkTrack/TrackCollection.h" 
*/

/*class TrigTimer;
class StoreGateSvc;
class IRegSelSvc;
class ITrigZFinder;
class IBeamCondSvc;
class ITrigHitFilter;*/

//#include "EventInfo/EventID.h"

  
class TrigL2DupTrackRemovalTool: public AthAlgTool, virtual public ITrigL2DupTrackRemovalTool { 
  
 public: 
  TrigL2DupTrackRemovalTool(const std::string&, const std::string&, const IInterface* );
  virtual ~TrigL2DupTrackRemovalTool(); 
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  void removeDupTracks(TrigInDetTrackCollection* trackColl, std::vector<unsigned int> placeHolder);

  private:
  int m_mode; // mode 1: use all tracks & DR criterion
              // mode 2: use tracks from different z vertices & SP criterion -- default

};



#endif


 
