/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
//
// filename: TrigFastTrackFinder.h
// 
// Description: a part of L2+EF HLT ID tracking
// 
// date: 16/04/2013
// 
// -------------------------------
// ATLAS Collaboration
////////////////////////////////////////////////////////////////////////////////

#ifndef TRIGFASTTRACKFINDER_TRIGFASTTRACKFINDER_H
#define TRIGFASTTRACKFINDER_TRIGFASTTRACKFINDER_H

#include<string>
#include<vector>
#include<map>
//#include<algorithm>

#include "GaudiKernel/ToolHandle.h"
#include "TrigInterfaces/FexAlgo.h"
#include "BeamSpotConditionsData/BeamSpotData.h"

#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkEventUtils/PRDtoTrackMap.h"

#include "TrigInDetPattRecoTools/TrigCombinatorialSettings.h"

#include "AthenaMonitoringKernel/Monitored.h"

class ITrigL2LayerNumberTool;
class ITrigSpacePointConversionTool;
class ITrigInDetTrackFitter;
class ITrigZFinder;
class TrigRoiDescriptor;
class TrigSiSpacePointBase;
class TrigInDetTriplet;
class Identifier;

namespace InDet { 
  class ISiTrackMaker;
  class SiTrackMakerEventData_xk;
}

namespace Trk {
  class ITrackSummaryTool;
  class SpacePoint;
}

class PixelID;
class SCT_ID;
class AtlasDetectorID;

// for GPU acceleration

class ITrigInDetAccelerationTool;
class ITrigInDetAccelerationSvc;

class TrigFastTrackFinder : public HLT::FexAlgo {

 public:
  
  TrigFastTrackFinder(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~TrigFastTrackFinder();
  virtual HLT::ErrorCode hltInitialize() override;
  virtual HLT::ErrorCode hltFinalize() override;
  virtual HLT::ErrorCode hltStart() override;

  virtual StatusCode execute() override;
  virtual
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* inputTE,
			    HLT::TriggerElement* outputTE) override;

  StatusCode findTracks(InDet::SiTrackMakerEventData_xk &event_data,
                        const TrigRoiDescriptor& roi,
                        TrackCollection& outputTracks,
                        const EventContext& ctx) const;

  double trackQuality(const Trk::Track* Tr) const;
  void filterSharedTracks(std::vector<std::tuple<bool, double, Trk::Track*>>& QT) const;

  virtual bool isClonable() const override { return true; }
  virtual unsigned int cardinality() const override { return 0; }//Mark as re-entrant

protected: 

  void updateClusterMap(long int, const Trk::Track*, std::map<Identifier, std::vector<long int> >&) const;
  void extractClusterIds(const Trk::SpacePoint*, std::vector<Identifier>&) const;
  bool usedByAnyTrack(const std::vector<Identifier>&, std::map<Identifier, std::vector<long int> >&) const;

 private:

  // AlgTools and Services

  ToolHandle<ITrigL2LayerNumberTool> m_numberingTool;
  ToolHandle<ITrigSpacePointConversionTool> m_spacePointTool;
  ToolHandle<InDet::ISiTrackMaker> m_trackMaker;   // Track maker 
  ToolHandle<ITrigInDetTrackFitter> m_trigInDetTrackFitter;
  ToolHandle<ITrigZFinder> m_trigZFinder;
  ToolHandle< Trk::ITrackSummaryTool > m_trackSummaryTool;
  ToolHandle< GenericMonitoringTool > m_monTool { this, "MonTool", "", "Monitoring tool" };

  //for GPU acceleration
  ToolHandle<ITrigInDetAccelerationTool> m_accelTool;
  ServiceHandle<ITrigInDetAccelerationSvc>     m_accelSvc;

  //DataHandles
  SG::ReadHandleKey<TrigRoiDescriptorCollection> m_roiCollectionKey;
  SG::WriteHandleKey<TrackCollection> m_outputTracksKey;

  SG::ReadHandleKey<Trk::PRDtoTrackMap>       m_prdToTrackMap
     {this,"PRDtoTrackMap",""};
 
  // Control flags

  bool m_doCloneRemoval;
  bool m_useBeamSpot; 

  bool m_vertexSeededMode;

  bool m_doZFinder;
  bool m_doZFinderOnly;

  bool m_storeZFinderVertices;

  bool m_doFastZVseeding;

  bool m_doResMonitoring;

  // Cuts and settings
  TrigCombinatorialSettings m_tcs;

  int  m_minHits;

  int                     m_nfreeCut;     // Min number free clusters 


  float m_tripletMinPtFrac;
  float m_pTmin;
  float m_initialD0Max;
  float m_Z0Max;                        
  bool m_checkSeedRedundancy;

  SG::ReadCondHandleKey<InDet::BeamSpotData> m_beamSpotKey { this, "BeamSpotKey", "BeamSpotData", "SG key for beam spot" };

  // Monitoring member functions 

  void fillMon(const TrackCollection& tracks, const TrigVertexCollection& vertices, const TrigRoiDescriptor& roi, const EventContext& ctx) const;
  void runResidualMonitoring(const Trk::Track& track, const EventContext&) const;

  //Setup functions
  void getBeamSpot(float&, float&, const EventContext&) const;
  HLT::ErrorCode getRoI(const HLT::TriggerElement* inputTE, const IRoiDescriptor*& roi);

  // Internal bookkeeping

  std::string m_instanceName, m_attachedFeatureName, m_outputCollectionSuffix;

  ATLAS_THREAD_SAFE mutable unsigned int m_countTotalRoI;
  ATLAS_THREAD_SAFE mutable unsigned int m_countRoIwithEnoughHits;
  ATLAS_THREAD_SAFE mutable unsigned int m_countRoIwithTracks;

  const PixelID* m_pixelId;
  const SCT_ID* m_sctId;
  const AtlasDetectorID* m_idHelper;


  Trk::ParticleHypothesis m_particleHypothesis;//particle hypothesis to attach to each track - usually pion, can be set to other values

  bool m_useNewLayerNumberScheme;

  // GPU acceleration

  bool m_useGPU;
  
  void makeSeedsOnGPU(const TrigCombinatorialSettings&, const IRoiDescriptor*, const std::vector<TrigSiSpacePointBase>&, std::vector<TrigInDetTriplet>&) const;

};

#endif // not TRIGFASTTRACKFINDER_TRIGFASTTRACKFINDER_H
