/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONTOOLINTERFACES_ITRIGMUONSEGMENTFINDERTOOL_H
#define TRIGMUONTOOLINTERFACES_ITRIGMUONSEGMENTFINDERTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "TrigSteeringEvent/Enums.h"
#include "TrigMuonEvent/CachingFeatureCollection.h"
#include "MuonSegment/MuonSegmentCombinationCollection.h"
#include "MuonPattern/MuonPatternCombinationCollection.h"
#include "TrkSegment/SegmentCollection.h"

/**
   @file ITrigMuonSegmentFinderTool.h
   @class ITrigMuonSegmentFinderTool

   @brief Interface for TrigMuonSegmentFinderTool
   
   @author Robert Harrington <R.Harrington@ed.ac.uk>
   @date 10/25/2011
*/

static const InterfaceID
IID_TRIGMUONEFINTERFACES_ITrigMuonSegmentFinderTool( "ITrigMuonSegmentFinderTool",1,0);

namespace HLT {
  class TriggerElement;
  class FexAlgo;
}

class TrigTimer;
class IRoiDescriptor;
class FexAlgo;
class TrigMuonEFInfoContainer;
class TrigMuonEFSegmentMonVars;

class ITrigMuonSegmentFinderTool : virtual public IAlgTool {
  
 public:
  virtual ~ITrigMuonSegmentFinderTool() {}
  
  static const InterfaceID& interfaceID();
  
  virtual StatusCode initialize() = 0;
  virtual StatusCode finalize() = 0;

  virtual HLT::ErrorCode getSegments(const IRoiDescriptor* muonRoI,
				     TrigMuonEFInfoContainer* muonEFInfoCont,
				     CachingFeatureCollection*& caching,
				     TrigMuonEFSegmentMonVars& monvars,
				     std::vector<TrigTimer*>& timers) = 0;

  virtual void segmentHandle() = 0;

  virtual void cleanCollections() = 0;

  virtual const MuonSegmentCombinationCollection* combiCol()=0;
  virtual const MuonPatternCombinationCollection* pattComb()=0;
  virtual const Trk::SegmentCollection* segments()=0;
  virtual       std::vector<std::vector<IdentifierHash> >* hashlist()=0;
   
  virtual void recordSegments() = 0;
  virtual void recordPatterns() = 0;

  virtual void declareSegmentMonitoringVariables(HLT::FexAlgo* fexAlgo, TrigMuonEFSegmentMonVars& monvars) = 0;

  virtual void addSegmentTimers(HLT::FexAlgo* fexAlgo, std::vector<TrigTimer*>& timers) = 0;

}; // end class definition

inline const InterfaceID& ITrigMuonSegmentFinderTool::interfaceID() {
  return IID_TRIGMUONEFINTERFACES_ITrigMuonSegmentFinderTool;
}
   
#endif // TRIGMUONTOOLINTERFACES_ITRIGMUONSEGMENTFINDERTOOL_H
