/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONTOOLINTERFACES_ITRIGMUONCOMBINERTOOL_H
#define TRIGMUONTOOLINTERFACES_ITRIGMUONCOMBINERTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "TrigSteeringEvent/Enums.h"
#include "TrigMuonEvent/CachingFeatureCollection.h"
#include "MuonSegment/MuonSegmentCombinationCollection.h"
#include "TrkTrack/TrackCollection.h"
#include "Particle/TrackParticleContainer.h"
#include "DataModel/ElementLinkVector.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTracking/TrackParticleContainer.h"

/**
   @file ITrigMuonCombinerTool.h
   @class ITrigMuonCombinerTool

   @brief Interface for TrigMuonCombinerTool
   
   @author Robert Harrington <R.Harrington@ed.ac.uk>
   @date 10/25/2011
*/

static const InterfaceID
IID_TRIGMUONEFINTERFACES_ITrigMuonCombinerTool( "ITrigMuonCombinerTool",1,0);

namespace HLT {
  class TriggerElement;
  class FexAlgo;
}

class TrigTimer;
class TrigRoiDescriptor;
class FexAlgo;
class TrigMuonEFTrack;
class TrigMuonEFInfoContainer;
class MuidTrackContainer;
class TrigMuonEFCBMonVars;


class ITrigMuonCombinerTool : virtual public IAlgTool {
  
 public:
  virtual ~ITrigMuonCombinerTool() {}
  
  static const InterfaceID& interfaceID();
  
  virtual StatusCode initialize() = 0;
  virtual StatusCode finalize() = 0;

  virtual void addTimers(HLT::FexAlgo* fexAlgo,
			 std::vector<TrigTimer*>& timers) = 0;

  virtual HLT::ErrorCode buildTracks(const TrackCollection* msTracks,
				     const TrackCollection* saTracks,
				     const TrackCollection* idTracks,
				     const TrigMuonEFInfoContainer* myMuonEFInfoCont,
				     xAOD::MuonContainer& muonContainer,
				     xAOD::TrackParticleContainer* extrapolatedTrackParticles,
				     const ElementLinkVector<Rec::TrackParticleContainer>& idTrackParticleLinks,
				     xAOD::TrackParticleContainer& combinedTrackParticles,
				     TrigMuonEFCBMonVars& monVars,
				     std::vector<TrigTimer*>& timers) = 0;


  virtual void handle(const Incident& inc) = 0;

  virtual void declareMonitoringVariables(HLT::FexAlgo* fexAlgo, TrigMuonEFCBMonVars& monVars) = 0;

  /// Function to be used by clients to retrieve TrackCollection for combined tracks
  virtual const TrackCollection* combinedTracksToAttach() = 0;

  /// Function to be used by clients to retrieve TrackCollection for extrapolated tracks
  virtual const TrackCollection* extrapolatedTracksToAttach() = 0;

 protected:
  HLT::FexAlgo* m_fexAlgo;

}; // end class definition

inline const InterfaceID& ITrigMuonCombinerTool::interfaceID() {
  return IID_TRIGMUONEFINTERFACES_ITrigMuonCombinerTool;
}
   
#endif // TRIGMUONTOOLINTERFACES_ITRIGMUONCOMBINERTOOL_H
