/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/SmartIF.h"
#include "ByteStreamData/RawEvent.h"
#include "GaudiKernel/HistoProperty.h"
#include "eformat/Status.h"
#include <stdint.h>

#include "TrigT1Result/MuCTPIRoI.h"

/////////////////////////////////////////////////////////////////////////////

namespace ROIB {
  class MuCTPIResult;
}
class MuCTPI_RDO;
class IROBDataProviderSvc;
class TH1F;       /// for monitoring purposes
class TH2F;       /// for monitoring purposes
class TProfile2D; /// for monitoring purposes

class TrigMuCTPiROBMonitor:public AthAlgorithm {
public:
  TrigMuCTPiROBMonitor(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  StatusCode start();  

private:

  typedef ServiceHandle<IROBDataProviderSvc> IIROBDataProviderSvc_t;
  /// Reference to the ROBDataProviderSvc service
  IIROBDataProviderSvc_t           m_robDataProviderSvc;

  /// Source identifiers for ROB fragments
  IntegerProperty                  m_lvl1CTPROBid ;
  IntegerProperty                  m_lvl1MuCTPiROBid ;
  IntegerProperty                  m_daqCTPROBid ;
  IntegerProperty                  m_daqMuCTPiROBid ;

  /// Number of pt Thresholds
  static const uint32_t NUMBER_OF_PT_THRESHOLDS = 6;   // Number of pT thresholds

  /// Number of sector IDs
  static const uint32_t NUMBER_OF_ENDCAP_UNITS  = 48;   // Number of endcap  sector IDs
  static const uint32_t NUMBER_OF_FORWARD_UNITS = 24;   // Number of forward sector IDs
  static const uint32_t NUMBER_OF_BARREL_UNITS  = 32;   // Number of barrel  sector IDs
  IntegerProperty                  m_Number_Of_Endcap_Units ;
  IntegerProperty                  m_Number_Of_Forward_Units ;
  IntegerProperty                  m_Number_Of_Barrel_Units ;

  /// Switch for setting the debug StreamTag and name for debug stream
  BooleanProperty                  m_setDebugStream;
  StringProperty                   m_debugStreamName;

  /// Switch for ROB checksum test
  BooleanProperty                  m_doROBChecksum;
  TH1F*                            m_hist_failedChecksumForROB;
  Histo1DProperty                  m_histProp_failedChecksumForROB;
  TH1F*                            m_hist_failedChecksumForSD;
  Histo1DProperty                  m_histProp_failedChecksumForSD;

  /// Switch for ROB status bit histograms
  BooleanProperty                  m_doROBStatus;
  TH2F*                            m_hist_genericStatusForROB;
  TH2F*                            m_hist_specificStatusForROB;
  std::map<eformat::GenericStatus, std::string> m_map_GenericStatus;
  std::vector<std::string>                      m_vec_SpecificStatus;

  /// vectors with CTP and muCTPi ROB Ids
  std::vector<uint32_t> m_ctpRobIds;
  std::vector<uint32_t> m_muCTPiRobIds;

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

  /// MuCTPi monitoring histograms
  TH1F*                            m_hist_NumberOfRoIs_RoIB;
  TH1F*                            m_hist_NumberOfRoIs_DAQ;
  Histo1DProperty                  m_histProp_NumberOfRoIs;

  std::vector<TH1F*>               m_hist_muCTPiL1_Barrel_SectorID;
  std::vector<TH1F*>               m_hist_muCTPiDaq_Barrel_SectorID;
  Histo1DProperty                  m_histProp_muCTPi_Barrel_SectorID;

  std::vector<TH1F*>               m_hist_muCTPiL1_Endcap_SectorID;
  std::vector<TH1F*>               m_hist_muCTPiDaq_Endcap_SectorID;
  Histo1DProperty                  m_histProp_muCTPi_Endcap_SectorID;

  std::vector<TH1F*>               m_hist_muCTPiL1_Forward_SectorID;
  std::vector<TH1F*>               m_hist_muCTPiDaq_Forward_SectorID;
  Histo1DProperty                  m_histProp_muCTPi_Forward_SectorID;

  std::vector<TH2F*>               m_hist_muCTPiL1_Barrel_SectorID_Pad;
  std::vector<TH2F*>               m_hist_muCTPiDaq_Barrel_SectorID_Pad;
  TProfile2D*                      m_hist_muCTPiDaq_Barrel_SectorID_Pad_DeltaBCID;

  TH1F*                            m_hist_differenceRoIs;
  Histo1DProperty                  m_histProp_differenceRoIs;

  TH1F*                            m_hist_muCTPiL1_Problem_Barrel_Hash;
  TH1F*                            m_hist_muCTPiDaq_Problem_Barrel_Hash;
  Histo1DProperty                  m_histProp_Problem_Barrel_Hash;

  TH1F*                            m_hist_muCTPiL1_Problem_Endcap_Hash;
  TH1F*                            m_hist_muCTPiDaq_Problem_Endcap_Hash;
  Histo1DProperty                  m_histProp_Problem_Endcap_Hash;

  TH1F*                            m_hist_muCTPiL1_Problem_Forward_Hash;
  TH1F*                            m_hist_muCTPiDaq_Problem_Forward_Hash;
  Histo1DProperty                  m_histProp_Problem_Forward_Hash;

  BooleanProperty                  m_doTiming;
  TH1F*                            m_hist_timeMuCTPi;
  Histo1DProperty                  m_histProp_timeMuCTPi;

  /// Helper for checksum test
  /// returns true if a ROB checksum failed
  bool verifyROBChecksum(OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment robFrag);

  /// Helper for status bits test
  void verifyROBStatusBits(OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment robFrag);

  /// Helper for decoding the muCTPi RoIB and DAQ ROB
  void decodeMuCTPi(OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment robFrag);

  /// Helper for converting a mirod DAQ data word to a muCTPi RoIB data word 
  uint32_t mirodToRoIBDataWord(uint32_t data_word );

  /// Helper to print contents of a muCTPi RoIB data word 
  void dumpRoIBDataWord(uint32_t data_word );
};
