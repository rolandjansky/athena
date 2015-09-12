/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/MsgStream.h"
#include "ByteStreamData/RawEvent.h"
#include "GaudiKernel/HistoProperty.h"
#include "eformat/Status.h"
#include <stdint.h>

#include "TrigT1Result/MuCTPIRoI.h"
#include "TrigConfInterfaces/ITrigConfigSvc.h"
#include "TrigConfInterfaces/ILVL1ConfigSvc.h"

/////////////////////////////////////////////////////////////////////////////

namespace ROIB {
  class MuCTPIResult;
}

class MuCTPI_RDO;
class StoreGateSvc;
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
  /** @brief Pointer to MsgStream.*/
  MsgStream* m_msg;

  /**
  * @brief Accessor method for the MsgStream.
  * @return handle to the MsgStream.
  */
  inline MsgStream& logStream() const { return *m_msg; }
	
  /**
   * @brief Accessor method for the message level variable.
   * @return value of the message level for this algorithm.
   */
  inline MSG::Level logLevel() const { return  (m_msg != 0) ? m_msg->level() : MSG::NIL; }

  typedef ServiceHandle<StoreGateSvc> StoreGateSvc_t;
  typedef ServiceHandle<TrigConf::ITrigConfigSvc> TrigConfigSvc_t;
  typedef ServiceHandle<TrigConf::ILVL1ConfigSvc> Lvl1ConfigSvc_t;

  /// Reference to StoreGateSvc;
  StoreGateSvc_t                   m_storeGateSvc;
  TrigConfigSvc_t		   m_configSvc;
  Lvl1ConfigSvc_t		   m_lvl1ConfSvc;
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
  TH2F*                            m_hist_ALFA_trig_validated_tracks[8][8]= {{0}}; //8 trigger condition &  8 alfa stations
  BooleanProperty                  m_doPMFMonitoring;
  TH2F*                            m_hist_pmfMonitoring[8]={0};

  /// Switch for ROB status bit histograms
  BooleanProperty                  m_doROBStatus;
  TH2F*                            m_hist_genericStatusForROB;
  TH2F*                            m_hist_specificStatusForROB;
  std::map<eformat::GenericStatus, std::string> m_map_GenericStatus;
  std::vector<std::string>                      m_vec_SpecificStatus;

  /// vectors with CTP and muCTPi ROB Ids
  std::vector<uint32_t> m_ctpRobIds;
  std::vector<uint32_t> m_muCTPiRobIds;
  std::vector<uint32_t> m_ALFARobIds;

  /// pointers to the CTP and muCTPi result objects
  ROIB::MuCTPIResult*              m_lvl1muCTPIResult;  // RoIB muCTPi Result
  MuCTPI_RDO*                      m_daqmuCTPIResult;   // DAQ  muCTPi Result  

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

  BooleanProperty                  m_doTiming;
  TH1F*                            m_hist_timeALFA;
  Histo1DProperty                  m_histProp_timeALFA;


// ALFA extensions
// geometry data
  int pmf2layer[24]={0,19,-1,-1,-1,15,17,18,14,16,11,13,10,12,7,9,1,6,8,3,5,0,2,4};
  int maroc2fiber[64]={0,6,4,12,1,14,3,9,2,11,8,19,17,28,24,26,30,10,32,16,34,40,36,50,48,56,18,42,22,44,38,20,52,58,25,46,57,60,33,54,62,49,59,21,41,61,51,53,35,13,63,43,27,39,37,55,45,47,29,31,23,15,5,7};

#include "../TrigOnlineMonitor/share/TrigALFAROBMon_geomTable.dat"

  std::vector <float> m_pU[8][10];
  std::vector <float> m_pV[8][10];


  /// Helper for checksum test
  /// returns true if a ROB checksum failed
  bool verifyROBChecksum(MsgStream& log, OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment robFrag);

  bool verifyALFAROBChecksum(MsgStream& log, OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment robFrag);

  /// Helper for status bits test
  void verifyROBStatusBits(MsgStream& log, OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment robFrag);

  /// Helper for decoding the ALFA ROB 
  void decodeALFA(MsgStream& log, OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment robFrag);
  void decodeRealPMT (MsgStream& log, uint32_t dataWord, uint32_t quarter, uint32_t mbNb, uint32_t pmf);
  uint32_t  decodePMT0 (uint32_t dataWord);

  /// find tacks in ALFA detectors
  void findALFATracks();

  /// Helper to print contents of a muCTPi RoIB data word 
  void dumpRoIBDataWord(MsgStream& log, uint32_t data_word );
};
