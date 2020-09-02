/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/HistoProperty.h"
#include "ByteStreamData/RawEvent.h"
#include "eformat/Status.h"
#include <stdint.h>

//#include "TrigT1Result/MuCTPIRoI.h"
#include "TrigConfInterfaces/ITrigConfigSvc.h"
#include "TrigConfInterfaces/ILVL1ConfigSvc.h"

#include "TrigSteeringEvent/Lvl1Result.h"
#include "TrigSteeringEvent/HLTResult.h"

#include "TrigConfHLTData/HLTChain.h"
#include "TrigConfHLTData/HLTChainList.h"

#include "TrigConfData/L1Menu.h"
#include "TrigT1Result/RoIBResult.h"

#include "AthenaMonitoringKernel/Monitored.h"



/////////////////////////////////////////////////////////////////////////////

namespace ROIB {
  class MuCTPIResult;
}

class MuCTPI_RDO;
class IROBDataProviderSvc;

class TH1F;       /// for monitoring purposes
class TH2F;       /// for monitoring purposes
class TProfile2D; /// for monitoring purposes

class TrigALFAROBMonitor:public AthReentrantAlgorithm {
public:
  TrigALFAROBMonitor(const std::string& name, ISvcLocator* pSvcLocator);

  StatusCode initialize();
  StatusCode execute(const EventContext& ctx) const;
  StatusCode start();  
  StatusCode stop();

private:
	
  /**
   * @brief Accessor method for the message level variable.
   * @return value of the message level for this algorithm.
   */

  typedef ServiceHandle<TrigConf::ITrigConfigSvc> TrigConfigSvc_t;
  typedef ServiceHandle<TrigConf::ILVL1ConfigSvc> Lvl1ConfigSvc_t;

  TrigConfigSvc_t		   m_configSvc;
  Lvl1ConfigSvc_t		   m_lvl1ConfSvc;
  ServiceHandle<ITHistSvc>         m_rootHistSvc;
  std::string                      m_keyRBResult;      // Key to retrieve the RoIBResult from SG
  std::string                      m_keyL1Result;      // key to retrieve the L1Result from SG
  
  typedef ServiceHandle<IROBDataProviderSvc> IIROBDataProviderSvc_t;
  /// Reference to the ROBDataProviderSvc service
  ServiceHandle<IROBDataProviderSvc>           m_robDataProviderSvc;

  /// Source identifiers for ROB fragments
  IntegerProperty                  m_lvl1CTPROBid ;
  IntegerProperty                  m_lvl1ALFA1ROBid ;
  IntegerProperty                  m_lvl1ALFA2ROBid ;
  IntegerProperty                  m_daqCTPROBid ;

  UnsignedIntegerProperty          m_ctpModuleID;

  SG::ReadHandleKey<TrigConf::L1Menu> m_L1MenuKey{ this, "L1TriggerMenu", "DetectorStore+L1TriggerMenu", "L1 Menu" };

  /// Switch for setting the debug StreamTag and name for debug stream
  BooleanProperty                  m_setDebugStream;
  StringProperty                   m_debugStreamName;

  StringProperty		   m_calibrationStreamName;

  /// Switch for ROB checksum test
  BooleanProperty                  m_doROBChecksum;
  TH1F*                            m_hist_failedChecksumForALFAROB;
  Histo1DProperty                  m_histProp_failedChecksumForALFAROB;
  //TH1F*                            m_hist_failedChecksumForROB;
  //Histo1DProperty                  m_histProp_failedChecksumForROB;
  //TH1F*                            m_hist_failedChecksumForSD;
  //Histo1DProperty                  m_histProp_failedChecksumForSD;

  /// Switch for ALFA fast online tracking
  BooleanProperty                  m_doALFATracking;
  TH2F*                            m_hist_ALFA_trig_validated_tracks[12][8]= {{0}}; //12 trigger condition &  8 alfa stations
  TH2F*                            m_hist_ALFA_trig_validated_tracks_1LB[12][8]= {{0}}; //reset after each LB
  TH2F*                            m_hist_ALFA_trig_validated_tracks_1LB_current[12][8]= {{0}}; //reset after 60 LB
  TH2F*                            m_hist_ALFA_trig_validated_tracks_10LB[12][8]= {{0}}; //reset after 10 LBs
  TH2F*                            m_hist_ALFA_trig_validated_tracks_60LB[12][8]= {{0}}; //reset after 60 LB
  TH2F*                            m_hist_ALFA_trig_validated_tracks_SB[12][8]= {{0}}; //reset after 60 LB
  BooleanProperty                  m_doPMFMonitoring;
  TH2F*                            m_hist_pmfMonitoring[8]={0};
  BooleanProperty                  m_doDataGoodMonitoring;
  TH1F*                            m_hist_goodData;
  TH2F*                            m_hist_goodDataLB15;
  TH2F*                            m_hist_goodDataLB18;
  TH2F*                            m_hist_corruptedROD_LB;
  BooleanProperty                  m_doODDistance;
  TH1F*                            m_hist_PosDetector[8][2];
  TH1F*                            m_hist_DistStation[8][2];

  /// Switch for ROB status bit histograms
  BooleanProperty                  m_doROBStatus;
  TH2F*                            m_hist_genericStatusForROB;
  TH2F*                            m_hist_specificStatusForROB;
  std::map<eformat::GenericStatus, std::string> m_map_GenericStatus;
  std::vector<std::string>                      m_vec_SpecificStatus;

  /// pointers to the CTP and muCTPi result objects
  ROIB::MuCTPIResult*              m_lvl1muCTPIResult;  // RoIB muCTPi Result

  ToolHandle<GenericMonitoringTool> m_monTool{this,"MonTool","","Monitoring tool"};

  /// vectors with CTP and muCTPi ROB Ids
  //std::vector<uint32_t> m_ctpRobIds;
  //std::vector<uint32_t> m_muCTPiRobIds;
  std::vector<uint32_t> m_ALFARobIds;

  /// trigger muCTPi RoIs from L1 and DAQ ROB
  //std::vector<ROIB::MuCTPIRoI>     m_lvl1muCTPIRoIs;    // RoIs from RoIB muCTPi ROB
  //std::vector<ROIB::MuCTPIRoI>     m_daqmuCTPIRoIs;     // RoIs from DAQ muCTPi ROB

  //std::vector<uint32_t>            m_lvl1muCTPIHash_Endcap;  // Hash for RoIs from RoIB 
  //std::vector<uint32_t>            m_lvl1muCTPIHash_Forward; // Hash for RoIs from RoIB 
  //std::vector<uint32_t>            m_lvl1muCTPIHash_Barrel;  // Hash for RoIs from RoIB 

  //std::vector<uint32_t>            m_daqmuCTPIHash_Endcap;   // Hash for RoIs from DAQ ROB 
  //std::vector<uint32_t>            m_daqmuCTPIHash_Forward;  // Hash for RoIs from DAQ ROB 
  //std::vector<uint32_t>            m_daqmuCTPIHash_Barrel;   // Hash for RoIs from DAQ ROB 

  Histo1DProperty                  m_histProp_NumberOfRoIs;

  std::map<std::string, int> m_map_TrgNamesToHistGroups;
  std::map<int, int>         m_map_TrgItemNumbersToHistGroups;

  //mutable std::map<int, int>  m_triggerHitPatternReady;
  //mutable std::map<int, int>  m_triggerHitPattern;

  std::string m_pathHisto;

  //TrigConf::HLTChain* m_HLTcostMon_chain;

  int m_elast15 {0}, m_elast18 {0};     // ctp-items id numbers to select golden alfa trigger for data quality assesment
  //mutable int m_nbOfTracksInDetectors[8]; // counters for track candidates - needed in data quality assesment

// ALFA extensions
// geometry data
  const int m_mbNb2RP[8]={2,1,8,3,5,6,7,4};
  const int m_maroc2mapmt[64]={56,32,40,41,24,33,16,25,48,17,57,18,26,43,59,51,35,49,60,58,52,61,44,54,62,63,50,53,34,45,36,42,46,55,27,37,31,47,28,38,39,30,23,10,29,15,22,14,20,9,7,21,19,4,12,6,13,5,11,3,2,1,8,0}; 
  const int m_maroc2fiber[64]={0,6,4,12,1,14,3,9,2,11,8,19,17,28,24,26,30,10,32,16,34,40,36,50,48,56,18,42,22,44,38,20,52,58,25,46,57,60,33,54,62,49,59,21,41,61,51,53,35,13,63,43,27,39,37,55,45,47,29,31,23,15,5,7};
  const int m_pmf2layer[24]={0,19,-1,-1,-1,15,17,18,14,16,11,13,10,12,7,9,1,6,8,3,5,0,2,4};

#include "../src/TrigALFAROBMon_geomTable.icc"

  //mutable std::vector <float> m_pU[8][10];
  //mutable std::vector <float> m_pV[8][10];

  const float m_y_min[2] = {0.,-35.};
  const float m_y_max[2] = {35.,0.};

  //mutable bool m_sFiberHitsODPos[8][3][30],  m_sFiberHitsODNeg[8][3][30];

  const std::vector<std::string> m_stationNames {"B7L1U", "B7L1L", "A7L1U", "A7L1L", "A7R1U", "A7R1L", "B7R1U", "B7R1L"};
  const std::vector<std::string> m_trigConditions{"elastic", "elastic_ALFA_BG", "singleDiffr", "ALFA_MBTS_singleDiffr", "ALFA_LUCID_singleDiffr", "ALFA_EM3", "ALFA_J12", "ALFA_TRT", "ANY", "ANY_UNPAIRED_ISO", "ANY_ALFA_BG", "ALFA_EMPTY"};


  /// Helper for checksum test
  /// returns true if a ROB checksum failed
  bool verifyROBChecksum(const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment& robFrag) const;

  bool verifyALFAROBChecksum(const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment& robFrag) const;

  /// Helper for status bits test
  void verifyROBStatusBits(const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment& robFrag) const ;

  /// Helper for decoding the ALFA ROB 
  uint32_t  decodeALFA(const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment& robFrag, std::vector<float> (&loc_pU) [8][10], 
                       std::vector<float> (&loc_pV) [8][10],
                       bool FiberHitsODNeg[][3][30], bool FiberHitsODPos[][3][30],
                       std::map<int,int>& triggerHitPattern, std::map<int,int>& triggerHitPatternReady) const;

  void   decodeRealPMT(uint32_t dataWord, uint32_t quarter, uint32_t mbNb, uint32_t pmf, std::vector<float> (&loc_pU) [8][10], 
                       std::vector<float> (&loc_pV) [8][10],
                       bool FiberHitsODNeg[][3][30], bool FiberHitsODPos[][3][30]) const;

  uint32_t  decodePMT0(uint32_t dataWord) const;

  /// find tacks in ALFA detectors
  void findALFATracks(const ROIB::RoIBResult* roIBResult, const int lumiBlockNb, const bool SBflag, 
                      std::vector<float> (&loc_pU) [8][10], std::vector<float> (&loc_pV) [8][10]) const;

  // find OD tracks and calculate distance
  void findODTracks (bool FiberHitsODNeg[][3][30], bool FiberHitsODPos[][3][30], std::map<int,int>& triggerHitPattern,std::map<int,int>& triggerHitPatternReady) const;

  // get lvl1 results to seed track histograms depending on the trigger items
  bool getLvl1Result(LVL1CTP::Lvl1Result &resultL1) const;

  bool getHLTResult(HLT::HLTResult &resultHLT) const;

  /// Helper to print contents of a muCTPi RoIB data word 
  void dumpRoIBDataWord(uint32_t data_word ) const;

  // routines to reset selected set of histograms
  void reset1LBhistos() const;
  void reset10LBhistos() const;
  void reset60LBhistos() const;
};
