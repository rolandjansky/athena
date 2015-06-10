/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRIG_L2_PATT_RECO_STRATEGY_B_H__
#define __TRIG_L2_PATT_RECO_STRATEGY_B_H__

#include "GaudiKernel/IAlgTool.h"
#include "TrigSteeringEvent/Enums.h"
#include "TrigInDetToolInterfaces/ITrigL2LayerNumberTool.h"
#include "TrigInDetRecoTools/ITrigL2PattRecoStrategy.h"
#include "TrigInDetRecoTools/StrBTrack.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrigInDetEvent/TrigVertexCollection.h"

class TrigTimer;
class TrigSiSpacePoint;
class IRoiDescriptor;
class IRegSelSvc;


  
class TrigL2PattRecoStrategyB: public AthAlgTool, virtual public ITrigL2PattRecoStrategy { 
  
 public: 
  TrigL2PattRecoStrategyB(const std::string&, const std::string&, const IInterface* );
  virtual ~TrigL2PattRecoStrategyB(); 
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  HLT::ErrorCode findTracks(const std::vector<const TrigSiSpacePoint*>&, const IRoiDescriptor&, TrigInDetTrackCollection*);
  HLT::ErrorCode findTracks(const std::vector<const TrigSiSpacePoint*>&, TrigInDetTrackCollection*);
  HLT::ErrorCode findTracks(const std::vector<const TrigSiSpacePoint*>&, const IRoiDescriptor&, const TrigVertexCollection*, TrigInDetTrackCollection*);
  HLT::ErrorCode findTracks(const std::vector<const TrigSiSpacePoint*>&, const TrigVertexCollection*, TrigInDetTrackCollection*);

  inline double getTimingA() const {
    return m_pattRecoTiming;
  }

 private:

  double m_pattRecoTiming;

  ServiceHandle<IRegSelSvc> m_regionSelector;

  void executeSeedRoads(float, float, float, float);
  void executeSeedDataSorting(const std::vector<const TrigSiSpacePoint*>&);
  HLT::ErrorCode executeSeedMaking();
  HLT::ErrorCode executeSeedMakingVertex(std::vector<float>&);
  void executeSeedFilter_Vertex();

  void executeTripletDataSorting(const std::vector<const TrigSiSpacePoint*>&);
  HLT::ErrorCode executeTripletMaking();
  void executeTripletAmbiguity();

  void executeTrackAmbiguity();
  void executeTrackMaking(TrigInDetTrackCollection*&);

  void executeSetup();
  void executeCleanup();

  /************************************/
  /** TRIPLET UTILITIES **************/
  /************************************/
  float tripletAmbiguityDistance(StrBTriplet* t1, StrBTriplet* t2);
  float tripletAmbiguityDistance(StrBTrack* t1, StrBTrack* t2);
  void  tripletMergeAmbiguity(StrBTrack* t1, StrBTrack* t2);


  /************************************/
  /** TRACK UTILITIES *****************/
  /************************************/
  void  trackFit(StrBTrack* t);
  float trackAmbiguityFraction(StrBTrack* t1, StrBTrack* t2);
  float trackAmbiguityDistance(StrBTrack* t1, StrBTrack* t2);
  void  trackMergeAmbiguity(StrBTrack* t1, StrBTrack* t2);
  void  trackRemoveAmbiguity(StrBTrack* t1, StrBTrack* t2);
  void  trackAmbiguityShared(StrBTrack* t1, StrBTrack* t2);
  bool  trackSort(StrBTrack* t1, StrBTrack* t2);
  bool  patternSort(StrBPattern* t1, StrBPattern* t2);
  
  /************************************/
  /** FITTING UTILITIES ***************/
  /************************************/
  bool fit(TrigInDetTrack* t);
  void circlefitRPhi(TrigInDetTrack* t, float &xc, float &yc, float &rc, float &phic, float &q, float &pt);
  void fitRZ(TrigInDetTrack* t, float &xc, float &yc, float &rc, float &phic, float &q, float &pt);
  bool fitRPhi(TrigInDetTrack* t, float &xc, float &yc);
  void shiftRPhi(TrigInDetTrack* t);
 
  float m_roiEta;
  float m_roiPhi;

  float roiPhiMin, roiPhiMax, m_phiHalfWidth;
  float roiEtaMin, roiEtaMax, m_etaHalfWidth;

  /************************************/
  /** LAYERS **************************/
  /************************************/
  float *m_geoR, *m_geoRmin, *m_geoRmax, *m_geoZ, *m_geoZmin, *m_geoZmax;
  
  std::vector< std::list<int> > m_layerGroups;

  std::set<int> m_mapSetLayersFailure;
  std::set<int> m_mapSetLayersRemove;
  std::set<int> m_mapSetPixelModulesNoisy;
  std::set<int> m_mapSetSCTModulesNoisy;
  
  std::vector< std::vector<const TrigSiSpacePoint*> > m_spSeed;
  std::vector<const TrigSiSpacePoint*>* m_spTriplet;
  unsigned int m_spTripletZ0Steps;
  unsigned int m_spTripletEtaSteps;

  bool m_detector_mask_not_checked;
  bool m_hasPixelBarrel, m_hasPixelDisk, m_hasPixelBLayer;
  bool m_hasSCTBarrelA, m_hasSCTEndcapA;
  bool m_hasSCTBarrelC, m_hasSCTEndcapC;

  int m_laySctB;
  int m_layPixEC;
  int m_laySctEC;
  int m_layMax;


  /************************************/
  /** VERTEXING ***********************/
  /************************************/
  std::vector<float> m_preZv;
  
  int m_vtxMaxBin;
  int* m_vtxHisto;
  std::vector<float> m_recZv;
  
  TrigVertexCollection* m_vtxCollection;

  /************************************/
  /** TRACKS **************************/
  /************************************/  
  static StrBTrk* m_trks;
  unsigned int m_trks_num;
  unsigned int m_trks_num_active;

  static StrBTrack* m_full_trks;
  unsigned int m_full_trks_num;
  unsigned int m_full_trks_num_active;

  /************************************/
  /** EVENT ***************************/
  /************************************/
  float m_xc;
  float m_yc;
  
  std::vector<StrBRoad*> m_roads;
  typedef std::vector<StrBRoad*>::iterator StrBRoadIterator;
  
  /************************************/
  /** HISTOGRAMS **********************/
  /************************************/
  std::vector<int> m_dataErrors;
  std::vector<int> m_pixelDataErrors;
  std::vector<int> m_sctDataErrors;
  std::vector<int> m_trtDataErrors;

  /************************************/
  /** COMMON TOOLS ********************/
  /************************************/
  ToolHandle<ITrigL2LayerNumberTool> m_numberingTool;

  /************************************/
  /** PROPERTIES **********************/
  /************************************/
  int                m_mapLayerOneMultiplicity;
  int                m_mapLayerTwoMultiplicity;
  std::vector<int>   m_mapForcedInefficientLayers;
  std::vector<int>   m_mapForcedTurnedOffLayers;
  float              m_mapMaxLayerFailureFraction;
  float              m_mapMaxTotalFailureFraction;

  bool               m_newCuts;
  bool               m_newCutsRZ;

  float              m_recoPtMinSeedDiff;
  float              m_recoD0MaxSeedDiff;
  float              m_recoPtMinTripletDiff;
  float              m_recoD0MaxTripletDiff;
  float              m_recoPtMin;
  float              m_recoD0Max;
  float              m_recoTimingFactor;
  float              m_recoAlignmentScaling;
  bool               m_recoOutlierRemoval;
  float              m_recoOutlierRemovalCut;
  int                m_recoOutlierRemovalLayers;
  float              m_recoOutlierRemovalD0;
  float              m_recoOutlierRemovalPt;
  int                m_recoMaxSeeds;
  int                m_recoMaxTracks;
  float              m_recoZ0Bin;
  float              m_recoEtaBin;
  
  bool               m_preVertexingEnable;
  std::string        m_preVertexingVtxName;
  float              m_preVertexingZCut;
  
  float              m_seedingPreCutRZ;
  float              m_seedingPreCutPhi;
  float              m_seedingPreCutRPhi;
  float              m_seedingCutRZ;
  
  bool               m_vertexingEnable;
  float              m_vertexingCutRZ;
  float              m_vertexingCutRPhi;
  int                m_vertexingMaxVertices;
  float              m_vertexingWindowSize;
  int                m_vertexingWindowSegments;

  float              m_extensionPreCutRZ;
  float              m_extensionPreCutPhi;
  float              m_extensionPreCutRPhi;
  float              m_extensionPreCutRPhiDiff;
  float              m_extensionCutRZPixel;
  float              m_extensionCutRZ;
  float              m_extensionCutRPhi;
  int                m_extensionCutSpacePoints;
  int                m_extensionCutSpacePointsMiss1;
  int                m_extensionCutSpacePointsMiss2;
  bool               m_extensionSolveTriplets;
  float              m_extensionSolveTripletsDRCut;
  float              m_extensionSolveTripletsPhiScale;
  float              m_extensionSolveTripletsEtaScale;
  bool               m_extensionSolveTracks;
  bool               m_extensionSolveV0Tracks;
  float              m_extensionSolveTracksDRCut;
  float              m_extensionSolveTracksPhiScale;
  float              m_extensionSolveTracksEtaScale;
  bool               m_extensionSolveShared;
  int                m_extensionMaxMissingLayers;
  bool               m_extensionRefit;
  int                m_extensionSpacePoints;
  int                m_extensionSpacePointsMiss1;
  int                m_extensionSpacePointsMiss2;
  bool               m_extensionTRTExtension;

  int                m_dynamic_extensionCutSpacePoints;
  int                m_dynamic_extensionSpacePoints;

  bool               m_debugTuning;
  bool               m_debugTuningGoodSeeds;
  bool               m_debugTuningBadSeeds;
  bool               m_debugTuningGoodTriplets;
  bool               m_debugTuningBadTriplets;
  
  bool               m_outputTrkEnable;
  std::string        m_outputTrkName;
  bool               m_outputHistoEnable;
  std::string        m_outputHistoDir;
  int                m_outputHistoName;
  
  // dinamically modified properties
  float              m_dynamicalSeedingPreCutRZ;
  float              m_dynamicalSeedingPreCutPhi;
  float              m_dynamicalSeedingPreCutRPhi;
  float              m_dynamicalSeedingCutRZ;
  float              m_dynamicalExtensionPreCutRZ;
  float              m_dynamicalExtensionPreCutPhi;
  float              m_dynamicalExtensionPreCutRPhi;
  float              m_dynamicalExtensionPreCutRPhiDiff;
  float              m_dynamicalExtensionCutRZPixel;
  float              m_dynamicalExtensionCutRZ;
  float              m_dynamicalExtensionCutRPhi;
  
  //tracking quantities to be monitored
  int m_nSP;

  int m_nTracks;
  int m_nPixelSP;
  int m_nSctSP;
  int m_lastStep;
  int m_nSeeds;
  int m_nExtended;
  std::string m_attachedFeatureName;
  std::string m_outputCollectionSuffix;
  
  std::vector<double> m_a0beam;
  std::vector<double> m_trkdPhi0, m_trkdEta;
  
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

  //timers
  bool m_timers;
  TrigTimer* m_timer[10];
};



#endif


 
