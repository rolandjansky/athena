/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONTOOLINTERFACES_ITRIGMUGIRLTOOL_H
#define TRIGMUONTOOLINTERFACES_ITRIGMUGIRLTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "TrigSteeringEvent/Enums.h"
#include "TrigMuonEvent/CachingFeatureCollection.h"
#include "TrigMuonEvent/CombinedMuonFeatureContainer.h"
#include "MuonSegment/MuonSegmentCombinationCollection.h"
#include "MuonPattern/MuonPatternCombinationCollection.h"
#include "TrkSegment/SegmentCollection.h"
#include "Particle/TrackParticleContainer.h"
#include "DataModel/ElementLinkVector.h"

#include "xAODTracking/TrackParticleContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

static const InterfaceID
IID_TRIGMUONEFINTERFACES_ITrigMuGirlTool( "ITrigMuGirlTool",1,0);

namespace HLT {
  class TriggerElement;
  class FexAlgo;
}

class TrigTimer;
class TrigRoiDescriptor;
class FexAlgo;
class TrigMuonEFInfoContainer;
class TrigMuGirlMonVars;
class MuonFeature;

class CaloClusterContainer;

class ITrigMuGirlTool : virtual public IAlgTool {
  
 public:
  virtual ~ITrigMuGirlTool() {}
  
  static const InterfaceID& interfaceID();
  
  virtual StatusCode initialize() = 0;
  virtual StatusCode finalize() = 0;
  
  virtual void declareMonitoringVariables(HLT::FexAlgo* fexAlgo, TrigMuGirlMonVars& monVars) = 0;

  virtual void addTimers(HLT::FexAlgo* fexAlgo, std::vector<TrigTimer*>& timers) = 0;

  virtual HLT::ErrorCode initNtuple(HLT::FexAlgo* fexAlgo) = 0;

  virtual HLT::ErrorCode initRoiProcessing(std::vector<TrigTimer*>& timers,
					   TrigMuonEFInfoContainer* eMuonEFInfoCont) = 0;

  virtual HLT::ErrorCode processMuonFeatures(const std::vector<const MuonFeature*>& vectorOfMuonFeature,
					     const ElementLink<CombinedMuonFeatureContainer>& CombinedMuonFeatureEL) = 0;

  virtual HLT::ErrorCode processTrackCollections(const ElementLinkVector<xAOD::TrackParticleContainer>& idTrackParticleLinks,
						 const std::vector<const xAOD::CaloClusterContainer*>& vectorOfClusterContainers) = 0;
 
  virtual HLT::ErrorCode finalizeRoiProcessing(std::vector<TrigTimer*>& timers,
					       TrigMuonEFInfoContainer* eMuonEFInfoCont,
					       Rec::TrackParticleContainer*& eTrkParCont,
					       TrigMuGirlMonVars& monVars) = 0;
  

}; // end class definition

inline const InterfaceID& ITrigMuGirlTool::interfaceID() {
  return IID_TRIGMUONEFINTERFACES_ITrigMuGirlTool;
}
   
#endif // TRIGMUONTOOLINTERFACES_ITRIGMUGIRLTOOL_H
