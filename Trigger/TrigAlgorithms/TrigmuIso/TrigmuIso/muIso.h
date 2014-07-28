/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
// 
// NAME:     muIso.h
// PACKAGE:  Trigger/TrigAlgorithms/TrigmuIso
// 
// AUTHOR:   S. Giagu <stefano.giagu@cern.ch>
// 
// PURPOSE:  LVL2 Muon Isolation FEX Algorithm: V4.0
// ********************************************************************

#ifndef TRIGMUISO_MUISO_H 
#define TRIGMUISO_MUISO_H

#include <string>
#include "TrigInterfaces/FexAlgo.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/DataHandle.h"

#include "CaloIdentifier/CaloIdManager.h"
#include "CaloInterface/ICalorimeterNoiseTool.h"

#include "TrigMuonBackExtrapolator/ITrigMuonBackExtrapolator.h"

#include "TileByteStream/TileCellCont.h"
#include "TrigT2CaloCommon/LArCellCont.h"

#include "Identifier/IdentifierHash.h"
#include "LArRawUtils/LArTT_Selector.h"

#include "LArRecEvent/LArCellCollection.h"
#include "TileEvent/TileCell.h"

#include "TrigT2CaloCommon/T2GeometryTool.h"
#include "TrigT2CaloCommon/T2Calibration.h"
#include "TrigT2CaloCommon/TrigDataAccess.h"
//#include "CaloEvent/CaloSampling.h"
#include "CaloGeoHelpers/CaloSampling.h" 

#include "TrigInDetEvent/TrigInDetTrack.h"

#include "EventInfo/EventID.h"

class StoreGateSvc;
class IsoMuonFeature;
class TrigTimer;

/** Main LVL2 Algorithm. Sided by a MuonFeature/CombinedMuonFeature, decode 
    calorimetric info around muon direction and fill a IsoMuonFeature. */
class muIso: public HLT::FexAlgo {
 enum { MaxNumberTools = 10 };     

 public:
  /** Constructor */
  muIso(const std::string & name, ISvcLocator* pSvcLocator);
  /** Destructor */
  ~muIso();

  /** hltExecute execute the muon isolation FEX. */
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* inputTE, HLT::TriggerElement* outputTE);
  /** hltInitialize. Called by the Steering. */
  HLT::ErrorCode hltInitialize();
  /** hltBeginRun. Called by the Steering. */
  HLT::ErrorCode hltBeginRun();
  /** hltFinalize. Called by the Steering. */
  HLT::ErrorCode hltFinalize();

  /** Prepare ROB requests */
  using HLT::FexAlgo::prepareRobRequests;
  HLT::ErrorCode prepareRobRequests(const HLT::TriggerElement* inputTE );
    
  /** recordFeature. Record on the event record the output IsoMuonFeature. */
  HLT::ErrorCode recordFeature(HLT::TriggerElement* outputTE, IsoMuonFeature* feature);

 private:

  /** vector to keep track of ROBs to be requested */
  std::vector<uint32_t> m_vec_robs;

  /** Pointer to Storegate. */
  StoreGateSvc*        m_pStoreGate;

  // /** Timer service Handle. */
  // ServiceHandle<ITrigTimerSvc> m_timersvc;
  /** Timer Items for the main algorithm. */
  TrigTimer*           m_timer[11];

  /** Handle to the LUT backExtrapolator tool */
  ToolHandle<ITrigMuonBackExtrapolator> m_backExtrapolatorLUT;

  /** ID algorithm. */
  TrigInDetTrack::AlgoId m_algoId;

  /** iterators to LArCells. 
  Used as input for TrigDataAccess::LoadCollections */
  LArTT_Selector<LArCellCont>::const_iterator m_iBegin;
  LArTT_Selector<LArCellCont>::const_iterator m_iEnd;
  LArTT_Selector<LArCellCont>::const_iterator m_it;

  /** iterators to TileCells. 
  Used as input for TrigDataAccess::LoadCollections */
  TileCellCollection::const_iterator m_itBegin;
  TileCellCollection::const_iterator m_itEnd;
  TileCellCollection::const_iterator m_itt;

  /** T2GeometryTool required. */
  IT2GeometryTool *m_geometryTool;
  //ToolHandle<IT2GeometryTool> m_geometryTool;

  /** Object that provides data access in a ROI. */
  ITrigDataAccess *m_data;
  //ToolHandle<ITrigDataAccess> m_data;

  /** Calorimeter Id Manager for calorimeter part determination (Barrel versus EndCap) */ 
  const DataHandle<CaloIdManager> m_larMgr;

  /** Tool for noise subtraction */
  ToolHandle<ICalorimeterNoiseTool> m_noiseTool;

  //Detector mask filling
  /** Fill the detector_mask */
  void setup_detector_mask(const EventID* eventId);

  //Properties
  /** True if detectr mask is chaked */
  BooleanProperty m_check_detector_mask;
  /** True if detectr mask is chaked */
  BooleanProperty m_detector_mask_on_event;

  bool m_detector_mask_not_checked;
  bool m_hasLArECBarrelA;
  bool m_hasLArECBarrelC;
  bool m_hasLArECEndcapA;
  bool m_hasLArECEndcapC;
  bool m_hasLArHCEndcapA;
  bool m_hasLArHCEndcapC;
  bool m_hasTileBarrelA;
  bool m_hasTileBarrelC;
  bool m_hasTileEndcapA;
  bool m_hasTileEndcapC;
  bool m_hasPixelBarrel;
  bool m_hasPixelDisk;
  bool m_hasSCTBarrelA;
  bool m_hasSCTBarrelC;
  bool m_hasSCTEndcapA;
  bool m_hasSCTEndcapC;
  bool m_hasTRTBarrelA;
  bool m_hasTRTBarrelC;
  bool m_hasTRTEndcapA;
  bool m_hasTRTEndcapC;

  uint32_t m_current_run_id;
  uint32_t m_current_lbk_id;
  uint32_t m_current_bcg_id;

  BooleanProperty           m_dummy;

  /** Eta width of calorimetric region. */
  DoubleProperty            m_etaWidth;
  /** Max eta of calorimetric region. */
  DoubleProperty            m_EtaLimit;
  /** Phi width of calorimetric region. */
  DoubleProperty            m_phiWidth;

  /** muIso seeding strategy:
   *  0: auto select best option
   *  1: Std: muComb seeded
   *  2: muFast seeded
   *  3: L1 seeded
   */
  IntegerProperty           m_AlgoStrategy;

  /** Use fast Extrapolation through Calo */
  BooleanProperty           m_useExtrapolator;

  /** Calo Noise suppression */
  BooleanProperty           m_applyNoiseCut;
  /** Use abs(energy) of cell in Noise cut value */
  BooleanProperty           m_useAbsNoiseCut;
  /** Calo noise supression cut (units of rms) */
  DoubleProperty            m_noiseNSigmaCut;
  /** Apply HEC Quality Cut */
  BooleanProperty           m_applyHECQualityCut;
  /** HEC Quality Cut threshold */
  IntegerProperty           m_HECcellQualityThreshold;

  /** Energy threshold for LAR barrel cells. */
  DoubleProperty            m_EthrEC;
  /** Inner cone radius for LAR barrel. */
  DoubleProperty            m_RinnEC;
  /** Outer cone radius for LAR barrel. */
  DoubleProperty            m_RoutEC;
  /** Energy threshold for LAR endcaps cells. */
  DoubleProperty            m_EthrECEC;
  /** Inner cone radius for LAR endcaps. */
  DoubleProperty            m_RinnECEC;
  /** Outer cone radius for LAR endcaps. */
  DoubleProperty            m_RoutECEC;
  /** Energy threshold for Tile cells. */
  DoubleProperty            m_EthrHC;
  /** Inner cone radius for Tile. */
  DoubleProperty            m_RinnHC;
  /** Outer cone radius for Tile. */
  DoubleProperty            m_RoutHC;
  /** Energy threshold for HCAL endcaps cells. */
  DoubleProperty            m_EthrHCEC;
  /** Inner cone radius for HCAL endcaps. */
  DoubleProperty            m_RinnHCEC;
  /** Outer cone radius for HCAL endcaps. */
  DoubleProperty            m_RoutHCEC;
  /** Cone radius for ID based isolation. */
  DoubleProperty            m_RID;
  /** Min Pt to select the ID track for matching */
  DoubleProperty            m_PtMinTrk;
  /** Max abs(eta) to select the ID track for matching */
  DoubleProperty            m_EtaMaxTrk;
  /** Max abs(Zeta) to select the ID track for matching */
  DoubleProperty            m_ZMaxTrk;
  /** Max chi2 to select the ID track for matching */
  DoubleProperty            m_Chi2MaxTrk;
  /** Min number of PIX space points to select the ID track for matching */
  IntegerProperty           m_NPIXhitMinTrk;
  /** Min number of SCT space points to select the ID track for matching */
  IntegerProperty           m_NSCThitMinTrk;
  /** Max abs(Z_ID - Z_muon) for ID tracks to enter the pt cone sum */
  DoubleProperty            m_DzetaMax;
  /** Eta shift trk-mu match in ID based isolation. */
  DoubleProperty            m_EtaShift_MC;
  DoubleProperty            m_EtaShift_MF;
  DoubleProperty            m_EtaShift_L1;
  /** Phi shift trk-mu match in ID based isolation. */
  DoubleProperty            m_PhiShift_MC;
  DoubleProperty            m_PhiShift_MF;
  DoubleProperty            m_PhiShift_L1;
  /** Eta Sigma trk-mu match in ID based isolation. */
  DoubleProperty            m_EtaSigma_MC;
  DoubleProperty            m_EtaSigma_MF;
  DoubleProperty            m_EtaSigma_L1;
  /** Phi Sigma trk-mu match in ID based isolation. */
  DoubleProperty            m_PhiSigma_MC;
  DoubleProperty            m_PhiSigma_MF;
  DoubleProperty            m_PhiSigma_L1;

  /** Do detailed timing. */
  BooleanProperty           m_doDetailedTiming;

  /** Use LAr barrel. */
  BooleanProperty           m_UseEC;
  /** Use LAr endcaps. */
  BooleanProperty           m_UseECEC;
  /** Use Tile. */
  BooleanProperty           m_UseHC;
  /** Use HCAL endcaps. */
  BooleanProperty           m_UseHCEC;
  /** Use ID. */
  BooleanProperty           m_UseID;
  /** ID Algorthm to use. */
  StringProperty            m_ID_algo_to_use;

  bool                      m_xUseEC;
  bool                      m_xUseECEC;
  bool                      m_xUseHC;
  bool                      m_xUseHCEC;
  bool                      m_xUseID;

  /** Isolation cut used for Monitoring (ID) */
  DoubleProperty            m_GlobIsoCut_ID;
  /** Isolation cut used for Monitoring (Cal) */
  DoubleProperty            m_GlobIsoCut_CAL;

  /** Monitored quantities */
  int    m_StrategyMI;
  int    m_ErrorFlagMI;
  float  m_ptFL;
  float  m_Sumpt;
  float  m_Sumet;
  float  m_EHC_I;   
  float  m_EEC_I;
  float  m_CALOiso;
  float  m_IDiso;
  float  m_MuPt;
  float  m_MuEta;
  float  m_MuPhi;
  float  m_NTRK;
  float  m_isIsolated;
};
#endif
