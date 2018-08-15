/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/HistoProperty.h"
#include "ByteStreamData/RawEvent.h"
#include "eformat/Status.h"
#include <stdint.h>

#include "TrigT1Result/MuCTPIRoI.h"
#include "TrigConfInterfaces/ITrigConfigSvc.h"
#include "TrigConfInterfaces/ILVL1ConfigSvc.h"

#include "TrigSteeringEvent/Lvl1Result.h"
#include "TrigSteeringEvent/HLTResult.h"

#include "TrigConfHLTData/HLTChain.h"
#include "TrigConfHLTData/HLTChainList.h"


/////////////////////////////////////////////////////////////////////////////

namespace ROIB {
  class MuCTPIResult;
}

class MuCTPI_RDO;
class IROBDataProviderSvc;
class ITrigROBDataProviderSvc;

class TH1F;       /// for monitoring purposes
class TH2F;       /// for monitoring purposes
class TProfile2D; /// for monitoring purposes

class TrigALFAROBMonitor:public AthAlgorithm {
public:
  TrigALFAROBMonitor(const std::string& name, ISvcLocator* pSvcLocator);

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  StatusCode beginRun();  
  StatusCode endRun();

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
  IIROBDataProviderSvc_t           m_robDataProviderSvc;
  /// Reference to a ROBDataProviderSvc which implements also the trigger additions
  SmartIF<ITrigROBDataProviderSvc> m_trigROBDataProviderSvc;

  /// Source identifiers for ROB fragments
  IntegerProperty                  m_lvl1CTPROBid ;
  IntegerProperty                  m_lvl1ALFA1ROBid ;
  IntegerProperty                  m_lvl1ALFA2ROBid ;
  IntegerProperty                  m_daqCTPROBid ;

  UnsignedIntegerProperty          m_ctpModuleID;

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

  bool                             m_FiberHitsODNeg[8][3][30], m_FiberHitsODPos[8][3][30];

  /// Switch for ROB status bit histograms
  BooleanProperty                  m_doROBStatus;
  TH2F*                            m_hist_genericStatusForROB;
  TH2F*                            m_hist_specificStatusForROB;
  std::map<eformat::GenericStatus, std::string> m_map_GenericStatus;
  std::vector<std::string>                      m_vec_SpecificStatus;

  /// pointers to the CTP and muCTPi result objects
  ROIB::MuCTPIResult*              m_lvl1muCTPIResult;  // RoIB muCTPi Result

  int m_LB; // luminosity block number
  int m_previousEventLB; // luminosity block number of the previous events
  int m_prevLB10reset;   // LB at which previous reset of 10LB histograms happened
  int m_prevLB60reset;   // LB  -- 60LB histograms were reset
  uint32_t m_prescKey; // current hlt prescale key

  BooleanProperty                  m_doTiming;
  TH1F*                            m_hist_timeALFA;
  Histo1DProperty                  m_histProp_timeALFA;

  /// vectors with CTP and muCTPi ROB Ids
  std::vector<uint32_t> m_ctpRobIds;
  std::vector<uint32_t> m_muCTPiRobIds;
  std::vector<uint32_t> m_ALFARobIds;

  /// trigger muCTPi RoIs from L1 and DAQ ROB
  std::vector<ROIB::MuCTPIRoI>     m_lvl1muCTPIRoIs;    // RoIs from RoIB muCTPi ROB
  std::vector<ROIB::MuCTPIRoI>     m_daqmuCTPIRoIs;     // RoIs from DAQ muCTPi ROB

  std::vector<uint32_t>            m_lvl1muCTPIHash_Endcap;  // Hash for RoIs from RoIB 
  std::vector<uint32_t>            m_lvl1muCTPIHash_Forward; // Hash for RoIs from RoIB 
  std::vector<uint32_t>            m_lvl1muCTPIHash_Barrel;  // Hash for RoIs from RoIB 

  std::vector<uint32_t>            m_daqmuCTPIHash_Endcap;   // Hash for RoIs from DAQ ROB 
  std::vector<uint32_t>            m_daqmuCTPIHash_Forward;  // Hash for RoIs from DAQ ROB 
  std::vector<uint32_t>            m_daqmuCTPIHash_Barrel;   // Hash for RoIs from DAQ ROB 

  Histo1DProperty                  m_histProp_NumberOfRoIs;

  std::map<std::string, int> m_map_TrgNamesToHistGroups;
  std::map<int, int>         m_map_TrgItemNumbersToHistGroups;

  std::map<int, int>  m_triggerHitPatternReady;
  std::map<int, int>  m_triggerHitPattern;

  std::string m_pathHisto;

  bool                m_SBflag;
  TrigConf::HLTChain* m_HLTcostMon_chain;

  int m_elast15, m_elast18;     // ctp-items id numbers to select golden alfa trigger for data quality assesment
  int m_nbOfTracksInDetectors[8]; // counters for track candidates - needed in data quality assesment

  float m_ODtracks[8][2]; // track coordinates in OD

  //std::vector<std::string> m_vect_TrgName_Elastic;
  //std::vector<std::string> m_vect_TrgName_ElasticInAlfaBG;
  //std::vector<std::string> m_vect_TrgName_AlfaSingleDiffr;
  //std::vector<std::string> m_vect_TrgName_AlfaMbtsSinglDiffr;
  //std::vector<std::string> m_vect_TrgName_AlfaLucidSinglDiffr;
  //std::vector<std::string> m_vect_TrgName_AlfaEm3;
  //std::vector<std::string> m_vect_TrgName_AlfaJ12;
  //std::vector<std::string> m_vect_TrgName_AlfaTRT;
  //std::vector<std::string> m_vect_TrgName_AlfaAny;
  //std::vector<std::string> m_vect_TrgName_AlfaNayUnpairedIso;
  //std::vector<std::string> m_vect_TrgName_AlfaAnyAlfaBg;
  //std::vector<std::string> m_vect_TrgName_AlfaEmpty;

// ALFA extensions
// geometry data
  int mbNb2RP[8]={2,1,8,3,5,6,7,4};
int maroc2mapmt[64]={56,32,40,41,24,33,16,25,48,17,57,18,26,43,59,51,35,49,60,58,52,61,44,54,62,63,50,53,34,45,36,42,46,55,27,37,31,47,28,38,39,30,23,10,29,15,22,14,20,9,7,21,19,4,12,6,13,5,11,3,2,1,8,0}; 
  int maroc2fiber[64]={0,6,4,12,1,14,3,9,2,11,8,19,17,28,24,26,30,10,32,16,34,40,36,50,48,56,18,42,22,44,38,20,52,58,25,46,57,60,33,54,62,49,59,21,41,61,51,53,35,13,63,43,27,39,37,55,45,47,29,31,23,15,5,7};
  int pmf2layer[24]={0,19,-1,-1,-1,15,17,18,14,16,11,13,10,12,7,9,1,6,8,3,5,0,2,4};

#include "../src/TrigALFAROBMon_geomTable.cxx"

  std::vector <float> m_pU[8][10];
  std::vector <float> m_pV[8][10];

  float m_y_min[2] = {0.,-35.};
  float m_y_max[2] = {35.,0.};

bool m_sFiberHitsODPos[8][3][30],  m_sFiberHitsODNeg[8][3][30];

  std::vector<std::string> m_stationNames;
  std::vector<std::string> m_trigConditions;


  /// Helper for checksum test
  /// returns true if a ROB checksum failed
  bool verifyROBChecksum(const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment& robFrag);

  bool verifyALFAROBChecksum(const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment& robFrag);

  /// Helper for status bits test
  void verifyROBStatusBits(const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment& robFrag);

  /// Helper for decoding the ALFA ROB 
  uint32_t  decodeALFA(const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment& robFrag);
  void decodeRealPMT (uint32_t dataWord, uint32_t quarter, uint32_t mbNb, uint32_t pmf);
  uint32_t  decodePMT0 (uint32_t dataWord);

  /// find tacks in ALFA detectors
  void findALFATracks(const LVL1CTP::Lvl1Result &resultL1);

  // find OD tracks and calculate distance
  void findODTracks ();

  // get lvl1 results to seed track histograms depending on the trigger items
  bool getLvl1Result(LVL1CTP::Lvl1Result &resultL1);

  bool getHLTResult(HLT::HLTResult &resultHLT);

  /// Helper to print contents of a muCTPi RoIB data word 
  void dumpRoIBDataWord(uint32_t data_word );

  // routines to reset selected set of histograms
  void reset1LBhistos(int lbNumber);
  void reset10LBhistos(int lbNumber);
  void reset60LBhistos(int lbNumber);
};
