/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigOnlineMonitor/TrigMuCTPiROBMonitor.h"
#include "TrigT1Result/RoIBResult.h"
#include "TrigT1Result/MuCTPI_RDO.h"
#include "TrigT1Result/MuCTPI_MultiplicityWord_Decoder.h"
#include "TrigT1Result/MuCTPI_DataWord_Decoder.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ThreadGaudi.h"
#include "GaudiKernel/ITHistSvc.h"
#include "AthenaKernel/Timeout.h"
#include "StoreGate/StoreGateSvc.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "TrigROBDataProviderSvc/ITrigROBDataProviderSvc.h"
#include "TrigMonitorBase/TrigLockedHist.h"
#include "EventInfo/TriggerInfo.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/EventType.h"
#include "eformat/eformat.h"
#include "eformat/index.h"

#include <algorithm>
#include <iostream>
#ifndef HAVE_NEW_IOSTREAMS
#include <strstream>  /*gnu-specific*/
typedef strstream __sstream;
#else
#include <sstream>
typedef std::ostringstream __sstream;
#endif
#include <iomanip>
#include <cassert>
#include <bitset>
#include <TH1F.h>
#include <TH2F.h>
#include <TProfile2D.h>

/////////////////////////////////////////////////////////////////////////////

TrigMuCTPiROBMonitor::TrigMuCTPiROBMonitor(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator), 
  m_msg(0),
  m_storeGateSvc( "StoreGateSvc", name ),
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
  m_map_GenericStatus[eformat::UNCLASSIFIED]      = "UNCLASSIFIED";
  m_map_GenericStatus[eformat::BCID_CHECK_FAIL]   = "BCID_CHECK_FAIL";
  m_map_GenericStatus[eformat::LVL1ID_CHECK_FAIL] = "LVL1ID_CHECK_FAIL";
  m_map_GenericStatus[eformat::TIMEOUT]           = "TIMEOUT";
  m_map_GenericStatus[eformat::DATA_CORRUPTION]   = "DATA_CORRUPTION";
  m_map_GenericStatus[eformat::INTERNAL_OVERFLOW] = "INTERNAL_OVERFLOW";

  // fill vector with specific status codes
  m_vec_SpecificStatus.reserve(16);
  m_vec_SpecificStatus.push_back("TRIGGER_TYPE_SYNC_ERROR");
  m_vec_SpecificStatus.push_back("FRAGMENT_SIZE_ERROR");
  m_vec_SpecificStatus.push_back("DATABLOCK_ERROR");
  m_vec_SpecificStatus.push_back("CTRL_WORD_ERROR");
  m_vec_SpecificStatus.push_back("MISSING_BOF");
  m_vec_SpecificStatus.push_back("MISSING_EOF");
  m_vec_SpecificStatus.push_back("INVALID_HEADER_MARKER");
  m_vec_SpecificStatus.push_back("FORMAT_ERROR");
  m_vec_SpecificStatus.push_back("DUPLICATE_EVENT");
  m_vec_SpecificStatus.push_back("SEQUENCE_ERROR");
  m_vec_SpecificStatus.push_back("TRANSMISSION_ERROR");
  m_vec_SpecificStatus.push_back("TRUNCATION");
  m_vec_SpecificStatus.push_back("SHORT_FRAGMENT");
  m_vec_SpecificStatus.push_back("FRAGMENT_LOST");
  m_vec_SpecificStatus.push_back("FRAGMENT_PENDING");
  m_vec_SpecificStatus.push_back("ROL_DISABLED");

  // muCTPi Sector ID plots
  for (int hemisphere=0; hemisphere <= 1; ++hemisphere) {
    m_hist_muCTPiL1_Barrel_SectorID.push_back(0);
    m_hist_muCTPiL1_Endcap_SectorID.push_back(0);
    m_hist_muCTPiL1_Forward_SectorID.push_back(0);

    m_hist_muCTPiDaq_Barrel_SectorID.push_back(0);
    m_hist_muCTPiDaq_Endcap_SectorID.push_back(0);
    m_hist_muCTPiDaq_Forward_SectorID.push_back(0);
  } // end loop over hemispheres

  // muCTPi Sector ID vs pad plots
  for (unsigned int threshold=0; threshold <= NUMBER_OF_PT_THRESHOLDS; ++threshold) {
    m_hist_muCTPiL1_Barrel_SectorID_Pad.push_back(0);
    m_hist_muCTPiDaq_Barrel_SectorID_Pad.push_back(0);
  } // end loop over thresholds
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode TrigMuCTPiROBMonitor::initialize(){

  // Get the messaging service
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "initialize()" << endreq;

  // Print out the property values
  log << MSG::INFO << " ROB ID: Lvl1 CTP                           = " << m_lvl1CTPROBid 
      << std::setw(6) << " (=0x" << MSG::hex << m_lvl1CTPROBid.value() << MSG::dec << ")" << endreq;
  log << MSG::INFO << " ROB ID: Lvl1 muCTPi                        = " << m_lvl1MuCTPiROBid
      << std::setw(6) << " (=0x" << MSG::hex << m_lvl1MuCTPiROBid.value() << MSG::dec << ")" << endreq;
  log << MSG::INFO << " ROB ID: DAQ CTP                            = " << m_daqCTPROBid
      << std::setw(6) << " (=0x" << MSG::hex << m_daqCTPROBid.value() << MSG::dec << ")" << endreq;
  log << MSG::INFO << " ROB ID: DAQ muCTPi                         = " << m_daqMuCTPiROBid
      << std::setw(6) << " (=0x" << MSG::hex << m_daqMuCTPiROBid.value() << MSG::dec << ")" << endreq;
  log << MSG::INFO << " Put events with ROB errors on DEBUG stream = " << m_setDebugStream << endreq;
  log << MSG::INFO << "         Name of used DEBUG stream          = " << m_debugStreamName << endreq;
  log << MSG::INFO << " Do ROB checksum test                       = " << m_doROBChecksum << endreq;
  log << MSG::INFO << "        Hist:FailedChecksumForROB           = " << m_histProp_failedChecksumForROB << endreq;
  log << MSG::INFO << "        Hist:FailedChecksumForSD            = " << m_histProp_failedChecksumForSD << endreq;
  log << MSG::INFO << " Do ROB status test                         = " << m_doROBStatus << endreq;
  log << MSG::INFO << "        Hist:NumberOfRoIs                   = " << m_histProp_NumberOfRoIs << endreq;
  log << MSG::INFO << " Number Of Barrel  Sector Units             = " << m_Number_Of_Barrel_Units << endreq;
  log << MSG::INFO << " Number Of Endcap  Sector Units             = " << m_Number_Of_Endcap_Units << endreq;
  log << MSG::INFO << " Number Of Forward Sector Units             = " << m_Number_Of_Forward_Units << endreq;
  log << MSG::INFO << "        Hist:SectorID_Barrel_muCTPi         = " << m_histProp_muCTPi_Barrel_SectorID  << endreq;
  log << MSG::INFO << "        Hist:SectorID_Endcap_muCTPi         = " << m_histProp_muCTPi_Endcap_SectorID  << endreq;
  log << MSG::INFO << "        Hist:SectorID_Forward_muCTPi        = " << m_histProp_muCTPi_Forward_SectorID << endreq;
  log << MSG::INFO << "        Hist:DifferenceRoIs_RoIB-DAQ_muCTPi = " << m_histProp_differenceRoIs << endreq;
  log << MSG::INFO << "        Hist:BarrelHashforProblemRoIs       = " << m_histProp_Problem_Barrel_Hash << endreq;
  log << MSG::INFO << "        Hist:EndcapHashforProblemRoIs       = " << m_histProp_Problem_Endcap_Hash << endreq;
  log << MSG::INFO << "        Hist:ForwardHashforProblemRoIs      = " << m_histProp_Problem_Forward_Hash << endreq;
  log << MSG::INFO << " Do muCTPi Monitoring Timing                = " << m_doTiming << endreq;
  log << MSG::INFO << "        Hist:TimeMuCTPiMonitor              = " << m_histProp_timeMuCTPi << endreq;

  // Locate the StoreGateSvc
  StatusCode sc =  m_storeGateSvc.retrieve();
  if (!sc.isSuccess()) {
    log << MSG::ERROR << "Could not find StoreGateSvc" << endreq;
    return sc;
  }

  // Locate the ROBDataProviderSvc
  sc = m_robDataProviderSvc.retrieve();
  if (!sc.isSuccess()) {
    log << MSG::ERROR << "Could not find ROBDataProviderSvc" << endreq;
    return sc;
  } else {
    // Setup the L2 ROB Data Provider Service when configured
#ifdef ATLAS_GAUDI_V21
    m_trigROBDataProviderSvc = SmartIF<ITrigROBDataProviderSvc>( &*m_robDataProviderSvc );
#else
    m_trigROBDataProviderSvc = SmartIF<ITrigROBDataProviderSvc>( IID_ITrigROBDataProviderSvc, &*m_robDataProviderSvc );
#endif
    if (m_trigROBDataProviderSvc.isValid()) {
      log << MSG::DEBUG << "A ROBDataProviderSvc implementing the Level-2 interface ITrigROBDataProviderSvc was found."
          << endreq;
    } else {
      log << MSG::DEBUG << "No ROBDataProviderSvc implementing the Level-2 interface ITrigROBDataProviderSvc was found."
          << endreq;
    }
  }

  // Build the vectors with ROB Ids to retrieve
  if (m_trigROBDataProviderSvc.isValid()) {                // L2 (retrieve only RoIB version)
    m_ctpRobIds.push_back(m_lvl1CTPROBid.value());
    m_muCTPiRobIds.push_back(m_lvl1MuCTPiROBid.value());
  } else {                                                 // EF (retrieve RoIB+DAQ version)
    m_ctpRobIds.push_back(m_lvl1CTPROBid.value());
    m_ctpRobIds.push_back(m_daqCTPROBid.value());

    m_muCTPiRobIds.push_back(m_lvl1MuCTPiROBid.value());
    m_muCTPiRobIds.push_back(m_daqMuCTPiROBid.value());
  }

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode TrigMuCTPiROBMonitor::execute() {

  struct timeval time_start;
  struct timeval time_stop;
  if ( m_doTiming.value() ) {
    gettimeofday(&time_start, 0);
  }

  if (outputLevel() <= MSG::DEBUG)  logStream() << MSG::DEBUG << "execute()" << endreq;

  //--------------------------------------------------------------------------
  // check that there is still time left
  //--------------------------------------------------------------------------
  if (Athena::Timeout::instance().reached()) {
    logStream() << MSG::INFO << " Time out reached in entry to execute." << endreq;
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
    if (outputLevel() <= MSG::DEBUG) logStream() << MSG::DEBUG << " No muCTPi ROB found." << endreq;
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
  for (std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>::iterator it = muCTPiRobFragmentVec.begin();
       it != muCTPiRobFragmentVec.end();++it) {
    // verify checksum
    if (verifyROBChecksum(logStream(), **it )) event_with_checksum_failure=true ;

    // verify status bits
    verifyROBStatusBits(logStream(), **it );

    // decode the muCTPi ROB
    decodeMuCTPi(logStream(), **it );
  }

  // compare the the RoIB and DAQ RoIs when running in EF
  if (!m_trigROBDataProviderSvc.isValid()) { // not running in L2

    // Total number of RoIs is different
    if (m_lvl1muCTPIRoIs.size() != m_daqmuCTPIRoIs.size()) {
      if (outputLevel() <= MSG::DEBUG)  logStream() << MSG::DEBUG 
						    << " ---> RoI Number mismatch:"
						    << " number of RoIB RoIs = " << m_lvl1muCTPIRoIs.size()   
						    << " number of DAQ  RoIs = " << m_daqmuCTPIRoIs.size()   
						    << endreq;
      float diff_roi = float(m_lvl1muCTPIRoIs.size())- float(m_daqmuCTPIRoIs.size());
      if (m_hist_differenceRoIs) m_hist_differenceRoIs->Fill(diff_roi, 1.);
    }

    // Comparison of Barrel RoIs
    if ((m_lvl1muCTPIHash_Barrel.size() != 0) && (m_daqmuCTPIHash_Barrel.size() != 0)) {
      sort(m_lvl1muCTPIHash_Barrel.begin(),m_lvl1muCTPIHash_Barrel.end());
      sort(m_daqmuCTPIHash_Barrel.begin(), m_daqmuCTPIHash_Barrel.end() );

      if ( !equal(m_daqmuCTPIHash_Barrel.begin(), m_daqmuCTPIHash_Barrel.end(), m_lvl1muCTPIHash_Barrel.begin()) ) {
	if (outputLevel() <= MSG::DEBUG)  logStream() << MSG::DEBUG 
						      << " ---> RoI mismatch for BARREL: Hash = RoI-ID*1000 + Sector_ID + hemisphere*"
						      << m_Number_Of_Barrel_Units.value() << "\n" 
						      << " Hash for RoIB RoIs = " << m_lvl1muCTPIHash_Barrel << "\n"   
						      << " Hash for DAQ  RoIs = " << m_daqmuCTPIHash_Barrel   
						      << endreq;
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
	    scoped_lock_histogram lock;
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
	    scoped_lock_histogram lock;
	    m_hist_muCTPiDaq_Problem_Barrel_Hash->Fill((ost.str()).c_str(), 1.);
	    m_hist_muCTPiDaq_Problem_Barrel_Hash->LabelsDeflate("X");
	  }
	}

	if (outputLevel() <= MSG::DEBUG)  logStream() << MSG::DEBUG 
						      << " ---> RoI mismatch for BARREL: Hash = RoI-ID*1000 + Sector_ID + hemisphere*" 
						      << m_Number_Of_Barrel_Units.value() << "\n" 
						      << " Hash for RoIs which are only in the RoIB list = " << only_in_RoIB << "\n"   
						      << " Hash for RoIs which are only in the DAQ  list = " << only_in_DAQ   
						      << endreq;
      }
    }

    // Comparison of Endcap RoIs
    if ((m_lvl1muCTPIHash_Endcap.size() != 0) && (m_daqmuCTPIHash_Endcap.size() != 0)) {
      sort(m_lvl1muCTPIHash_Endcap.begin(),m_lvl1muCTPIHash_Endcap.end());
      sort(m_daqmuCTPIHash_Endcap.begin(), m_daqmuCTPIHash_Endcap.end() );

      if ( !equal(m_daqmuCTPIHash_Endcap.begin(), m_daqmuCTPIHash_Endcap.end(), m_lvl1muCTPIHash_Endcap.begin()) ) {
	if (outputLevel() <= MSG::DEBUG)  logStream() << MSG::DEBUG 
						      << " ---> RoI mismatch for ENDCAP: Hash = RoI-ID*1000 + Sector_ID + hemisphere*"
						      << m_Number_Of_Endcap_Units.value() << "\n" 
						      << " Hash for RoIB RoIs = " << m_lvl1muCTPIHash_Endcap << "\n"   
						      << " Hash for DAQ  RoIs = " << m_daqmuCTPIHash_Endcap   
						      << endreq;
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
	    scoped_lock_histogram lock;
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
	    scoped_lock_histogram lock;
	    m_hist_muCTPiDaq_Problem_Endcap_Hash->Fill((ost.str()).c_str(), 1.);
	    m_hist_muCTPiDaq_Problem_Endcap_Hash->LabelsDeflate("X");
	  }
	}

	if (outputLevel() <= MSG::DEBUG)  logStream() << MSG::DEBUG 
						      << " ---> RoI mismatch for ENDCAP: Hash = RoI-ID*1000 + Sector_ID + hemisphere*" 
						      << m_Number_Of_Endcap_Units.value() << "\n" 
						      << " Hash for RoIs which are only in the RoIB list = " << only_in_RoIB << "\n"   
						      << " Hash for RoIs which are only in the DAQ  list = " << only_in_DAQ   
						      << endreq;
      }
    }

    // Comparison of Forward RoIs
    if ((m_lvl1muCTPIHash_Forward.size() != 0) && (m_daqmuCTPIHash_Forward.size() != 0)) {
      sort(m_lvl1muCTPIHash_Forward.begin(),m_lvl1muCTPIHash_Forward.end());
      sort(m_daqmuCTPIHash_Forward.begin(), m_daqmuCTPIHash_Forward.end() );

      if ( !equal(m_daqmuCTPIHash_Forward.begin(), m_daqmuCTPIHash_Forward.end(), m_lvl1muCTPIHash_Forward.begin()) ) {
	if (outputLevel() <= MSG::DEBUG)  logStream() << MSG::DEBUG 
						      << " ---> RoI mismatch for FORWARD: Hash = RoI-ID*1000 + Sector_ID + hemisphere*"
						      << m_Number_Of_Forward_Units.value() << "\n" 
						      << " Hash for RoIB RoIs = " << m_lvl1muCTPIHash_Forward << "\n"   
						      << " Hash for DAQ  RoIs = " << m_daqmuCTPIHash_Forward   
						      << endreq;
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
	    scoped_lock_histogram lock;
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
	    scoped_lock_histogram lock;
	    m_hist_muCTPiDaq_Problem_Forward_Hash->Fill((ost.str()).c_str(), 1.);
	    m_hist_muCTPiDaq_Problem_Forward_Hash->LabelsDeflate("X");
	  }
	}

	if (outputLevel() <= MSG::DEBUG)  logStream() << MSG::DEBUG 
						      << " ---> RoI mismatch for FORWARD: Hash = RoI-ID*1000 + Sector_ID + hemisphere*" 
						      << m_Number_Of_Forward_Units.value() << "\n" 
						      << " Hash for RoIs which are only in the RoIB list = " << only_in_RoIB << "\n"   
						      << " Hash for RoIs which are only in the DAQ  list = " << only_in_DAQ   
						      << endreq;
      }
    }
  } // end test for L2


  // if the event shows errors, set the DEBUG stream tag when requested
  if ((m_setDebugStream.value()) && (event_with_checksum_failure)) {
    // get EventInfo
    const EventInfo* p_EventInfo(0);
    StatusCode sc = m_storeGateSvc->retrieve(p_EventInfo);
    if(sc.isFailure()){
      logStream() << MSG::ERROR << "Can't get EventInfo object for updating the StreamTag" << endreq;
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
      p_EventInfo->trigger_info()->setStreamTags(vecStreamTags);
    }
  }

  if ( m_doTiming.value() ) {
    gettimeofday(&time_stop, 0);
    int secs = 0 ;
    if (time_stop.tv_sec >= time_start.tv_sec) secs = time_stop.tv_sec-time_start.tv_sec;
	
    int usecs = time_stop.tv_usec - time_start.tv_usec;
    float mtime = static_cast<float>(secs)*1000 + static_cast<float>(usecs)/1000;

    if (outputLevel() <= MSG::DEBUG) logStream() << MSG::DEBUG << " ---> Time used [ms] = " << mtime << endreq; 
	       
    //* timing histogram
    if (m_hist_timeMuCTPi) m_hist_timeMuCTPi->Fill(mtime,1.);	
  }

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode TrigMuCTPiROBMonitor::finalize() {

  // Get the messaging service
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "finalize()" << endreq;

  // delete decoded objects
  if (m_lvl1muCTPIResult) {
    delete m_lvl1muCTPIResult;
    m_lvl1muCTPIResult=0;
  }
  if (m_daqmuCTPIResult) {
    delete m_daqmuCTPIResult;
    m_daqmuCTPIResult=0;
  }

#ifdef ATLAS_GAUDI_V21
  m_trigROBDataProviderSvc.reset();
#endif

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode TrigMuCTPiROBMonitor::beginRun() {

  // Get a message stream instance
  m_msg = new MsgStream( msgSvc(), name() );

  logStream() << MSG::INFO << "beginRun()" << endreq;

  // Define histograms only when checks are requested
  if ((not m_doROBChecksum.value()) && (not m_doROBStatus.value())) return StatusCode::SUCCESS;

  // find histogramming service
  ServiceHandle<ITHistSvc> rootHistSvc("THistSvc", name());
  if ((rootHistSvc.retrieve()).isFailure()) {
    logStream() << MSG::ERROR << "Unable to locate THistSvc" << endreq;
    rootHistSvc.release().ignore();
    return StatusCode::FAILURE;
  }

  // *-- booking path
  std::string path = std::string("/EXPERT/")+getGaudiThreadGenericName(name())+"/";

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
      m_hist_failedChecksumForROB->SetBit(TH1::kCanRebin);
      if( rootHistSvc->regHist(path + m_hist_failedChecksumForROB->GetName(), m_hist_failedChecksumForROB).isFailure() ) {
	logStream() << MSG::WARNING << "Can not register monitoring histogram: " << m_hist_failedChecksumForROB->GetName() << endreq;
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
	logStream() << MSG::WARNING << "Can not register monitoring histogram: " << m_hist_failedChecksumForSD->GetName() << endreq;
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
      for (std::map<eformat::GenericStatus, std::string>::const_iterator it = m_map_GenericStatus.begin();it != m_map_GenericStatus.end();++it) {
	m_hist_genericStatusForROB->GetYaxis()->SetBinLabel( n_tmp_bin, (*it).second.c_str() );
	n_tmp_bin++;
      }

      if( rootHistSvc->regHist(path + m_hist_genericStatusForROB->GetName(), m_hist_genericStatusForROB).isFailure() ) {
	logStream() << MSG::WARNING << "Can not register monitoring histogram: " << m_hist_genericStatusForROB->GetName() << endreq;
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
      for (std::vector<std::string>::const_iterator it = m_vec_SpecificStatus.begin();it != m_vec_SpecificStatus.end();++it) {
	m_hist_specificStatusForROB->GetYaxis()->SetBinLabel( n_tmp_bin, (*it).c_str() );
	n_tmp_bin++;
      }

      if( rootHistSvc->regHist(path + m_hist_specificStatusForROB->GetName(), m_hist_specificStatusForROB).isFailure() ) {
	logStream() << MSG::WARNING << "Can not register monitoring histogram: " << m_hist_specificStatusForROB->GetName() << endreq;
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
	logStream() << MSG::WARNING << "Can not register monitoring histogram: " << m_hist_timeMuCTPi->GetName() << endreq;
      }
    }
  }

  // muCTPi plots, loop over the hemisperes 
  std::vector<std::string> strHemisphere;
  strHemisphere.push_back("Hemisphere_0(C)");
  strHemisphere.push_back("Hemisphere_1(A)");

  std::vector<std::string> strThreshold;
  strThreshold.push_back("All");
  strThreshold.push_back("PT1");
  strThreshold.push_back("PT2");
  strThreshold.push_back("PT3");
  strThreshold.push_back("PT4");
  strThreshold.push_back("PT5");
  strThreshold.push_back("PT6");

  std::string histTitle;
  // *-- Number of muCTPi RoIs
  // *-- RoIB
  histTitle = m_histProp_NumberOfRoIs.value().title()+std::string("_RoIB");
  m_hist_NumberOfRoIs_RoIB = new TH1F ( histTitle.c_str(),
					(histTitle+";Number of RoIs").c_str(),
					m_histProp_NumberOfRoIs.value().bins(),
					m_histProp_NumberOfRoIs.value().lowEdge(),
					m_histProp_NumberOfRoIs.value().highEdge());
  if (m_hist_NumberOfRoIs_RoIB) {
    if( rootHistSvc->regHist(path + m_hist_NumberOfRoIs_RoIB->GetName(), m_hist_NumberOfRoIs_RoIB).isFailure() ) {
      logStream() << MSG::WARNING << "Can not register monitoring histogram: " << m_hist_NumberOfRoIs_RoIB->GetName() << endreq;
    }
  }

  // *-- Sector_ID vs. Pad_Id for Barrel RoIs from L1
  for (unsigned int threshold=0; threshold <= NUMBER_OF_PT_THRESHOLDS; ++threshold) {
    histTitle = std::string("Sector_ID_vs_Pad_ID_Barrel_L1_Threshold_")+strThreshold[threshold];
    m_hist_muCTPiL1_Barrel_SectorID_Pad[threshold] = new TH2F ( histTitle.c_str(),
								(histTitle+";(pad_ID+1)*(2*hemispere-1);SD id;").c_str(),
								17,-8,8,
								m_Number_Of_Barrel_Units.value(), 0, m_Number_Of_Barrel_Units.value());
    if (m_hist_muCTPiL1_Barrel_SectorID_Pad[threshold]) {
      if( rootHistSvc->regHist(path + m_hist_muCTPiL1_Barrel_SectorID_Pad[threshold]->GetName(), m_hist_muCTPiL1_Barrel_SectorID_Pad[threshold]).isFailure() ) {
	logStream() << MSG::WARNING << "Can not register monitoring histogram: " << m_hist_muCTPiL1_Barrel_SectorID_Pad[threshold]->GetName() << endreq;
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
      //m_hist_muCTPiL1_Endcap_SectorID[hemisphere]->SetBit(TH1::kCanRebin);
      if( rootHistSvc->regHist(path + m_hist_muCTPiL1_Endcap_SectorID[hemisphere]->GetName(), m_hist_muCTPiL1_Endcap_SectorID[hemisphere]).isFailure() ) {
	logStream() << MSG::WARNING << "Can not register monitoring histogram: " << m_hist_muCTPiL1_Endcap_SectorID[hemisphere]->GetName() << endreq;
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
      //m_hist_muCTPiL1_Forward_SectorID[hemisphere]->SetBit(TH1::kCanRebin);
      if( rootHistSvc->regHist(path + m_hist_muCTPiL1_Forward_SectorID[hemisphere]->GetName(), m_hist_muCTPiL1_Forward_SectorID[hemisphere]).isFailure() ) {
	logStream() << MSG::WARNING << "Can not register monitoring histogram: " << m_hist_muCTPiL1_Forward_SectorID[hemisphere]->GetName() << endreq;
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
      //m_hist_muCTPiL1_Barrel_SectorID[hemisphere]->SetBit(TH1::kCanRebin);
      if( rootHistSvc->regHist(path + m_hist_muCTPiL1_Barrel_SectorID[hemisphere]->GetName(), m_hist_muCTPiL1_Barrel_SectorID[hemisphere]).isFailure() ) {
	logStream() << MSG::WARNING << "Can not register monitoring histogram: " << m_hist_muCTPiL1_Barrel_SectorID[hemisphere]->GetName() << endreq;
      }
    }

    if (!m_trigROBDataProviderSvc.isValid()) { // Define DAQ histograms only when not running in L2
      // *-- DAQ Endcap 
      histTitle = m_histProp_muCTPi_Endcap_SectorID.value().title()+std::string("_DAQ_")+strHemisphere[hemisphere];
      m_hist_muCTPiDaq_Endcap_SectorID[hemisphere] = new TH1F ( histTitle.c_str(),
								(histTitle+";Sector Id").c_str(),
								m_histProp_muCTPi_Endcap_SectorID.value().bins(),
								m_histProp_muCTPi_Endcap_SectorID.value().lowEdge(),
								m_histProp_muCTPi_Endcap_SectorID.value().highEdge());
      if (m_hist_muCTPiDaq_Endcap_SectorID[hemisphere]) {
	//m_hist_muCTPiDaq_Endcap_SectorID[hemisphere]->SetBit(TH1::kCanRebin);
	if( rootHistSvc->regHist(path + m_hist_muCTPiDaq_Endcap_SectorID[hemisphere]->GetName(), m_hist_muCTPiDaq_Endcap_SectorID[hemisphere]).isFailure() ) {
	  logStream() << MSG::WARNING << "Can not register monitoring histogram: " << m_hist_muCTPiDaq_Endcap_SectorID[hemisphere]->GetName() << endreq;
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
	//m_hist_muCTPiDaq_Forward_SectorID[hemisphere]->SetBit(TH1::kCanRebin);
	if( rootHistSvc->regHist(path + m_hist_muCTPiDaq_Forward_SectorID[hemisphere]->GetName(), m_hist_muCTPiDaq_Forward_SectorID[hemisphere]).isFailure() ) {
	  logStream() << MSG::WARNING << "Can not register monitoring histogram: " << m_hist_muCTPiDaq_Forward_SectorID[hemisphere]->GetName() << endreq;
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
	//m_hist_muCTPiDaq_Barrel_SectorID[hemisphere]->SetBit(TH1::kCanRebin);
	if( rootHistSvc->regHist(path + m_hist_muCTPiDaq_Barrel_SectorID[hemisphere]->GetName(), m_hist_muCTPiDaq_Barrel_SectorID[hemisphere]).isFailure() ) {
	  logStream() << MSG::WARNING << "Can not register monitoring histogram: " << m_hist_muCTPiDaq_Barrel_SectorID[hemisphere]->GetName() << endreq;
	}
      }
    } // end check for L2
  } // end loop over hemispheres

  if (!m_trigROBDataProviderSvc.isValid()) { // Define RoIB-DAQ comparison histograms only when not running in L2
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
	logStream() << MSG::WARNING << "Can not register monitoring histogram: " << m_hist_NumberOfRoIs_DAQ->GetName() << endreq;
      }
    }

    // *-- Sector_ID vs. Pad_Id for Barrel RoIs from DAQ
    for (unsigned int threshold=0; threshold <= NUMBER_OF_PT_THRESHOLDS; ++threshold) {
      histTitle = std::string("Sector_ID_vs_Pad_ID_Barrel_DAQ_Threshold_")+strThreshold[threshold];
      m_hist_muCTPiDaq_Barrel_SectorID_Pad[threshold] = new TH2F ( histTitle.c_str(),
								   (histTitle+";(pad_ID+1)*(2*hemispere-1);SD id;").c_str(),
								   17,-8,8,
								   m_Number_Of_Barrel_Units.value(), 0, m_Number_Of_Barrel_Units.value());
      if (m_hist_muCTPiDaq_Barrel_SectorID_Pad[threshold]) {
	if( rootHistSvc->regHist(path + m_hist_muCTPiDaq_Barrel_SectorID_Pad[threshold]->GetName(), m_hist_muCTPiDaq_Barrel_SectorID_Pad[threshold]).isFailure() ) {
	  logStream() << MSG::WARNING << "Can not register monitoring histogram: " << m_hist_muCTPiDaq_Barrel_SectorID_Pad[threshold]->GetName() << endreq;
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
      if( rootHistSvc->regHist(path + m_hist_muCTPiDaq_Barrel_SectorID_Pad_DeltaBCID->GetName(), m_hist_muCTPiDaq_Barrel_SectorID_Pad_DeltaBCID).isFailure() ) {
	logStream() << MSG::WARNING << "Can not register monitoring histogram: " << m_hist_muCTPiDaq_Barrel_SectorID_Pad_DeltaBCID->GetName() << endreq;
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
	logStream() << MSG::WARNING << "Can not register monitoring histogram: " << m_hist_differenceRoIs->GetName() << endreq;
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
      m_hist_muCTPiL1_Problem_Barrel_Hash->SetBit(TH1::kCanRebin);
      if( rootHistSvc->regHist(path + m_hist_muCTPiL1_Problem_Barrel_Hash->GetName(), m_hist_muCTPiL1_Problem_Barrel_Hash).isFailure() ) {
	logStream() << MSG::WARNING << "Can not register monitoring histogram: " << m_hist_muCTPiL1_Problem_Barrel_Hash->GetName() << endreq;
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
      m_hist_muCTPiDaq_Problem_Barrel_Hash->SetBit(TH1::kCanRebin);
      if( rootHistSvc->regHist(path + m_hist_muCTPiDaq_Problem_Barrel_Hash->GetName(), m_hist_muCTPiDaq_Problem_Barrel_Hash).isFailure() ) {
	logStream() << MSG::WARNING << "Can not register monitoring histogram: " << m_hist_muCTPiDaq_Problem_Barrel_Hash->GetName() << endreq;
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
      m_hist_muCTPiL1_Problem_Endcap_Hash->SetBit(TH1::kCanRebin);
      if( rootHistSvc->regHist(path + m_hist_muCTPiL1_Problem_Endcap_Hash->GetName(), m_hist_muCTPiL1_Problem_Endcap_Hash).isFailure() ) {
	logStream() << MSG::WARNING << "Can not register monitoring histogram: " << m_hist_muCTPiL1_Problem_Endcap_Hash->GetName() << endreq;
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
      m_hist_muCTPiDaq_Problem_Endcap_Hash->SetBit(TH1::kCanRebin);
      if( rootHistSvc->regHist(path + m_hist_muCTPiDaq_Problem_Endcap_Hash->GetName(), m_hist_muCTPiDaq_Problem_Endcap_Hash).isFailure() ) {
	logStream() << MSG::WARNING << "Can not register monitoring histogram: " << m_hist_muCTPiDaq_Problem_Endcap_Hash->GetName() << endreq;
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
      m_hist_muCTPiL1_Problem_Forward_Hash->SetBit(TH1::kCanRebin);
      if( rootHistSvc->regHist(path + m_hist_muCTPiL1_Problem_Forward_Hash->GetName(), m_hist_muCTPiL1_Problem_Forward_Hash).isFailure() ) {
	logStream() << MSG::WARNING << "Can not register monitoring histogram: " << m_hist_muCTPiL1_Problem_Forward_Hash->GetName() << endreq;
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
      m_hist_muCTPiDaq_Problem_Forward_Hash->SetBit(TH1::kCanRebin);
      if( rootHistSvc->regHist(path + m_hist_muCTPiDaq_Problem_Forward_Hash->GetName(), m_hist_muCTPiDaq_Problem_Forward_Hash).isFailure() ) {
	logStream() << MSG::WARNING << "Can not register monitoring histogram: " << m_hist_muCTPiDaq_Problem_Forward_Hash->GetName() << endreq;
      }
    }
  } // end check for L2


  // release histogramming service
  rootHistSvc.release().ignore();

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode TrigMuCTPiROBMonitor::endRun() {

  logStream() << MSG::INFO << "endRun()" << endreq;

  // delete message stream
  if ( m_msg ) delete m_msg;

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
bool TrigMuCTPiROBMonitor::verifyROBChecksum(MsgStream& log, OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment robFrag) {

  bool failed_checksum(false);
  OFFLINE_FRAGMENTS_NAMESPACE::PointerType it(0); 
  uint32_t current_value(0);

  // print check for received ROB
  if (log.level() <= MSG::VERBOSE) {
    robFrag.payload(it);
    current_value = eformat::helper::checksum(robFrag.checksum_type(), it, robFrag.payload_size_word());

    log << MSG::VERBOSE
	<< " ROB id = 0x"             << std::setw(6)  << MSG::hex << robFrag.source_id() << MSG::dec 
	<< " checksum: type = "       << std::setw(2)  << robFrag.checksum_type()
	<< " value = "                << std::setw(12) << robFrag.checksum_value()
	<< " value (recalculated) = " << std::setw(12) << current_value
	<< " check = "                << std::setw(2)  << robFrag.checksum()
	<< endreq;
  }

  // checksum test failed
  if ( not robFrag.checksum() ) {
    failed_checksum = true;

    // recalculate checksum value
    robFrag.payload(it);
    current_value = eformat::helper::checksum(robFrag.checksum_type(), it, robFrag.payload_size_word());

    // print warning
    log << MSG::WARNING 
	<< " ROB checksum verification failed." 
	<< " ROB id = 0x"             << std::setw(6)  << MSG::hex << robFrag.source_id() << MSG::dec 
	<< " checksum type = "        << std::setw(2)  << robFrag.checksum_type()
	<< " value = "                << std::setw(12) << robFrag.checksum_value()
	<< " value (recalculated) = " << std::setw(12) << current_value
	<< " check = "                << std::setw(2)  << robFrag.checksum()
	<< endreq;

    // fill the histograms
    std::ostringstream ost;
    ost << "0x" << std::hex << robFrag.source_id();
    if (m_hist_failedChecksumForROB) {
      scoped_lock_histogram lock;
      m_hist_failedChecksumForROB->Fill((ost.str()).c_str(), 1.);
      m_hist_failedChecksumForROB->LabelsDeflate("X");
    }

    if (m_hist_failedChecksumForSD) {
      scoped_lock_histogram lock;
      m_hist_failedChecksumForSD->Fill(eformat::helper::SourceIdentifier(robFrag.source_id()).human_detector().c_str(),1.);
      m_hist_failedChecksumForSD->LabelsDeflate("X");
    }
  }

  return failed_checksum;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
void TrigMuCTPiROBMonitor::verifyROBStatusBits(MsgStream& log, OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment robFrag) {

  // print check for received ROB
  if (log.level() <= MSG::VERBOSE) {
    log << MSG::VERBOSE
	<< " verifyROBStatusBits: ROB id = 0x" << std::setw(6)  << MSG::hex << robFrag.source_id() << MSG::dec 
	<< endreq;
  }

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
void TrigMuCTPiROBMonitor::decodeMuCTPi(MsgStream& log, OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment robFrag) {
  // save input stream flags
  std::ios_base::fmtflags log_flags_save = (log.stream()).flags();
  char log_fill_char_save = (log.stream()).fill();

  // print check for received ROB
  if (log.level() <= MSG::DEBUG) {
    log << MSG::DEBUG
	<< " decodeMuCTPi: ROB id = 0x" << std::setw(6)  << MSG::hex << robFrag.source_id() << MSG::dec 
	<< endreq;
  }

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

  if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "ROB ID 0x" << MSG::hex << robId <<  " ROD ID 0x"
				     << rodId << MSG::dec << " ROB fragment size "
				     << robFragSize << " ROD fragment size " << rodFragSize << endreq;

  /* RoIB MuCTPI ROB */
  if (rodId == (uint32_t)m_lvl1MuCTPiROBid.value()) {
    if (log.level() <= MSG::DEBUG) {
      log << MSG::DEBUG << "   Found RoIB MuCTPI ROB." << endreq;
      log << MSG::DEBUG << "   Dumping RoI Words:" << endreq;
    }

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
      if (log.level() <= MSG::DEBUG) {
	log << MSG::DEBUG << "       0x" << MSG::hex << std::setw( 8 )
	    << static_cast< uint32_t >( *data ) << endreq;
      }
      ROIB::MuCTPIRoI thisRoI( static_cast< uint32_t >( *data ) );
      m_lvl1muCTPIRoIs.push_back( thisRoI );
    }
    /* Create trailer */
    ROIB::Trailer muCTPITrail( m_lvl1muCTPIRoIs.size(), errorStat );
    /* Create MuCTPIResult object */
    m_lvl1muCTPIResult = new ROIB::MuCTPIResult( muCTPIHead, muCTPITrail, m_lvl1muCTPIRoIs );
    /* Dump object if requested */
    if (log.level() <= MSG::DEBUG) {
      log << MSG::DEBUG << m_lvl1muCTPIResult->dump() << endreq;
      m_lvl1muCTPIResult->dumpData(log);
    }

    // fill histograms and compute RoI hashes
    float num_roib_rois =  m_lvl1muCTPIRoIs.size();
    if (m_hist_NumberOfRoIs_RoIB) m_hist_NumberOfRoIs_RoIB->Fill(num_roib_rois, 1.);

    unsigned int hemi;
    uint32_t hash;
    float sector_id_f, pad_id_f;
    for (std::vector< ROIB::MuCTPIRoI >::iterator it = m_lvl1muCTPIRoIs.begin(); it != m_lvl1muCTPIRoIs.end(); ++it) {
      hemi = ( (*it).getSectorAddress() & MuCTPI_RDO::SECTOR_HEMISPHERE_MASK);
      sector_id_f  = (*it).getSectorID();
      if( (*it).getSectorLocation() == MuCTPI_RDO::ENDCAP ) {
	if (m_hist_muCTPiL1_Endcap_SectorID[hemi]) m_hist_muCTPiL1_Endcap_SectorID[hemi]->Fill( sector_id_f, 1.); 
	hash = ((*it).getSectorID() + hemi * m_Number_Of_Endcap_Units.value()) + 1000*(*it).getRoiNumber() ;
	m_lvl1muCTPIHash_Endcap.push_back(hash);
      }
      else if( (*it).getSectorLocation() == MuCTPI_RDO::FORWARD ) {
	if (m_hist_muCTPiL1_Forward_SectorID[hemi]) m_hist_muCTPiL1_Forward_SectorID[hemi]->Fill( sector_id_f, 1.); 
	hash = ((*it).getSectorID() + hemi * m_Number_Of_Forward_Units.value()) + 1000*(*it).getRoiNumber() ;
	m_lvl1muCTPIHash_Forward.push_back(hash);
      }
      else if( (*it).getSectorLocation() == MuCTPI_RDO::BARREL ) {
	if (m_hist_muCTPiL1_Barrel_SectorID[hemi]) m_hist_muCTPiL1_Barrel_SectorID[hemi]->Fill( sector_id_f, 1.); 
	int pad_id = (((*it).getRoiNumber()/4)+1)*(2*(int) hemi-1);
	pad_id_f = pad_id;
	if (m_hist_muCTPiL1_Barrel_SectorID_Pad[0]) m_hist_muCTPiL1_Barrel_SectorID_Pad[0]->Fill( pad_id_f, sector_id_f, 1.);
	if (m_hist_muCTPiL1_Barrel_SectorID_Pad[(*it).pt()]) 
	  m_hist_muCTPiL1_Barrel_SectorID_Pad[(*it).pt()]->Fill( pad_id_f, sector_id_f, 1.);
	hash = ((*it).getSectorID() + hemi * m_Number_Of_Barrel_Units.value()) + 1000*(*it).getRoiNumber() ;
	m_lvl1muCTPIHash_Barrel.push_back(hash);
      }
    } // end loop over RoIs 

  /* DAQ MuCTPI ROB */
  } else if (rodId == (uint32_t)m_daqMuCTPiROBid.value()) {
    if (log.level() <= MSG::DEBUG) {
      log << MSG::DEBUG << "   Found DAQ MuCTPI ROB." << endreq;
      log << MSG::DEBUG << "   ROD Header BCID " << bcId << endreq;
      log << MSG::DEBUG << "   Dumping RoI Words:" << endreq;
    }

    OFFLINE_FRAGMENTS_NAMESPACE::PointerType it_data;
    robFrag.rod_data( it_data );
    const uint32_t ndata = robFrag.rod_ndata();
    if (log.level() <= MSG::DEBUG) 
      log << MSG::DEBUG << " number of data words: " << ndata << endreq;

    // candidate multiplicity
    std::vector< uint32_t > candidateMultiplicity;
    // data words
    std::vector< uint32_t > dataWord;
    for( uint32_t i = 0; i < ndata; ++i, ++it_data ) {
      if( *it_data >> MuCTPI_RDO::MULT_WORD_FLAG_SHIFT ) {
        candidateMultiplicity.push_back( static_cast< uint32_t >( *it_data ) );
	if (log.level() <= MSG::DEBUG) 
	  log << MSG::DEBUG << "     0x" << MSG::hex << std::setw( 8 ) << std::setfill( '0' )
	      << ( *it_data ) << " (candidate multiplicity)" << std::setfill( log_fill_char_save ) << endreq;
      } else {
        dataWord.push_back( static_cast< uint32_t >( *it_data ) );
	if (log.level() <= MSG::DEBUG)
	  log << MSG::DEBUG << "     0x" << MSG::hex << std::setw( 8 ) << std::setfill( '0' )
	      << ( *it_data ) << " (candidate word)" 
	      << " (--> RoI word = 0x" << MSG::hex << std::setw( 8 ) << std::setfill( '0' ) 
	      << mirodToRoIBDataWord( *it_data ) << ")" 
	      << std::setfill( log_fill_char_save ) << endreq;
      }
    }

    // create MuCTPI RDO
    m_daqmuCTPIResult = new MuCTPI_RDO( candidateMultiplicity, dataWord );

    // print contents
    if (log.level() <= MSG::DEBUG) {
      MuCTPI_MultiplicityWord_Decoder(m_daqmuCTPIResult->candidateMultiplicity()).dumpData(log);
      for(std::vector< uint32_t >::const_iterator it = m_daqmuCTPIResult->dataWord().begin();
	  it != m_daqmuCTPIResult->dataWord().end(); ++it) {
	MuCTPI_DataWord_Decoder(*it).dumpData(log);
	dumpRoIBDataWord(log, mirodToRoIBDataWord(*it));
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
    for(std::vector< uint32_t >::const_iterator it = m_daqmuCTPIResult->dataWord().begin();
	it != m_daqmuCTPIResult->dataWord().end(); ++it) {
      MuCTPI_DataWord_Decoder daqRoI(*it);

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
	ROIB::MuCTPIRoI roI( mirodToRoIBDataWord(*it) );
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
    if (log.level() <= MSG::DEBUG) {
      log << MSG::DEBUG << "   No MuCTPI ROB found." << endreq;
    }
  }

  // reset log stream flags to original values
  log.flags(log_flags_save);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
uint32_t TrigMuCTPiROBMonitor::mirodToRoIBDataWord( uint32_t data_word ) {
  return ( ( ( data_word & 0x8000000 ) >> 4 ) | ( ( data_word & 0x3fe0000 ) >> 3 ) |
	   ( data_word & 0x3fff ) );
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
void TrigMuCTPiROBMonitor::dumpRoIBDataWord( MsgStream& log, uint32_t data_word ) {

  if (log.level() <= MSG::DEBUG) {
    ROIB::MuCTPIRoI roI(data_word);

    std::string loc = "UNDEFINED"; 
    if( roI.getSectorLocation() == MuCTPI_RDO::ENDCAP )
      loc = "ENDCAP";
    else if( roI.getSectorLocation() == MuCTPI_RDO::FORWARD )
      loc = "FORWARD";
    else if( roI.getSectorLocation() == MuCTPI_RDO::BARREL )
      loc = "BARREL";

    log << MSG::DEBUG << "RoIB word               : 0x"
	<< MSG::hex << roI.roIWord() << MSG::dec << endreq;
    log << MSG::DEBUG << "Threshold               :  pt" << roI.pt() << endreq;
    log << MSG::DEBUG << "Sector location         :  " << loc << endreq;
    std::string sectorOffset("");  
    if ((roI.getSectorAddress() & MuCTPI_RDO::SECTOR_HEMISPHERE_MASK) &&
	(roI.getSectorLocation() == MuCTPI_RDO::BARREL)) sectorOffset = " + 32 for Hemisphere = 1 "; 
    log << MSG::DEBUG << "Sector ID               :  " << roI.getSectorID() << sectorOffset << endreq;
    log << MSG::DEBUG << "Sector addr             :  0x" << MSG::hex
	<< roI.getSectorAddress() << MSG::dec << endreq;
    log << MSG::DEBUG << "Sector overflow         :  " << roI.getSectorOverflow() << endreq;
    log << MSG::DEBUG << "RoI overflow            :  " << roI.getRoiOverflow() << endreq;
    log << MSG::DEBUG << "RoI number              :  " << roI.getRoiNumber() << endreq;
    log << MSG::DEBUG << "IsHighestPt             :  " << roI.getCandidateIsHighestPt() << endreq;
    log << MSG::DEBUG << "Overlap                 :  " << roI.getOverlapBits() << endreq;
    log << MSG::DEBUG << "Hemisphere              :  " << (roI.getSectorAddress() & MuCTPI_RDO::SECTOR_HEMISPHERE_MASK) << endreq;
    log << MSG::DEBUG << "=================================================" << endreq;
  }
  return;
}
