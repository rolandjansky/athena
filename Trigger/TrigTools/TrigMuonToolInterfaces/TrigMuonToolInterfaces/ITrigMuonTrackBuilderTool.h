/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONTOOLINTERFACES_ITRIGMUONTRACKBUILDERTOOL_H
#define TRIGMUONTOOLINTERFACES_ITRIGMUONTRACKBUILDERTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "TrigSteeringEvent/Enums.h"
#include "TrigMuonEvent/CachingFeatureCollection.h"
#include "MuonSegment/MuonSegmentCombinationCollection.h"
#include "TrkTrack/TrackCollection.h"

#include "TrigMuonToolInterfaces/TrigMuonEFMonVars.h"

/**
   @file ITrigMuonTrackBuilderTool.h
   @class ITrigMuonTrackBuilderTool

   @brief Interface for TrigMuonTrackBuilderTool
   
   @author Robert Harrington <R.Harrington@ed.ac.uk>
   @date 10/25/2011
*/

static const InterfaceID
IID_TRIGMUONEFINTERFACES_ITrigMuonTrackBuilderTool( "ITrigMuonTrackBuilderTool",1,0);

namespace HLT {
  class TriggerElement;
  class FexAlgo;
}

class TrigTimer;
class TrigRoiDescriptor;
class FexAlgo;
class TrigMuonEFTrack;
class TrigMuonEFInfoContainer;

class ITrigMuonTrackBuilderTool : virtual public IAlgTool {
  
 public:
  virtual ~ITrigMuonTrackBuilderTool() {}
  
  static const InterfaceID& interfaceID();
  
  virtual StatusCode initialize() = 0;
  virtual StatusCode finalize() = 0;

  virtual void addMSTimers(HLT::FexAlgo* fexAlgo, 
			   std::vector<TrigTimer*>& timers) = 0;

  virtual HLT::ErrorCode buildTracks(const MuonSegmentCombinationCollection* segments,
				     const CachingFeatureCollection* caching,
				     const TrigMuonEFInfoContainer* myMuonEFInfoCont,
				     const TrackCollection*& outputTracks,
				     TrigMuonEFMSMonVars& monVars,
				     std::vector<TrigTimer*>& timers) = 0;


  virtual void recordMSTracks(const TrackCollection* tracks) = 0;

  virtual void msHandle() = 0;

  virtual void declareMSMonitoringVariables(HLT::FexAlgo* fexAlgo, 
					    TrigMuonEFMSMonVars& monVars) = 0;


}; // end class definition

inline const InterfaceID& ITrigMuonTrackBuilderTool::interfaceID() {
  return IID_TRIGMUONEFINTERFACES_ITrigMuonTrackBuilderTool;
}
   
#endif // TRIGMUONTOOLINTERFACES_ITRIGMUONTRACKBUILDERTOOL_H
