// emacs: this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
//
// filename: TrigIDSCAN.h
// 
// author: Malte Muller
//         mm@hep.ucl.ac.uk
//		 
// Description: calls the IdScan routines in order and controls general flow
// 
// date: 19/11/2002
// 
// -------------------------------
// ATLAS Collaboration
////////////////////////////////////////////////////////////////////////////////

#include <string>

#ifndef IDSCAN_TRIGIDSCAN_H
#define IDSCAN_TRIGIDSCAN_H

#include "GaudiKernel/ToolHandle.h"
#include "TrigInterfaces/FexAlgo.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"

class ISpacePointProvider;
class ITrigInDetTrackFitter;
class ITrigTRT_TrackExtensionTool;
class ITrigL2ResidualCalculator;
class TrigToTrkTrackTool;
class ITrigHitFilter;
class ITrigZFinder;
class IShifterTool;
class IRegSelSvc;
class IBeamCondSvc;
class EventID;

#ifdef IDSCANDUMP
class ITrigL2SpacePointTruthTool;
class IDScanAthenaEventHeader;
class TTree;
class TFile;
#endif

//#include "JiveXML/EventData2XML.h"

namespace TrigIdScan {
class RoI_Shape;
}


////////////////////////////////////////////////////////////////////////////////
////    TrigIDSCAN
////////////////////////////////////////////////////////////////////////////////

class TrigIDSCAN : public HLT::FexAlgo
{
 public:
  
  TrigIDSCAN(const std::string& name, ISvcLocator* pSvcLocator);
  ~TrigIDSCAN();
  
  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode hltBeginRun();

  HLT::ErrorCode hltExecute(const HLT::TriggerElement* inputTE,
			    HLT::TriggerElement* outputTE);

  using HLT::FexAlgo::prepareRobRequests;
  HLT::ErrorCode prepareRobRequests(const HLT::TriggerElement* inputTE);

 private:
  
  // everything else

  HLT::ErrorCode storeInfo(double zPosition, HLT::TriggerElement* outputTE);

  // fill the detector mask
  void setup_detector_mask(const EventID* eventId);
  BooleanProperty m_check_detector_mask;
  BooleanProperty m_detector_mask_on_event;

  bool m_detector_mask_not_checked;
  bool m_hasPixelBarrel, m_hasPixelDisk, m_hasPixelBLayer;
  bool m_hasSCTBarrelA, m_hasSCTEndcapA;
  bool m_hasSCTBarrelC, m_hasSCTEndcapC;

  std::string m_instanceName;

  std::string m_TrueVertexLocation;
  std::string m_pixelSpName,m_sctSpName;

  ToolHandle<ISpacePointProvider> m_SpacePointProvider;
  ToolHandle<ITrigInDetTrackFitter> m_TrigInDetTrackFitter;
  ToolHandle<ITrigTRT_TrackExtensionTool> m_TrigTRT_TrackExtensionTool;
  ToolHandle<ITrigL2ResidualCalculator> m_TrigL2ResidualCalculator;

  ToolHandle<ITrigZFinder>   m_zFinder;
  ToolHandle<TrigToTrkTrackTool> m_convertorTool;
  ToolHandle<ITrigHitFilter>  m_hitFilter;
  ToolHandle<IShifterTool>    m_shifterTool;
  
  std::string       m_regionSelectorName;
  IRegSelSvc*       m_regionSelector;

  bool m_useBeamSpot;
  IBeamCondSvc* m_iBeamCondSvc;

  //RoI info for selecting the right space points
  double m_etaHalfWidth;
  double m_phiHalfWidth;

  bool m_fullScanMode;

  TrigIdScan::RoI_Shape*  m_roiShape;

  int m_minHits;

  unsigned int m_maxNumTracks;

  long   m_zFinderMode;         // 0: run zfinder, 
                                // 1:use z0 from MC truth 
                                // 2:use z0=m_zVertexValue

  bool m_findMultipleZ;         //false: only take tracks from one vertex
                                //true: use tracks from all verticies


  bool  m_doTRTpropagation;

  bool  m_doShift;
  int   m_useShifterZ;  // 0: Do not use, 1: Overwrite zfinder, 2: zfinder+shifterZ
  
  double m_zVertexValue;
 
  long           m_trackConvertorMode; // 0: do not create Trk::Tracks 1: otherwise
  std::string    m_TrkTrackCollName;
  std::string    m_outputCollectionSuffix;

  bool m_printDiagnosticMessages;

  unsigned int m_l1Id;
  // Counters
  unsigned int m_countTotalEvents;
  unsigned int m_countTotalRoI;
  unsigned int m_countRoIwithEnoughHits;
  unsigned int m_countRoIzFinderFailed;
  unsigned int m_countRoIwithGroups[2];
  unsigned int m_countRoIwithTracks[2];

  std::vector<unsigned int> m_placeHolder;

  TrigTimer* m_SpacePointReaderTimer;
  TrigTimer* m_ZFinderTimer;
  TrigTimer* m_IdScanHitsTimer;
  TrigTimer* m_HitFilterTimer;
  TrigTimer* m_SctKFitterTimer;
  TrigTimer* m_inZFTimer;
  TrigTimer* m_inHFTimer;
  TrigTimer* m_IdScanTRTTimer;

  TrigInDetTrackCollection* m_recoTracks;

  //tracking quantities to be monitored
  float nStraw_TRT; 
  float nTR_TRT;
  int m_ntracks;
  int m_nPixSPsInRoI;  // Total number of (accepted) pixel SPs in the RoI
  int m_nSCTSPsInRoI;  // Total number of (accepted) SCT SPs in the RoI
  int m_currentStage;  // The last stage reached during the processing of a given RoI
  int m_nZvertices;
  double m_zPosition;  // This was turned into a class variable for monitoring
  double m_shift;      // Ditto.
  double m_roiPhi, m_roiEta;  // Ditto.

  std::vector<double> m_a0beam;
  std::vector<double> m_trkdPhi0, m_trkdEta;

  // monitor residuals
  bool m_doResMonitoring;
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

  //functions to retrieve monitored quantities
  static inline double monPt(const TrigInDetTrack *t){return t->param()->pT(); }
  static inline double monA0(const TrigInDetTrack *t){return t->param()->a0(); }
  static inline double monZ0(const TrigInDetTrack *t){return t->param()->z0(); }
  static inline double monPhi0(const TrigInDetTrack *t){return t->param()->phi0();}
  static inline double monEta(const TrigInDetTrack *t){return t->param()->eta(); }
  static inline double monNHit_Si(const TrigInDetTrack *t){return t->siSpacePoints()->size();}
  static inline double monChi2(const TrigInDetTrack *t){return (t->chi2()>9e7)?-9.9:t->chi2();}

  static inline bool isPixel(const long lyrno){
    return ( lyrno < OffsetBarrelSCT || ( lyrno >= OffsetEndcapPixels && lyrno < OffsetEndcapSCT ) );}
  static inline int monNPixSP(const TrigInDetTrack *t){
    int count=0;
    for(unsigned int n=0; n<t->siSpacePoints()->size(); n++)
      if ( isPixel( (*(t->siSpacePoints()))[n]->layer() ) ) count++; 
    return count;
  }
  static inline int monNSctSP(const TrigInDetTrack *t){
    int count=0;
    for(unsigned int n=0; n<t->siSpacePoints()->size(); n++)
      if ( ! isPixel( (*(t->siSpacePoints()))[n]->layer() ) ) count++; 
    return count;
  }

  // Mark's standalone starts here
  // the job options are always needed because of 
  // the python configurable
  bool                     mWriteFlag;
  std::string              mWriteName;
  bool                     mWriteTruth;
  std::vector<int> m_dataErrors;
  std::vector<int> m_pixelDataErrors;
  std::vector<int> m_sctDataErrors;
  std::vector<int> m_trtDataErrors;

#ifdef IDSCANDUMP
  ToolHandle<ITrigL2SpacePointTruthTool> m_TrigL2SpacePointTruthTool;
  IDScanAthenaEventHeader* mHeader;
  TTree*                   mTree;
  TFile*                   mFile;
  int                      mWriteCounter;
  // ends here
#endif

  bool   m_translateOnly;
  double m_xShift;
  double m_yShift;

  bool   m_adjustLayerThreshold;
  bool   m_generateRoI;

  bool   m_useRoiDescriptorWidths;
  double m_minZ, m_maxZ;

  std::string m_attachedFeatureName;

};

#endif
