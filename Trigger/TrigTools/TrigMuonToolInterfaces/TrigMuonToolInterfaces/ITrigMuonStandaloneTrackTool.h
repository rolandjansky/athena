/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONTOOLINTERFACES_ITRIGMUONSTANDALONETRACKTOOL_H
#define TRIGMUONTOOLINTERFACES_ITRIGMUONSTANDALONETRACKTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "TrigSteeringEvent/Enums.h"
#include "TrigMuonEvent/CachingFeatureCollection.h"
#include "MuonPattern/MuonPatternCombinationCollection.h"
#include "TrkSegment/SegmentCollection.h"
#include "TrkTrack/TrackCollection.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticleAuxContainer.h"
#include "MuonCombinedEvent/MuonCandidateCollection.h"

/**
   @file ITrigMuonStandaloneTrackTool.h
   @class ITrigMuonStandaloneTrackTool

   @brief Interface for TrigMuonSegmentFinderTool
   
   @author Robert Harrington <R.Harrington@ed.ac.uk>
   @date 10/25/2011
*/

static const InterfaceID
IID_TRIGMUONEFINTERFACES_ITrigMuonStandaloneTrackTool( "ITrigMuonStandaloneTrackTool",1,0);

namespace HLT {
  class TriggerElement;
  class Algo;
}

class TrigTimer;
class IRoiDescriptor;
class TrigMuonEFInfoContainer;
class TrigMuonEFMonVars;
class TrigMuonEFSegmentMonVars;
class TrigMuonEFMSMonVars;
class TrigMuonEFSAMonVars;

class ITrigMuonStandaloneTrackTool : virtual public IAlgTool {
  
 public:
  virtual ~ITrigMuonStandaloneTrackTool() {}
  
  static const InterfaceID& interfaceID();
  
  /** Run segment finding only */
  virtual HLT::ErrorCode getSegments(const IRoiDescriptor* muonRoI,
				     TrigMuonEFMonVars& monvars,
				     std::vector<TrigTimer*>& timers) = 0;

  /** Run segment finding and spectrometer track building */
  virtual HLT::ErrorCode getSpectrometerTracks(const IRoiDescriptor* muonRoI,
					       TrigMuonEFMonVars& monVars,
					       std::vector<TrigTimer*>& timers) = 0;

  /** Run segment finding, spectrometer track building and extrapolation */
  virtual HLT::ErrorCode getExtrapolatedTracks(const IRoiDescriptor* muonRoI,
					       MuonCandidateCollection& candidateCollection,
					       TrackCollection& extrapolatedTracks,
					       TrigMuonEFMonVars& monvars,
					       std::vector<TrigTimer*>& timers) = 0;
  
  /** return last created MuonPatternCombinationCollection. Object will be deleted by tool at the end of the event.
      NB: You can not attach this object to the TriggerElement */
  virtual const MuonPatternCombinationCollection* patternCombis()=0;

  /** return last created Trk::SegmentCollection. Object will be deleted by tool at the end of the event.
      NB: You can not attach this object to the TriggerElement */
  virtual const Trk::SegmentCollection* segments()=0;

  /** return last created TrackCollection with muon spectrometer tracks. Object will be deleted by tool at the end of the event.
      NB: You can not attach this object to the TriggerElement */
  virtual const TrackCollection* spectrometerTracks() = 0;

  /** return last created TrackCollection with extrapolated tracks. Object will be deleted by tool at the end of the event.
      NB: You can not attach this object to the TriggerElement */
  virtual const TrackCollection* extrapolatedTracks() = 0;


  /** return last created MuonPatternCombinationCollection. Caller is responsible for deletion of object.
      Call this function if you want to attach the object to the TriggerElement */
  virtual const MuonPatternCombinationCollection* patternCombisToAttach()=0;

  /** return last created Trk::SegmentCollection. Caller is responsible for deletion of object.
      Call this function if you want to attach the object to the TriggerElement */
  virtual const Trk::SegmentCollection* segmentsToAttach()=0;

  /** return last created TrackCollection with muon spectrometer tracks. Caller is responsible for deletion of object.
      Call this function if you want to attach the object to the TriggerElement */
  virtual const TrackCollection* spectrometerTracksToAttach() = 0;

  /** return last created TrackCollection with extrapolated tracks. Caller is responsible for deletion of object.
      Call this function if you want to attach the object to the TriggerElement */
  virtual const TrackCollection* extrapolatedTracksToAttach() = 0;

  /** return last created TrackParticleContainer with extrapolated tracks. Caller is responsible for deletion of object.
      Call this function if you want to attach the object to the TriggerElement */
  virtual const xAOD::TrackParticleContainer* trackParticleContainerToAttach()=0;
  virtual const xAOD::TrackParticleAuxContainer* trackParticleAuxContainerToAttach()=0;

  virtual void recordPatterns() = 0;
  virtual void recordSpectrometerTracks() = 0;

  /** Get list of hashIDs (used for internal caching) **/
  virtual std::vector<std::vector<IdentifierHash> > getHashList(const IRoiDescriptor* muonRoI)=0;

  /** Declares all segment monitoring histograms. returned monVars should be passed to getSegments() */
  virtual void declareSegmentMonitoringVariables(HLT::Algo* fexAlgo, 
						 TrigMuonEFMonVars& monVars) = 0;
  
  /** Declares all segment and spectrometer track histograms. returned monVars should be passed to getSpectrometerTracks() */
  virtual void declareSpectrometerMonitoringVariables(HLT::Algo* fexAlgo, 
						      TrigMuonEFMonVars& monVars) = 0;

  /** Declares all segment, spectrometer and extrapolated histograms. returned monVars should be passed to getExtrapolatedTracks() */
  virtual void declareExtrapolatedMonitoringVariables(HLT::Algo* fexAlgo, 
						      TrigMuonEFMonVars& monVars) = 0;
  
  /** return segment timers. timers to be passed to getSegments() */
  virtual void setSegmentTimers(HLT::Algo* fexAlgo, std::vector<TrigTimer*>& timers) = 0;

  /** return segment + spectrometer timers. timers should be passed to getSpectrometerTracks() */
  virtual void setSpectrometerTimers(HLT::Algo* fexAlgo, std::vector<TrigTimer*>& timers) = 0;

  /** return segment + spectrometer + extrapolated timers. timers should be passed to getExtrapolatedTracks() */
  virtual void setExtrapolatedTimers(HLT::Algo* fexAlgo, std::vector<TrigTimer*>& timers) = 0;

  /**Used for PrepareRobRequests in calling algorithm*/
  virtual const std::vector<uint32_t>& getMdtRobList(const IRoiDescriptor*) = 0;
  virtual const std::vector<uint32_t>& getRpcRobList(const IRoiDescriptor*) = 0;
  virtual const std::vector<uint32_t>& getCscRobList(const IRoiDescriptor*) = 0;
  virtual const std::vector<uint32_t>& getTgcRobList(const IRoiDescriptor*) = 0;
  virtual void clearRoiCache() = 0;

  /** return decoding modes - also used for prepareRobRequests */
  virtual bool useMdtSeededDecoding() = 0;
  virtual bool useRpcSeededDecoding() = 0;
  virtual bool useTgcSeededDecoding() = 0;
  virtual bool useCscSeededDecoding() = 0;

}; // end class definition

inline const InterfaceID& ITrigMuonStandaloneTrackTool::interfaceID() {
  return IID_TRIGMUONEFINTERFACES_ITrigMuonStandaloneTrackTool;
}
   
#endif // TRIGMUONTOOLINTERFACES_ITRIGMUONSTANDALONETRACKTOOL_H
