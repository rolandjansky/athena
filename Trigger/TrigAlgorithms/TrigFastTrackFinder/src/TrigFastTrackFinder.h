/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
//
// filename: TrigFastTrackFinder.h
// 
// Description: a part of HLT ID tracking
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

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "BeamSpotConditionsData/BeamSpotData.h"

#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkEventUtils/PRDtoTrackMap.h"

#include "TrkToolInterfaces/ITrackSummaryTool.h"

#include "TrigInDetToolInterfaces/ITrigInDetTrackFitter.h"
#include "TrigInDetToolInterfaces/ITrigZFinder.h"


#include "InDetRecToolInterfaces/ISiTrackMaker.h" 
#include "SiSPSeededTrackFinderData/SiTrackMakerEventData_xk.h"
#include "TrigInDetPattRecoTools/TrigCombinatorialSettings.h"
#include "TrigInDetPattRecoTools/TrigTrackSeedGenerator.h"

#include "AthenaMonitoringKernel/Monitored.h"

#include "TrkTrack/TrackCollection.h"
#include "TrkTrack/Track.h"

#include "TrigInDetEvent/TrigVertexCollection.h"
#include "TrigInDetEvent/TrigSiSpacePointBase.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "xAODTrigger/TrigCompositeContainer.h"


#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/PixelID.h" 

#include "TrigInDetToolInterfaces/ITrigL2LayerNumberTool.h"
#include "TrigInDetToolInterfaces/ITrigSpacePointConversionTool.h"
#include "TrigInDetToolInterfaces/ITrigL2SpacePointTruthTool.h"
#include "TrigInDetToolInterfaces/TrigL2HitResidual.h"

#include "TrigInDetPattRecoEvent/TrigInDetTriplet.h"


//for UTT
#include "TrigT1Interfaces/RecJetRoI.h"

//for GPU acceleration

#include "TrigInDetAccelerationTool/ITrigInDetAccelerationTool.h"
#include "TrigInDetAccelerationService/ITrigInDetAccelerationSvc.h"
#include "TrigAccelEvent/TrigInDetAccelEDM.h"
#include "TrigAccelEvent/TrigInDetAccelCodes.h"

namespace InDet {
  class ExtendedSiTrackMakerEventData_xk : public InDet::SiTrackMakerEventData_xk
  {
  public:
    ExtendedSiTrackMakerEventData_xk(const SG::ReadHandleKey<Trk::PRDtoTrackMap> &key, const EventContext& ctx) { 
      if (!key.key().empty()) {
        m_prdToTrackMap = SG::ReadHandle<Trk::PRDtoTrackMap>(key, ctx);
        if (!m_prdToTrackMap.isValid()) {
          throw std::runtime_error(std::string("Failed to get PRD to track map:") + key.key());
        }
        setPRDtoTrackMap(m_prdToTrackMap.cptr());
      }
    }
  private:
    void dummy() {}
    SG::ReadHandle<Trk::PRDtoTrackMap> m_prdToTrackMap;
  };
}

class TrigFastTrackFinder : public AthReentrantAlgorithm {

 public:
  
  TrigFastTrackFinder(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~TrigFastTrackFinder();
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  virtual StatusCode start() override;

  virtual StatusCode execute(const EventContext& ctx) const override;

  StatusCode findTracks(InDet::SiTrackMakerEventData_xk &event_data,
                        const TrigRoiDescriptor& roi,
                        const TrackCollection* inputTracks,
                        TrackCollection& outputTracks,
                        const EventContext& ctx) const;

  double trackQuality(const Trk::Track* Tr) const;
  void filterSharedTracks(std::vector<std::tuple<bool, double, Trk::Track*>>& QT) const;

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
  SG::ReadHandleKey<TrackCollection> m_inputTracksKey;
  SG::WriteHandleKey<TrackCollection> m_outputTracksKey;

  SG::ReadHandleKey<Trk::PRDtoTrackMap>       m_prdToTrackMap
     {this,"PRDtoTrackMap",""};
 
  // DataHandles for UTT
  SG::ReadHandleKey<DataVector<LVL1::RecJetRoI>> m_recJetRoiCollectionKey {this, "RecJetRoI", "", ""};
  SG::WriteHandleKey<xAOD::TrigCompositeContainer> m_hitDVSeedKey{this, "HitDVSeed", "", ""};
  SG::WriteHandleKey<xAOD::TrigCompositeContainer> m_hitDVTrkKey{this, "HitDVTrk", "", ""};
  SG::WriteHandleKey<xAOD::TrigCompositeContainer> m_hitDVSPKey{this, "HitDVSP", "", ""};

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

  // Internal bookkeeping


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

  // read input tracks and remove previously-used clusters from seed-making

  // Large Radius Tracking
  bool m_LRTmode;
  
  std::string m_trigseedML_LUT;//ML-based track seeding LUT name

  // L1 J seeded hit-based displaced vertex
  bool m_doJseedHitDV;
  StatusCode findJseedHitDV(const EventContext&, const std::vector<TrigSiSpacePointBase>&, const TrackCollection&) const;
  StatusCode calcdEdx(const EventContext&, const TrackCollection&) const;
  float deltaR(float, float, float, float) const;
};


#endif // not TRIGFASTTRACKFINDER_TRIGFASTTRACKFINDER_H
