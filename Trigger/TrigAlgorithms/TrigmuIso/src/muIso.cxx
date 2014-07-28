/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     muIso.cxx
// PACKAGE:  Trigger/TrigAlgorithms/TrigmuIso
//
// AUTHOR:   S.Giagu
//
// PURPOSE:  LVL2 Muon Isolation FEX Algorithm: V4.0
// ********************************************************************
#include <iostream>
#include <math.h>

#include "TrigmuIso/muIso.h"
#include "TrigmuIso/muIsoUtil.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/StatusCode.h"

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"

#include "TrigTimeAlgs/TrigTimer.h"
#include "AthenaKernel/Timeout.h"

#include "EventInfo/EventInfo.h"
#include "eformat/DetectorMask.h"
#include "eformat/SourceIdentifier.h"

#include "CaloIdentifier/LArEM_ID.h"
#include "LArRecEvent/LArCell.h"
#include "TileEvent/TileCell.h"

//#include "CaloEvent/CaloSampling.h"
#include "CaloEvent/CaloSamplingHelper.h"
#include "CaloGeoHelpers/CaloSampling.h"
#include "TrigT2CaloCommon/Calo_Def.h"
#include "TrigT2CaloCommon/TrigDataAccess.h"

#include "TrigInDetEvent/TrigInDetTrackFitPar.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrigSiSpacePointTool/ISpacePointProvider.h"
#include "TrigInDetEvent/TrigInDetTrackHelper.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigT1Interfaces/RecMuonRoI.h"
#include "TrigMuonEvent/MuonFeature.h"
#include "TrigMuonEvent/MuonFeatureContainer.h"
#include "TrigMuonEvent/CombinedMuonFeature.h"
#include "TrigMuonEvent/CombinedMuonFeatureContainer.h"
#include "TrigMuonEvent/IsoMuonFeature.h"

class ISvcLocator;

muIso::muIso(const std::string & name, ISvcLocator* pSvcLocator):
   HLT::FexAlgo(name, pSvcLocator),
   m_backExtrapolatorLUT("TrigMuonBackExtrapolator"),
   m_noiseTool("CaloNoiseTool/CaloNoiseToolDefault")
{
   //HLT::FexAlgo(name, pSvcLocator), m_timersvc("TrigTimerSvc/TrigTimerSvc","muIso") {

   //declareProperty("TrigTimerSvc", m_timersvc,"TrigTimerSvc");

   //detector mask
   declareProperty("DetMaskCheck",        m_check_detector_mask = false);
   declareProperty("DetMaskCheckOnEvent", m_detector_mask_on_event = false);

   // Eta-Phi Width of the input region for the region selector
   declareProperty("EtaWidth",          m_etaWidth = 0.4);
   declareProperty("PhiWidth",          m_phiWidth = 0.4);
   declareProperty("EtaLimit",          m_EtaLimit = 2.5);

   // Activate detector specific Fex

   //Strategy
   //0: auto select best option
   //1: Std: muComb seeded
   //2: muFast seeded
   //3: L1 seeded
   declareProperty("MuIsoStrategy", m_AlgoStrategy = 1);
   m_AlgoStrategy.verifier().setBounds(0, 3);

   //Use Extrapolator
   declareProperty("UseExtrapolator",                  m_useExtrapolator = true);
   declareProperty("BackExtrapolatorLUT", m_backExtrapolatorLUT, "public tool for back extrapolating the muon tracks to the IV LUT based");

   //Calo Noise suppression
   declareProperty("noiseTool", m_noiseTool, "handle for noise tool");
   declareProperty("ApplyNoiseCut",                    m_applyNoiseCut  = false);
   declareProperty("UseAbsNoiseCut",                   m_useAbsNoiseCut = true);
   declareProperty("NoiseCutValue",                    m_noiseNSigmaCut = 2.);
   declareProperty("ApplyHECQualityCut",           m_applyHECQualityCut = false);
   declareProperty("HECcellQualityThreshold", m_HECcellQualityThreshold = 0);

   declareProperty("UseCombinedMuon",      m_dummy = true); //not used anymore

   declareProperty("UseECAL",              m_UseEC = true);
   declareProperty("UseECALEndCaps",     m_UseECEC = true);
   declareProperty("UseHCAL",              m_UseHC = true);
   declareProperty("UseHCALEndCaps",     m_UseHCEC = true);

   //ID
   declareProperty("UseID",                m_UseID = true);

   // Algorithm parameters
   //ECAL
   declareProperty("EthrECAL",            m_EthrEC = 60.0);
   declareProperty("RinnECAL",            m_RinnEC = 0.07);
   declareProperty("RoutECAL",            m_RoutEC = 0.4);
   declareProperty("EthrECALEndCaps",   m_EthrECEC = 60.0);
   declareProperty("RinnECALEndCaps",   m_RinnECEC = 0.07);
   declareProperty("RoutECALEndCaps",   m_RoutECEC = 0.4);

   //HCAL
   declareProperty("EthrHCAL",            m_EthrHC = 60.0);
   declareProperty("RinnHCAL",            m_RinnHC = 0.1);
   declareProperty("RoutHCAL",            m_RoutHC = 0.4);
   declareProperty("EthrHCALEndCaps",   m_EthrHCEC = 60.0);
   declareProperty("RinnHCALEndCaps",   m_RinnHCEC = 0.1);
   declareProperty("RoutHCALEndCaps",   m_RoutHCEC = 0.4);

   //ID
   declareProperty("IDalgo",      m_ID_algo_to_use = "SITRACK");
   declareProperty("RID",                    m_RID = 0.4);
   //ID tracks quality parameters
   declareProperty("MinPtTRK",          m_PtMinTrk = 1.0);
   declareProperty("MaxAbsEtaTRK",     m_EtaMaxTrk = 2.5);
   declareProperty("MaxAbsZetaTRK",      m_ZMaxTrk = 999999999.);
   declareProperty("MaxChi2TRK",      m_Chi2MaxTrk = 999999999.);
   declareProperty("MinNPIXHitTRK", m_NPIXhitMinTrk = 0);
   declareProperty("MinNSCTHitTRK", m_NSCThitMinTrk = 0);
   declareProperty("MaxDzetaIDMuon",     m_DzetaMax = 15.0);

   declareProperty("EtaShiftID_MC",  m_EtaShift_MC = 0.0);
   declareProperty("PhiShiftID_MC",  m_PhiShift_MC = 0.0);
   declareProperty("EtaSigmaID_MC",  m_EtaSigma_MC = 0.005);
   declareProperty("PhiSigmaID_MC",  m_PhiSigma_MC = 0.005);
   declareProperty("EtaShiftID_MF",  m_EtaShift_MF = 0.0);
   declareProperty("PhiShiftID_MF",  m_PhiShift_MF = 0.01);
   declareProperty("EtaSigmaID_MF",  m_EtaSigma_MF = 0.01);
   declareProperty("PhiSigmaID_MF",  m_PhiSigma_MF = 0.008);
   declareProperty("EtaShiftID_L1",  m_EtaShift_L1 = 0.0);
   declareProperty("PhiShiftID_L1",  m_PhiShift_L1 = 0.0);
   declareProperty("EtaSigmaID_L1",  m_EtaSigma_L1 = 0.1);
   declareProperty("PhiSigmaID_L1",  m_PhiSigma_L1 = 0.1);

   // Perform detailed timing
   declareProperty("doDetailedTiming",   m_doDetailedTiming = false);

   // Isolation cuts used for monitoring
   declareProperty("MonitoringGlobalIsoCut_ID",  m_GlobIsoCut_ID  = 0.2);
   declareProperty("MonitoringGlobalIsoCut_CAL", m_GlobIsoCut_CAL = 5000.);

   //Monitored quantities
   declareMonitoredVariable("StrategyMI",         m_StrategyMI);
   declareMonitoredVariable("ErrorFlagMI",        m_ErrorFlagMI);
   declareMonitoredVariable("PtFL",               m_ptFL);
   declareMonitoredVariable("EsumECInner"       , m_EEC_I);
   declareMonitoredVariable("EsumHCInner"       , m_EHC_I);
   declareMonitoredVariable("Sumpt"             , m_Sumpt);
   declareMonitoredVariable("Sumet"             , m_Sumet);
   declareMonitoredVariable("TEMuPt"            , m_MuPt);
   declareMonitoredVariable("TEMuEta"           , m_MuEta);
   declareMonitoredVariable("TEMuPhi"           , m_MuPhi);
   declareMonitoredVariable("NTRK"              , m_NTRK);
   declareMonitoredVariable("IDiso"             , m_IDiso);
   declareMonitoredVariable("CALOiso"           , m_CALOiso);
   declareMonitoredVariable("IsIsolated"        , m_isIsolated);

   //RoBs cached access
   m_vec_robs.reserve(200);
}


muIso::~muIso()
{
}

HLT::ErrorCode muIso::hltInitialize()
{

   msg() << MSG::DEBUG << "on initialize()" << endreq;

   //Store Gate will handle I/O data to/from the algorithm
   if (service("StoreGateSvc", m_pStoreGate).isFailure()) {
      msg() << MSG::FATAL << "could not find StoreGateSvc" << endreq;
      return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
   }

   // Algorithms need m_larMgr
   StoreGateSvc* detStore = 0;
   if (service("DetectorStore", detStore).isSuccess()) {
      if ((detStore->retrieve(m_larMgr)).isFailure()) {
         msg() << MSG::ERROR <<
               "Unable to retrieve CaloIdManager from DetectorStore"
               << endreq;
         return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
      } // End of if for retrieve of CaloIdManager
   } // End of if for DetectorStore
   else return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);

   IToolSvc* toolSvc;
   if (service("ToolSvc", toolSvc).isFailure()) {
      msg() << MSG::ERROR << "Cannot Retrieve IToolSvc. Exiting! " << endreq;
      return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
   }
   msg() << MSG::INFO << "Retrieved ToolSvc " << endreq;

   if (toolSvc->retrieveTool("TrigDataAccess", m_data).isFailure()) {
      msg() << MSG::ERROR << "Could not get TrogDataAccess servic" << endreq;
      return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
   } else
      msg() << MSG::INFO << "Retrieved TrigDataAccess tool " << endreq;

   if (toolSvc->retrieveTool("T2GeometryTool", m_geometryTool).isFailure()) {
      msg() << MSG::ERROR << "Could not get m_geometryTool" << endreq;
      return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
   } else
      msg() << MSG::INFO << "Retrieved T2GeometryTool " << endreq;

   if (m_applyNoiseCut) {
      if (m_noiseTool.retrieve().isFailure()) {
         msg() << MSG::ERROR << "Could not get CaloNoiseTool tool" << endreq;
         return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
      } else
         msg() << MSG::INFO << "Retrieved tool " << m_noiseTool << endreq;
   }

   // BackExtrapolator services
   if (m_useExtrapolator) {
      if (m_backExtrapolatorLUT.retrieve().isFailure()) {
         msg() << MSG::ERROR << "Could not retrieve " << m_backExtrapolatorLUT << endreq;
         return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
      } else {
         msg() << MSG::INFO << "Retrieved tool " << m_backExtrapolatorLUT << endreq;
      }
   }

   if (m_doDetailedTiming) {
      //m_timer[0]  = addTimer("muIso_Tot");
      m_timer[1]  = addTimer("muIso_Tot_EC");
      m_timer[2]  = addTimer("muIso_RSel_EC");
      m_timer[3]  = addTimer("muIso_BSCv_EC");
      m_timer[4]  = addTimer("muIso_Algo_EC");
      m_timer[5]  = addTimer("muIso_Tot_HC");
      m_timer[6]  = addTimer("muIso_RSel_HC");
      m_timer[7]  = addTimer("muIso_BSCv_HC");
      m_timer[8]  = addTimer("muIso_Algo_HC");
      m_timer[9]  = addTimer("muIso_Save");
      m_timer[10] = addTimer("muIso_Tot_ID");
   }

   //Set ID Algo to use
   std::string algoId = m_ID_algo_to_use;

   if (algoId == "IDSCAN")           m_algoId = TrigInDetTrack::IDSCANID;
   else if (algoId == "SITRACK")    m_algoId = TrigInDetTrack::SITRACKID;
   else if (algoId == "STRATEGY_A") m_algoId = TrigInDetTrack::STRATEGY_A_ID;
   else if (algoId == "STRATEGY_B") m_algoId = TrigInDetTrack::STRATEGY_B_ID;
   else if (algoId == "STRATEGY_C") m_algoId = TrigInDetTrack::STRATEGY_C_ID;
   else if (algoId == "STRATEGY_F") m_algoId = TrigInDetTrack::STRATEGY_F_ID;
   else if (algoId == "TRTXK")      m_algoId = TrigInDetTrack::TRTXKID;
   else if (algoId == "TRTSEG")     m_algoId = TrigInDetTrack::TRTLUTID;
   else                           m_algoId = TrigInDetTrack::IDSCANID;

   m_detector_mask_not_checked = m_check_detector_mask;

   m_hasLArECBarrelA = true;
   m_hasLArECBarrelC = true;
   m_hasLArECEndcapA = true;
   m_hasLArECEndcapC = true;
   m_hasLArHCEndcapA = true;
   m_hasLArHCEndcapC = true;
   m_hasTileBarrelA  = true;
   m_hasTileBarrelC  = true;
   m_hasTileEndcapA  = true;
   m_hasTileEndcapC  = true;
   m_hasPixelBarrel  = true;
   m_hasPixelDisk    = true;
   m_hasSCTBarrelA   = true;
   m_hasSCTBarrelC   = true;
   m_hasSCTEndcapA   = true;
   m_hasSCTEndcapC   = true;
   m_hasTRTBarrelA   = true;
   m_hasTRTBarrelC   = true;
   m_hasTRTEndcapA   = true;
   m_hasTRTEndcapC   = true;

   m_xUseEC          = m_UseEC;
   m_xUseECEC        = m_UseECEC;
   m_xUseHC          = m_UseHC;
   m_xUseHCEC        = m_UseHCEC;
   m_xUseID          = m_UseID;

   msg() << MSG::DEBUG
         << "Initialization completed successfully"
         << endreq;

   return HLT::OK;
}

HLT::ErrorCode muIso::hltFinalize()
{

   msg() << MSG::DEBUG << "in finalize()" << endreq;

   return HLT::OK;
}

HLT::ErrorCode muIso::hltBeginRun()
{

   msg() << MSG::INFO << "At BeginRun of " << name() << " - package version "
         << PACKAGE_VERSION << endreq;

   // set storegate variable
   m_pStoreGate = store();

   // get EventInfo
   const EventInfo* p_EventInfo(0);
   StatusCode sc = m_pStoreGate->retrieve(p_EventInfo);
   if (sc.isFailure()) {
      msg() << MSG::FATAL << "Can't get EventInfo object in beginRun()" << endreq;
      return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::NAV_ERROR);
   }

   // setup the detector mask if required by the configuration
   if (m_detector_mask_not_checked) setup_detector_mask(p_EventInfo->event_ID());

   return HLT::OK;
}

void muIso::setup_detector_mask(const EventID* eventId)
{

   // Retrieve run number and detector mask

   m_current_run_id = eventId->run_number();
   uint64_t mask    = eventId->detector_mask();
   //uint32_t mask0   = eventId->detector_mask0();
   //uint32_t mask1   = eventId->detector_mask1();

   if (msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG << "Retrieved the detector mask:" << endreq;
      msg() << MSG::DEBUG << "  mask = 0x" << MSG::hex << mask
            << MSG::dec << endreq;
      /*msg() << MSG::DEBUG << "  mask0 = 0x" << MSG::hex << mask0
       << MSG::dec << endreq;
      msg() << MSG::DEBUG << "  mask1 = 0x" << MSG::hex << mask1
       << MSG::dec << endreq;
      */
   }

   //if(mask0==0 && mask1==0) {
   if (mask == 0) {
      if (msgLvl() <= MSG::DEBUG) {
         msg() << MSG::DEBUG << "detector mask == 0: do nothing!" << endreq;
      }
      return;
   }

   //tdaq::DetectorMask decoder;
   eformat::helper::DetectorMask decoder(mask);

   m_hasLArECBarrelA = decoder.is_set(eformat::LAR_EM_BARREL_A_SIDE);
   m_hasLArECBarrelC = decoder.is_set(eformat::LAR_EM_BARREL_C_SIDE);
   m_hasLArECEndcapA = decoder.is_set(eformat::LAR_EM_ENDCAP_A_SIDE);
   m_hasLArECEndcapC = decoder.is_set(eformat::LAR_EM_ENDCAP_C_SIDE);
   m_hasLArHCEndcapA = decoder.is_set(eformat::LAR_HAD_ENDCAP_A_SIDE);
   m_hasLArHCEndcapC = decoder.is_set(eformat::LAR_HAD_ENDCAP_C_SIDE);
   m_hasTileBarrelA  = decoder.is_set(eformat::TILECAL_BARREL_A_SIDE);
   m_hasTileBarrelC  = decoder.is_set(eformat::TILECAL_BARREL_C_SIDE);
   m_hasTileEndcapA  = decoder.is_set(eformat::TILECAL_EXT_A_SIDE);
   m_hasTileEndcapC  = decoder.is_set(eformat::TILECAL_EXT_C_SIDE);
   m_hasPixelBarrel  = decoder.is_set(eformat::PIXEL_BARREL);
   m_hasPixelDisk    = decoder.is_set(eformat::PIXEL_DISK);
   m_hasSCTBarrelA   = decoder.is_set(eformat::SCT_BARREL_A_SIDE);
   m_hasSCTBarrelC   = decoder.is_set(eformat::SCT_BARREL_C_SIDE);
   m_hasSCTEndcapA   = decoder.is_set(eformat::SCT_ENDCAP_A_SIDE);
   m_hasSCTEndcapC   = decoder.is_set(eformat::SCT_ENDCAP_C_SIDE);
   m_hasTRTBarrelA   = decoder.is_set(eformat::TRT_BARREL_A_SIDE);
   m_hasTRTBarrelC   = decoder.is_set(eformat::TRT_BARREL_C_SIDE);
   m_hasTRTEndcapA   = decoder.is_set(eformat::TRT_ENDCAP_A_SIDE);
   /*
   m_hasLArECBarrelA = decoder.present(mask0,mask1,(eformat::LAR_EM_BARREL_A_SIDE));
   m_hasLArECBarrelC = decoder.present(mask0,mask1,(eformat::LAR_EM_BARREL_C_SIDE));
   m_hasLArECEndcapA = decoder.present(mask0,mask1,(eformat::LAR_EM_ENDCAP_A_SIDE));
   m_hasLArECEndcapC = decoder.present(mask0,mask1,(eformat::LAR_EM_ENDCAP_C_SIDE));
   m_hasLArHCEndcapA = decoder.present(mask0,mask1,(eformat::LAR_HAD_ENDCAP_A_SIDE));
   m_hasLArHCEndcapC = decoder.present(mask0,mask1,(eformat::LAR_HAD_ENDCAP_C_SIDE));
   m_hasTileBarrelA  = decoder.present(mask0,mask1,(eformat::TILECAL_BARREL_A_SIDE));
   m_hasTileBarrelC  = decoder.present(mask0,mask1,(eformat::TILECAL_BARREL_C_SIDE));
   m_hasTileEndcapA  = decoder.present(mask0,mask1,(eformat::TILECAL_EXT_A_SIDE));
   m_hasTileEndcapC  = decoder.present(mask0,mask1,(eformat::TILECAL_EXT_C_SIDE));
   m_hasPixelBarrel  = decoder.present(mask0,mask1,(eformat::PIXEL_BARREL));
   m_hasPixelDisk    = decoder.present(mask0,mask1,(eformat::PIXEL_DISK));
   m_hasSCTBarrelA   = decoder.present(mask0,mask1,(eformat::SCT_BARREL_A_SIDE));
   m_hasSCTBarrelC   = decoder.present(mask0,mask1,(eformat::SCT_BARREL_C_SIDE));
   m_hasSCTEndcapA   = decoder.present(mask0,mask1,(eformat::SCT_ENDCAP_A_SIDE));
   m_hasSCTEndcapC   = decoder.present(mask0,mask1,(eformat::SCT_ENDCAP_C_SIDE));
   m_hasTRTBarrelA   = decoder.present(mask0,mask1,(eformat::TRT_BARREL_A_SIDE));
   m_hasTRTBarrelC   = decoder.present(mask0,mask1,(eformat::TRT_BARREL_C_SIDE));
   m_hasTRTEndcapA   = decoder.present(mask0,mask1,(eformat::TRT_ENDCAP_A_SIDE));
   m_hasTRTEndcapC   = decoder.present(mask0,mask1,(eformat::TRT_ENDCAP_C_SIDE));
   */
   //0x61   // MDT Barrel A
   //m_hasMdtBarrelA = decoder.present(mask0,mask1,(eformat::SubDetector)0x61);

   if (msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG << " ==== Detector Mask settings ==== " << endreq;
      msg() << MSG::DEBUG << " ---> LAr EC Barrel side A: "
            << ((m_hasLArECBarrelA) ? "ON" : "OFF") << endreq;
      msg() << MSG::DEBUG << " ---> LAr EC Barrel side C: "
            << ((m_hasLArECBarrelC) ? "ON" : "OFF") << endreq;
      msg() << MSG::DEBUG << " ---> LAr EC Endcap side A: "
            << ((m_hasLArECEndcapA) ? "ON" : "OFF") << endreq;
      msg() << MSG::DEBUG << " ---> LAr EC Endcap side C: "
            << ((m_hasLArECEndcapC) ? "ON" : "OFF") << endreq;
      msg() << MSG::DEBUG << " ---> LAr HC Endcap side A: "
            << ((m_hasLArHCEndcapA) ? "ON" : "OFF") << endreq;
      msg() << MSG::DEBUG << " ---> LAr HC Endcap side C: "
            << ((m_hasLArHCEndcapC) ? "ON" : "OFF") << endreq;
      msg() << MSG::DEBUG << " ---> Tile Barrel side A: "
            << ((m_hasTileBarrelA) ? "ON" : "OFF") << endreq;
      msg() << MSG::DEBUG << " ---> Tile Barrel side C: "
            << ((m_hasTileBarrelC) ? "ON" : "OFF") << endreq;
      msg() << MSG::DEBUG << " ---> Tile Endcap side A: "
            << ((m_hasTileEndcapA) ? "ON" : "OFF") << endreq;
      msg() << MSG::DEBUG << " ---> Tile Endcap side C: "
            << ((m_hasTileEndcapC) ? "ON" : "OFF") << endreq;
      msg() << MSG::DEBUG << " ---> Pixel Barrel: "
            << ((m_hasPixelBarrel) ? "ON" : "OFF") << endreq;
      msg() << MSG::DEBUG << " ---> Pixel Disk: "
            << ((m_hasPixelDisk) ? "ON" : "OFF") << endreq;
      msg() << MSG::DEBUG << " ---> SCT Barrel side A: "
            << ((m_hasSCTBarrelA) ? "ON" : "OFF") << endreq;
      msg() << MSG::DEBUG << " ---> SCT Barrel side C: "
            << ((m_hasSCTBarrelC) ? "ON" : "OFF") << endreq;
      msg() << MSG::DEBUG << " ---> SCT Endcap side A: "
            << ((m_hasSCTEndcapA) ? "ON" : "OFF") << endreq;
      msg() << MSG::DEBUG << " ---> SCT Endcap side C: "
            << ((m_hasSCTEndcapC) ? "ON" : "OFF") << endreq;
      msg() << MSG::DEBUG << " ---> TRT Barrel side A: "
            << ((m_hasTRTBarrelA) ? "ON" : "OFF") << endreq;
      msg() << MSG::DEBUG << " ---> TRT Barrel side C: "
            << ((m_hasTRTBarrelC) ? "ON" : "OFF") << endreq;
      msg() << MSG::DEBUG << " ---> TRT Endcap side A: "
            << ((m_hasTRTEndcapA) ? "ON" : "OFF") << endreq;
      msg() << MSG::DEBUG << " ---> TRT Endcap side C: "
            << ((m_hasTRTEndcapC) ? "ON" : "OFF") << endreq;
   }

   if (!m_detector_mask_on_event) m_detector_mask_not_checked = false;
}

HLT::ErrorCode muIso::hltExecute(const HLT::TriggerElement* inputTE, HLT::TriggerElement* outputTE)
{

   msg() << MSG::DEBUG << "in execute()" << endreq;

   // Time total time
   //if (timerSvc()) if (m_doDetailedTiming) m_timer[0]->start();

   //Initalize Monitored variables
   m_StrategyMI  = 0;
   m_ErrorFlagMI = 0;
   m_EEC_I       = -9999.;
   m_EHC_I       = -9999.;
   m_Sumpt       = -9999.;
   m_Sumet       = -9999.;
   m_MuPt        = -9999.;
   m_MuEta       = -9999.;
   m_MuPhi       = -9999.;
   m_NTRK        = -9999;
   m_ptFL        = -9999.;
   m_IDiso       = -9999.;
   m_CALOiso     = -9999.;
   m_isIsolated  = -1.0;

   // Retrieve store
   m_pStoreGate = store();

   const EventInfo* pEvent(0);
   StatusCode sc = m_pStoreGate->retrieve(pEvent);
   if (sc.isFailure()) {
      m_ErrorFlagMI = 1;
      msg() << MSG::ERROR << "Could not find EventInfo object" << endreq;
      return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::NAV_ERROR);
   }

   const EventID* pEventId = pEvent->event_ID();
   if (pEventId == 0) {
      m_ErrorFlagMI = 1;
      msg() << MSG::ERROR << "Could not find EventID object" << endreq;
      return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::NAV_ERROR);
   }
   m_current_run_id = pEventId->run_number();
   m_current_lbk_id = pEventId->lumi_block();
   m_current_bcg_id = pEventId->bunch_crossing_id();

   if (msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG << " ---> Run Number       : "
            << m_current_run_id << endreq;
      msg() << MSG::DEBUG << " ---> Luminosity Block : " << std::hex
            << m_current_lbk_id << std::dec << endreq;
      msg() << MSG::DEBUG << " ---> Bunch Crossing ID: " << std::hex
            << m_current_bcg_id << std::dec << endreq;
   }

   //detector_mask driven decoding
   if (m_detector_mask_not_checked && m_detector_mask_on_event) setup_detector_mask(pEventId);

   //loose requirements
   bool hasPixel = (m_hasPixelBarrel || m_hasPixelDisk);
   bool hasSCT   = (m_hasSCTBarrelA || m_hasSCTBarrelC || m_hasSCTEndcapA || m_hasSCTEndcapC);
   //bool hasTRT   = (m_hasTRTBarrelA || m_hasTRTBarrelC || m_hasTRTEndcapA || m_hasTRTEndcapC);
   bool hasID    = (hasPixel || hasSCT);
   bool hasEC    = (m_hasLArECBarrelA || m_hasLArECBarrelC);
   bool hasECEC  = (m_hasLArECEndcapA || m_hasLArECEndcapC);
   bool hasHC    = (m_hasTileBarrelA || m_hasTileBarrelC || m_hasTileEndcapA || m_hasTileEndcapC);
   bool hasHCEC  = (m_hasLArHCEndcapA || m_hasLArHCEndcapC);

   if (!hasID && m_xUseID) {
      if (msgLvl() <= MSG::DEBUG)
         msg() << MSG::DEBUG << "ID fex disabled due to detector_mask status" << endreq;
      m_xUseID     = false;
   }
   if (!hasEC && m_xUseEC) {
      if (msgLvl() <= MSG::DEBUG)
         msg() << MSG::DEBUG << "EC Barrel fex disabled due to detector_mask status" << endreq;
      m_xUseEC     = false;
   }
   if (!hasECEC && m_xUseECEC) {
      if (msgLvl() <= MSG::DEBUG)
         msg() << MSG::DEBUG << "EC EC fex disabled due to detector_mask status" << endreq;
      m_xUseECEC = false;
   }
   if (!hasHC && m_xUseHC) {
      if (msgLvl() <= MSG::DEBUG)
         msg() << MSG::DEBUG << "HC Barrel fex disabled due to detector_mask status" << endreq;
      m_xUseHC     = false;
   }
   if (!hasHCEC && m_xUseHCEC) {
      if (msgLvl() <= MSG::DEBUG)
         msg() << MSG::DEBUG << "HC EC fex disabled due to detector_mask status" << endreq;
      m_xUseHCEC = false;
   }

   int m_xAlgoStrategy = m_AlgoStrategy;

   //Debug printout
   if (msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG << "Configured to use seeding strategy: " << m_xAlgoStrategy << endreq;
      if (m_applyNoiseCut) {
         msg() << MSG::DEBUG << "Configured to use CaloNoiseTool: " << endreq;
         msg() << MSG::DEBUG << "  NoiseCutValue:           " << m_noiseNSigmaCut << endreq;
         msg() << MSG::DEBUG << "  ApplyHECQualityCut:      " << (m_noiseNSigmaCut ? "True" : "False") << endreq;
         msg() << MSG::DEBUG << "  HECcellQualityThreshold: " << m_HECcellQualityThreshold << endreq;
      }
   }

   if (m_xUseEC) {
      if (msgLvl() <= MSG::DEBUG) {
         msg() << MSG::DEBUG << "Configured to fex LAr Barrel" << endreq;
         msg() << MSG::DEBUG << "EthrECAL:         " << m_EthrEC  << endreq;
         msg() << MSG::DEBUG << "RinnECAL:         " << m_RinnEC  << endreq;
         msg() << MSG::DEBUG << "RoutECAL:         " << m_RoutEC  << endreq;
      }
   }
   if (m_xUseECEC) {
      if (msgLvl() <= MSG::DEBUG) {
         msg() << MSG::DEBUG << "Configured to fex LAr Endcaps" << endreq;
         msg() << MSG::DEBUG << "EthrECALEndCaps:  " << m_EthrECEC  << endreq;
         msg() << MSG::DEBUG << "RinnECALEndCaps:  " << m_RinnECEC  << endreq;
         msg() << MSG::DEBUG << "RoutECALEndCaps:  " << m_RoutECEC  << endreq;
      }
   }
   if (m_xUseHC) {
      if (msgLvl() <= MSG::DEBUG) {
         msg() << MSG::DEBUG << "Configured to fex Tile" << endreq;
         msg() << MSG::DEBUG << "EthrHCAL:         " << m_EthrHC  << endreq;
         msg() << MSG::DEBUG << "RinnHCAL:         " << m_RinnHC  << endreq;
         msg() << MSG::DEBUG << "RoutHCAL:         " << m_RoutHC  << endreq;
      }
   }
   if (m_xUseHCEC) {
      if (msgLvl() <= MSG::DEBUG) {
         msg() << MSG::DEBUG << "Configured to fex HCAL Endcaps" << endreq;
         msg() << MSG::DEBUG << "EthrHCALEndCaps:  " << m_EthrHCEC  << endreq;
         msg() << MSG::DEBUG << "RinnHCALEndCaps:  " << m_RinnHCEC  << endreq;
         msg() << MSG::DEBUG << "RoutHCALEndCaps:  " << m_RoutHCEC  << endreq;
      }
   }

   if (m_xUseID) {
      if (msgLvl() <= MSG::DEBUG) {
         msg() << MSG::DEBUG << "Configured to fex ID:   " << endreq;
         msg() << MSG::DEBUG << "R ID:                   " << m_RID           << endreq;
         msg() << MSG::DEBUG << "PtMin ID:               " << m_PtMinTrk      << endreq;
         msg() << MSG::DEBUG << "AbsEtaMax ID:           " << m_EtaMaxTrk     << endreq;
         msg() << MSG::DEBUG << "AbsZeta Max ID:         " << m_ZMaxTrk       << endreq;
         msg() << MSG::DEBUG << "MaxChi2 ID:             " << m_Chi2MaxTrk    << endreq;
         msg() << MSG::DEBUG << "MinNPIXHit ID:          " << m_NPIXhitMinTrk << endreq;
         msg() << MSG::DEBUG << "MinNSCTHit ID:          " << m_NSCThitMinTrk << endreq;
         msg() << MSG::DEBUG << "Shift Eta Match MC-ID:  " << m_EtaShift_MC << endreq;
         msg() << MSG::DEBUG << "Shift Phi Match MC-ID:  " << m_PhiShift_MC << endreq;
         msg() << MSG::DEBUG << "Sigma Eta Match MC-ID:  " << m_EtaSigma_MC << endreq;
         msg() << MSG::DEBUG << "Sigma Phi Match MC-ID:  " << m_PhiSigma_MC << endreq;
         msg() << MSG::DEBUG << "Shift Eta Match MF-ID:  " << m_EtaShift_MF << endreq;
         msg() << MSG::DEBUG << "Shift Phi Match MF-ID:  " << m_PhiShift_MF << endreq;
         msg() << MSG::DEBUG << "Sigma Eta Match MF-ID:  " << m_EtaSigma_MF << endreq;
         msg() << MSG::DEBUG << "Sigma Phi Match MF-ID:  " << m_PhiSigma_MF << endreq;
         msg() << MSG::DEBUG << "Shift Eta Match L1-ID:  " << m_EtaShift_L1 << endreq;
         msg() << MSG::DEBUG << "Shift Phi Match L1-ID:  " << m_PhiShift_L1 << endreq;
         msg() << MSG::DEBUG << "Sigma Eta Match L1-ID:  " << m_EtaSigma_L1 << endreq;
         msg() << MSG::DEBUG << "Sigma Phi Match L1-ID:  " << m_PhiSigma_L1 << endreq;
      }
   }

   // Some debug output:
   if (msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG << "inputTE->ID():  " << inputTE->getId() << endreq;
      msg() << MSG::DEBUG << "outputTE->ID(): " << outputTE->getId() << endreq;
   }

   // Start Trigger Element Processing
   bool mc_ok   = false;
   bool mf_ok   = false;
   //bool l1_ok   = false;
   int  usealgo = 0;

   ElementLink<CombinedMuonFeatureContainer> combMuonFeatureEL;
   ElementLink<MuonFeatureContainer>         muonFeatureEL;
   const CombinedMuonFeature*  pCombinedMuonFeature = 0;
   const MuonFeature*          pMuonFeature = 0;
   const LVL1::RecMuonRoI*     muonRoI = 0;

   double pt    = 0.0;
   double eta   = 0.0;
   double phi   = 0.0;
   double zmuon = -99999999.;
   int    roiid = 0;

   if (m_xAlgoStrategy == 0 || m_xAlgoStrategy == 1) {  // CombinedMuonFeature is used
      usealgo = 1;
      if (HLT::OK != getFeatureLink<CombinedMuonFeatureContainer, CombinedMuonFeature> (outputTE, combMuonFeatureEL)) {
         if (m_xAlgoStrategy == 1) {
            msg() << MSG::ERROR << " getFeatureLink fails to get CombinedMuonFeature " << endreq;
            m_ErrorFlagMI = 2;
            return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::MISSING_FEATURE);
         } else {
            if (msgLvl() <= MSG::DEBUG)
               msg() << MSG::DEBUG << "CombinedMuonFeature not found, try MuonFeature" << endreq;
         }
      } else {
         if (!combMuonFeatureEL.isValid()) {
            if (m_xAlgoStrategy == 1) {
               msg() << MSG::ERROR << " getFeatureLink finds no CombinedMuonFeature (EL invalid)" << endreq;
               m_ErrorFlagMI = 2;
               return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::MISSING_FEATURE);
            } else {
               if (msgLvl() <= MSG::DEBUG)
                  msg() << MSG::DEBUG << " getFeatureLink finds no CombinedMuonFeature (EL invalid), try MuonFeature" << endreq;
            }
         } else {
            pCombinedMuonFeature = *combMuonFeatureEL;
            pMuonFeature         = pCombinedMuonFeature->muFastTrack();
            if (!pMuonFeature) {
               if (m_xAlgoStrategy == 1) {
                  m_ErrorFlagMI = 2;
                  msg() << MSG::ERROR << "Retrieval of MuonFeature from CombinedMuonFeature failed!" << endreq;
                  return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::NAV_ERROR);
               } else {
                  if (msgLvl() <= MSG::DEBUG)
                     msg() << MSG::DEBUG << "Retrieval of MuonFeature from CombinedMuonFeature failed, try using directly MuonFeature" << endreq;
               }
            } else {
               if (pCombinedMuonFeature->ptq() == 0) {
                  if (m_xAlgoStrategy == 1) {
                     m_ErrorFlagMI = 2;
                     //msg() << MSG::WARNING << "muComb Pt = 0 and AlgoStrategy == 1 --> no isolation" << endreq;
                     msg() << MSG::DEBUG << "muComb Pt = 0 and AlgoStrategy == 1 --> no isolation" << endreq;
                     IsoMuonFeature * mFeature = new IsoMuonFeature();
                     mFeature->setRoiMu(roiid, pt, eta, phi, 0.0, m_xAlgoStrategy * 10000 + m_ErrorFlagMI);
                     return recordFeature(outputTE, mFeature);
                  } else {
                     if (msgLvl() <= MSG::DEBUG)
                        msg() << MSG::DEBUG << "muComb Pt = 0, try to use MuonFeature" << endreq;
                     if (pMuonFeature->pt() != 0) { //Use muComb->muFast quantities
                        usealgo = 2;
                        roiid  = pMuonFeature->roiId();
                        pt     = pMuonFeature->pt() * 1000.;
                        eta    = pMuonFeature->eta();
                        phi    = pMuonFeature->phi();
                        zmuon  = -99999999.;
                        mc_ok  = true;
                        if (msgLvl() <= MSG::DEBUG) {
                           msg() << MSG::DEBUG
                                 << "The MuonFeature: "
                                 << " pt = "
                                 << (pMuonFeature ->pt()) * 1000.
                                 << " / radius = "
                                 << pMuonFeature ->radius()
                                 << " / eta = "
                                 << pMuonFeature->eta()
                                 << " / phi = "
                                 << pMuonFeature ->phi()
                                 << " / roiid = "
                                 << pMuonFeature->roiId()
                                 << endreq;
                        }
                     }
                  }
               } else { //Use muComb quantities
                  mc_ok = true;
                  roiid  = pMuonFeature->roiId();
                  pt     = pCombinedMuonFeature->ptq();
                  eta    = pCombinedMuonFeature->eta();
                  phi    = pCombinedMuonFeature->phi();
                  const TrigInDetTrack* itrack = pCombinedMuonFeature->IDTrack();
                  zmuon = (itrack) ? itrack->param()->z0() : -99999999.;
                  if (msgLvl() <= MSG::DEBUG) {
                     msg() << MSG::DEBUG
                           << "The CombinedMuonFeature: "
                           << " pt = "
                           << pCombinedMuonFeature ->ptq()
                           << " / eta = "
                           << pCombinedMuonFeature->eta()
                           << " / phi = "
                           << pCombinedMuonFeature ->phi()
                           << " / roiid = "
                           << pMuonFeature->roiId()
                           << endreq;
                  }
               }
            }
         }
      }
   }

   if ((m_xAlgoStrategy == 0 && !mc_ok) || m_xAlgoStrategy == 2) {  // MuonFeature is used
      usealgo = 2;
      if (HLT::OK != getFeatureLink<MuonFeatureContainer, MuonFeature> (outputTE, muonFeatureEL)) {
         if (m_xAlgoStrategy == 2) {
            msg() << MSG::ERROR << " getFeatureLink fails to get MuonFeature " << endreq;
            m_ErrorFlagMI = 3;
            return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::MISSING_FEATURE);
         } else {
            if (msgLvl() <= MSG::DEBUG)
               msg() << MSG::DEBUG << "MuonFeature not found, try L1 RoI" << endreq;
         }
      } else {
         if (!muonFeatureEL.isValid()) {
            if (m_xAlgoStrategy == 2) {
               msg() << MSG::ERROR << " getFeatureLink finds no MuonFeature (EL invalid)" << endreq;
               m_ErrorFlagMI = 3;
               return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::NAV_ERROR);
            } else {
               if (msgLvl() <= MSG::DEBUG)
                  msg() << MSG::DEBUG << " getFeatureLink finds no MuonFeature (EL invalid), try L1 RoI" << endreq;
            }
         } else {
            pMuonFeature = *muonFeatureEL;
            if (pMuonFeature->pt() == 0) {
               if (m_xAlgoStrategy == 2) {
                  m_ErrorFlagMI = 3;
                  //msg() << MSG::WARNING << "muFast` Pt = 0 and AlgoStrategy == 2 --> no isolation" << endreq;
                  msg() << MSG::DEBUG << "muFast` Pt = 0 and AlgoStrategy == 2 --> no isolation" << endreq;
                  IsoMuonFeature * mFeature = new IsoMuonFeature();
                  mFeature->setRoiMu(roiid, pt, eta, phi, 0.0, m_xAlgoStrategy * 10000 + m_ErrorFlagMI);
                  return recordFeature(outputTE, mFeature);
               } else {
                  if (msgLvl() <= MSG::DEBUG)
                     msg() << MSG::DEBUG << "muFast Pt = 0, try to use L1 RoI" << endreq;
               }
            } else { //Use muFast quantities
               mf_ok = true;
               roiid  = pMuonFeature->roiId();
               pt     = pMuonFeature->pt() * 1000.;
               eta    = pMuonFeature->eta();
               phi    = pMuonFeature->phi();
               zmuon  = -99999999.;
               if (msgLvl() <= MSG::DEBUG) {
                  msg() << MSG::DEBUG
                        << "The MuonFeature: "
                        << " pt = "
                        << (pMuonFeature ->pt()) * 1000.
                        << " / radius = "
                        << pMuonFeature ->radius()
                        << " / eta = "
                        << pMuonFeature->eta()
                        << " / phi = "
                        << pMuonFeature ->phi()
                        << " / roiid = "
                        << pMuonFeature->roiId()
                        << endreq;
               }
            }
         }
      }
   }

   if ((m_xAlgoStrategy == 0 && !mc_ok && !mf_ok) || m_xAlgoStrategy == 3) {  // L1 RoI is used
      usealgo = 3;
      if (HLT::OK != getFeature(inputTE, muonRoI, "")) {
         m_ErrorFlagMI = 4;
         if (m_xAlgoStrategy == 3)
            msg() << MSG::ERROR  << "Could not find the LVL1 roi and AlgoStrategy == 3" << endreq;
         else
            msg() << MSG::ERROR  << "Could not find CombinedMuonFeature or MuonFeature or LVL1 roi" << endreq;
         return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::MISSING_FEATURE);
      }
      //l1_ok = true;
      roiid = -1;
      pt    = muonRoI->getThresholdValue() * 1000.;
      eta   = muonRoI->eta();
      phi   = muonRoI->phi();
      zmuon = -99999999.;
      if (msgLvl() <= MSG::DEBUG) {
         msg() << MSG::DEBUG
               << "The L1 RoI: "
               << " pt = "
               << pt
               << " / eta = "
               << eta
               << " / phi = "
               << phi
               << endreq;
      }
   }


   //update monitoring quantities
   m_MuPt        = pt / 1000.;
   if (m_MuPt >  100.) m_MuPt =  101.5;
   if (m_MuPt < -100.) m_MuPt = -101.5;
   m_ptFL        = pt / 1000.;
   if (m_ptFL >  100.) m_ptFL =  101.5;
   if (m_ptFL < -100.) m_ptFL = -101.5;
   m_MuEta       = eta;
   m_MuPhi       = phi;
   m_StrategyMI  = usealgo;
   m_isIsolated  = 1.0;

   // if still pt = 0 end game here
   if (pt == 0) {
      //msg() << MSG::WARNING
      msg() << MSG::DEBUG << "Muon pt = 0 --> No isolation" << endreq;
      IsoMuonFeature * mFeature = new IsoMuonFeature();
      mFeature->setRoiMu(roiid, pt, eta, phi, 0.0, usealgo * 10000 + m_ErrorFlagMI);
      return recordFeature(outputTE, mFeature);
   }

   double m_etaShift = m_EtaShift_MC;
   double m_phiShift = m_PhiShift_MC;
   double m_etaSigma = m_EtaSigma_MC;
   double m_phiSigma = m_PhiSigma_MC;

   if (usealgo == 2) {
      m_etaShift = m_EtaShift_MF;
      m_phiShift = (pt > 0) ? -m_PhiShift_MF.value() : m_PhiShift_MF.value();
      m_etaSigma = m_EtaSigma_MF;
      m_phiSigma = m_PhiSigma_MF;
   } else if (usealgo == 3) {
      m_etaShift = m_EtaShift_L1;
      m_phiShift = m_PhiShift_L1;
      m_etaSigma = m_EtaSigma_L1;
      m_phiSigma = m_PhiSigma_L1;
   }

   // Compute eta-phi region around ROI to decode ECAL/HCAL quantities
   double etamin[7];
   double etamax[7];
   double phimin[7];
   double phimax[7];
   double exeta[7];
   double exphi[7];
   double etamin0 = -2.5;
   double etamax0 =  2.5;
   double phimin0 = -M_PI;
   double phimax0 =  M_PI;;

   int charge = (pt > 0 ? 1 : -1);

   if (usealgo == 2 && m_useExtrapolator) {  //muFast seeded case (back-extrapolation to IP)
      double mfexeta = eta;
      double mfexphi = phi;
      double mfeexeta = 0.;
      double mfeexphi = 0.;
      if (m_backExtrapolatorLUT->give_eta_phi_at_vertex(pMuonFeature, mfexeta, mfeexeta, mfexphi, mfeexphi, 0.).isSuccess()) {
         eta = mfexeta;
         phi = mfexphi;
      }
   }

   for (int k = 0; k < 7; k++) {
      exeta[k] = eta;
      exphi[k] = phi;

      if (usealgo == 1 && m_useExtrapolator) {  //muComb seeded case (forward-extrapolation to calo samplings)
         double etac = eta;
         double phic = muIsoUtil::phiATCal(phi, eta, pt, charge);
         double etam = muIsoUtil::etaATMS(phi, eta, pt, charge);
         double phim = muIsoUtil::phiATMS(phi, eta, pt, charge);
         exeta[k]    = muIsoUtil::parATSampling(k, etac, etam);
         exphi[k]    = muIsoUtil::parATSampling(k, phic, phim);
      }

      etamin[k] = std::max(-m_EtaLimit.value(), exeta[k] - m_etaWidth.value());
      etamax[k] = std::min(m_EtaLimit.value(), exeta[k] + m_etaWidth.value());
      phimin[k] = exphi[k] - m_phiWidth;
      phimax[k] = exphi[k] + m_phiWidth;
      // Ensuring that phimin and phi max are within (-pi,pi]
      while (phimin[k] < -M_PI) phimin[k] += 2. * M_PI;
      while (phimax[k] >  M_PI) phimax[k] -= 2. * M_PI;

      //find max and min bounds (optimize rob access)
      if (k == 0) {
         etamin0 = etamin[0];
         etamax0 = etamax[0];
         phimin0 = phimin[0];
         phimax0 = phimax[0];
      } else {
         if (etamin[k] < etamin0) etamin0 = etamin[k];
         if (etamax[k] > etamax0) etamax0 = etamax[k];
         if (phimin[k] < phimin0) phimin0 = phimin[k];
         if (phimax[k] > phimax0) phimax0 = phimax[k];
      }
   }
   while (phimin0 < -M_PI) phimin0 += 2. * M_PI;
   while (phimax0 >  M_PI) phimax0 -= 2. * M_PI;

   double phimean = 0.5 * (phimin0 + phimax0);
   if (phimin0 > phimax0) phimean += M_PI;

   double etamean = 0.5 * (etamin0 + etamax0);

   TrigRoiDescriptor roical(etamean, etamin0, etamax0, phimean, phimin0, phimax0);

   if (msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG  << " usealgo  = " << usealgo << endreq;
      msg() << MSG::DEBUG  << " muon pt  = " << pt << endreq;
      msg() << MSG::DEBUG  << " eta      = " << eta << endreq;
      msg() << MSG::DEBUG  << " phi      = " << phi << endreq;
      msg() << MSG::DEBUG  << " etaWidth = " << m_etaWidth << endreq;
      msg() << MSG::DEBUG  << " phiWidth = " << m_phiWidth << endreq;
      msg() << MSG::DEBUG  << " etalimit = " << m_EtaLimit << endreq;
      if (m_useExtrapolator) {
         if (usealgo == 1) {
            msg() << MSG::DEBUG  << " Extrapolated eta's: sampling : eta / etamin / etamax " << endreq;
            for (int k = 0; k < 7; k++)
               msg() << MSG::DEBUG  << "   " << k << " : " << exeta[k] << " / " << etamin[k] << " / " << etamax[k] << endreq;
            msg() << MSG::DEBUG  << " Extrapolated phi's: sampling : phi / phimin / phimax " << endreq;
            for (int k = 0; k < 7; k++)
               msg() << MSG::DEBUG  << "   " << k << " : " << exphi[k] << " / " << phimin[k] << " / " << phimax[k] << endreq;
         }
      } else {
         msg() << MSG::DEBUG  << " Extrapolated eta: eta / etamin / etamax " << endreq;
         msg() << MSG::DEBUG  << "   " << exeta[0] << " / " << etamin[0] << " / " << etamax[0] << endreq;
         msg() << MSG::DEBUG  << " Extrapolated phi: phi / phimin / phimax " << endreq;
         msg() << MSG::DEBUG  << "   " << exphi[0] << " / " << phimin[0] << " / " << phimax[0] << endreq;
      }
   }

   msg() << MSG::DEBUG  << " REGTEST inputMu pt / eta / phi:  "
         << " / " << pt
         << " / " << eta
         << " / " << phi
         << endreq;

   //Compute weights
   //float wla = muIsoUtil::normaLAr(eta);
   //float wtl = muIsoUtil::normaTile(eta);

   // Create TrigmuIsoFeature
   msg() << MSG::DEBUG  << " Creating IsoMuonFeature " << endreq;
   IsoMuonFeature * mFeature = new IsoMuonFeature();

   //ID based isolation
   if (timerSvc()) if (m_doDetailedTiming) m_timer[10]->start();
   if (m_xUseID) {

      float  sumpt01 = 0.0;
      float  sumpt02 = 0.0;
      float  sumpt03 = 0.0;
      float  sumpt04 = 0.0;
      float  ptmuID  = 0.0;
      float  maxptID = 0.0;
      int    ntrk    = 0;

      std::vector<const TrigInDetTrackCollection*> vectorOfTrackCollections;
      HLT::ErrorCode status = getFeatures(outputTE, vectorOfTrackCollections, "");

      if (status != HLT::OK) {
         msg() << MSG::DEBUG << " Failed to get InDetTrackCollections " << endreq;
      } else {
         msg() << MSG::DEBUG << " Got " << vectorOfTrackCollections.size()
               << " InDetTrackCollections " << endreq;

         std::vector<const TrigInDetTrackCollection*>::iterator
         theTrackColl = vectorOfTrackCollections.begin(),
         endTrackColl = vectorOfTrackCollections.end();

         for (; theTrackColl != endTrackColl;  theTrackColl++) {

            // Check if event timeout was reached
            if (Athena::Timeout::instance().reached()) {
               m_ErrorFlagMI = 5;
               if (msgLvl() <= MSG::DEBUG)
                  msg() << MSG::DEBUG << "Timeout reached. Trk Loop, Aborting sequence." << endreq;
               return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::TIMEOUT);
            }

            TrigInDetTrackCollection::const_iterator
            track     = (*theTrackColl)->begin(),
            lasttrack = (*theTrackColl)->end();

            double chisq_min =  99999999999.;
            double ptid_max  = -99999999999.;
            double ptid_sec  = -99999999999.;
            bool   seltr = false;
            for (; track != lasttrack; track++) {

               if ((*track)->algorithmId() != m_algoId) continue;

               if (!((*track)->param())) continue;   //Must have helix parameters
               double pt_id    = (*track)->param()->pT(); //in MeV/c
               double phi_id   = (*track)->param()->phi0();
               double eta_id   = (*track)->param()->eta();
               double zPos_id  = (*track)->param()->z0();
               double chi2_id  = (*track)->chi2();
               int    npixh_id = (*track)->NPixelSpacePoints();
               int    nscth_id = (*track)->NSCT_SpacePoints();
               double dzeta    = 0.0;
               if (zmuon > -9999999.) dzeta = zPos_id - zmuon;

               if (msgLvl() <= MSG::DEBUG)
                  msg() << MSG::DEBUG << "Found track: "
                        << m_ID_algo_to_use
                        << "  with pt = " << pt_id
                        << ", eta = " << eta_id
                        << ", phi = " << phi_id
                        << ", Zid = " << zPos_id
                        << ", Chi2 = " << chi2_id
                        << ", NPix = " << npixh_id
                        << ", NSCT = " << nscth_id
                        << ", DZeta = " << dzeta
                        << endreq;

               if (fabs(pt_id / 1000.) < m_PtMinTrk)       continue;
               if (fabs(eta_id)      > m_EtaMaxTrk)      continue;
               if (fabs(zPos_id)     > m_ZMaxTrk)        continue;
               if (chi2_id           > m_Chi2MaxTrk)     continue;
               if (npixh_id          < m_NPIXhitMinTrk)  continue;
               if (nscth_id          < m_NSCThitMinTrk)  continue;
               if (fabs(dzeta)       > m_DzetaMax)       continue;

               if (msgLvl() <= MSG::DEBUG)
                  msg() << MSG::DEBUG << "Track selected"  << endreq;

               seltr = true;

               //see if in cone
               double deta = fabs(eta_id - eta);
               double dphi = fabs(phi_id - phi);
               if (dphi > M_PI) dphi = 2.* M_PI - dphi;

               double DR = sqrt(deta * deta + dphi * dphi);
               if (DR <= m_RID) {
                  ntrk++;
                  if (DR <= 0.1) sumpt01 += fabs(pt_id);
                  if (DR <= 0.2) sumpt02 += fabs(pt_id);
                  if (DR <= 0.3) sumpt03 += fabs(pt_id);
                  if (DR <= 0.4) sumpt04 += fabs(pt_id);

                  double chisq  = (eta - eta_id - m_etaShift) * (eta - eta_id - m_etaShift) / m_etaSigma / m_etaSigma;
                  chisq += (phi - phi_id - m_phiShift) * (phi - phi_id - m_phiShift) / m_phiSigma / m_phiSigma;
                  if (chisq <= chisq_min) {
                     ptmuID = fabs(pt_id);
                     chisq_min = chisq;
                  }
                  if (fabs(pt_id) >= ptid_max) {
                     ptid_max = fabs(pt_id);
                  } else {
                     if (fabs(pt_id) >= ptid_sec) {
                        ptid_sec = fabs(pt_id);
                     }
                  }
               }
            }//
            if (seltr) {
               maxptID = ptid_max;
               if (maxptID == ptmuID) maxptID = ptid_sec;
               if (maxptID <= 0.0) maxptID = 0.0;
               if (ptmuID  <= 0.0) ptmuID  = 0.0;
            }
         }
      }


      msg() << MSG::DEBUG << " REGTEST ID ALGO: ntrk / sumpt02 / ptmuID / maxptID "
            << " / " << ntrk
            << " / " << sumpt02
            << " / " << ptmuID
            << " / " << maxptID
            << endreq;

      //updated monitored variables
      m_NTRK     = ntrk;
      m_Sumpt    = sumpt02 / 1000.;
      if (ptmuID != 0.) m_IDiso = (sumpt02 - ptmuID) / ptmuID;

      //Fill feature
      mFeature->setTrackInfos(sumpt01, sumpt02, sumpt03, sumpt04, ptmuID, maxptID);

      //Global Isolation (ID)
      if (ptmuID != 0) {
         if ((sumpt02 - ptmuID) / ptmuID > m_GlobIsoCut_ID) m_isIsolated = 0.0;
      }

   }//m_xUseID
   if (timerSvc()) if (m_doDetailedTiming) m_timer[10]->stop();

   // Time total ECAL time
   if (timerSvc()) if (m_doDetailedTiming) m_timer[1]->start();

   if (timerSvc()) if (m_doDetailedTiming) m_timer[2]->start();
   if (timerSvc()) if (m_doDetailedTiming) m_timer[2]->pause();
   if (timerSvc()) if (m_doDetailedTiming) m_timer[3]->start();
   if (timerSvc()) if (m_doDetailedTiming) m_timer[3]->pause();
   if (timerSvc()) if (m_doDetailedTiming) m_timer[4]->start();
   if (timerSvc()) if (m_doDetailedTiming) m_timer[4]->pause();

   double sumet01 = 0.0;
   double sumet02 = 0.0;
   double sumet03 = 0.0;
   double sumet04 = 0.0;
   double etcone_i_EC = 0.0;
   double etcone_i_HC = 0.0;

   if (m_xUseEC || m_xUseECEC) {


      // Decode and fill ECAL-based quantities
      msg() << MSG::DEBUG  << " Fill ECAL quantities " << endreq;

      //Used to differentiate among LAr Barrel/EC
      const LArEM_ID*   emID  = m_larMgr->getEM_ID();

      //Loop over LAr samplings
      for (int sampling = 0; sampling < 4; sampling++) { //Sampling Loop

         // Check if event timeout was reached
         if (Athena::Timeout::instance().reached()) {
            m_ErrorFlagMI = 6;
            if (msgLvl() <= MSG::DEBUG)
               msg() << MSG::DEBUG << "Timeout reached. LAr sampling loop, Aborting sequence." << endreq;
            return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::TIMEOUT);
         }

         // Get detector offline ID's for Collections
         if (timerSvc()) if (m_doDetailedTiming) m_timer[2]->resume();  //region selector time

         //m_data->RegionSelector(sampling, etamin0, etamax0, phimin0, phimax0);
         m_data->RegionSelector(sampling, roical);

         if (timerSvc()) if (m_doDetailedTiming) m_timer[2]->pause();

         //Prepare collections (only first time)
         if (timerSvc()) if (m_doDetailedTiming) m_timer[3]->resume(); //BS conversion time
         if (m_data->LoadCollections(m_iBegin, m_iEnd, sampling, !sampling).isFailure())  {
            m_ErrorFlagMI = 7;
            msg() <<  MSG::WARNING << " Failed to LoadCollections for LAr " << endreq;
            mFeature->setRoiMu(roiid, pt, eta, phi, usealgo * 10000 + m_ErrorFlagMI);
            /*HLT::ErrorCode sc =*/
            recordFeature(outputTE, mFeature);
            return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::UNKNOWN);
         }
         if (timerSvc()) if (m_doDetailedTiming) m_timer[3]->pause();

         if (timerSvc()) if (m_doDetailedTiming) m_timer[4]->resume(); //Algo time


         //Loop over LAr cells
         for (m_it = m_iBegin; m_it != m_iEnd; ++m_it) {

            // Check if event timeout was reached
            if (Athena::Timeout::instance().reached()) {
               m_ErrorFlagMI = 8;
               if (msgLvl() <= MSG::DEBUG)
                  msg() << MSG::DEBUG << "Timeout reached. LAr cells loop, Aborting sequence." << endreq;
               return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::TIMEOUT);
            }

            const LArCell* larcell = (*m_it);

            bool isBarrel = emID->is_em_barrel(larcell->ID());
            if (!isBarrel && !m_xUseECEC) continue;
            if (isBarrel && !m_xUseEC)    continue;

            double ethr = m_EthrEC;
            double rin  = m_RinnEC;
            double rou  = m_RoutEC;
            if (!isBarrel) {
               ethr = m_EthrECEC;
               rin  = m_RinnECEC;
               rou  = m_RoutECEC;
            }
            if (m_applyNoiseCut) {
               double rms = m_noiseTool->getNoise(larcell, ICalorimeterNoiseTool::TOTALNOISE);
               ethr = m_noiseNSigmaCut * rms;
            }

            double energy = larcell->energy();

            double ecut = energy;
            if (m_useAbsNoiseCut) ecut = fabs(energy);

            if (ecut > ethr) {

               double etac = larcell->eta();
               double sthc = sin(2 * atan(exp(-etac)));
               double phic = larcell->phi();
               double deta = fabs(etac - exeta[sampling]);
               double dphi = fabs(phic - exphi[sampling]);
               if (dphi > M_PI) dphi = 2.* M_PI - dphi;

               double DR = sqrt(deta * deta + dphi * dphi);
               if (DR <= rin) {
                  etcone_i_EC += energy * sthc;
               }
               if (DR <= rou) {
                  if (DR <= 0.1) sumet01 += energy * sthc;
                  if (DR <= 0.2) sumet02 += energy * sthc;
                  if (DR <= 0.3) sumet03 += energy * sthc;
                  if (DR <= 0.4) sumet04 += energy * sthc;
               }
            }
         }
         if (timerSvc()) if (m_doDetailedTiming) m_timer[4]->pause(); //Algo time
      }//sampling
   }

   // Time total ECAL time
   if (timerSvc()) if (m_doDetailedTiming) m_timer[2]->stop();
   if (timerSvc()) if (m_doDetailedTiming) m_timer[3]->stop();
   if (timerSvc()) if (m_doDetailedTiming) m_timer[4]->stop();
   if (timerSvc()) if (m_doDetailedTiming) m_timer[1]->stop();

   // Time total HCAL time
   if (timerSvc()) if (m_doDetailedTiming) m_timer[5]->start();

   if (timerSvc()) if (m_doDetailedTiming) m_timer[6]->start();
   if (timerSvc()) if (m_doDetailedTiming) m_timer[6]->pause();
   if (timerSvc()) if (m_doDetailedTiming) m_timer[7]->start();
   if (timerSvc()) if (m_doDetailedTiming) m_timer[7]->pause();
   if (timerSvc()) if (m_doDetailedTiming) m_timer[8]->start();
   if (timerSvc()) if (m_doDetailedTiming) m_timer[8]->pause();

   if (m_xUseHC || m_xUseHCEC) { // Decode and fill HCAL-based quantities

      msg() << MSG::DEBUG  << " Fill HCAL quantities " << endreq;

      if (m_xUseHCEC) { // First Lar HCAL (EndCaps)

         for (unsigned int sampling = 0; sampling < 3; sampling++) {

            // Check if event timeout was reached
            if (Athena::Timeout::instance().reached()) {
               m_ErrorFlagMI = 9;
               if (msgLvl() <= MSG::DEBUG)
                  msg() << MSG::DEBUG << "Timeout reached. TTHEC sampling loop, Aborting sequence." << endreq;
               return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::TIMEOUT);
            }

            // Region selector
            if (timerSvc()) if (m_doDetailedTiming) m_timer[6]->resume();  //region selector time

            //m_data->RegionSelector(sampling, etamin0, etamax0, phimin0, phimax0, TTHEC);
            m_data->RegionSelector(sampling, roical, TTHEC);

            if (timerSvc()) if (m_doDetailedTiming) m_timer[6]->pause();

            // For the first sample create the containers for the others no
            if (timerSvc()) if (m_doDetailedTiming) m_timer[7]->resume();  //BS conversion
            if (m_data->LoadCollections(m_iBegin, m_iEnd, sampling, !sampling).isFailure()) {
               m_ErrorFlagMI = 10;
               msg() <<  MSG::WARNING << " Failed to LoadCollections for TTHAEC " << endreq;
               mFeature->setRoiMu(roiid, pt, eta, phi, usealgo * 10000 + m_ErrorFlagMI);
               /*HLT::ErrorCode sc =*/
               recordFeature(outputTE, mFeature);
               return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::UNKNOWN);
            }
            if (timerSvc()) if (m_doDetailedTiming) m_timer[7]->pause();

            if (timerSvc()) if (m_doDetailedTiming) m_timer[8]->resume();  //Algo time

            // Loop over cells
            for (m_it = m_iBegin; m_it != m_iEnd; ++m_it) {

               // Check if event timeout was reached
               if (Athena::Timeout::instance().reached()) {
                  m_ErrorFlagMI = 11;
                  if (msgLvl() <= MSG::DEBUG)
                     msg() << MSG::DEBUG << "Timeout reached. TTHEC cells loop, Aborting sequence." << endreq;
                  return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::TIMEOUT);
               }

               const LArCell* larcell = (*m_it);

               double ethr = m_EthrHCEC;
               if (m_applyNoiseCut) {
                  double rms = m_noiseTool->getNoise(larcell, ICalorimeterNoiseTool::TOTALNOISE);
                  ethr = m_noiseNSigmaCut * rms;
               }

               double energy = larcell->energy();

               bool               passHECQ = true;
               if (energy < -5e3) passHECQ = false;
               if (m_applyHECQualityCut) {
                  if (((larcell->quality()) && 0xffff) > m_HECcellQualityThreshold) passHECQ = false;
               }

               double ecut = energy;
               if (m_useAbsNoiseCut) ecut = fabs(energy);

               if (ecut > ethr && passHECQ) {
                  double etac = larcell->eta();
                  double sthc = sin(2 * atan(exp(-etac)));
                  double phic = larcell->phi();
                  double deta = fabs(etac - exeta[sampling + 4]);
                  double dphi = fabs(phic - exphi[sampling + 4]);
                  if (dphi > M_PI) dphi = 2.* M_PI - dphi;

                  double DR = sqrt(deta * deta + dphi * dphi);
                  if (DR <= m_RinnHCEC) {
                     etcone_i_HC += energy * sthc;
                  }
                  if (DR <= m_RoutHCEC) {
                     if (DR <= 0.1) sumet01 += energy * sthc;
                     if (DR <= 0.2) sumet02 += energy * sthc;
                     if (DR <= 0.3) sumet03 += energy * sthc;
                     if (DR <= 0.4) sumet04 += energy * sthc;
                  }
               }
            }
            if (timerSvc()) if (m_doDetailedTiming) m_timer[8]->stop();
         }
      }//m_xUseHCEC

      // Then HCAL Barrel (TileCal)
      if (m_xUseHC) {

         //No sampling needed for Tile
         if (timerSvc()) if (m_doDetailedTiming) m_timer[6]->resume();  //region selector time

         //Fix for RS (reduced Tile)
         double tphimin0 = muIsoUtil::check_tileregion(0, phimin0);
         double tphimax0 = muIsoUtil::check_tileregion(1, phimax0);

         double tphimean = 0.5 * (tphimin0 + tphimax0);
         if (tphimin0 > tphimax0) tphimean += M_PI;

         double tetamean = 0.5 * (etamin0 + etamax0);

         TrigRoiDescriptor troical(tetamean, etamin0, etamax0, tphimean, tphimin0, tphimax0);

         //m_data->RegionSelector(0, etamin0, etamax0, tphimin, tphimax, TILE);
         m_data->RegionSelector(0, troical, TILE);

         if (timerSvc()) if (m_doDetailedTiming) m_timer[6]->pause();

         for (unsigned int iR = 0; iR < m_data->TileContSize(); iR++) {

            // For the first sample create the containers for the others no
            if (timerSvc()) if (m_doDetailedTiming) m_timer[7]->resume();  //BS conversion
            if (m_data->LoadCollections(m_itBegin, m_itEnd, iR, !iR).isFailure()) {
               m_ErrorFlagMI = 12;
               msg() <<  MSG::WARNING << " Failed to LoadCollections for Tile " << endreq;
               mFeature->setRoiMu(roiid, pt, eta, phi, usealgo * 10000 + m_ErrorFlagMI);
               /*HLT::ErrorCode sc =*/
               recordFeature(outputTE, mFeature);
               return HLT::TOOL_FAILURE;
            }
            if (timerSvc()) if (m_doDetailedTiming) m_timer[7]->pause();

            if (timerSvc()) if (m_doDetailedTiming) m_timer[8]->resume();  //algo time
            // Loop over cells
            for (m_itt = m_itBegin; m_itt != m_itEnd; ++m_itt) {

               // Check if event timeout was reached
               if (Athena::Timeout::instance().reached()) {
                  m_ErrorFlagMI = 13;
                  if (msgLvl() <= MSG::DEBUG)
                     msg() << MSG::DEBUG << "Timeout reached. Tile cells loop, Aborting sequence." << endreq;
                  return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::TIMEOUT);
               }

               const TileCell* tilecell = (*m_itt);

               double ethr = m_EthrHC;
               if (m_applyNoiseCut) {
                  double rms = m_noiseTool->getNoise(tilecell, ICalorimeterNoiseTool::TOTALNOISE);
                  ethr = m_noiseNSigmaCut * rms;
               }

               double energy = tilecell->energy();

               double ecut = energy;
               if (m_useAbsNoiseCut) ecut = fabs(energy);

               //CaloSampling::CaloSample samp = CaloSampling::getSampling(*tilecell);
               CaloSamplingHelper::CaloSample samp = CaloSamplingHelper::getSampling(*tilecell);
               int idxsamp = 4;
               if (CaloSampling::TileBar0 == samp || CaloSampling::TileExt0 == samp)      idxsamp = 4;
               else if (CaloSampling::TileBar1 == samp || CaloSampling::TileExt1 == samp) idxsamp = 5;
               else if (CaloSampling::TileBar2 == samp || CaloSampling::TileExt2 == samp) idxsamp = 6;

               if (ecut > ethr) {
                  double etac = tilecell->eta();
                  double sthc = sin(2 * atan(exp(-etac)));
                  double phic = tilecell->phi();
                  double deta = fabs(etac - exeta[idxsamp]);
                  double dphi = fabs(phic - exphi[idxsamp]);
                  if (dphi > M_PI) dphi = 2.* M_PI - dphi;

                  double DR = sqrt(deta * deta + dphi * dphi);
                  if (DR <= m_RinnHC) {
                     etcone_i_HC += energy * sthc;
                  }
                  if (DR <= m_RoutHC) {
                     if (DR <= 0.1) sumet01 += energy * sthc;
                     if (DR <= 0.2) sumet02 += energy * sthc;
                     if (DR <= 0.3) sumet03 += energy * sthc;
                     if (DR <= 0.4) sumet04 += energy * sthc;
                  }
               }
            }
            if (timerSvc()) if (m_doDetailedTiming) m_timer[8]->pause();
         }
      }//m_xUseHC

   }

   //Subtract muon energy loss
   sumet01 -= (etcone_i_EC + etcone_i_HC);
   sumet02 -= (etcone_i_EC + etcone_i_HC);
   sumet03 -= (etcone_i_EC + etcone_i_HC);
   sumet04 -= (etcone_i_EC + etcone_i_HC);

   // Fill IsoMuonFeature with CALO quantities
   mFeature->setCaloInfos(sumet01, sumet02, sumet03, sumet04);
   mFeature->setRoiMu(roiid, pt, eta, phi, usealgo * 10000 + m_ErrorFlagMI);

   //Debug Printout
   msg() << MSG::DEBUG << " REGTEST CALO: sumet01 / sumet02 / sumet03 / sumet04 / etcone_i_EC / etcone_i_HC : "
         << " / " << sumet01
         << " / " << sumet02
         << " / " << sumet03
         << " / " << sumet04
         << " / " << etcone_i_EC
         << " / " << etcone_i_HC
         << endreq;

   m_EEC_I = etcone_i_EC / 1000.;
   m_EHC_I = etcone_i_HC / 1000.;
   m_CALOiso = sumet02 / 1000.;
   if (sumet02 > m_GlobIsoCut_CAL) m_isIsolated = 0.0;

   // Time total HCAL time
   if (timerSvc()) if (m_doDetailedTiming) m_timer[6]->stop();
   if (timerSvc()) if (m_doDetailedTiming) m_timer[7]->stop();
   if (timerSvc()) if (m_doDetailedTiming) m_timer[8]->stop();
   if (timerSvc()) if (m_doDetailedTiming) m_timer[5]->stop();

   if (m_isIsolated == 1) m_ptFL = -999999.;

   //Record results in SG to seed next algorithm
   if (timerSvc()) if (m_doDetailedTiming) m_timer[9]->start();      //Save feature timing
   if (recordFeature(outputTE, mFeature) != HLT::OK) {
      m_ErrorFlagMI = 14;
      msg() << MSG::ERROR << "Record of Muon Isolation Feature failed" << endreq;
      return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::UNKNOWN);
   }
   if (timerSvc()) if (m_doDetailedTiming) m_timer[9]->stop();

   //Time total time
   //if (timerSvc()) if (m_doDetailedTiming) m_timer[0]->stop();


   return HLT::OK;
}


HLT::ErrorCode muIso::recordFeature(HLT::TriggerElement* outputTE, IsoMuonFeature* feature)
{

   //std::string Key = "";
   //HLT::ErrorCode status = recordAndAttachFeature( outputTE, feature, Key, "");
   HLT::ErrorCode status = attachFeature(outputTE, feature);
   if (status != HLT::OK) {
      msg() << MSG::ERROR << "Record of Muon Isolation Feature in TriggerElement failed" << endreq;
      return status;
   } else {
      if (msgLvl() <= MSG::DEBUG)
         msg() << MSG::DEBUG << "Muon Isolation Feature attached to the TriggerElement" << endreq;
   }
   outputTE->setActiveState(true);

   return HLT::OK;
}

HLT::ErrorCode muIso::prepareRobRequests(const HLT::TriggerElement* inputTE)
{

   // Calculate ROBs needed
   const TrigRoiDescriptor* roiDescriptor = 0;
   HLT::ErrorCode hltStatus = getFeature(inputTE, roiDescriptor);

   if (hltStatus != HLT::OK) {
      std::cout <<  MSG::WARNING << " Failed to find RoiDescriptor " << std::endl;
      return hltStatus;
   }
   // setup ROI bounds
   double etamin = std::max(-1.0 * m_EtaLimit, roiDescriptor->eta() - (m_etaWidth + 0.05));
   double etamax = std::min(1.0 * m_EtaLimit, roiDescriptor->eta() + (m_etaWidth + 0.05));
   double phimin = roiDescriptor->phi() - (m_phiWidth + 0.05);
   double phimax = roiDescriptor->phi() + (m_phiWidth + 0.05);
   while (phimin < 0)        phimin += 2. * M_PI;
   while (phimax > 2 * M_PI) phimax -= 2. * M_PI;

   //Look at RoIs in FCal
   if (fabs(roiDescriptor->eta()) > 3.2) {
      if (roiDescriptor->eta() < -3.2) {
         etamin = -5.;
         etamax = -3.;
      }
      if (roiDescriptor->eta() > 3.2) {
         etamin = 3.;
         etamax = 5.;
      }
   }

   m_data->ROBList(etamin, etamax, phimin, phimax, m_vec_robs);

   config()->robRequestInfo()->addRequestScheduledRobIDs(m_vec_robs);

   return HLT::OK;
}
