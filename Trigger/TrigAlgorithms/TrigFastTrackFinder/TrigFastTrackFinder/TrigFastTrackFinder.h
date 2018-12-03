/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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


#include "TrkEventPrimitives/ParticleHypothesis.h"

#include "TrigInDetPattRecoTools/TrigCombinatorialSettings.h"

class ITrigL2LayerNumberTool;
class ITrigL2LayerSetPredictorTool;
class ITrigSpacePointConversionTool;
class ITrigL2SpacePointTruthTool;
class ITrigL2ResidualCalculator;
class ITrigInDetTrackFitter;
class ITrigZFinder;
class IRegSelSvc;
class TrigRoiDescriptor;
class TrigSiSpacePointBase;
class Identifier;
namespace InDet { 
  class SiSpacePointsSeed;
  class ISiTrackMaker; 
}

namespace Trk {
  class ITrackSummaryTool;
  class SpacePoint;
}

class IFTK_DataProviderSvc;

class TrigL2LayerSetLUT;
class TrigSpacePointStorage;
class TrigInDetTriplet;
class IBeamCondSvc;
//class EventID;
class PixelID;
class SCT_ID;
class AtlasDetectorID;

class TrigFastTrackFinder : public HLT::FexAlgo {

 public:
  
  TrigFastTrackFinder(const std::string& name, ISvcLocator* pSvcLocator);
  ~TrigFastTrackFinder();
  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode hltBeginRun();

  HLT::ErrorCode hltExecute(const HLT::TriggerElement* inputTE,
			    HLT::TriggerElement* outputTE);

  double trackQuality(const Trk::Track* Tr);
  void filterSharedTracks(std::vector<std::tuple<bool, double, Trk::Track*>>& QT);
  void convertToTrigInDetTrack(const TrackCollection& offlineTracks, TrigInDetTrackCollection& trigInDetTracks);

protected: 

  void updateClusterMap(long int, const Trk::Track*, std::map<Identifier, std::vector<long int> >&);
  void extractClusterIds(const Trk::SpacePoint*, std::vector<Identifier>&);
  bool usedByAnyTrack(const std::vector<Identifier>&, std::map<Identifier, std::vector<long int> >&);

  int findBarCodeInData(int, const std::vector<TrigSiSpacePointBase>&);
  void showBarCodeInData(int, const std::vector<TrigSiSpacePointBase>&);
  int findBarCodeInTriplets(int, const std::vector<std::shared_ptr<TrigInDetTriplet>>&);
  void assignTripletBarCodes(const std::vector<std::shared_ptr<TrigInDetTriplet>>&, std::vector<int>&);
  void assignTripletBarCodes(const std::vector<TrigInDetTriplet*>&, std::vector<int>&);
  
 private:

  // AlgTools and Services

  ToolHandle<ITrigL2LayerNumberTool> m_numberingTool;
  ToolHandle<ITrigSpacePointConversionTool> m_spacePointTool;
  ToolHandle<ITrigL2SpacePointTruthTool> m_TrigL2SpacePointTruthTool;
  ToolHandle<ITrigL2ResidualCalculator> m_trigL2ResidualCalculator;
  ToolHandle<InDet::ISiTrackMaker> m_trackMaker;   // Track maker 
  ToolHandle<ITrigInDetTrackFitter> m_trigInDetTrackFitter;
  ToolHandle<ITrigZFinder> m_trigZFinder;
  ToolHandle< Trk::ITrackSummaryTool > m_trackSummaryTool;
  ServiceHandle<IFTK_DataProviderSvc > m_ftkDataProviderSvc;
  std::string m_ftkDataProviderSvcName;

 
  double m_shift_x, m_shift_y;

  // Control flags

  bool m_doCloneRemoval;
  bool m_ftkMode;//If True: Retrieve FTK tracks
  bool m_ftkRefit;//If True: Refit FTK tracks
  bool m_useBeamSpot; 
  bool m_vertexSeededMode;
  bool m_doTrigInDetTrack;
  bool m_doZFinder;
   bool m_doFTKZFinder;
   bool m_doFTKFastVtxFinder;
  bool m_doFastZVseeding;
  bool m_doResMonitoring;

  // Cuts and settings
  TrigCombinatorialSettings m_tcs;

  int  m_minHits;

  int                     m_nSeeds;       //!< Number seeds 
  int                     m_nfreeCut;     // Min number free clusters 


  bool m_retrieveBarCodes;

  float m_tripletMinPtFrac;
  float m_pTmin;
  float m_initialD0Max;
  float m_Z0Max;                        
  bool m_checkSeedRedundancy;

  // Names of IDCs with input data 
  //
  //
  // Vertex (from beamspot)
  Amg::Vector3D m_vertex;

 
  // Reconstructed tracks 

  IBeamCondSvc* m_iBeamCondSvc;
  
  // Data members for monitoring

  int m_nTracks;
  int m_nPixSPsInRoI;  // Total number of (filtered) pixel SPs in the RoI
  int m_nSCTSPsInRoI;  // Total number of (filtered) SCT SPs in the RoI
  int m_currentStage;  // The last stage reached during the processing of a given RoI

  int m_roi_nSPs;

  double m_roiPhi, m_roiEta, m_roiZ;  
  double m_roiPhiWidth, m_roiEtaWidth, m_roiZ_Width;  
  double m_timePattReco;

  int m_nZvertices; 
  std::vector<float> m_zVertices; 
  std::vector<float> m_nTrk_zVtx; 

  std::vector<float> m_trk_pt;
  std::vector<float> m_trk_a0;
  std::vector<float> m_trk_z0;
  std::vector<float> m_trk_phi0;
  std::vector<float> m_trk_eta;
  std::vector<float> m_trk_chi2dof;
  std::vector<float> m_trk_nSiHits;
  std::vector<float> m_trk_nPIXHits;
  std::vector<float> m_trk_nSCTHits;
  std::vector<float> m_trk_a0beam;
  std::vector<float> m_trk_dPhi0;
  std::vector<float> m_trk_dEta;
  //std::vector<double> m_sp_x, m_sp_y, m_sp_z, m_sp_r;//Spacepoint coordinates
  //
  std::vector<int> m_IBL_layer;
  std::vector<int> m_PixB_layer;
  std::vector<int> m_PixE_layer;
  std::vector<int> m_SCTB_layer;
  std::vector<int> m_SCTE_layer;

  std::vector<double> m_iblResPhi;
  std::vector<double> m_iblResEta;
  std::vector<double> m_iblPullPhi;
  std::vector<double> m_iblPullEta;
  std::vector<double> m_pixResPhiBarrel;
  std::vector<double> m_pixResEtaBarrel;
  std::vector<double> m_pixPullPhiBarrel;
  std::vector<double> m_pixPullEtaBarrel;
  std::vector<double> m_sctResBarrel;
  std::vector<double> m_sctPullBarrel;
  std::vector<double> m_pixResPhiEC;
  std::vector<double> m_pixResEtaEC;
  std::vector<double> m_pixPullPhiEC;
  std::vector<double> m_pixPullEtaEC;
  std::vector<double> m_sctResEC;
  std::vector<double> m_sctPullEC;

  std::vector<double> m_pixResPhiBarrelL1;
  std::vector<double> m_pixResEtaBarrelL1;
  std::vector<double> m_pixResPhiBarrelL2;
  std::vector<double> m_pixResEtaBarrelL2;
  std::vector<double> m_pixResPhiBarrelL3;
  std::vector<double> m_pixResEtaBarrelL3;

  std::vector<double> m_pixResPhiECL1;
  std::vector<double> m_pixResEtaECL1;
  std::vector<double> m_pixResPhiECL2;
  std::vector<double> m_pixResEtaECL2;
  std::vector<double> m_pixResPhiECL3;
  std::vector<double> m_pixResEtaECL3;

  std::vector<double> m_sctResPhiBarrelL1;
  std::vector<double> m_sctResPhiBarrelL2;
  std::vector<double> m_sctResPhiBarrelL3;
  std::vector<double> m_sctResPhiBarrelL4;

  std::vector<double> m_sctResPhiEndcapL1;
  std::vector<double> m_sctResPhiEndcapL2;
  std::vector<double> m_sctResPhiEndcapL3;
  std::vector<double> m_sctResPhiEndcapL4;
  std::vector<double> m_sctResPhiEndcapL5;
  std::vector<double> m_sctResPhiEndcapL6;
  std::vector<double> m_sctResPhiEndcapL7;
  std::vector<double> m_sctResPhiEndcapL8;
  std::vector<double> m_sctResPhiEndcapL9;

  // Monitoring member functions 

  void fillMon(const TrackCollection& tracks);
  void runResidualMonitoring(const Trk::Track& track);

  void calculateRecoEfficiency(const std::vector<TrigSiSpacePointBase>&,
			       const std::map<int,int>&,
			       const std::map<int,int>&);


  //Setup functions
  void clearMembers();
  void getBeamSpot();
  HLT::ErrorCode getRoI(const HLT::TriggerElement* inputTE, const IRoiDescriptor*& roi);

  // Timers 

  TrigTimer* m_SpacePointConversionTimer;
  TrigTimer* m_ZFinderTimer;
  TrigTimer* m_PatternRecoTimer; 
  TrigTimer* m_TripletMakingTimer; 
  TrigTimer* m_CombTrackingTimer; 
  TrigTimer* m_TrackFitterTimer; 

  // Internal bookkeeping

  std::string m_instanceName, m_attachedFeatureName, m_attachedFeatureName_TIDT,
    m_outputCollectionSuffix;

  unsigned int m_countTotalRoI;
  unsigned int m_countRoIwithEnoughHits;
  unsigned int m_countRoIwithTracks;

  //efficiency calculations
  std::vector<int> m_vSignalBarCodes;

  int m_nSignalPresent;
  int m_nSignalDetected;
  int m_nSignalTracked;
  int m_nSignalClones;
  int m_minSignalSPs;

  const PixelID* m_pixelId;
  const SCT_ID* m_sctId;
  const AtlasDetectorID* m_idHelper;


  Trk::ParticleHypothesis m_particleHypothesis;//particle hypothesis to attach to each track - usually pion, can be set to other values

  std::map<Identifier, std::vector<long int> > m_siClusterMap;

  bool m_useNewLayerNumberScheme;

};

#endif // not TRIGFASTTRACKFINDER_TRIGFASTTRACKFINDER_H
