/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONTOOLINTERFACES_ITRIGMUONEXTRAPOLATORTOOL_H
#define TRIGMUONTOOLINTERFACES_ITRIGMUONEXTRAPOLATORTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "TrigSteeringEvent/Enums.h"
#include "TrigMuonEvent/CachingFeatureCollection.h"
#include "MuonSegment/MuonSegmentCombinationCollection.h"
#include "TrkTrack/TrackCollection.h"

/**
   @file ITrigMuonExtrapolatorTool.h
   @class ITrigMuonExtrapolatorTool

   @brief Interface for TrigMuonExtrapolatorTool
   
   @author Robert Harrington <R.Harrington@ed.ac.uk>
   @date 10/25/2011
*/

static const InterfaceID
IID_TRIGMUONEFINTERFACES_ITrigMuonExtrapolatorTool( "ITrigMuonExtrapolatorTool",1,0);

namespace HLT {
  class TriggerElement;
  class FexAlgo;
}

class TrigTimer;
class TrigRoiDescriptor;
class FexAlgo;
class TrigMuonEFTrack;
class TrigMuonEFInfoContainer;
class TrigMuonEFMSMonVars;

class ITrigMuonExtrapolatorTool : virtual public IAlgTool {
  
 public:
  virtual ~ITrigMuonExtrapolatorTool() {}
  
  static const InterfaceID& interfaceID();
  
  virtual StatusCode initialize() = 0;
  virtual StatusCode finalize() = 0;

  virtual void addSATimers(HLT::FexAlgo* fexAlgo, std::vector<TrigTimer*>& timers) = 0;
  
  virtual HLT::ErrorCode extrapolate(const TrackCollection* tracks, 
				     const TrigMuonEFInfoContainer* myMuonEFInfoCont,
				     const TrackCollection*& outputTracks,
				     TrigMuonEFMSMonVars& monvars,
				     std::vector<TrigTimer*>& timers) = 0;


  virtual void saHandle() = 0;

  virtual void declareSAMonitoringVariables(HLT::FexAlgo* fexAlgo, TrigMuonEFMSMonVars& monvars) = 0;

}; // end class definition

inline const InterfaceID& ITrigMuonExtrapolatorTool::interfaceID() {
  return IID_TRIGMUONEFINTERFACES_ITrigMuonExtrapolatorTool;
}
   
#endif // TRIGMUONTOOLINTERFACES_ITRIGMUONEXTRAPOLATORTOOL_H
