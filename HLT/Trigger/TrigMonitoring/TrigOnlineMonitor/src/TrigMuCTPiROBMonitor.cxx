/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigOnlineMonitor/TrigMuCTPiROBMonitor.h"
#include "TrigT1Result/RoIBResult.h"
#include "TrigT1Result/MuCTPI_RDO.h"
#include "TrigT1Result/MuCTPI_MultiplicityWord_Decoder.h"
#include "TrigT1Result/MuCTPI_DataWord_Decoder.h"
#include "GaudiKernel/ITHistSvc.h"
#include "AthenaKernel/Timeout.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "AthenaMonitoringKernel/OHLockedHist.h"
#include "EventInfo/TriggerInfo.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/EventType.h"
#include "eformat/eformat.h"
#include "eformat/index.h"

#include <algorithm>
#include <cassert>
#include <bitset>
#include <TH1F.h>
#include <TH2F.h>
#include <TProfile2D.h>

const uint32_t TrigMuCTPiROBMonitor::NUMBER_OF_ENDCAP_UNITS;
const uint32_t TrigMuCTPiROBMonitor::NUMBER_OF_FORWARD_UNITS;
const uint32_t TrigMuCTPiROBMonitor::NUMBER_OF_BARREL_UNITS;

/////////////////////////////////////////////////////////////////////////////

TrigMuCTPiROBMonitor::TrigMuCTPiROBMonitor(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator), 
  m_robDataProviderSvc( "ROBDataProviderSvc", name ),
  m_hist_failedChecksumForROB(0),
  m_histProp_failedChecksumForROB(Gaudi::Histo1DDef("FailedChecksumForROB" ,0,1,1)),
  m_hist_failedChecksumForSD(0),
  m_histProp_failedChecksumForSD(Gaudi::Histo1DDef("FailedChecksumForSubDetector" ,0,2,2)),
  m_hist_genericStatusForROB(0),
  m_hist_specificStatusForROB(0),
  m_lvl1muCTPIResult(0),
  m_daqmuCTPIResult(0),
  m_hist_NumberOfRoIs_RoIB(0),
  m_hist_NumberOfRoIs_DAQ(0),
  m_histProp_NumberOfRoIs(Gaudi::Histo1DDef("Number_of_RoIs" ,0.,50.,50)),
  m_histProp_muCTPi_Barrel_SectorID( Gaudi::Histo1DDef("muCTPi_SectorID_Barrel"  ,0,NUMBER_OF_BARREL_UNITS,  NUMBER_OF_BARREL_UNITS)),
  m_histProp_muCTPi_Endcap_SectorID( Gaudi::Histo1DDef("muCTPi_SectorID_Endcap"  ,0,NUMBER_OF_ENDCAP_UNITS,  NUMBER_OF_ENDCAP_UNITS)),
  m_histProp_muCTPi_Forward_SectorID(Gaudi::Histo1DDef("muCTPi_SectorID_Forward" ,0,NUMBER_OF_FORWARD_UNITS, NUMBER_OF_FORWARD_UNITS)),
  m_hist_muCTPiDaq_Barrel_SectorID_Pad_DeltaBCID(0),
  m_hist_differenceRoIs(0),
  m_histProp_differenceRoIs(Gaudi::Histo1DDef("muCTPi_differenceRoIs_RoIB_DAQ" ,-50.,50.,100)),
  m_hist_muCTPiL1_Problem_Barrel_Hash(0),
  m_hist_muCTPiDaq_Problem_Barrel_Hash(0),
  m_histProp_Problem_Barrel_Hash(Gaudi::Histo1DDef("muCTPi_Barrel_Hash_for_RoIs_only_in" ,0,1,1)),
  m_hist_muCTPiL1_Problem_Endcap_Hash(0),
  m_hist_muCTPiDaq_Problem_Endcap_Hash(0),
  m_histProp_Problem_Endcap_Hash(Gaudi::Histo1DDef("muCTPi_Endcap_Hash_for_RoIs_only_in" ,0,1,1)),
  m_hist_muCTPiL1_Problem_Forward_Hash(0),
  m_hist_muCTPiDaq_Problem_Forward_Hash(0),
  m_histProp_Problem_Forward_Hash(Gaudi::Histo1DDef("muCTPi_Forward_Hash_for_RoIs_only_in" ,0,1,1)),
  m_hist_timeMuCTPi(0),
  m_histProp_timeMuCTPi(Gaudi::Histo1DDef("Time_MuCTPi_Monitor" ,0.,2.,50))
{
  // Declare the properties
  declareProperty("Lvl1CTPROBid",                       m_lvl1CTPROBid=0x770001);
  declareProperty("Lvl1MuCTPiROBid",                    m_lvl1MuCTPiROBid=0x760001);
  declareProperty("DaqCTPROBid",                        m_daqCTPROBid=0x770000);
  declareProperty("DaqMuCTPiROBid",                     m_daqMuCTPiROBid=0x760000);
  declareProperty("SetDebugStream",                     m_setDebugStream=false);
  declareProperty("DebugStreamName",                    m_debugStreamName="MuCTPiROBErrorStream");
  declareProperty("NumberOfBarrelUnits",                m_Number_Of_Barrel_Units=NUMBER_OF_BARREL_UNITS);
  declareProperty("NumberOfEndcapUnits",                m_Number_Of_Endcap_Units=NUMBER_OF_ENDCAP_UNITS);
  declareProperty("NumberOfForwardUnits",               m_Number_Of_Forward_Units=NUMBER_OF_FORWARD_UNITS);
  declareProperty("TestROBChecksum",                    m_doROBChecksum=true);
  declareProperty("HistFailedChecksumForROB",           m_histProp_failedChecksumForROB,"ROBs with inconsistent checksum");
  declareProperty("HistFailedChecksumForSD",            m_histProp_failedChecksumForSD, "SDs with inconsistent checksum");
  declareProperty("TestROBStatus",                      m_doROBStatus=true);
  declareProperty("HistNumberOfRoIs",                   m_histProp_NumberOfRoIs, "Number of MuCTPi RoIs");
  declareProperty("HistSectorID_Barrel_muCTPi",         m_histProp_muCTPi_Barrel_SectorID, "SectorID for Barrel RoIs");
  declareProperty("HistSectorID_Endcap_muCTPi",         m_histProp_muCTPi_Endcap_SectorID, "SectorID for Endcap RoIs");
  declareProperty("HistSectorID_Forward_muCTPi",        m_histProp_muCTPi_Forward_SectorID,"SectorID for Forward RoIs");
  declareProperty("HistDifferenceRoIs_RoIB_DAQ_muCTPi", m_histProp_differenceRoIs,"Difference in number of Rois for RoIB-DAQ");
  declareProperty("HistBarrelHashforProblemRoIs",       m_histProp_Problem_Barrel_Hash,  "Hash for problematic Barrel RoIs");
  declareProperty("HistEndcapHashforProblemRoIs",       m_histProp_Problem_Endcap_Hash,  "Hash for problematic Endcap RoIs");
  declareProperty("HistForwardHashforProblemRoIs",      m_histProp_Problem_Forward_Hash, "Hash for problematic Forward RoIs");
  declareProperty("DoTiming",                           m_doTiming=true);
  declareProperty("HistTimeMuCTPiMonitor",              m_histProp_timeMuCTPi, "Timing for muCTPi monitoring algorithm");

  // fill map with generic status codes
  m_map_GenericStatus = {{eformat::UNCLASSIFIED      , "UNCLASSIFIED"},
                         {eformat::BCID_CHECK_FAIL   , "BCID_CHECK_FAIL"},
                         {eformat::LVL1ID_CHECK_FAIL , "LVL1ID_CHECK_FAIL"},
                         {eformat::TIMEOUT           , "TIMEOUT"},
                         {eformat::DATA_CORRUPTION   , "DATA_CORRUPTION"},
                         {eformat::INTERNAL_OVERFLOW , "INTERNAL_OVERFLOW"}};

  // fill vector with specific status codes
  m_vec_SpecificStatus = {"TRIGGER_TYPE_SYNC_ERROR",
                          "FRAGMENT_SIZE_ERROR",
                          "DATABLOCK_ERROR",
                          "CTRL_WORD_ERROR",
                          "MISSING_BOF",
                          "MISSING_EOF",
                          "INVALID_HEADER_MARKER",
                          "FORMAT_ERROR",
                          "DUPLICATE_EVENT",
                          "SEQUENCE_ERROR",
                          "TRANSMISSION_ERROR",
                          "TRUNCATION",
                          "SHORT_FRAGMENT",
                          "FRAGMENT_LOST",
                          "FRAGMENT_PENDING",
                          "ROL_DISABLED"};

  // muCTPi Sector ID plots per hemisphere
  m_hist_muCTPiL1_Barrel_SectorID = {nullptr,nullptr};
  m_hist_muCTPiL1_Endcap_SectorID = {nullptr,nullptr};
  m_hist_muCTPiL1_Forward_SectorID = {nullptr,nullptr};
  m_hist_muCTPiDaq_Barrel_SectorID = {nullptr,nullptr};
  m_hist_muCTPiDaq_Endcap_SectorID = {nullptr,nullptr};
  m_hist_muCTPiDaq_Forward_SectorID = {nullptr,nullptr};

  // muCTPi Sector ID vs pad plots
  for (unsigned int threshold=0; threshold <= NUMBER_OF_PT_THRESHOLDS; ++threshold) {
    m_hist_muCTPiL1_Barrel_SectorID_Pad.push_back(0);
    m_hist_muCTPiDaq_Barrel_SectorID_Pad.push_back(0);
  } // end loop over thresholds
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode TrigMuCTPiROBMonitor::initialize(){

  // Get the messaging service
  ATH_MSG_INFO( "initialize()" );

  // Print out the property values
  ATH_MSG_INFO( " ROB ID: Lvl1 CTP                           = " << m_lvl1CTPROBid 
      << std::setw(6) << " (=0x" << MSG::hex << m_lvl1CTPROBid.value() << MSG::dec << ")" );
  ATH_MSG_INFO( " ROB ID: Lvl1 muCTPi                        = " << m_lvl1MuCTPiROBid
      << std::setw(6) << " (=0x" << MSG::hex << m_lvl1MuCTPiROBid.value() << MSG::dec << ")" );
  ATH_MSG_INFO( " ROB ID: DAQ CTP                            = " << m_daqCTPROBid
      << std::setw(6) << " (=0x" << MSG::hex << m_daqCTPROBid.value() << MSG::dec << ")" );
  ATH_MSG_INFO( " ROB ID: DAQ muCTPi                         = " << m_daqMuCTPiROBid
      << std::setw(6) << " (=0x" << MSG::hex << m_daqMuCTPiROBid.value() << MSG::dec << ")" );
  ATH_MSG_INFO( " Put events with ROB errors on DEBUG stream = " << m_setDebugStream );
  ATH_MSG_INFO( "         Name of used DEBUG stream          = " << m_debugStreamName );
  ATH_MSG_INFO( " Do ROB checksum test                       = " << m_doROBChecksum );
  ATH_MSG_INFO( "        Hist:FailedChecksumForROB           = " << m_histProp_failedChecksumForROB );
  ATH_MSG_INFO( "        Hist:FailedChecksumForSD            = " << m_histProp_failedChecksumForSD );
  ATH_MSG_INFO( " Do ROB status test                         = " << m_doROBStatus );
  ATH_MSG_INFO( "        Hist:NumberOfRoIs                   = " << m_histProp_NumberOfRoIs );
  ATH_MSG_INFO( " Number Of Barrel  Sector Units             = " << m_Number_Of_Barrel_Units );
  ATH_MSG_INFO( " Number Of Endcap  Sector Units             = " << m_Number_Of_Endcap_Units );
  ATH_MSG_INFO( " Number Of Forward Sector Units             = " << m_Number_Of_Forward_Units );
  ATH_MSG_INFO( "        Hist:SectorID_Barrel_muCTPi         = " << m_histProp_muCTPi_Barrel_SectorID  );
  ATH_MSG_INFO( "        Hist:SectorID_Endcap_muCTPi         = " << m_histProp_muCTPi_Endcap_SectorID  );
  ATH_MSG_INFO( "        Hist:SectorID_Forward_muCTPi        = " << m_histProp_muCTPi_Forward_SectorID );
  ATH_MSG_INFO( "        Hist:DifferenceRoIs_RoIB-DAQ_muCTPi = " << m_histProp_differenceRoIs );
  ATH_MSG_INFO( "        Hist:BarrelHashforProblemRoIs       = " << m_histProp_Problem_Barrel_Hash );
  ATH_MSG_INFO( "        Hist:EndcapHashforProblemRoIs       = " << m_histProp_Problem_Endcap_Hash );
  ATH_MSG_INFO( "        Hist:ForwardHashforProblemRoIs      = " << m_histProp_Problem_Forward_Hash );
  ATH_MSG_INFO( " Do muCTPi Monitoring Timing                = " << m_doTiming );
  ATH_MSG_INFO( "        Hist:TimeMuCTPiMonitor              = " << m_histProp_timeMuCTPi );


  // Locate the ROBDataProviderSvc
  ATH_CHECK( m_robDataProviderSvc.retrieve() );

  // Build the vectors with ROB Ids to retrieve
  m_ctpRobIds.push_back(m_lvl1CTPROBid.value());
  m_ctpRobIds.push_back(m_daqCTPROBid.value());

  m_muCTPiRobIds.push_back(m_lvl1MuCTPiROBid.value());
  m_muCTPiRobIds.push_back(m_daqMuCTPiROBid.value());

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode TrigMuCTPiROBMonitor::execute() {

  struct timeval time_start;
  struct timeval time_stop;
  if ( m_doTiming.value() ) {
    gettimeofday(&time_start, 0);
  }

  //--------------------------------------------------------------------------
  // check that there is still time left
  //--------------------------------------------------------------------------
  if (Athena::Timeout::instance().reached()) {
    ATH_MSG_INFO( " Time out reached in entry to execute." );
    return StatusCode::SUCCESS;
  }

  //--------------------------------------------------------------------------
  // Loop over all ROB fragments held in the ROBDataProviderSvc and do the checks
  //--------------------------------------------------------------------------
  bool event_with_checksum_failure(false);

  // clean up old decoded results
  if (m_lvl1muCTPIResult) { // delete an old L1 muCTPi result if it exists
    delete m_lvl1muCTPIResult;
    m_lvl1muCTPIResult=0;
  }
  
  if (m_daqmuCTPIResult) {  // delete an old DAQ muCTPi result if it exists
    delete m_daqmuCTPIResult;
    m_daqmuCTPIResult=0;
  }

  m_lvl1muCTPIRoIs.clear();
  m_daqmuCTPIRoIs.clear();
  m_lvl1muCTPIHash_Endcap.clear();
  m_lvl1muCTPIHash_Forward.clear();
  m_lvl1muCTPIHash_Barrel.clear();
  m_daqmuCTPIHash_Endcap.clear();
  m_daqmuCTPIHash_Forward.clear();
  m_daqmuCTPIHash_Barrel.clear();

  // get the muCTPi ROBs
  std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*> muCTPiRobFragmentVec;
  muCTPiRobFragmentVec.reserve(m_muCTPiRobIds.size());
  m_robDataProviderSvc->getROBData(m_muCTPiRobIds,muCTPiRobFragmentVec);
  if (muCTPiRobFragmentVec.size()==0) { 
    ATH_MSG_DEBUG( " No muCTPi ROB found." );
    if ( m_doTiming.value() ) {
      gettimeofday(&time_stop, 0);
      int secs = 0 ;
      if (time_stop.tv_sec >= time_start.tv_sec)
        secs = time_stop.tv_sec - time_start.tv_sec;
	
      int usecs = time_stop.tv_usec - time_start.tv_usec;
      float mtime = static_cast<float>(secs)*1000 + static_cast<float>(usecs)/1000;
	
      //* timing histogram
      if (m_hist_timeMuCTPi) m_hist_timeMuCTPi->Fill(mtime,1.);	
    }
    return StatusCode::SUCCESS;
  } 

  // loop over retrieved ROBs and do checks
  for (const auto& rob : muCTPiRobFragmentVec) {
    // verify checksum
    if (verifyROBChecksum(*rob)) event_with_checksum_failure=true ;

    // verify status bits
    verifyROBStatusBits(*rob);

    // decode the muCTPi ROB
    decodeMuCTPi(*rob);
  }

  // compare the the RoIB and DAQ RoIs when running in EF
  // Total number of RoIs is different
  if (m_lvl1muCTPIRoIs.size() != m_daqmuCTPIRoIs.size()) {
    ATH_MSG_DEBUG(   " ---> RoI Number mismatch:"
                     << " number of RoIB RoIs = " << m_lvl1muCTPIRoIs.size()
                     << " number of DAQ  RoIs = " << m_daqmuCTPIRoIs.size());
    float diff_roi = float(m_lvl1muCTPIRoIs.size())- float(m_daqmuCTPIRoIs.size());
    if (m_hist_differenceRoIs) m_hist_differenceRoIs->Fill(diff_roi, 1.);
  }

  // Comparison of Barrel RoIs
  if ((m_lvl1muCTPIHash_Barrel.size() != 0) && (m_daqmuCTPIHash_Barrel.size() != 0)) {
    sort(m_lvl1muCTPIHash_Barrel.begin(),m_lvl1muCTPIHash_Barrel.end());
    sort(m_daqmuCTPIHash_Barrel.begin(), m_daqmuCTPIHash_Barrel.end() );

    if ( !equal(m_daqmuCTPIHash_Barrel.begin(), m_daqmuCTPIHash_Barrel.end(), m_lvl1muCTPIHash_Barrel.begin()) ) {
      ATH_MSG_DEBUG(   " ---> RoI mismatch for BARREL: Hash = RoI-ID*1000 + Sector_ID + hemisphere*"
                       << m_Number_Of_Barrel_Units.value() << "\n"
                       << " Hash for RoIB RoIs = " << m_lvl1muCTPIHash_Barrel << "\n"
                       << " Hash for DAQ  RoIs = " << m_daqmuCTPIHash_Barrel);
      std::vector<uint32_t> only_in_RoIB(m_lvl1muCTPIHash_Barrel.size()),
        only_in_DAQ(m_daqmuCTPIHash_Barrel.size());
      std::vector<uint32_t>::iterator only_in_RoIB_it, only_in_DAQ_it;
      only_in_RoIB_it = set_difference( m_lvl1muCTPIHash_Barrel.begin(), m_lvl1muCTPIHash_Barrel.end(),
                                        m_daqmuCTPIHash_Barrel.begin() , m_daqmuCTPIHash_Barrel.end() ,
                                        only_in_RoIB.begin());
      for (std::vector<uint32_t>::iterator it=only_in_RoIB.begin(); it != only_in_RoIB_it; ++it) {
        std::ostringstream ost;
        ost << *it;
        if (m_hist_muCTPiL1_Problem_Barrel_Hash) {
          oh_scoped_lock_histogram lock;
          m_hist_muCTPiL1_Problem_Barrel_Hash->Fill((ost.str()).c_str(), 1.);
          m_hist_muCTPiL1_Problem_Barrel_Hash->LabelsDeflate("X");
        }
      }

      only_in_DAQ_it   = set_difference( m_daqmuCTPIHash_Barrel.begin() , m_daqmuCTPIHash_Barrel.end() ,
                                         m_lvl1muCTPIHash_Barrel.begin(), m_lvl1muCTPIHash_Barrel.end(),
                                         only_in_DAQ.begin());
      for (std::vector<uint32_t>::iterator it=only_in_DAQ.begin(); it != only_in_DAQ_it; ++it) {
        std::ostringstream ost;
        ost << *it;
        if (m_hist_muCTPiDaq_Problem_Barrel_Hash) {
          oh_scoped_lock_histogram lock;
          m_hist_muCTPiDaq_Problem_Barrel_Hash->Fill((ost.str()).c_str(), 1.);
          m_hist_muCTPiDaq_Problem_Barrel_Hash->LabelsDeflate("X");
        }
      }

      ATH_MSG_DEBUG(   " ---> RoI mismatch for BARREL: Hash = RoI-ID*1000 + Sector_ID + hemisphere*"
                       << m_Number_Of_Barrel_Units.value() << "\n"
                       << " Hash for RoIs which are only in the RoIB list = " << only_in_RoIB << "\n"
                       << " Hash for RoIs which are only in the DAQ  list = " << only_in_DAQ
                       );
    }
  }

  // Comparison of Endcap RoIs
  if ((m_lvl1muCTPIHash_Endcap.size() != 0) && (m_daqmuCTPIHash_Endcap.size() != 0)) {
    sort(m_lvl1muCTPIHash_Endcap.begin(),m_lvl1muCTPIHash_Endcap.end());
    sort(m_daqmuCTPIHash_Endcap.begin(), m_daqmuCTPIHash_Endcap.end() );

    if ( !equal(m_daqmuCTPIHash_Endcap.begin(), m_daqmuCTPIHash_Endcap.end(), m_lvl1muCTPIHash_Endcap.begin()) ) {
      ATH_MSG_DEBUG(   " ---> RoI mismatch for ENDCAP: Hash = RoI-ID*1000 + Sector_ID + hemisphere*"
                       << m_Number_Of_Endcap_Units.value() << "\n"
                       << " Hash for RoIB RoIs = " << m_lvl1muCTPIHash_Endcap << "\n"
                       << " Hash for DAQ  RoIs = " << m_daqmuCTPIHash_Endcap);

      std::vector<uint32_t> only_in_RoIB(m_lvl1muCTPIHash_Endcap.size()),
        only_in_DAQ(m_daqmuCTPIHash_Endcap.size());
      std::vector<uint32_t>::iterator only_in_RoIB_it, only_in_DAQ_it;
      only_in_RoIB_it = set_difference( m_lvl1muCTPIHash_Endcap.begin(), m_lvl1muCTPIHash_Endcap.end(),
                                        m_daqmuCTPIHash_Endcap.begin() , m_daqmuCTPIHash_Endcap.end() ,
                                        only_in_RoIB.begin());
      for (std::vector<uint32_t>::iterator it=only_in_RoIB.begin(); it != only_in_RoIB_it; ++it) {
        std::ostringstream ost;
        ost << *it;
        if (m_hist_muCTPiL1_Problem_Endcap_Hash) {
          oh_scoped_lock_histogram lock;
          m_hist_muCTPiL1_Problem_Endcap_Hash->Fill((ost.str()).c_str(), 1.);
          m_hist_muCTPiL1_Problem_Endcap_Hash->LabelsDeflate("X");
        }
      }

      only_in_DAQ_it   = set_difference( m_daqmuCTPIHash_Endcap.begin() , m_daqmuCTPIHash_Endcap.end() ,
                                         m_lvl1muCTPIHash_Endcap.begin(), m_lvl1muCTPIHash_Endcap.end(),
                                         only_in_DAQ.begin());
      for (std::vector<uint32_t>::iterator it=only_in_DAQ.begin(); it != only_in_DAQ_it; ++it) {
        std::ostringstream ost;
        ost << *it;
        if (m_hist_muCTPiDaq_Problem_Endcap_Hash) {
          oh_scoped_lock_histogram lock;
          m_hist_muCTPiDaq_Problem_Endcap_Hash->Fill((ost.str()).c_str(), 1.);
          m_hist_muCTPiDaq_Problem_Endcap_Hash->LabelsDeflate("X");
        }
      }

      ATH_MSG_DEBUG(   " ---> RoI mismatch for ENDCAP: Hash = RoI-ID*1000 + Sector_ID + hemisphere*"
                       << m_Number_Of_Endcap_Units.value() << "\n"
                       << " Hash for RoIs which are only in the RoIB list = " << only_in_RoIB << "\n"
                       << " Hash for RoIs which are only in the DAQ  list = " << only_in_DAQ
                       );
    }
  }

  // Comparison of Forward RoIs
  if ((m_lvl1muCTPIHash_Forward.size() != 0) && (m_daqmuCTPIHash_Forward.size() != 0)) {
    sort(m_lvl1muCTPIHash_Forward.begin(),m_lvl1muCTPIHash_Forward.end());
    sort(m_daqmuCTPIHash_Forward.begin(), m_daqmuCTPIHash_Forward.end() );

    if ( !equal(m_daqmuCTPIHash_Forward.begin(), m_daqmuCTPIHash_Forward.end(), m_lvl1muCTPIHash_Forward.begin()) ) {
      ATH_MSG_DEBUG(   " ---> RoI mismatch for FORWARD: Hash = RoI-ID*1000 + Sector_ID + hemisphere*"
                       << m_Number_Of_Forward_Units.value() << "\n"
                       << " Hash for RoIB RoIs = " << m_lvl1muCTPIHash_Forward << "\n"
                       << " Hash for DAQ  RoIs = " << m_daqmuCTPIHash_Forward
                       );
      std::vector<uint32_t> only_in_RoIB(m_lvl1muCTPIHash_Forward.size()),
        only_in_DAQ(m_daqmuCTPIHash_Forward.size());
      std::vector<uint32_t>::iterator only_in_RoIB_it, only_in_DAQ_it;
      only_in_RoIB_it = set_difference( m_lvl1muCTPIHash_Forward.begin(), m_lvl1muCTPIHash_Forward.end(),
                                        m_daqmuCTPIHash_Forward.begin() , m_daqmuCTPIHash_Forward.end() ,
                                        only_in_RoIB.begin());
      for (std::vector<uint32_t>::iterator it=only_in_RoIB.begin(); it != only_in_RoIB_it; ++it) {
        std::ostringstream ost;
        ost << *it;
        if (m_hist_muCTPiL1_Problem_Forward_Hash) {
          oh_scoped_lock_histogram lock;
          m_hist_muCTPiL1_Problem_Forward_Hash->Fill((ost.str()).c_str(), 1.);
          m_hist_muCTPiL1_Problem_Forward_Hash->LabelsDeflate("X");
        }
      }

      only_in_DAQ_it   = set_difference( m_daqmuCTPIHash_Forward.begin() , m_daqmuCTPIHash_Forward.end() ,
                                         m_lvl1muCTPIHash_Forward.begin(), m_lvl1muCTPIHash_Forward.end(),
                                         only_in_DAQ.begin());
      for (std::vector<uint32_t>::iterator it=only_in_DAQ.begin(); it != only_in_DAQ_it; ++it) {
        std::ostringstream ost;
        ost << *it;
        if (m_hist_muCTPiDaq_Problem_Forward_Hash) {
          oh_scoped_lock_histogram lock;
          m_hist_muCTPiDaq_Problem_Forward_Hash->Fill((ost.str()).c_str(), 1.);
          m_hist_muCTPiDaq_Problem_Forward_Hash->LabelsDeflate("X");
        }
      }

      ATH_MSG_DEBUG(   " ---> RoI mismatch for FORWARD: Hash = RoI-ID*1000 + Sector_ID + hemisphere*"
                       << m_Number_Of_Forward_Units.value() << "\n"
                       << " Hash for RoIs which are only in the RoIB list = " << only_in_RoIB << "\n"
                       << " Hash for RoIs which are only in the DAQ  list = " << only_in_DAQ
                       );
    }
  }


  // if the event shows errors, set the DEBUG stream tag when requested
  if ((m_setDebugStream.value()) && (event_with_checksum_failure)) {
    // get EventInfo
    const EventInfo* p_EventInfo(0);
    StatusCode sc = evtStore()->retrieve(p_EventInfo);
    if(sc.isFailure()) {
      ATH_MSG_ERROR( "Can't get EventInfo object for updating the StreamTag" );
      if ( m_doTiming.value() ) {
        gettimeofday(&time_stop, 0);
        int secs = 0 ;
        if (time_stop.tv_sec >= time_start.tv_sec)
          secs = time_stop.tv_sec - time_start.tv_sec;
        
        int usecs = time_stop.tv_usec - time_start.tv_usec;
        float mtime = static_cast<float>(secs)*1000 + static_cast<float>(usecs)/1000;
        
        //* timing histogram
        if (m_hist_timeMuCTPi) m_hist_timeMuCTPi->Fill(mtime,1.);	
      }
      return sc;
    }

    // set the stream tag
    typedef std::vector< TriggerInfo::StreamTag > StreamTagVector_t;
    if (p_EventInfo) {
      StreamTagVector_t vecStreamTags = p_EventInfo->trigger_info()->streamTags();
      vecStreamTags.push_back( TriggerInfo::StreamTag(m_debugStreamName,"debug",false) );
      // FIXME: const_cast
      const_cast<TriggerInfo*>(p_EventInfo->trigger_info())->setStreamTags(vecStreamTags);
    }
  }

  if ( m_doTiming.value() ) {
    gettimeofday(&time_stop, 0);
    int secs = 0 ;
    if (time_stop.tv_sec >= time_start.tv_sec) secs = time_stop.tv_sec-time_start.tv_sec;
	
    int usecs = time_stop.tv_usec - time_start.tv_usec;
    float mtime = static_cast<float>(secs)*1000 + static_cast<float>(usecs)/1000;
    
    ATH_MSG_DEBUG( " ---> Time used [ms] = " << mtime ); 
	
    //* timing histogram
    if (m_hist_timeMuCTPi) m_hist_timeMuCTPi->Fill(mtime,1.);	
  }

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode TrigMuCTPiROBMonitor::finalize() {

  // delete decoded objects
  if (m_lvl1muCTPIResult) {
    delete m_lvl1muCTPIResult;
    m_lvl1muCTPIResult=0;
  }
  if (m_daqmuCTPIResult) {
    delete m_daqmuCTPIResult;
    m_daqmuCTPIResult=0;
  }

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode TrigMuCTPiROBMonitor::start() {

  // Define histograms only when checks are requested
  if ((not m_doROBChecksum.value()) && (not m_doROBStatus.value())) return StatusCode::SUCCESS;

  // find histogramming service
  ServiceHandle<ITHistSvc> rootHistSvc("THistSvc", name());
  ATH_CHECK(rootHistSvc.retrieve());

  // *-- booking path
  std::string path = "/EXPERT/" + name() + "/";

  // *-- number of bins for sub detector plots (55 SubDet max.)
  uint32_t n_bins_partEBSubDet = m_histProp_failedChecksumForSD.value().bins();

  // Specific source identifiers
  eformat::helper::SourceIdentifier srcID_muCTPi( eformat::TDAQ_MUON_CTP_INTERFACE ,0);
  eformat::helper::SourceIdentifier srcID_CTP( eformat::TDAQ_CTP ,0);

  if ( m_doROBChecksum.value() ) {
    // *-- ROBs with failed checksum
    m_hist_failedChecksumForROB = new TH1F (m_histProp_failedChecksumForROB.value().title().c_str(),
                                            (m_histProp_failedChecksumForROB.value().title()+";ROB id").c_str(),
                                            m_histProp_failedChecksumForROB.value().bins(),
                                            m_histProp_failedChecksumForROB.value().lowEdge(),
                                            m_histProp_failedChecksumForROB.value().highEdge());
    if (m_hist_failedChecksumForROB) {
      m_hist_failedChecksumForROB->SetCanExtend(TH1::kAllAxes);
      if( rootHistSvc->regHist(path + m_hist_failedChecksumForROB->GetName(), m_hist_failedChecksumForROB).isFailure() ) {
        ATH_MSG_WARNING( "Can not register monitoring histogram: " << m_hist_failedChecksumForROB->GetName() );
      }
    }
    
    // *-- Sub detectors with ROBs with failed checksum
    m_hist_failedChecksumForSD = new TH1F (m_histProp_failedChecksumForSD.value().title().c_str(),
                                           (m_histProp_failedChecksumForSD.value().title()+";SD id").c_str(),
                                           m_histProp_failedChecksumForSD.value().bins(),
                                           m_histProp_failedChecksumForSD.value().lowEdge(),
                                           m_histProp_failedChecksumForSD.value().highEdge());
    if (m_hist_failedChecksumForSD) {
      uint32_t n_tmp_bin = 1;
      m_hist_failedChecksumForSD->GetXaxis()->SetBinLabel( n_tmp_bin, srcID_muCTPi.human_detector().c_str() );
      n_tmp_bin++;
      m_hist_failedChecksumForSD->GetXaxis()->SetBinLabel( n_tmp_bin, srcID_CTP.human_detector().c_str() );

      if( rootHistSvc->regHist(path + m_hist_failedChecksumForSD->GetName(), m_hist_failedChecksumForSD).isFailure() ) {
        ATH_MSG_WARNING( "Can not register monitoring histogram: " << m_hist_failedChecksumForSD->GetName() );
      }
    }
  }

  if ( m_doROBStatus.value() ) {
    // *-- Generic Status for ROBs per sub detector
    m_hist_genericStatusForROB = new TH2F ("GenericStatusForROBsFromSubDetectors",
                                           "GenericStatusForROBsFromSubDetectors;SD id;",
                                           n_bins_partEBSubDet,0.,(float) n_bins_partEBSubDet,
                                           m_map_GenericStatus.size(),0., (float) m_map_GenericStatus.size());
    if (m_hist_genericStatusForROB) {
      uint32_t n_tmp_bin = 1;
      m_hist_genericStatusForROB->GetXaxis()->SetBinLabel( n_tmp_bin, srcID_muCTPi.human_detector().c_str() );
      n_tmp_bin++;
      m_hist_genericStatusForROB->GetXaxis()->SetBinLabel( n_tmp_bin, srcID_CTP.human_detector().c_str() );

      n_tmp_bin = 1;
      for (const auto& s : m_map_GenericStatus) {
        m_hist_genericStatusForROB->GetYaxis()->SetBinLabel( n_tmp_bin, s.second.c_str() );
        n_tmp_bin++;
      }
      
      if( rootHistSvc->regHist(path + m_hist_genericStatusForROB->GetName(), m_hist_genericStatusForROB).isFailure() ) {
        ATH_MSG_WARNING( "Can not register monitoring histogram: " << m_hist_genericStatusForROB->GetName() );
      }
    }

    // *-- Specific Status Bits for ROBs per sub detector
    m_hist_specificStatusForROB = new TH2F ("SpecificStatusBitsForROBsFromSubDetectors",
                                            "SpecificStatusBitsForROBsFromSubDetectors;SD id;",
                                            n_bins_partEBSubDet,0.,(float) n_bins_partEBSubDet,
                                            m_vec_SpecificStatus.size(),0., (float) m_vec_SpecificStatus.size());
    if (m_hist_specificStatusForROB) {
      uint32_t n_tmp_bin = 1;
      m_hist_specificStatusForROB->GetXaxis()->SetBinLabel( n_tmp_bin, srcID_muCTPi.human_detector().c_str() );
      n_tmp_bin++;
      m_hist_specificStatusForROB->GetXaxis()->SetBinLabel( n_tmp_bin, srcID_CTP.human_detector().c_str() );

      n_tmp_bin = 1;
      for (const auto& s : m_vec_SpecificStatus) {
        m_hist_specificStatusForROB->GetYaxis()->SetBinLabel( n_tmp_bin, s.c_str() );
        n_tmp_bin++;
      }

      if( rootHistSvc->regHist(path + m_hist_specificStatusForROB->GetName(), m_hist_specificStatusForROB).isFailure() ) {
        ATH_MSG_WARNING( "Can not register monitoring histogram: " << m_hist_specificStatusForROB->GetName() );
      }
    }
  }

  if ( m_doTiming.value() ) {
    // *-- Timing histogram for monitoring algorithm
    m_hist_timeMuCTPi = new TH1F (m_histProp_timeMuCTPi.value().title().c_str(),
                                  (m_histProp_timeMuCTPi.value().title()+";ms").c_str(),
                                  m_histProp_timeMuCTPi.value().bins(),
                                  m_histProp_timeMuCTPi.value().lowEdge(),
                                  m_histProp_timeMuCTPi.value().highEdge());
    if (m_hist_timeMuCTPi) {
      //      m_hist_timeMuCTPi->SetBit(TH1::kCanRebin);
      if( rootHistSvc->regHist(path + m_hist_timeMuCTPi->GetName(), m_hist_timeMuCTPi).isFailure() ) {
        ATH_MSG_WARNING( "Can not register monitoring histogram: " << m_hist_timeMuCTPi->GetName() );
      }
    }
  }

  // muCTPi plots, loop over the hemisperes 
  std::vector<std::string> strHemisphere = {"Hemisphere_0(C)","Hemisphere_1(A)"};
  std::vector<std::string> strThreshold = {"All","PT1","PT2","PT3","PT4","PT5","PT6"};

  // *-- Number of muCTPi RoIs
  // *-- RoIB
  std::string histTitle = m_histProp_NumberOfRoIs.value().title()+std::string("_RoIB");
  m_hist_NumberOfRoIs_RoIB = new TH1F ( histTitle.c_str(),
                                        (histTitle+";Number of RoIs").c_str(),
                                        m_histProp_NumberOfRoIs.value().bins(),
                                        m_histProp_NumberOfRoIs.value().lowEdge(),
                                        m_histProp_NumberOfRoIs.value().highEdge());
  if (m_hist_NumberOfRoIs_RoIB) {
    if( rootHistSvc->regHist(path + m_hist_NumberOfRoIs_RoIB->GetName(), m_hist_NumberOfRoIs_RoIB).isFailure() ) {
      ATH_MSG_WARNING( "Can not register monitoring histogram: " << m_hist_NumberOfRoIs_RoIB->GetName() );
    }
  }

  // *-- Sector_ID vs. Pad_Id for Barrel RoIs from L1
  for (unsigned int threshold=0; threshold <= NUMBER_OF_PT_THRESHOLDS; ++threshold) {
    histTitle = std::string("Sector_ID_vs_Pad_ID_Barrel_L1_Threshold_")+strThreshold[threshold];
    m_hist_muCTPiL1_Barrel_SectorID_Pad[threshold] = 
      new TH2F ( histTitle.c_str(),
                 (histTitle+";(pad_ID+1)*(2*hemispere-1);SD id;").c_str(),
                 17,-8,8,
                 m_Number_Of_Barrel_Units.value(), 0, m_Number_Of_Barrel_Units.value());
    if (m_hist_muCTPiL1_Barrel_SectorID_Pad[threshold]) {
      if( rootHistSvc->regHist(path + m_hist_muCTPiL1_Barrel_SectorID_Pad[threshold]->GetName(), 
                               m_hist_muCTPiL1_Barrel_SectorID_Pad[threshold]).isFailure() ) {
	ATH_MSG_WARNING( "Can not register monitoring histogram: " << m_hist_muCTPiL1_Barrel_SectorID_Pad[threshold]->GetName() );
      }
    }
  } // end loop over threshold

  for (int hemisphere=0; hemisphere < 2; ++hemisphere) {
    // *-- muCTPi ROB: sector ID --- 
    // *-- L1 Endcap 
    histTitle = m_histProp_muCTPi_Endcap_SectorID.value().title()+std::string("_L1_")+strHemisphere[hemisphere];
    m_hist_muCTPiL1_Endcap_SectorID[hemisphere] = new TH1F ( histTitle.c_str(),
                                                             (histTitle+";Sector Id").c_str(),
                                                             m_histProp_muCTPi_Endcap_SectorID.value().bins(),
                                                             m_histProp_muCTPi_Endcap_SectorID.value().lowEdge(),
                                                             m_histProp_muCTPi_Endcap_SectorID.value().highEdge());
    if (m_hist_muCTPiL1_Endcap_SectorID[hemisphere]) {
      if( rootHistSvc->regHist(path + m_hist_muCTPiL1_Endcap_SectorID[hemisphere]->GetName(), 
                               m_hist_muCTPiL1_Endcap_SectorID[hemisphere]).isFailure() ) {
        ATH_MSG_WARNING( "Can not register monitoring histogram: " << m_hist_muCTPiL1_Endcap_SectorID[hemisphere]->GetName() );
      }
    }
    // *-- L1 Forward 
    histTitle = m_histProp_muCTPi_Forward_SectorID.value().title()+std::string("_L1_")+strHemisphere[hemisphere];
    m_hist_muCTPiL1_Forward_SectorID[hemisphere] = new TH1F ( histTitle.c_str(),
                                                              (histTitle+";Sector Id").c_str(),
                                                              m_histProp_muCTPi_Forward_SectorID.value().bins(),
                                                              m_histProp_muCTPi_Forward_SectorID.value().lowEdge(),
                                                              m_histProp_muCTPi_Forward_SectorID.value().highEdge());
    if (m_hist_muCTPiL1_Forward_SectorID[hemisphere]) {
      if( rootHistSvc->regHist(path + m_hist_muCTPiL1_Forward_SectorID[hemisphere]->GetName(), 
                               m_hist_muCTPiL1_Forward_SectorID[hemisphere]).isFailure() ) {
        ATH_MSG_WARNING( "Can not register monitoring histogram: " << m_hist_muCTPiL1_Forward_SectorID[hemisphere]->GetName() );
      }
    }
    // *-- L1 Barrel 
    histTitle = m_histProp_muCTPi_Barrel_SectorID.value().title()+std::string("_L1_")+strHemisphere[hemisphere];
    m_hist_muCTPiL1_Barrel_SectorID[hemisphere] = new TH1F ( histTitle.c_str(),
                                                             (histTitle+";Sector Id").c_str(),
                                                             m_histProp_muCTPi_Barrel_SectorID.value().bins(),
                                                             m_histProp_muCTPi_Barrel_SectorID.value().lowEdge(),
                                                             m_histProp_muCTPi_Barrel_SectorID.value().highEdge());
    if (m_hist_muCTPiL1_Barrel_SectorID[hemisphere]) {
      if( rootHistSvc->regHist(path + m_hist_muCTPiL1_Barrel_SectorID[hemisphere]->GetName(), 
                               m_hist_muCTPiL1_Barrel_SectorID[hemisphere]).isFailure() ) {
        ATH_MSG_WARNING( "Can not register monitoring histogram: " << m_hist_muCTPiL1_Barrel_SectorID[hemisphere]->GetName() );
      }
    }

    // *-- DAQ Endcap
    histTitle = m_histProp_muCTPi_Endcap_SectorID.value().title()+std::string("_DAQ_")+strHemisphere[hemisphere];
    m_hist_muCTPiDaq_Endcap_SectorID[hemisphere] = new TH1F ( histTitle.c_str(),
                                                              (histTitle+";Sector Id").c_str(),
                                                              m_histProp_muCTPi_Endcap_SectorID.value().bins(),
                                                              m_histProp_muCTPi_Endcap_SectorID.value().lowEdge(),
                                                              m_histProp_muCTPi_Endcap_SectorID.value().highEdge());
    if (m_hist_muCTPiDaq_Endcap_SectorID[hemisphere]) {
      if( rootHistSvc->regHist(path + m_hist_muCTPiDaq_Endcap_SectorID[hemisphere]->GetName(),
                               m_hist_muCTPiDaq_Endcap_SectorID[hemisphere]).isFailure() ) {
        ATH_MSG_WARNING( "Can not register monitoring histogram: " << m_hist_muCTPiDaq_Endcap_SectorID[hemisphere]->GetName() );
      }
    }
    // *-- DAQ Forward
    histTitle = m_histProp_muCTPi_Forward_SectorID.value().title()+std::string("_DAQ_")+strHemisphere[hemisphere];
    m_hist_muCTPiDaq_Forward_SectorID[hemisphere] = new TH1F ( histTitle.c_str(),
                                                               (histTitle+";Sector Id").c_str(),
                                                               m_histProp_muCTPi_Forward_SectorID.value().bins(),
                                                               m_histProp_muCTPi_Forward_SectorID.value().lowEdge(),
                                                               m_histProp_muCTPi_Forward_SectorID.value().highEdge());
    if (m_hist_muCTPiDaq_Forward_SectorID[hemisphere]) {
      if( rootHistSvc->regHist(path + m_hist_muCTPiDaq_Forward_SectorID[hemisphere]->GetName(),
                               m_hist_muCTPiDaq_Forward_SectorID[hemisphere]).isFailure() ) {
        ATH_MSG_WARNING( "Can not register monitoring histogram: " << m_hist_muCTPiDaq_Forward_SectorID[hemisphere]->GetName() );
      }
    }
    // *-- DAQ Barrel
    histTitle = m_histProp_muCTPi_Barrel_SectorID.value().title()+std::string("_DAQ_")+strHemisphere[hemisphere];
    m_hist_muCTPiDaq_Barrel_SectorID[hemisphere] = new TH1F ( histTitle.c_str(),
                                                              (histTitle+";Sector Id").c_str(),
                                                              m_histProp_muCTPi_Barrel_SectorID.value().bins(),
                                                              m_histProp_muCTPi_Barrel_SectorID.value().lowEdge(),
                                                              m_histProp_muCTPi_Barrel_SectorID.value().highEdge());
    if (m_hist_muCTPiDaq_Barrel_SectorID[hemisphere]) {
      if( rootHistSvc->regHist(path + m_hist_muCTPiDaq_Barrel_SectorID[hemisphere]->GetName(),
                               m_hist_muCTPiDaq_Barrel_SectorID[hemisphere]).isFailure() ) {
        ATH_MSG_WARNING( "Can not register monitoring histogram: " << m_hist_muCTPiDaq_Barrel_SectorID[hemisphere]->GetName() );
      }
    }
  } // end loop over hemispheres


    // *-- Number of muCTPi RoIs
    // *-- DAQ
  histTitle = m_histProp_NumberOfRoIs.value().title()+std::string("_DAQ");
  m_hist_NumberOfRoIs_DAQ = new TH1F ( histTitle.c_str(),
                                       (histTitle+";Number of RoIs").c_str(),
                                       m_histProp_NumberOfRoIs.value().bins(),
                                       m_histProp_NumberOfRoIs.value().lowEdge(),
                                       m_histProp_NumberOfRoIs.value().highEdge());
  if (m_hist_NumberOfRoIs_DAQ) {
    if( rootHistSvc->regHist(path + m_hist_NumberOfRoIs_DAQ->GetName(), m_hist_NumberOfRoIs_DAQ).isFailure() ) {
      ATH_MSG_WARNING( "Can not register monitoring histogram: " << m_hist_NumberOfRoIs_DAQ->GetName() );
    }
  }

  // *-- Sector_ID vs. Pad_Id for Barrel RoIs from DAQ
  for (unsigned int threshold=0; threshold <= NUMBER_OF_PT_THRESHOLDS; ++threshold) {
    histTitle = std::string("Sector_ID_vs_Pad_ID_Barrel_DAQ_Threshold_")+strThreshold[threshold];
    m_hist_muCTPiDaq_Barrel_SectorID_Pad[threshold] =
      new TH2F ( histTitle.c_str(),
                 (histTitle+";(pad_ID+1)*(2*hemispere-1);SD id;").c_str(),
                 17,-8,8,
                 m_Number_Of_Barrel_Units.value(), 0, m_Number_Of_Barrel_Units.value());
    if (m_hist_muCTPiDaq_Barrel_SectorID_Pad[threshold]) {
      if( rootHistSvc->regHist(path + m_hist_muCTPiDaq_Barrel_SectorID_Pad[threshold]->GetName(),
                               m_hist_muCTPiDaq_Barrel_SectorID_Pad[threshold]).isFailure() ) {
        ATH_MSG_WARNING( "Can not register monitoring histogram: " << m_hist_muCTPiDaq_Barrel_SectorID_Pad[threshold]->GetName() );
      }
    }
  } // end loop over threshold

    // *-- Profile: Difference of BCID to event BCID as function of Sector_ID vs. Pad_Id for Barrel RoIs from DAQ
  histTitle = std::string("Difference_BCID_for_Sector_ID_vs_Pad_ID_Barrel_DAQ_ROB");
  m_hist_muCTPiDaq_Barrel_SectorID_Pad_DeltaBCID = new TProfile2D(histTitle.c_str(),
                                                                  (histTitle+";(pad_ID+1)*(2*hemispere-1);SD id;").c_str(),
                                                                  17,-8,8,
                                                                  m_Number_Of_Barrel_Units.value(), 0, m_Number_Of_Barrel_Units.value(), -10., 10.);
  if (m_hist_muCTPiDaq_Barrel_SectorID_Pad_DeltaBCID) {
    if( rootHistSvc->regHist(path + m_hist_muCTPiDaq_Barrel_SectorID_Pad_DeltaBCID->GetName(),
                             m_hist_muCTPiDaq_Barrel_SectorID_Pad_DeltaBCID).isFailure() ) {
      ATH_MSG_WARNING( "Can not register monitoring histogram: " << m_hist_muCTPiDaq_Barrel_SectorID_Pad_DeltaBCID->GetName() );
    }
  }

  // *-- Difference # of RoIs in RoIB - DAQ ROB
  m_hist_differenceRoIs = new TH1F (m_histProp_differenceRoIs.value().title().c_str(),
                                    (m_histProp_differenceRoIs.value().title()+";Difference (RoIB-DAQ) RoIs").c_str(),
                                    m_histProp_differenceRoIs.value().bins(),
                                    m_histProp_differenceRoIs.value().lowEdge(),
                                    m_histProp_differenceRoIs.value().highEdge());
  if (m_hist_differenceRoIs) {
    if( rootHistSvc->regHist(path + m_hist_differenceRoIs->GetName(), m_hist_differenceRoIs).isFailure() ) {
      ATH_MSG_WARNING( "Can not register monitoring histogram: " << m_hist_differenceRoIs->GetName() );
    }
  }

  // *-- Barrel: Hash for RoIs which are only in RoIB ROB
  histTitle = m_histProp_Problem_Barrel_Hash.value().title()+std::string("_RoIB_ROB");
  m_hist_muCTPiL1_Problem_Barrel_Hash = new TH1F (histTitle.c_str(),
                                                  (histTitle+";RoI_ID*1000 + Sector_ID + hemisphere*32").c_str(),
                                                  m_histProp_Problem_Barrel_Hash.value().bins(),
                                                  m_histProp_Problem_Barrel_Hash.value().lowEdge(),
                                                  m_histProp_Problem_Barrel_Hash.value().highEdge());
  if (m_hist_muCTPiL1_Problem_Barrel_Hash) {
    m_hist_muCTPiL1_Problem_Barrel_Hash->SetCanExtend(TH1::kAllAxes);
    if( rootHistSvc->regHist(path + m_hist_muCTPiL1_Problem_Barrel_Hash->GetName(),
                             m_hist_muCTPiL1_Problem_Barrel_Hash).isFailure() ) {
      ATH_MSG_WARNING( "Can not register monitoring histogram: " << m_hist_muCTPiL1_Problem_Barrel_Hash->GetName() );
    }
  }

  // *-- Barrel: Hash for RoIs which are only in DAQ ROB
  histTitle = m_histProp_Problem_Barrel_Hash.value().title()+std::string("_DAQ_ROB");
  m_hist_muCTPiDaq_Problem_Barrel_Hash = new TH1F (histTitle.c_str(),
                                                   (histTitle+";RoI_ID*1000 + Sector_ID + hemisphere*32").c_str(),
                                                   m_histProp_Problem_Barrel_Hash.value().bins(),
                                                   m_histProp_Problem_Barrel_Hash.value().lowEdge(),
                                                   m_histProp_Problem_Barrel_Hash.value().highEdge());
  if (m_hist_muCTPiDaq_Problem_Barrel_Hash) {
    m_hist_muCTPiDaq_Problem_Barrel_Hash->SetCanExtend(TH1::kAllAxes);
    if( rootHistSvc->regHist(path + m_hist_muCTPiDaq_Problem_Barrel_Hash->GetName(),
                             m_hist_muCTPiDaq_Problem_Barrel_Hash).isFailure() ) {
      ATH_MSG_WARNING( "Can not register monitoring histogram: " << m_hist_muCTPiDaq_Problem_Barrel_Hash->GetName() );
    }
  }

  // *-- Endcap: Hash for RoIs which are only in RoIB ROB
  histTitle = m_histProp_Problem_Endcap_Hash.value().title()+std::string("_RoIB_ROB");
  m_hist_muCTPiL1_Problem_Endcap_Hash = new TH1F (histTitle.c_str(),
                                                  (histTitle+";RoI_ID*1000 + Sector_ID + hemisphere*48").c_str(),
                                                  m_histProp_Problem_Endcap_Hash.value().bins(),
                                                  m_histProp_Problem_Endcap_Hash.value().lowEdge(),
                                                  m_histProp_Problem_Endcap_Hash.value().highEdge());
  if (m_hist_muCTPiL1_Problem_Endcap_Hash) {
    m_hist_muCTPiL1_Problem_Endcap_Hash->SetCanExtend(TH1::kAllAxes);
    if( rootHistSvc->regHist(path + m_hist_muCTPiL1_Problem_Endcap_Hash->GetName(),
                             m_hist_muCTPiL1_Problem_Endcap_Hash).isFailure() ) {
      ATH_MSG_WARNING( "Can not register monitoring histogram: " << m_hist_muCTPiL1_Problem_Endcap_Hash->GetName() );
    }
  }

  // *-- Endcap: Hash for RoIs which are only in DAQ ROB
  histTitle = m_histProp_Problem_Endcap_Hash.value().title()+std::string("_DAQ_ROB");
  m_hist_muCTPiDaq_Problem_Endcap_Hash = new TH1F (histTitle.c_str(),
                                                   (histTitle+";RoI_ID*1000 + Sector_ID + hemisphere*48").c_str(),
                                                   m_histProp_Problem_Endcap_Hash.value().bins(),
                                                   m_histProp_Problem_Endcap_Hash.value().lowEdge(),
                                                   m_histProp_Problem_Endcap_Hash.value().highEdge());
  if (m_hist_muCTPiDaq_Problem_Endcap_Hash) {
    m_hist_muCTPiDaq_Problem_Endcap_Hash->SetCanExtend(TH1::kAllAxes);
    if( rootHistSvc->regHist(path + m_hist_muCTPiDaq_Problem_Endcap_Hash->GetName(),
                             m_hist_muCTPiDaq_Problem_Endcap_Hash).isFailure() ) {
      ATH_MSG_WARNING( "Can not register monitoring histogram: " << m_hist_muCTPiDaq_Problem_Endcap_Hash->GetName() );
    }
  }

  // *-- Forward: Hash for RoIs which are only in RoIB ROB
  histTitle = m_histProp_Problem_Forward_Hash.value().title()+std::string("_RoIB_ROB");
  m_hist_muCTPiL1_Problem_Forward_Hash = new TH1F (histTitle.c_str(),
                                                   (histTitle+";RoI_ID*1000 + Sector_ID + hemisphere*24").c_str(),
                                                   m_histProp_Problem_Forward_Hash.value().bins(),
                                                   m_histProp_Problem_Forward_Hash.value().lowEdge(),
                                                   m_histProp_Problem_Forward_Hash.value().highEdge());
  if (m_hist_muCTPiL1_Problem_Forward_Hash) {
    m_hist_muCTPiL1_Problem_Forward_Hash->SetCanExtend(TH1::kAllAxes);
    if( rootHistSvc->regHist(path + m_hist_muCTPiL1_Problem_Forward_Hash->GetName(),
                             m_hist_muCTPiL1_Problem_Forward_Hash).isFailure() ) {
      ATH_MSG_WARNING( "Can not register monitoring histogram: " << m_hist_muCTPiL1_Problem_Forward_Hash->GetName() );
    }
  }

  // *-- Forward: Hash for RoIs which are only in DAQ ROB
  histTitle = m_histProp_Problem_Forward_Hash.value().title()+std::string("_DAQ_ROB");
  m_hist_muCTPiDaq_Problem_Forward_Hash = new TH1F (histTitle.c_str(),
                                                    (histTitle+";RoI_ID*1000 + Sector_ID + hemisphere*24").c_str(),
                                                    m_histProp_Problem_Forward_Hash.value().bins(),
                                                    m_histProp_Problem_Forward_Hash.value().lowEdge(),
                                                    m_histProp_Problem_Forward_Hash.value().highEdge());
  if (m_hist_muCTPiDaq_Problem_Forward_Hash) {
    m_hist_muCTPiDaq_Problem_Forward_Hash->SetCanExtend(TH1::kAllAxes);
    if( rootHistSvc->regHist(path + m_hist_muCTPiDaq_Problem_Forward_Hash->GetName(),
                             m_hist_muCTPiDaq_Problem_Forward_Hash).isFailure() ) {
      ATH_MSG_WARNING( "Can not register monitoring histogram: " << m_hist_muCTPiDaq_Problem_Forward_Hash->GetName() );
    }
  }


  // release histogramming service
  rootHistSvc.release().ignore();

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
bool TrigMuCTPiROBMonitor::verifyROBChecksum(OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment robFrag) {

  bool failed_checksum(false);
  OFFLINE_FRAGMENTS_NAMESPACE::PointerType it(0); 
  uint32_t current_value(0);

  // print check for received ROB
  if (msg().level() <= MSG::VERBOSE) {
    robFrag.payload(it);
    current_value = eformat::helper::checksum(robFrag.checksum_type(), it, robFrag.payload_size_word());

    ATH_MSG_VERBOSE(
	   " ROB id = 0x"             << std::setw(6)  << MSG::hex << robFrag.source_id() << MSG::dec 
	<< " checksum: type = "       << std::setw(2)  << robFrag.checksum_type()
	<< " value = "                << std::setw(12) << robFrag.checksum_value()
	<< " value (recalculated) = " << std::setw(12) << current_value
	<< " check = "                << std::setw(2)  << robFrag.checksum()
	);
  }

  // checksum test failed
  if ( not robFrag.checksum() ) {
    failed_checksum = true;

    // recalculate checksum value
    robFrag.payload(it);
    current_value = eformat::helper::checksum(robFrag.checksum_type(), it, robFrag.payload_size_word());

    // print warning
    ATH_MSG_WARNING( 
       " ROB checksum verification failed." 
	<< " ROB id = 0x"             << std::setw(6)  << MSG::hex << robFrag.source_id() << MSG::dec 
	<< " checksum type = "        << std::setw(2)  << robFrag.checksum_type()
	<< " value = "                << std::setw(12) << robFrag.checksum_value()
	<< " value (recalculated) = " << std::setw(12) << current_value
	<< " check = "                << std::setw(2)  << robFrag.checksum()
	);

    // fill the histograms
    std::ostringstream ost;
    ost << "0x" << std::hex << robFrag.source_id();
    if (m_hist_failedChecksumForROB) {
      oh_scoped_lock_histogram lock;
      m_hist_failedChecksumForROB->Fill((ost.str()).c_str(), 1.);
      m_hist_failedChecksumForROB->LabelsDeflate("X");
    }

    if (m_hist_failedChecksumForSD) {
      m_hist_failedChecksumForSD->Fill(eformat::helper::SourceIdentifier(robFrag.source_id()).human_detector().c_str(),1.);
    }
  }

  return failed_checksum;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
void TrigMuCTPiROBMonitor::verifyROBStatusBits(OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment robFrag) {

  // print check for received ROB
  ATH_MSG_VERBOSE(" verifyROBStatusBits: ROB id = 0x" << std::setw(6)  << MSG::hex << robFrag.source_id() << MSG::dec );

  // fill monitoring histogram for ROB generic status
  if ( ( m_hist_genericStatusForROB ) && ( robFrag.nstatus() != 0 ) ) {
    const uint32_t* it_status;
    robFrag.status(it_status);
    if ((*it_status) != 0) m_hist_genericStatusForROB->Fill(eformat::helper::SourceIdentifier(robFrag.source_id()).human_detector().c_str(),
							    m_map_GenericStatus[eformat::helper::Status(*it_status).generic()].c_str(),1.);
  }

  // fill monitoring histogram for ROB specific status
  if ( ( m_hist_specificStatusForROB ) && ( robFrag.nstatus() != 0 ) ) {
    const uint32_t* it_status;
    robFrag.status(it_status);
    if ((*it_status) != 0) {
      std::bitset<16> specificBits(eformat::helper::Status(*it_status).specific());
      for (unsigned int index=0; index < 16; ++index) {
	if (specificBits[index]) m_hist_specificStatusForROB->Fill(eformat::helper::SourceIdentifier(robFrag.source_id()).human_detector().c_str(),
								   m_vec_SpecificStatus[index].c_str(),1.);
      }
    }
  }
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
void TrigMuCTPiROBMonitor::decodeMuCTPi(OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment robFrag) {
  // save input stream flags
  std::ios_base::fmtflags log_flags_save = (msg().stream()).flags();
  char log_fill_char_save = (msg().stream()).fill();

  // print check for received ROB
  ATH_MSG_DEBUG( " decodeMuCTPi: ROB id = 0x" << std::setw(6)  << MSG::hex << robFrag.source_id() << MSG::dec );

  uint32_t formatVersion = robFrag.rod_version();
  uint32_t evtNum        = robFrag.rod_lvl1_id();
  uint32_t robFragSize   = robFrag.fragment_size_word();
  uint32_t rodFragSize   = robFrag.rod_fragment_size_word();
  uint32_t robId         = robFrag.source_id();
  uint32_t rodId         = robFrag.rod_source_id();
  const uint32_t bcId    = robFrag.rod_bc_id();

  const uint32_t* status;
  robFrag.rod_status( status );
  uint32_t nstatus = robFrag.rod_nstatus();

  uint32_t errorStat( 0 );
  if( nstatus > 0 ) errorStat = static_cast< uint32_t >( *status );

  ATH_MSG_DEBUG( "ROB ID 0x" << MSG::hex << robId <<  " ROD ID 0x"
		 << rodId << MSG::dec << " ROB fragment size "
		 << robFragSize << " ROD fragment size " << rodFragSize );
  
  /* RoIB MuCTPI ROB */
  if (rodId == (uint32_t)m_lvl1MuCTPiROBid.value()) {
    ATH_MSG_DEBUG( "   Found RoIB MuCTPI ROB." );
    ATH_MSG_DEBUG( "   Dumping RoI Words:" );

    /* Create header */
    ROIB::Header muCTPIHead( rodId, evtNum, formatVersion);
    /* Create content body */
    const uint32_t* data;
    robFrag.rod_data( data );
    uint32_t ndata = robFrag.rod_ndata();
    m_lvl1muCTPIRoIs.reserve(ndata);
    m_lvl1muCTPIHash_Endcap.reserve(ndata);
    m_lvl1muCTPIHash_Forward.reserve(ndata);
    m_lvl1muCTPIHash_Barrel.reserve(ndata);

    for( uint32_t i = 0; i < ndata; ++i, ++data ) {
      ATH_MSG_DEBUG( "       0x" << MSG::hex << std::setw( 8 )
                     << static_cast< uint32_t >( *data ) );
      ROIB::MuCTPIRoI thisRoI( static_cast< uint32_t >( *data ) );
      m_lvl1muCTPIRoIs.push_back( thisRoI );
    }
    /* Create trailer */
    ROIB::Trailer muCTPITrail( m_lvl1muCTPIRoIs.size(), errorStat );
    /* Create MuCTPIResult object */
    m_lvl1muCTPIResult = new ROIB::MuCTPIResult( muCTPIHead, muCTPITrail, m_lvl1muCTPIRoIs );
    /* Dump object if requested */
    if (msgLvl(MSG::DEBUG)) {
      ATH_MSG_DEBUG( m_lvl1muCTPIResult->dump() );
      m_lvl1muCTPIResult->dumpData(msg());
    }

    // fill histograms and compute RoI hashes
    float num_roib_rois =  m_lvl1muCTPIRoIs.size();
    if (m_hist_NumberOfRoIs_RoIB) m_hist_NumberOfRoIs_RoIB->Fill(num_roib_rois, 1.);

    unsigned int hemi;
    uint32_t hash;
    float sector_id_f, pad_id_f;
    for (const auto& roi : m_lvl1muCTPIRoIs) {
      hemi = ( roi.getSectorAddress() & MuCTPI_RDO::SECTOR_HEMISPHERE_MASK);
      sector_id_f  = roi.getSectorID();
      if( roi.getSectorLocation() == MuCTPI_RDO::ENDCAP ) {
        if (m_hist_muCTPiL1_Endcap_SectorID[hemi]) m_hist_muCTPiL1_Endcap_SectorID[hemi]->Fill( sector_id_f, 1.); 
        hash = (roi.getSectorID() + hemi * m_Number_Of_Endcap_Units.value()) + 1000*roi.getRoiNumber() ;
        m_lvl1muCTPIHash_Endcap.push_back(hash);
      }
      else if( roi.getSectorLocation() == MuCTPI_RDO::FORWARD ) {
        if (m_hist_muCTPiL1_Forward_SectorID[hemi]) m_hist_muCTPiL1_Forward_SectorID[hemi]->Fill( sector_id_f, 1.); 
        hash = (roi.getSectorID() + hemi * m_Number_Of_Forward_Units.value()) + 1000*roi.getRoiNumber() ;
        m_lvl1muCTPIHash_Forward.push_back(hash);
      }
      else if( roi.getSectorLocation() == MuCTPI_RDO::BARREL ) {
        if (m_hist_muCTPiL1_Barrel_SectorID[hemi]) m_hist_muCTPiL1_Barrel_SectorID[hemi]->Fill( sector_id_f, 1.); 
        int pad_id = ((roi.getRoiNumber()/4)+1)*(2*(int) hemi-1);
        pad_id_f = pad_id;
        if (m_hist_muCTPiL1_Barrel_SectorID_Pad[0]) m_hist_muCTPiL1_Barrel_SectorID_Pad[0]->Fill( pad_id_f, sector_id_f, 1.);
        if (m_hist_muCTPiL1_Barrel_SectorID_Pad[roi.pt()]) 
          m_hist_muCTPiL1_Barrel_SectorID_Pad[roi.pt()]->Fill( pad_id_f, sector_id_f, 1.);
        hash = (roi.getSectorID() + hemi * m_Number_Of_Barrel_Units.value()) + 1000*roi.getRoiNumber() ;
        m_lvl1muCTPIHash_Barrel.push_back(hash);
      }
    } // end loop over RoIs 

  /* DAQ MuCTPI ROB */
  } else if (rodId == (uint32_t)m_daqMuCTPiROBid.value()) {
    ATH_MSG_DEBUG( "   Found DAQ MuCTPI ROB." );
    ATH_MSG_DEBUG( "   ROD Header BCID " << bcId );
    ATH_MSG_DEBUG( "   Dumping RoI Words:" );

    OFFLINE_FRAGMENTS_NAMESPACE::PointerType it_data;
    robFrag.rod_data( it_data );
    const uint32_t ndata = robFrag.rod_ndata();
    ATH_MSG_DEBUG( " number of data words: " << ndata );

    // candidate multiplicity
    std::vector< uint32_t > candidateMultiplicity;
    // data words
    std::vector< uint32_t > dataWord;
    for( uint32_t i = 0; i < ndata; ++i, ++it_data ) {
      if( *it_data >> MuCTPI_RDO::MULT_WORD_FLAG_SHIFT ) {
        candidateMultiplicity.push_back( static_cast< uint32_t >( *it_data ) );
	ATH_MSG_DEBUG( "     0x" << MSG::hex << std::setw( 8 ) << std::setfill( '0' )
		       << ( *it_data ) << " (candidate multiplicity)" << std::setfill( log_fill_char_save ) );
      } else {
        dataWord.push_back( static_cast< uint32_t >( *it_data ) );
	ATH_MSG_DEBUG( "     0x" << MSG::hex << std::setw( 8 ) << std::setfill( '0' )
		       << ( *it_data ) << " (candidate word)" 
		       << " (--> RoI word = 0x" << MSG::hex << std::setw( 8 ) << std::setfill( '0' ) 
		       << mirodToRoIBDataWord( *it_data ) << ")" 
		       << std::setfill( log_fill_char_save ) );
      }
    }

    // create MuCTPI RDO
    m_daqmuCTPIResult = new MuCTPI_RDO( candidateMultiplicity, dataWord );

    // print contents
    if (msgLvl(MSG::DEBUG)) {
      MuCTPI_MultiplicityWord_Decoder(m_daqmuCTPIResult->candidateMultiplicity()).dumpData(msg());
      for (uint32_t w : m_daqmuCTPIResult->dataWord()) {
        MuCTPI_DataWord_Decoder(w).dumpData(msg());
        dumpRoIBDataWord(mirodToRoIBDataWord(w));
      }
    }

    // now select out the RoI candidates for the BCID which triggered the event and save them in 
    // a special list indexed by RoI ID
    uint16_t roiEventBCID = MuCTPI_MultiplicityWord_Decoder(m_daqmuCTPIResult->candidateMultiplicity()).getBCID();
    uint16_t roiEventNCan = MuCTPI_MultiplicityWord_Decoder(m_daqmuCTPIResult->candidateMultiplicity()).getNCandidates();

    m_daqmuCTPIRoIs.reserve( roiEventNCan );
    m_daqmuCTPIHash_Endcap.reserve( roiEventNCan );
    m_daqmuCTPIHash_Forward.reserve( roiEventNCan );
    m_daqmuCTPIHash_Barrel.reserve( roiEventNCan );

    unsigned int hemi;
    uint32_t hash;
    float sector_id_f, pad_id_f, delta_bcid_f;
    for (uint32_t w :  m_daqmuCTPIResult->dataWord()) {
      MuCTPI_DataWord_Decoder daqRoI(w);

      // fill profile for BCID difference
      if ( (m_hist_muCTPiDaq_Barrel_SectorID_Pad_DeltaBCID) && (daqRoI.getSectorLocation() == MuCTPI_RDO::BARREL) ) {
        int sector_id = int(daqRoI.getSectorID()) - m_Number_Of_Barrel_Units.value() * int(daqRoI.getHemisphere());
        sector_id_f = sector_id;
        delta_bcid_f = float(roiEventBCID) - float(daqRoI.getBCID());
        int pad_id =((daqRoI.getRoiNumber()/4)+1)*(2*(int) daqRoI.getHemisphere()-1);
        pad_id_f = pad_id;
        m_hist_muCTPiDaq_Barrel_SectorID_Pad_DeltaBCID->Fill( pad_id_f, sector_id_f, delta_bcid_f, 1.); 
      }

      if (roiEventBCID == daqRoI.getBCID()) { // RoI matches event BCID
        // create a L1 RoI
        ROIB::MuCTPIRoI roI( mirodToRoIBDataWord(w) );
        m_daqmuCTPIRoIs.push_back(roI);
        
        // fill histograms and compute RoI hashes
        hemi = (roI.getSectorAddress() & MuCTPI_RDO::SECTOR_HEMISPHERE_MASK);
        sector_id_f = roI.getSectorID();
        if( roI.getSectorLocation() == MuCTPI_RDO::ENDCAP ) {
          if (m_hist_muCTPiDaq_Endcap_SectorID[hemi]) m_hist_muCTPiDaq_Endcap_SectorID[hemi]->Fill( sector_id_f, 1.); 
          hash = (roI.getSectorID() + hemi * m_Number_Of_Endcap_Units.value()) + 1000*roI.getRoiNumber() ;
          m_daqmuCTPIHash_Endcap.push_back(hash);
        }
        else if( roI.getSectorLocation() == MuCTPI_RDO::FORWARD ) {
          if (m_hist_muCTPiDaq_Forward_SectorID[hemi]) m_hist_muCTPiDaq_Forward_SectorID[hemi]->Fill( sector_id_f, 1.); 
          hash = (roI.getSectorID() + hemi * m_Number_Of_Forward_Units.value()) + 1000*roI.getRoiNumber() ;
          m_daqmuCTPIHash_Forward.push_back(hash);
        }
        else if( roI.getSectorLocation() == MuCTPI_RDO::BARREL ) {
          int pad_id =((daqRoI.getRoiNumber()/4)+1)*(2*(int) daqRoI.getHemisphere()-1);
          pad_id_f = pad_id;
          if (m_hist_muCTPiDaq_Barrel_SectorID[hemi]) m_hist_muCTPiDaq_Barrel_SectorID[hemi]->Fill( sector_id_f, 1.); 
          if (m_hist_muCTPiDaq_Barrel_SectorID_Pad[0]) m_hist_muCTPiDaq_Barrel_SectorID_Pad[0]->Fill( pad_id_f, sector_id_f, 1.);
          if (m_hist_muCTPiDaq_Barrel_SectorID_Pad[roI.pt()]) 
            m_hist_muCTPiDaq_Barrel_SectorID_Pad[roI.pt()]->Fill( pad_id_f, sector_id_f, 1.);
          hash = (roI.getSectorID() + hemi * m_Number_Of_Barrel_Units.value()) + 1000*roI.getRoiNumber() ;
          m_daqmuCTPIHash_Barrel.push_back(hash);
        }
      }
    } // end loop over data words
    
    float num_daq_rois =  m_daqmuCTPIRoIs.size();
    if (m_hist_NumberOfRoIs_DAQ) m_hist_NumberOfRoIs_DAQ->Fill(num_daq_rois ,1.);

  /* No MuCTPI ROB */
  } else {
    ATH_MSG_DEBUG( "   No MuCTPI ROB found." );
  }

  // reset log stream flags to original values
  msg().flags(log_flags_save);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
uint32_t TrigMuCTPiROBMonitor::mirodToRoIBDataWord( uint32_t data_word ) {
  return ( ( ( data_word & 0x8000000 ) >> 4 ) | ( ( data_word & 0x3fe0000 ) >> 3 ) |
	   ( data_word & 0x3fff ) );
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
void TrigMuCTPiROBMonitor::dumpRoIBDataWord(uint32_t data_word ) {

  if (msg().level() <= MSG::DEBUG) {
    ROIB::MuCTPIRoI roI(data_word);

    std::string loc = "UNDEFINED"; 
    if( roI.getSectorLocation() == MuCTPI_RDO::ENDCAP )
      loc = "ENDCAP";
    else if( roI.getSectorLocation() == MuCTPI_RDO::FORWARD )
      loc = "FORWARD";
    else if( roI.getSectorLocation() == MuCTPI_RDO::BARREL )
      loc = "BARREL";

    ATH_MSG_DEBUG( "RoIB word               : 0x"
	<< MSG::hex << roI.roIWord() << MSG::dec );
    ATH_MSG_DEBUG( "Threshold               :  pt" << roI.pt() );
    ATH_MSG_DEBUG( "Sector location         :  " << loc );
    std::string sectorOffset("");  
    if ((roI.getSectorAddress() & MuCTPI_RDO::SECTOR_HEMISPHERE_MASK) &&
	(roI.getSectorLocation() == MuCTPI_RDO::BARREL)) sectorOffset = " + 32 for Hemisphere = 1 "; 
    ATH_MSG_DEBUG( "Sector ID               :  " << roI.getSectorID() << sectorOffset );
    ATH_MSG_DEBUG( "Sector addr             :  0x" << MSG::hex
	<< roI.getSectorAddress() << MSG::dec );
    ATH_MSG_DEBUG( "Sector overflow         :  " << roI.getSectorOverflow() );
    ATH_MSG_DEBUG( "RoI overflow            :  " << roI.getRoiOverflow() );
    ATH_MSG_DEBUG( "RoI number              :  " << roI.getRoiNumber() );
    ATH_MSG_DEBUG( "IsHighestPt             :  " << roI.getCandidateIsHighestPt() );
    ATH_MSG_DEBUG( "Overlap                 :  " << roI.getOverlapBits() );
    ATH_MSG_DEBUG( "Hemisphere              :  " << (roI.getSectorAddress() & MuCTPI_RDO::SECTOR_HEMISPHERE_MASK) );
    ATH_MSG_DEBUG( "=================================================" ) ;
  }
  return;
}
