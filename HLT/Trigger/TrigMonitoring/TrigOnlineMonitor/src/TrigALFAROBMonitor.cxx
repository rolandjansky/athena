/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "../src/TrigALFAROBMonitor.h"
#include "TrigT1Result/RoIBResult.h"
#include "TrigConfL1Data/CTPConfig.h"
#include "TrigConfL1Data/Menu.h"
#include "TrigConfL1Data/TriggerItem.h"
#include "TrigConfHLTData/HLTChain.h"
#include "TrigConfHLTData/HLTChainList.h"
#include "TrigSteeringEvent/Lvl1Result.h"
#include "TrigSteeringEvent/HLTResult.h"
#include "TrigT1ResultByteStream/RoIBResultByteStreamTool.h"
#include "TrigT1Result/MuCTPI_RDO.h"
#include "TrigT1Result/MuCTPI_MultiplicityWord_Decoder.h"
#include "TrigT1Result/MuCTPI_DataWord_Decoder.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ThreadGaudi.h"
#include "GaudiKernel/ITHistSvc.h"
#include "AthenaKernel/Timeout.h"
#include "TrigConfInterfaces/ITrigConfigSvc.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "TrigROBDataProviderSvc/ITrigROBDataProviderSvc.h"
#include "TrigMonitorBase/TrigLockedHist.h"
#include "EventInfo/TriggerInfo.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/EventType.h"
#include "eformat/eformat.h"
#include "eformat/index.h"
#include "eformat/SourceIdentifier.h"

#include <algorithm>
#include <sstream>
#include <iomanip>
#include <cassert>
#include <bitset>
#include <TH1F.h>
#include <TH2F.h>
#include <TProfile2D.h>


#if ROOT_VERSION_CODE >= ROOT_VERSION(6,0,0)
#   define CAN_REBIN(hist)  hist->SetCanExtend(TH1::kAllAxes)
#else
#   define CAN_REBIN(hist)  hist->SetBit(TH1::kCanRebin)
#endif

/// Unique interface ID of the tool that identifies it to the framweork
static const InterfaceID IID_IRoIBResultByteStreamTool( "RoIBResultByteStreamTool", 2, 0 );
 
/**
 * This function is needed by the framework to know what kind of tool
 * this is.
 */
const InterfaceID& RoIBResultByteStreamTool::interfaceID() {
  return IID_IRoIBResultByteStreamTool;
}
 
////////////////////////////////////////////////////////////////////////////

TrigALFAROBMonitor::TrigALFAROBMonitor(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator), 
  m_configSvc("TrigConf::TrigConfigSvc/TrigConfigSvc", name),
  m_lvl1ConfSvc("TrigConf::LVL1ConfigSvc/LVL1ConfigSvc", name),
  m_rootHistSvc("THistSvc", name),
  m_robDataProviderSvc( "ROBDataProviderSvc", name ),

  m_hist_failedChecksumForALFAROB(0),
  m_histProp_failedChecksumForALFAROB(Gaudi::Histo1DDef("FailedChecksumForALFAROB" ,0,1,1)),
  //m_hist_ALFA_trig_validated_tracks({ {0} } ),
  //m_hist_pmfMonitoring( {0} ),
  m_hist_genericStatusForROB(0),
  m_hist_specificStatusForROB(0),
  m_lvl1muCTPIResult(0),
  m_LB(-1),
  m_previousEventLB(-1),
  m_prescKey(-999),
  m_hist_timeALFA(0),
  m_histProp_timeALFA(Gaudi::Histo1DDef("Time_ALFA_Monitor" ,0.,100.,100))
{
  // Declare the properties
  declareProperty("Lvl1CTPROBid",                       m_lvl1CTPROBid=0x770001);
  declareProperty("Lvl1ALFA2ROBid",                     m_lvl1ALFA2ROBid=0x840000);
  declareProperty("Lvl1ALFA1ROBid",                     m_lvl1ALFA1ROBid=0x840001);
  declareProperty("DaqCTPROBid",                        m_daqCTPROBid=0x770000);
  declareProperty("SetDebugStream",                     m_setDebugStream=false);
  declareProperty("DebugStreamName",                    m_debugStreamName="ALFAROBErrorStream");
  declareProperty("CalibrationStreamName",              m_calibrationStreamName="ALFACalib");
  declareProperty("TestROBChecksum",                    m_doROBChecksum=true);
  declareProperty("HistFailedChecksumForALFAROB",       m_histProp_failedChecksumForALFAROB,"ALFA ROBs with inconsistent checksum");
  declareProperty("TestROBStatus",                      m_doROBStatus=true);
  declareProperty("MonitorALFATracks",                  m_doALFATracking=true);
  declareProperty("MonitorPMFactivity",                 m_doPMFMonitoring=true);
  declareProperty("DoTiming",                           m_doTiming=true);
  declareProperty("DoGoodDataMonitoring",               m_doDataGoodMonitoring=true);
  declareProperty("DoODDistanceHistograming",           m_doODDistance=true);
  declareProperty("HistTimeMuCTPiMonitor",              m_histProp_timeALFA, "Timing for ALFA monitoring algorithm");

  declareProperty("keyRBResult",  m_keyRBResult = "");

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

 // fill vectors with names of trigger items
 m_map_TrgNamesToHistGroups["L1_ALFA_ELAST15"] = 0;
 m_map_TrgNamesToHistGroups["L1_ALFA_ELAST18"] = 0;

 m_map_TrgNamesToHistGroups["L1_ALFA_ELAST15_Calib"] = 1;
 m_map_TrgNamesToHistGroups["L1_ALFA_ELAST18_Calib"] = 1;

 m_map_TrgNamesToHistGroups["L1_ALFA_SDIFF5"] = 2;
 m_map_TrgNamesToHistGroups["L1_ALFA_SDIFF6"] = 2;
 m_map_TrgNamesToHistGroups["L1_ALFA_SDIFF7"] = 2;
 m_map_TrgNamesToHistGroups["L1_ALFA_SDIFF8"] = 2;

 m_map_TrgNamesToHistGroups["L1_MBTS_1_A_ALFA_C"] = 3;
 m_map_TrgNamesToHistGroups["L1_MBTS_1_C_ALFA_A"] = 3;

 m_map_TrgNamesToHistGroups["L1_LUCID_A_ALFA_C"] = 4;
 m_map_TrgNamesToHistGroups["L1_LUCID_C_ALFA_A"] = 4;

 m_map_TrgNamesToHistGroups["L1_EM3_ALFA_ANY"] = 5;

 m_map_TrgNamesToHistGroups["L1_J12_ALFA_ANY"] = 6;

 m_map_TrgNamesToHistGroups["L1_TRT_ALFA_ANY"] = 7;

 m_map_TrgNamesToHistGroups["L1_ALFA_ANY"] = 8;

 m_map_TrgNamesToHistGroups["L1_ALFA_ANY_UNPAIRED_ISO"] = 9;

 m_map_TrgNamesToHistGroups["L1_ALFA_ANY_CALIB"] = 10;

 m_map_TrgNamesToHistGroups["L1_ALFA_ANY_A_EMPTY"] = 11;
 m_map_TrgNamesToHistGroups["L1_ALFA_ANY_C_EMPTY"] = 11;


  //declareInterface< RoIBResultByteStreamTool >( this );

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode TrigALFAROBMonitor::initialize(){

  // Get the messaging service
  
  // Print out the property values
  ATH_MSG_INFO( " ROB ID: Lvl1 CTP                           = " << m_lvl1CTPROBid 
      << std::setw(6) << " (=0x" << MSG::hex << m_lvl1CTPROBid.value() << MSG::dec << ")" );
  ATH_MSG_INFO( " ROB ID: Lvl1 ALFA1                        = " << m_lvl1ALFA1ROBid
      << std::setw(6) << " (=0x" << MSG::hex << m_lvl1ALFA1ROBid.value() << MSG::dec << ")" );
  ATH_MSG_INFO( " ROB ID: Lvl1 ALFA2                        = " << m_lvl1ALFA2ROBid
      << std::setw(6) << " (=0x" << MSG::hex << m_lvl1ALFA2ROBid.value() << MSG::dec << ")" );
  ATH_MSG_INFO( " ROB ID: DAQ CTP                            = " << m_daqCTPROBid
      << std::setw(6) << " (=0x" << MSG::hex << m_daqCTPROBid.value() << MSG::dec << ")" );
  ATH_MSG_INFO( " Put events with ROB errors on DEBUG stream = " << m_setDebugStream );
  ATH_MSG_INFO( "         Name of used DEBUG stream          = " << m_debugStreamName );
  ATH_MSG_INFO( " Name of streamTag to select events for monitoring  = " << m_calibrationStreamName );
  ATH_MSG_INFO( " Do ROB checksum test                       = " << m_doROBChecksum );
  ATH_MSG_INFO( "        Hist:FailedChecksumForALFAROB       = " << m_histProp_failedChecksumForALFAROB );
  ATH_MSG_INFO( " Do ROB status test                         = " << m_doROBStatus );
  ATH_MSG_INFO( " Do ALFA Monitoring Timing                = " << m_doTiming );
  ATH_MSG_INFO( "        Hist:TimeALFAMonitor              = " << m_histProp_timeALFA );

  // Locate the ROBDataProviderSvc
  StatusCode sc = m_robDataProviderSvc.retrieve();
  if (!sc.isSuccess()) {
    ATH_MSG_ERROR( "Could not find ROBDataProviderSvc" );
    return sc;
  } else {
    // Setup the L2 ROB Data Provider Service when configured
    m_trigROBDataProviderSvc = SmartIF<ITrigROBDataProviderSvc>( &*m_robDataProviderSvc );
    if (m_trigROBDataProviderSvc.isValid()) {
      ATH_MSG_DEBUG( "A ROBDataProviderSvc implementing the Level-2 interface ITrigROBDataProviderSvc was found."
          );
    } else {
      ATH_MSG_DEBUG( "No ROBDataProviderSvc implementing the Level-2 interface ITrigROBDataProviderSvc was found."
          );
    }
  }

  // locate the TrigConfSvc
  sc = m_configSvc.retrieve();
  if (!sc.isSuccess()) {
    ATH_MSG_ERROR( "Could not find TrigConfSvc" );
    return sc;
  } else {
    ATH_MSG_DEBUG( "TrigConfSvc identified" ); 
  }

  // connect to the LVL1ConfigSvc
  sc = m_lvl1ConfSvc.retrieve();
  if (!sc.isSuccess()) {
    ATH_MSG_ERROR( "Could not find LVL1ConfSvc" );
    return sc;
  } else {
    ATH_MSG_DEBUG( "LVL1ConfSvc service identified" );
  }



  // Build the vectors with ROB Ids to retrieve
  if (m_trigROBDataProviderSvc.isValid()) {                // L2 (retrieve only RoIB version)
    //m_ALFARobIds.push_back(m_lvl1CTPROBid.value());
    //m_ALFARobIds.push_back(m_daqCTPROBid.value()); // commented to avoid requesting the DAQ CTP ROB - not needed when all tracks go into single histo.

    m_ALFARobIds.push_back(m_lvl1ALFA1ROBid.value());
    m_ALFARobIds.push_back(m_lvl1ALFA2ROBid.value());
  }

  const TrigConf::HLTChainList *chainlist = m_configSvc->chainList();
  if (chainlist) {
         for (auto *chain: *chainlist) {
            if (chain->chain_name() == "HLT_costmonitor") {
               m_HLTcostMon_chain = chain;               
               ATH_MSG_INFO ("found HLT_costmonitor chain with prescale " << m_HLTcostMon_chain->prescale()
                                << " and the SB flag set to: "<<m_SBflag);
               if (m_HLTcostMon_chain->prescale() >=1 ) {
                      m_SBflag = true;
               } else {
                      m_SBflag = false;
               }
            } else {
                      //ATH_MSG_INFO ("HLT prescale key evaluation - " << chain->chain_name());
            }  
         }
  } else {
       ATH_MSG_WARNING ("HLT prescale key evaluation  - failed");
  }



  ATH_MSG_INFO("Initialize completed");
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode TrigALFAROBMonitor::execute() {

  //--------------------------------------------------------------------------
  // check that there is still time left after all HLT chains and TopAlg completed
  //--------------------------------------------------------------------------
  if (Athena::Timeout::instance().reached()) {
    ATH_MSG_DEBUG(" Time out reached in entry to execute.");
    return StatusCode::SUCCESS;
  }

  struct timeval time_start;
  struct timeval time_stop;
  if ( m_doTiming.value() ) {
    gettimeofday(&time_start, 0);
  }

  // get EventInfo
  const EventInfo* p_EventInfo(0);
  StatusCode sc = evtStore()->retrieve(p_EventInfo);
  if(sc.isFailure()){
      ATH_MSG_ERROR("Can't get EventIinfo object");
      return StatusCode::SUCCESS;
  } 
       
  //--------------------------------------------------------------------------
  // take only events with alfaCalibrationStream tag  
  //--------------------------------------------------------------------------
  bool eventInCalibStream = false;
  typedef std::vector< TriggerInfo::StreamTag > StreamTagVector_t;
  StreamTagVector_t vecStreamTags = p_EventInfo->trigger_info()->streamTags();
  for (std::vector<TriggerInfo::StreamTag>::iterator iter = vecStreamTags.begin(); iter!=vecStreamTags.end(); iter++) {
         if ((*iter).name().compare(m_calibrationStreamName.value()) == 0 ){
             eventInCalibStream = true;
             break;
         }
  }
  if (!eventInCalibStream) {
       //ATH_MSG_INFO ("event not tagged for calibration stream - return without ROS data request ");
    return StatusCode::SUCCESS;
  }

  bool event_with_checksum_failure(false);
  
  //ATH_MSG_INFO ("new event");
  // get EventID
  const EventID* p_EventID = p_EventInfo->event_ID();
  m_LB = p_EventID->lumi_block();

  if (m_previousEventLB < 0) {
    m_previousEventLB = m_LB;  // first event
    m_prevLB10reset = m_LB;
    m_prevLB60reset = m_LB;
  } else {
     if (m_LB > m_previousEventLB){ // new LB
        reset1LBhistos(m_previousEventLB);
        if ((m_LB - m_prevLB10reset) > 10 ) {reset10LBhistos(m_previousEventLB); m_prevLB10reset = m_LB;};
        if ((m_LB - m_prevLB10reset) > 60 ) {reset60LBhistos(m_previousEventLB); m_prevLB60reset = m_LB;};
        uint32_t newPrescKey = m_configSvc->hltPrescaleKey();
        if (newPrescKey != m_prescKey) {
             ATH_MSG_INFO ("HLT prescale key changed to "<<newPrescKey );
             
             // check with cont monitor if the SB fla has been set
             if (m_HLTcostMon_chain->prescale() >=1 ) {
                        m_SBflag = true;
             } else {
                        m_SBflag = false;
             }
             ATH_MSG_INFO ("found HLT_costmonitor chain with prescale " << m_HLTcostMon_chain->prescale()
                                << " and the SB flag set to: "<<m_SBflag);
             m_prescKey = newPrescKey;
        }
        m_previousEventLB = m_LB;
     }
  }

  // read Lvl1Result from SG
  LVL1CTP::Lvl1Result resultL1(true);
  if (!getLvl1Result(resultL1)) {
     return StatusCode::SUCCESS;
  }

  // read HLTResult from SG
  HLT::HLTResult resultHLT;
  if (!getHLTResult(resultHLT)) {
    return StatusCode::SUCCESS;
  }

 //if(evtStore()->contains<LVL1CTP::Lvl1Result>(m_keyL1Result)) {
     //const LVL1CTP::Lvl1Result* l1ptr = 0;
     //if(evtStore()->retrieve<LVL1CTP::Lvl1Result>(l1ptr, m_keyL1Result).isSuccess() && l1ptr) {
          //LVL1CTP::Lvl1Result resultL1 = *l1ptr;
     //}
  //} else {
    //ATH_MSG_DEBUG("Lvl1Result does not exist with key: ");
 //}

  // Now try to extract L1 decisons from ROIB fragment
  if(!evtStore()->contains<ROIB::RoIBResult>(m_keyRBResult)) {
       ATH_MSG_DEBUG("RoIBResult does not exist with key: " << m_keyRBResult);
  }

  const ROIB::RoIBResult* roIBResult=0;
  sc = evtStore()->retrieve(roIBResult,m_keyRBResult);

  if(sc.isFailure()){
    ATH_MSG_DEBUG(" Unable to retrieve RoIBResult from storeGate!");
               return StatusCode::SUCCESS; //HLT::NO_LVL1_RESULT;
  } else {
    const std::vector<ROIB::CTPRoI> ctpRoIVecAV = roIBResult->cTPResult().TAV();
    for (unsigned int iWord = 0; iWord < ctpRoIVecAV.size(); ++iWord) {
          uint32_t roIWord = ctpRoIVecAV[iWord].roIWord();
          ATH_MSG_DEBUG(" roiAV "<<std::hex<<roIWord<<std::dec);
    }
 }
  
  // get the ALFA ROBs
  std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*> ALFARobFragmentVec;
  ALFARobFragmentVec.reserve(m_ALFARobIds.size());
  m_robDataProviderSvc->getROBData(m_ALFARobIds,ALFARobFragmentVec);
  if (ALFARobFragmentVec.size()==0) {
    ATH_MSG_DEBUG(" No ALFA ROB found.");
    if ( m_doTiming.value() ) {
      gettimeofday(&time_stop, 0);
      int secs = 0 ;
      if (time_stop.tv_sec >= time_start.tv_sec)
	secs = time_stop.tv_sec - time_start.tv_sec;
	
      int usecs = time_stop.tv_usec - time_start.tv_usec;
      float mtime = static_cast<float>(secs)*1000 + static_cast<float>(usecs)/1000;
	
      //* timing histogram
      if (m_hist_timeALFA) m_hist_timeALFA->Fill(mtime,1.);	
    }
    return StatusCode::SUCCESS;
  } 
 

  // initialize to positive values arrays which will be used in distance measurements with ODs - data needed accross both ROBs (RODs).
  for (int detector=0; detector <8; detector++)
    for (int side=0; side<2; side++)
         m_ODtracks[detector][side] = 1.;

  // loop over retrieved ROBs and do checks
  for (std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>::iterator it = ALFARobFragmentVec.begin();
       it != ALFARobFragmentVec.end();++it) {
    // verify checksum
    if (verifyALFAROBChecksum(**it )) event_with_checksum_failure=true ; 

    // verify status bits
    verifyROBStatusBits(**it);

    // decode the muCTPi ROB
    //decodeMuCTPi(**it);

    // decode ALFA ROBs
    if (! event_with_checksum_failure) {
       if (decodeALFA(**it ) == 0) {
          findALFATracks(resultL1);
          findODTracks ();
      }
    }
  }

  // if the event shows errors, set the DEBUG stream tag when requested
  if ((m_setDebugStream.value()) && (event_with_checksum_failure)) {
    // get EventInfo
    const EventInfo* p_EventInfo(0);
    StatusCode sc = evtStore()->retrieve(p_EventInfo);
    if(sc.isFailure()){
      ATH_MSG_ERROR("Can't get EventInfo object for updating the StreamTag");
      if ( m_doTiming.value() ) {
	gettimeofday(&time_stop, 0);
	int secs = 0 ;
	if (time_stop.tv_sec >= time_start.tv_sec)
	  secs = time_stop.tv_sec - time_start.tv_sec;
	
	int usecs = time_stop.tv_usec - time_start.tv_usec;
	float mtime = static_cast<float>(secs)*1000 + static_cast<float>(usecs)/1000;
	
	//* timing histogram
	if (m_hist_timeALFA) m_hist_timeALFA->Fill(mtime,1.);	
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

    ATH_MSG_DEBUG(" ---> Time used [ms] = " << mtime); 
	      
    //* timing histogram
    if (m_hist_timeALFA) m_hist_timeALFA->Fill(mtime,1.);	
  }

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode TrigALFAROBMonitor::finalize() {

  // Get the messaging service
  ATH_MSG_INFO( "finalize()" );

  // delete decoded objects

//  m_trigROBDataProviderSvc.reset();

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode TrigALFAROBMonitor::beginRun() {

  // Get a message stream instance
  ATH_MSG_INFO("beginRun()");

  const TrigConf::CTPConfig *ctp_confg = m_configSvc->ctpConfig();
  if(!ctp_confg) {
     ATH_MSG_INFO("Failed to get CTPConfig");
     return StatusCode::SUCCESS;
  }
  for(TrigConf::TriggerItem *item : ctp_confg->menu().items()) {
     if(!item) {
       ATH_MSG_INFO("Null TriggerItem pointer");
       continue;
     }
     ATH_MSG_INFO(" triggerItem "<<item->name().c_str()<< "ctpId "<<item->ctpId());
     std::map<std::string, int>::iterator it = m_map_TrgNamesToHistGroups.find(item->name());
     if (it != m_map_TrgNamesToHistGroups.end()) {
       m_map_TrgItemNumbersToHistGroups[item->ctpId()] = it->second;
     }
     // locate golden alfa triggers for data quality assesment base on the ratio of tracks in elastic triggered events
     if (item->name().compare("L1_ALFA_ELAST15") == 0) m_elast15 = item->ctpId();
     if (item->name().compare("L1_ALFA_ELAST18") == 0) m_elast18 = item->ctpId();
  }
  for (std::map<int, int>::iterator it=m_map_TrgItemNumbersToHistGroups.begin(); it != m_map_TrgItemNumbersToHistGroups.end(); ++it) {
          ATH_MSG_INFO(" triggerItem number: "<<it->first<< " histo group: "<<it->second);
  }
 
  // Define histograms only when checks are requested
  if ((not m_doROBChecksum.value()) && (not m_doROBStatus.value())) return StatusCode::SUCCESS;

  // find histogramming service
  //m_rootHistSvc("THistSvc", name());
  //if ((m_rootHistSvc.retrieve()).isFailure()) {
    //ATH_MSG_ERROR("Unable to locate THistSvc");
    //m_rootHistSvc.release().ignore();
    //return StatusCode::FAILURE;
  //}

  // *-- booking path
  m_pathHisto = std::string("/EXPERT/")+getGaudiThreadGenericName(name())+"/";

  // Specific source identifiers
  //eformat::helper::SourceIdentifier srcID_ALFA( eformat::FORWARD_ALPHA ,0);
  //eformat::helper::SourceIdentifier srcID_CTP( eformat::TDAQ_CTP ,0);
  //eformat::helper::SourceIdentifier srcID_HLT( eformat::TDAQ_HLT, 0);

  if ( m_doROBChecksum.value() ) {
    // *-- ROBs with failed checksum
    m_hist_failedChecksumForALFAROB = new TH1F (m_histProp_failedChecksumForALFAROB.value().title().c_str(),
					       (m_histProp_failedChecksumForALFAROB.value().title()+";ALFA ROB id").c_str(),
					        m_histProp_failedChecksumForALFAROB.value().bins(),
					        m_histProp_failedChecksumForALFAROB.value().lowEdge(),
					        m_histProp_failedChecksumForALFAROB.value().highEdge());
    if (m_hist_failedChecksumForALFAROB) {
      CAN_REBIN(m_hist_failedChecksumForALFAROB);
      if( m_rootHistSvc->regHist(m_pathHisto + "common/" + m_hist_failedChecksumForALFAROB->GetName(), m_hist_failedChecksumForALFAROB).isFailure() ) {
	ATH_MSG_WARNING("Can not register ALFA ROB checksum monitoring histogram: " << m_hist_failedChecksumForALFAROB->GetName());
      }
    }
  }

  if ( m_doDataGoodMonitoring.value() ) {
    // *-- book histo to assess fraction of elastic triggered events with elastic tracks candidates
    std::string histTitle = "goodDataAssessment";
    m_hist_goodData = new TH1F (histTitle.c_str(), (histTitle + " elastics").c_str(), 10, -0.5, 9.5);
    if (m_hist_goodData) {
      CAN_REBIN(m_hist_goodData);
      if( m_rootHistSvc->regHist(m_pathHisto + "common/" + m_hist_goodData->GetName(), m_hist_goodData).isFailure() ) {
	ATH_MSG_WARNING("Can not register ALFA ROB good data elastic monitoring histogram: " << m_hist_goodData->GetName());
      }
    }
    histTitle = "goodDataAssessmentLB15";
    m_hist_goodDataLB15 = new TH2F (histTitle.c_str(), (histTitle + " elasticsLB").c_str(), 1000, -0.5, 999.5, 2, 0.5, 2.5);
    if( m_rootHistSvc->regHist(m_pathHisto + "common/" + m_hist_goodDataLB15->GetName(), m_hist_goodDataLB15).isFailure() ) {
	ATH_MSG_WARNING("Can not register ALFA ROB good data elastic LB 15 monitoring histogram: " << m_hist_goodDataLB15->GetName());
    }
    histTitle = "goodDataAssessmentLB18";
    m_hist_goodDataLB18 = new TH2F (histTitle.c_str(), (histTitle + " elasticsLB").c_str(), 1000, -0.5, 999.5, 2, 0.5, 2.5);
    if( m_rootHistSvc->regHist(m_pathHisto + "common/" + m_hist_goodDataLB18->GetName(), m_hist_goodDataLB18).isFailure() ) {
	ATH_MSG_WARNING("Can not register ALFA ROB good data elastic LB 18 monitoring histogram: " << m_hist_goodDataLB18->GetName());
    }
    histTitle = "corruptedROD";
    m_hist_corruptedROD_LB = new TH2F (histTitle.c_str(), (histTitle + " perLB").c_str(), 1000, -0.5, 999.5, 2, -0.5, 1.5);
    if( m_rootHistSvc->regHist(m_pathHisto + "common/" + m_hist_corruptedROD_LB->GetName(), m_hist_corruptedROD_LB).isFailure() ) {
	ATH_MSG_WARNING("Can not register ALFA ROB good data elastic LB 18 monitoring histogram: " << m_hist_corruptedROD_LB->GetName());
    }
  }


     m_stationNames.push_back("B7L1U");
     m_stationNames.push_back("B7L1L");
     m_stationNames.push_back("A7L1U");
     m_stationNames.push_back("A7L1L");
     m_stationNames.push_back("A7R1U");
     m_stationNames.push_back("A7R1L");
     m_stationNames.push_back("B7R1U");
     m_stationNames.push_back("B7R1L");

     m_trigConditions.push_back("elastic");
     m_trigConditions.push_back("elastic_ALFA_BG");
     m_trigConditions.push_back("singleDiffr");
     m_trigConditions.push_back("ALFA_MBTS_singleDiffr");
     m_trigConditions.push_back("ALFA_LUCID_singleDiffr");
     m_trigConditions.push_back("ALFA_EM3");
     m_trigConditions.push_back("ALFA_J12");
     m_trigConditions.push_back("ALFA_TRT");
     m_trigConditions.push_back("ANY");
     m_trigConditions.push_back("ANY_UNPAIRED_ISO");
     m_trigConditions.push_back("ANY_ALFA_BG");
     m_trigConditions.push_back("ALFA_EMPTY");

  if ( m_doALFATracking.value() ) {
     std::string histTitle;

     for (uint32_t trgCond = 0; trgCond < 12; trgCond++) {
         for (uint32_t station = 0; station < 8; station++) {
              histTitle = m_stationNames[station] + "_f_" + m_trigConditions[trgCond];
              m_hist_ALFA_trig_validated_tracks[trgCond][station] = new TH2F (histTitle.c_str(), (histTitle).c_str(),
                                                                                    260,-23,23,175,m_y_min[station%2],m_y_max[station%2]); 
              if (m_hist_ALFA_trig_validated_tracks[trgCond][station]) {
                 if( m_rootHistSvc->regHist(m_pathHisto + "tracking/full/" + m_trigConditions[trgCond] + "/" + m_stationNames[station] , 
                        m_hist_ALFA_trig_validated_tracks[trgCond][station]).isFailure() ) {
                       ATH_MSG_WARNING("Can not register ALFA tracking histogram: " 
                                   << (m_hist_ALFA_trig_validated_tracks[trgCond][station])->GetName());
                 }
              }
         }
     }

     for (uint32_t trgCond = 0; trgCond < 12; trgCond++) {
         for (uint32_t station = 0; station < 8; station++) {
              histTitle = m_stationNames[station] + "_f_SB_" + m_trigConditions[trgCond];
              m_hist_ALFA_trig_validated_tracks_SB[trgCond][station] = new TH2F (histTitle.c_str(), (histTitle).c_str(),
                                                                                    260,-23,23,175,m_y_min[station%2],m_y_max[station%2]); 
              if (m_hist_ALFA_trig_validated_tracks_SB[trgCond][station]) {
                 if( m_rootHistSvc->regHist(m_pathHisto + "tracking/full_SB/" + m_trigConditions[trgCond] + "/" + m_stationNames[station] , 
                        m_hist_ALFA_trig_validated_tracks_SB[trgCond][station]).isFailure() ) {
                       ATH_MSG_WARNING("Can not register ALFA tracking histogram: " 
                                   << (m_hist_ALFA_trig_validated_tracks_SB[trgCond][station])->GetName());
                 }
              }
         }
     }

     for (uint32_t trgCond = 0; trgCond < 12; trgCond++) {
         for (uint32_t station = 0; station < 8; station++) {
              histTitle = m_stationNames[station] + "_current_" + m_trigConditions[trgCond];
              m_hist_ALFA_trig_validated_tracks_1LB_current[trgCond][station] = new TH2F (histTitle.c_str(), (histTitle).c_str(),
                                                                                    260,-23,23,175,m_y_min[station%2],m_y_max[station%2]); 
              if (m_hist_ALFA_trig_validated_tracks_1LB_current[trgCond][station]) {
                 if( m_rootHistSvc->regHist(m_pathHisto + "tracking/current/" + m_trigConditions[trgCond] + "/" + m_stationNames[station] , 
                        m_hist_ALFA_trig_validated_tracks_1LB_current[trgCond][station]).isFailure() ) {
                       ATH_MSG_WARNING("Can not register ALFA tracking histogram: " 
                                   << (m_hist_ALFA_trig_validated_tracks_1LB_current[trgCond][station])->GetName());
                 } 
              }
         }
     }

     for (uint32_t trgCond = 0; trgCond < 12; trgCond++) {
         for (uint32_t station = 0; station < 8; station++) {
              histTitle = m_stationNames[station] + "_1_" + m_trigConditions[trgCond];
              m_hist_ALFA_trig_validated_tracks_1LB[trgCond][station] = new TH2F (histTitle.c_str(), (histTitle).c_str(),
                                                                                    260,-23,23,175,m_y_min[station%2],m_y_max[station%2]); 
              if (m_hist_ALFA_trig_validated_tracks_1LB[trgCond][station]) {
                 if( m_rootHistSvc->regHist(m_pathHisto + "tracking/reset1LB/" + m_trigConditions[trgCond] + "/" + m_stationNames[station] , 
                        m_hist_ALFA_trig_validated_tracks_1LB[trgCond][station]).isFailure() ) {
                       ATH_MSG_WARNING("Can not register ALFA tracking histogram: " 
                                   << (m_hist_ALFA_trig_validated_tracks_1LB[trgCond][station])->GetName());
                 } 
              }
         }
     }

     for (uint32_t trgCond = 0; trgCond < 12; trgCond++) {
         for (uint32_t station = 0; station < 8; station++) {
              histTitle = m_stationNames[station] + "_10_" + m_trigConditions[trgCond];
              m_hist_ALFA_trig_validated_tracks_10LB[trgCond][station] = new TH2F (histTitle.c_str(), (histTitle).c_str(),
                                                                                    260,-23,23,175,m_y_min[station%2],m_y_max[station%2]); 
              if (m_hist_ALFA_trig_validated_tracks_10LB[trgCond][station]) {
                 if( m_rootHistSvc->regHist(m_pathHisto + "tracking/reset10LB/" + m_trigConditions[trgCond] + "/" + m_stationNames[station] , 
                        m_hist_ALFA_trig_validated_tracks_10LB[trgCond][station]).isFailure() ) {
                       ATH_MSG_WARNING("Can not register ALFA tracking histogram: " 
                                   << (m_hist_ALFA_trig_validated_tracks_10LB[trgCond][station])->GetName());
                 }
              }
         }
     }

     for (uint32_t trgCond = 0; trgCond < 12; trgCond++) {
         for (uint32_t station = 0; station < 8; station++) {
              histTitle = m_stationNames[station] + "_60_" + m_trigConditions[trgCond];
              m_hist_ALFA_trig_validated_tracks_60LB[trgCond][station] = new TH2F (histTitle.c_str(), (histTitle).c_str(),
                                                                                    260,-23,23,175,m_y_min[station%2],m_y_max[station%2]); 
              if (m_hist_ALFA_trig_validated_tracks_60LB[trgCond][station]) {
                 if( m_rootHistSvc->regHist(m_pathHisto + "tracking/reset60LB/" + m_trigConditions[trgCond] + "/" + m_stationNames[station] , 
                        m_hist_ALFA_trig_validated_tracks_60LB[trgCond][station]).isFailure() ) {
                       ATH_MSG_WARNING("Can not register ALFA tracking histogram: " 
                                   << (m_hist_ALFA_trig_validated_tracks_60LB[trgCond][station])->GetName());
                 }
              }
         }
     }
  }

  if ( m_doPMFMonitoring.value() ) {

     std::string histTitle;

     for (uint32_t station = 0; station < 8; station++) {
         histTitle = "RP_" + std::to_string(station+1) + " PMT_activity";
         m_hist_pmfMonitoring[station] = new TH2F (histTitle.c_str(), (histTitle + " all PMTs").c_str(), 64,0.,64.,23.,1.,24.);
              if (m_hist_pmfMonitoring[station]) {
                 if( m_rootHistSvc->regHist(m_pathHisto + "detectors/"+ m_stationNames[station] + "/" + (m_hist_pmfMonitoring[station])->GetName(), 
                       m_hist_pmfMonitoring[station]).isFailure() ) {
                       ATH_MSG_WARNING("Can not register ALFA PMT monitoring histogram: " 
                                   << (m_hist_pmfMonitoring[station])->GetName());
                 }
              }
     }
  }

  if ( m_doODDistance.value() ) {

     std::string histTitle;

     for (uint32_t iDet = 0; iDet < 8; iDet++) {
      for (uint32_t iSide=0; iSide<2; iSide++) {
         histTitle = "RP_" + std::to_string(iDet+1) + "_" + std::to_string(iSide) + " position";
         m_hist_PosDetector[iDet][iSide] = new TH1F (histTitle.c_str(), (histTitle).c_str(), 200.,-145.0,-125.0);
         if (m_hist_PosDetector[iDet][iSide]) {
             if( m_rootHistSvc->regHist(m_pathHisto + "OD/"+ m_stationNames[iDet] + "/" + (m_hist_PosDetector[iDet][iSide])->GetName(), 
                     m_hist_PosDetector[iDet][iSide]).isFailure() ) {
                     ATH_MSG_WARNING("Can not register ALFA PMT monitoring histogram: " 
                                 << (m_hist_PosDetector[iDet][iSide])->GetName());
             }
         }
       }
     }
     for (uint32_t iStation = 0; iStation < 4; iStation++) {
       for (uint32_t iSide=0; iSide<2; iSide++) {
         histTitle = "Distance_" + std::to_string(2*iStation+1) + "_" + std::to_string(2*iStation+2) + "_side_" + std::to_string(iSide);
         m_hist_DistStation[2*iStation][iSide] = new TH1F (histTitle.c_str(), (histTitle).c_str(), 401.,-20.05,20.05);
         if (m_hist_DistStation[2*iStation][iSide]) {
             if( m_rootHistSvc->regHist(m_pathHisto + "OD/"+ m_stationNames[2*iStation] + "/" + (m_hist_DistStation[2*iStation][iSide])->GetName(),                   
                     m_hist_DistStation[2*iStation][iSide]).isFailure() ) {
                     ATH_MSG_WARNING("Can not register ALFA PMT monitoring histogram: "  
                                 << (m_hist_DistStation[2*iStation][iSide])->GetName());
             }
         }
  
       }
     }
  }


  if ( m_doTiming.value() ) {
    // *-- Timing histogram for monitoring algorithm
    m_hist_timeALFA = new TH1F (m_histProp_timeALFA.value().title().c_str(),
				  (m_histProp_timeALFA.value().title()+";ms").c_str(),
				  m_histProp_timeALFA.value().bins(),
				  m_histProp_timeALFA.value().lowEdge(),
				  m_histProp_timeALFA.value().highEdge());
    if (m_hist_timeALFA) {
      //      m_hist_timeMuCTPi->SetBit(TH1::kCanRebin);
      if( m_rootHistSvc->regHist(m_pathHisto + m_hist_timeALFA->GetName(), m_hist_timeALFA).isFailure() ) {
	ATH_MSG_WARNING("Can not register monitoring histogram: " << m_hist_timeALFA->GetName());
      }
    }
  }

  // release histogramming service
  // when we plan to book now histograms at the LB boundaries we should not release the histogramming service ...m_rootHistSvc.release().ignore();

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode TrigALFAROBMonitor::endRun() {

  ATH_MSG_INFO("endRun()");
  reset1LBhistos(m_LB);

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
bool TrigALFAROBMonitor::verifyALFAROBChecksum(const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment& robFrag) {

  bool failed_checksum(false);
  OFFLINE_FRAGMENTS_NAMESPACE::PointerType it(0); 
  uint32_t current_value(0);

  // print check for received ROB
  if(msgLvl(MSG::VERBOSE)) {
    robFrag.payload(it);
    current_value = eformat::helper::checksum(robFrag.checksum_type(), it, robFrag.payload_size_word());

    ATH_MSG_DEBUG(
     " ALFA ROB id = 0x"             << std::setw(6)  << MSG::hex << robFrag.source_id() << MSG::dec 
	<< " checksum: type = "            << std::setw(2)  << robFrag.checksum_type()
	<< " value = "                     << std::setw(12) << robFrag.checksum_value()
	<< " value (recalculated) = "      << std::setw(12) << current_value
	<< " check = "                     << std::setw(2)  << robFrag.checksum());
  }

  // checksum test failed
  if ( not robFrag.checksum() ) {
    failed_checksum = true;

    // recalculate checksum value
    robFrag.payload(it);
    current_value = eformat::helper::checksum(robFrag.checksum_type(), it, robFrag.payload_size_word());

    // print warning
    ATH_MSG_WARNING(
	   " ALFA ROB checksum verification failed." 
	<< " ALFA ROB id = 0x"             << std::setw(6)  << MSG::hex << robFrag.source_id() << MSG::dec 
	<< " checksum type = "             << std::setw(2)  << robFrag.checksum_type()
	<< " value = "                     << std::setw(12) << robFrag.checksum_value()
	<< " value (recalculated) = "      << std::setw(12) << current_value
	<< " check = "                     << std::setw(2)  << robFrag.checksum());

    // fill the histograms
    std::ostringstream ost;
    ost << "0x" << std::hex << robFrag.source_id();
    if (m_hist_failedChecksumForALFAROB) {
      scoped_lock_histogram lock;
      m_hist_failedChecksumForALFAROB->Fill((ost.str()).c_str(), 1.);
      m_hist_failedChecksumForALFAROB->LabelsDeflate("X");
    }
  }

  return failed_checksum;

}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
void TrigALFAROBMonitor::verifyROBStatusBits(const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment& robFrag) {

  // print check for received ROB
	ATH_MSG_VERBOSE(" verifyROBStatusBits: ROB id = 0x" << std::setw(6)  << MSG::hex << robFrag.source_id() << MSG::dec);

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
uint32_t TrigALFAROBMonitor::decodeALFA(const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment& robFrag) {

  ATH_MSG_DEBUG(" decodeALFA: ROB id = 0x" << std::setw(6)  << MSG::hex << robFrag.source_id() << MSG::dec);

  uint32_t formatVersion = robFrag.rod_version();
  uint32_t evtNum        = robFrag.rod_lvl1_id();
  uint32_t robFragSize   = robFrag.fragment_size_word();
  uint32_t rodFragSize   = robFrag.rod_fragment_size_word();
  uint32_t robId         = robFrag.source_id();
  uint32_t rodId         = robFrag.rod_source_id();
  //const uint32_t bcId    = robFrag.rod_bc_id();

  const uint32_t* status;
  robFrag.rod_status( status );
  //uint32_t nstatus = robFrag.rod_nstatus();

  //uint32_t errorStat( 0 );
  //if( nstatus > 0 ) errorStat = static_cast< uint32_t >( *status );

  ATH_MSG_DEBUG( "ALFA ROB ID 0x" << MSG::hex << robId <<  " ROD ID 0x"
				     << rodId << MSG::dec << " ROB fragment size "
				     << robFragSize << " ROD fragment size " << rodFragSize << " evtNum " <<evtNum );

  // check if we have real data in the ROB - ALFA has fixed fragment size of 0x1e1 - skip such event and return if not real
  if (robFragSize != 0x1e1) return (1);

  if ((rodId == (uint32_t)m_lvl1ALFA1ROBid.value()) || (rodId == (uint32_t)m_lvl1ALFA2ROBid.value())) {
    ATH_MSG_DEBUG( "   Found ALFA ROB " << std::to_string(rodId) );
    ATH_MSG_DEBUG( "   Dumping ALFA ROB header - data - trailer words:" );
    /* Create header */
    ROIB::Header ALFAHead( rodId, evtNum, formatVersion);
    /* Create content body */
    const uint32_t* data;
    robFrag.rod_data( data );

    const uint32_t* lwcPtr = data + 1;
    const uint32_t* twcPtr = lwcPtr + ((*lwcPtr) & 0xffff) - 1;

     
    uint32_t ndata = robFrag.rod_ndata();
    for( uint32_t i = 0; i < ndata; ++i, ++data ) {
      ATH_MSG_DEBUG( MSG::dec<< " i: "<<i<<"       0x" << MSG::hex << std::setw( 8 )
	    << static_cast< uint32_t >( *data ) );
    } 
    /* Create trailer */
    /*
    ROIB::Trailer ALFATrail( ndata, errorStat );
      if (log.level() <= MSG::DEBUG) {
	log << MSG::DEBUG ;
        ALFATrail.dumpData( log );
      }
    */

  {
    for (int layer = 0; layer < 10; layer++) {
        for (int station = 0; station < 8; station++) {
            m_pV[station][layer].clear();
            m_pU[station][layer].clear();
        }
    }
   m_triggerHitPatternReady.clear();
   m_triggerHitPattern.clear();

    for (int station=0; station<8; station++)
        for (int layer=0; layer<3; layer++)
            for (int fiber=0; fiber<30; fiber++) {
                m_FiberHitsODPos[station][layer][fiber] = false;
                m_FiberHitsODNeg[station][layer][fiber] = false;
            }

    
    while ( 1 ) {
	uint32_t mbNb = 0; // from 20.07.2010 MB numbers are as PMF0 15-0 bits - counting from 1 and coded as 1 from N 
        
        // check consistency of the ROD data - if data from LWC point to TWC
        if ((*lwcPtr & 0xff000000) != 0x81000000) {
	    //AlfaEventObj->framingStatus |= 0x4;
	    //char hex_display[100];
	    //sprintf(hex_display, "0x%x", *lwcPtr);
            m_hist_corruptedROD_LB->Fill(m_LB, rodId&0x1);
    	    ATH_MSG_DEBUG("ROD "<< MSG::hex<<rodId<<" skipped - LWC(-1): "<< *(lwcPtr-1) <<" LWC: "<<*lwcPtr << " LWC+1: "<< *(lwcPtr+1) );
    	    ATH_MSG_INFO("ROD "<< MSG::hex<<rodId<<"skipped - LWC(-1): "<< *(lwcPtr-1) <<" LWC: "<<*lwcPtr << " LWC+1: "<< *(lwcPtr+1) );
            return (1); //continue;
        }
        if ((*twcPtr & 0xff000000) != 0x8a000000) {
	    //AlfaEventObj->framingStatus |= 0x8;
	    //char hex_display[100];
	    //sprintf(hex_display, "0x%x", *twcPtr);
            m_hist_corruptedROD_LB->Fill(m_LB, rodId&0x1);
    	    ATH_MSG_DEBUG( "ROD "<< MSG::hex<<rodId<<" skipped - TWC: "<< *twcPtr );
    	    ATH_MSG_INFO( "ROD "<< MSG::hex<<rodId<<" skipped - TWC(-1): "<< *(twcPtr-1)<< " TWC: "<< *twcPtr <<" TWC+1: " << *(twcPtr+1) 
                           <<" LWC: " << *lwcPtr << " mbNb: "<< mbNb);
            return (1); //continue;
        }


        const uint32_t* dataPtr = lwcPtr;
        dataPtr++; // bol

	dataPtr++; // tlp - always 5 TDCs (5 captons) - use it as mask for scanning TDC data
        uint32_t tlp = (*dataPtr) & 0xffff; 
        dataPtr++; // bot

        for ( ; tlp; tlp>>=1 ) {

            dataPtr++; // TDC data
	    uint32_t data16channels = *dataPtr;
    	    //ATH_MSG_DEBUG( "current tlp: " << tlp << "; data16channels: "<< MSG::hex << std::setw( 8 )<< data16channels );
            //ERS_INFO ("Decode: mrodNb: "<<mrodNb<<" tlp: "<<tlp<<" bot:" << bot <<" data16channels: "<<data16channels);
            while ((data16channels & 0xf0000000) == 0x30000000) {
                int pmf = (data16channels >> 19 ) & 0x1F;
                int quarter = (data16channels >> 24) & 0x3;

                if ((pmf >= 1)&&(pmf <= 23)){
                  if (mbNb > 0) {
                    if (data16channels & 0x20000) {
                        // check the PMF error bit; set when PMF BCX & EventNumber counters differ from MB's
                        //AlfaEventObj->synchMBvsPMF[mbNb-1] |= (0x1)<<(pmf-1);
                    } else {
                        if (data16channels & 0xffff) {
                              decodeRealPMT (data16channels, quarter, mbNb-1, pmf);
                        }
                    }
                  }
                }
                else {
                  // PMF0 - control data: quarter=0: motherboard nb coded as 1 from N, quarter=2: "dead", quarter=3: "face"
                  // PMF24 - trigger mezzanine data: q=0: trigger pattern, q=1: QDC0, q=2: QDC1, q=3: rate_counter
                  if ( pmf == 0) {
                        if (quarter == 0){
                             mbNb =  decodePMT0 (data16channels);
                         //    AlfaEventObj->stationTrackData[mbNb-1] = 1;
                        }
                  }
                  else {
                      if ( pmf == 24) {// trigger mezzanine data
                        if (data16channels & 0x20000) {
                            // check the PMF error bit; set when PMF BCX & EventNumber counters differ from MB's
                          //  AlfaEventObj->synchMBvsPMF[mbNb-1] |= (0x1)<<(pmf-1);
                        } else {
                            //decodePMT24 (data16channels, quarter, mbNb);
                            if (quarter == 0) {
                                  m_triggerHitPattern[mbNb-1] = data16channels & 0xFFFF;
                                  m_triggerHitPatternReady[mbNb-1] = 1;
                            } else {
                            } 
                        }
                      }
                  }
                }
                dataPtr++;
                data16channels = *dataPtr;
            }

            // TDC trailer EOT ?
            if (( *dataPtr & 0xF0000000) != 0xC0000000) {
                break;
            }
            dataPtr++; //should point to BOT of next TDC or to TWC for the last TDC analysed
	}
        // dataPtr should point now to TWC. if next word is LWC continue, if EOB end otherwise error
        dataPtr++;
        if ((*dataPtr & 0xff000000) == 0x81000000)   {  // LWC
            lwcPtr = dataPtr;
            twcPtr = lwcPtr + ((*lwcPtr) & 0xffff) - 1;
            continue;
        }
        
        return (0); //break;  // end of data or data corrupted - break decoding
    } //ERS_INFO ("gnamDecode - end of decoding MROD:" << MrodId);
  }
 }
 return (0);
}

void TrigALFAROBMonitor::decodeRealPMT (uint32_t dataWord, uint32_t quarter, uint32_t mbNb, uint32_t pmf) {

  // save input stream flags

    int layerNb = pmf2layer[pmf];
    int RPNumber = mbNb2RP[mbNb];
        RPNumber = RPNumber - 1;  // to access data from array in C - which starts with index 0
	
    int mask = 0x1;
	//ATH_MSG_DEBUG( "decodeRealPMT - dataWord: " << dataWord << " quarter: " << quarter << " mbNb: " << mbNb << " pmf: " << pmf );
    for (int offset = 0 ; offset <= 15; offset++) {
       if (dataWord & mask) {
           int channel = offset + quarter*16;

           m_hist_pmfMonitoring[mbNb]->Fill(double(channel),double(pmf)); 

           if (layerNb >= 0) {

		//std::cout <<"mbNb " << mbNb << " layerNb: " << layerNb << " channel: " << channel << " maroc2fib: " << maroc2fiber[channel] << std::endl;

	   	ATH_MSG_DEBUG( "ROD data "<< "mbNb [counts from 0]: " << mbNb << " layerNb: " << layerNb << " channel: " << channel << " maroc2fib: " << maroc2fiber[channel] );
		Float_t data = mm_a_f[mbNb][layerNb][maroc2fiber[channel]];

	   	if (layerNb &0x1) {
	   		m_pV[mbNb][layerNb>>1].push_back(data);
	   	}else {
	   		m_pU[mbNb][layerNb>>1].push_back(data);
	   	}
           } else {
               // OD data
               int od_offset = (4 - pmf) * 64 + maroc2mapmt[channel];
               int side = od_channel2side[RPNumber][od_offset];

               if (od_channel2fiber[RPNumber][od_offset]<35) {
                  if (side==1) {
                          m_FiberHitsODPos[mbNb][od_channel2layer[RPNumber][od_offset]-1][od_channel2fiber[RPNumber][od_offset]-1] = true;
                   } else { 
                          m_FiberHitsODNeg[mbNb][od_channel2layer[RPNumber][od_offset]-1][od_channel2fiber[RPNumber][od_offset]-1] = true;
                  }
	   	  //ATH_MSG_INFO( "OD hit "<< "mbNb [counts from 0]: " << mbNb << "side: "<< side << " RPNumber: " << RPNumber << " od_offset: " << od_offset << " chan2layer: " << od_channel2layer[RPNumber][od_offset]-1  << 
                                            //"chan2fiber: "<<od_channel2fiber[RPNumber][od_offset]-1) ;
            }

           }
       }
       mask <<= 1;
    }
    return;
 }

uint32_t  TrigALFAROBMonitor::decodePMT0 (uint32_t dataWord) {
         uint32_t mbNb = 0;
         int mask = 1;
         for (int index = 1; index <= 8; index++) {
                 if ( mask & dataWord ) {
                         mbNb = index;
                         break;
                 }
                 mask = mask<<1;
         }
         return mbNb;
 }


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
void TrigALFAROBMonitor::dumpRoIBDataWord(uint32_t data_word) {

  if (msgLvl(MSG::DEBUG)) {
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
    ATH_MSG_DEBUG( "=================================================" );
  }
  return;
}

bool TrigALFAROBMonitor::getLvl1Result(LVL1CTP::Lvl1Result &resultL1) {

   if(evtStore()->contains<LVL1CTP::Lvl1Result>("Lvl1Result")) {

	const LVL1CTP::Lvl1Result* l1ptr = 0;    
	if(evtStore()->retrieve<LVL1CTP::Lvl1Result>(l1ptr, "Lvl1Result").isSuccess() && l1ptr) {
		resultL1 = *l1ptr;
                //ATH_MSG_INFO ("Success in retrieving Lvl1Result from StoreGate");
		return true;
	}
	else {
		//log() << MSG::WARNING << "Error retrieving Lvl1Result from StoreGate" << endmsg;
                ATH_MSG_INFO ("Error retrieving Lvl1Result from StoreGate");
		return false;
	}
    }
    else {
	if(1) /* outputLevel() <= MSG::DEBUG) */ {
		//log() << MSG::DEBUG << "Lvl1Result does not exist with key: " << m_keyL1Result << endmsg;
		ATH_MSG_INFO ("Lvl1Result does not exist with key: " << "Lvl1Result");
                return false;
	}
  }

}

bool TrigALFAROBMonitor::getHLTResult(HLT::HLTResult &resultHLT) {

   if(evtStore()->contains<HLT::HLTResult>("HLTResult_HLT")) {

	const HLT::HLTResult* hltptr = 0;    
	if(evtStore()->retrieve<HLT::HLTResult>(hltptr, "HLTResult_HLT").isSuccess() && hltptr) {
		resultHLT = *hltptr;
		return true;
	}
	else {
                ATH_MSG_INFO ("Error retrieving HLTResult from StoreGate");
		return false;
	}
    }
    else {
	if(1) /* outputLevel() <= MSG::DEBUG) */ {
		ATH_MSG_INFO ("HLTResult does not exist with key: " << "HLTResult_HLT");
                return false;
	}
    }
}

void TrigALFAROBMonitor::reset1LBhistos(int lbNumber) {

     std::ostringstream ost_LB;

     ost_LB << lbNumber;
     ATH_MSG_INFO ("reset 1LB histos: " << m_LB);
     for (uint32_t trgCond = 0; trgCond < 12; trgCond++) {
         for (uint32_t station = 0; station < 8; station++) {
              (m_hist_ALFA_trig_validated_tracks_1LB[trgCond][station])->Reset();
              (m_hist_ALFA_trig_validated_tracks_1LB[trgCond][station])->Add(m_hist_ALFA_trig_validated_tracks_1LB_current[trgCond][station]);
              (m_hist_ALFA_trig_validated_tracks_1LB_current[trgCond][station])->Reset();
         }
     }
}


void TrigALFAROBMonitor::reset10LBhistos(int lbNumber) {

     ATH_MSG_INFO ("reset 10LB histos: " << lbNumber-10 << "-"<<lbNumber);
     for (uint32_t trgCond = 0; trgCond < 12; trgCond++) {
         for (uint32_t station = 0; station < 8; station++) {
             (m_hist_ALFA_trig_validated_tracks_10LB[trgCond][station])->Reset();
         }
     }
}


void TrigALFAROBMonitor::reset60LBhistos(int lbNumber) {

     ATH_MSG_INFO ("reset 60LB histos: " << lbNumber-60 << "-"<<lbNumber);
     for (uint32_t trgCond = 0; trgCond < 12; trgCond++) {
         for (uint32_t station = 0; station < 8; station++) {
            (m_hist_ALFA_trig_validated_tracks_60LB[trgCond][station])->Reset();
         }
     }
}



void TrigALFAROBMonitor::findALFATracks( const LVL1CTP::Lvl1Result &resultL1 ) {
	float x_Rec[8];
	float y_Rec[8];
	
	float MeanPos_U=0;
	float MeanPos_V=0;
	
	float MeanCutPos_U=0;
	float MeanCutPos_V=0;
	
	float RecPos_U=0;
	float RecPos_V=0;
	
	float Closest_Fib_U = 0;
	float Closest_Fib_V = 0;
	
	int cnt_fib_U=0;
	int cnt_fib_V=0;

	int cnt_lay_U=0;
	int cnt_lay_V=0;

        std::vector <int> u_hits;
        std::vector <int> v_hits;

	float sign;

	for (int iDet=0;iDet<8;iDet++) {
		MeanPos_U=0;
		MeanPos_V=0;

		MeanCutPos_U=0;
		MeanCutPos_V=0;

		RecPos_U=0;
		RecPos_V=0;
		
		cnt_fib_U=0;
		cnt_fib_V=0;

                u_hits.clear();
                v_hits.clear();

                m_nbOfTracksInDetectors[iDet] = 0;                

                ATH_MSG_DEBUG( "findALFATracks starts" );

                for (int iLay=0;iLay<10;iLay++) {

                        if (m_pU[iDet][iLay].size()<=3 && m_pU[iDet][iLay].size()>0) u_hits.push_back(iLay);
                        if (m_pV[iDet][iLay].size()<=3 && m_pV[iDet][iLay].size()>0) v_hits.push_back(iLay);

                }

                if (u_hits.size()>=3 && v_hits.size()>=3) {
                        ATH_MSG_DEBUG( "findALFATracks 3" );
                        for (int iLay=0;iLay<(int)u_hits.size();iLay++) {
                                for (int iFib=0; iFib<(int)m_pU[iDet][u_hits[iLay]].size();iFib++) {
                                        MeanPos_U+=m_pU[iDet][u_hits[iLay]][iFib];
					cnt_fib_U++;
                                }
			}

                        for (int iLay=0;iLay<(int)v_hits.size();iLay++) {
                                for (int iFib=0; iFib<(int)m_pV[iDet][v_hits[iLay]].size();iFib++) {
                                        MeanPos_V+=m_pV[iDet][v_hits[iLay]][iFib];
					cnt_fib_V++;
                                }
                        }

                        if (cnt_fib_U > 0) MeanPos_U/=float(cnt_fib_U);
                        if (cnt_fib_V > 0) MeanPos_V/=float(cnt_fib_V);

                        cnt_fib_U=0;
                        cnt_fib_V=0;

                        for (int iLay=0;iLay<(int)u_hits.size();iLay++) {
                                for (int iFib=0; iFib<(int)m_pU[iDet][u_hits[iLay]].size();iFib++) {
                                        if (fabs(m_pU[iDet][u_hits[iLay]][iFib]-MeanPos_U)<2.) {
                                                MeanCutPos_U+=m_pU[iDet][u_hits[iLay]][iFib];
                                                cnt_fib_U++;
                                        }
                                }
			}
                        for (int iLay=0;iLay<(int)v_hits.size();iLay++) {
                                for (int iFib=0; iFib<(int)m_pV[iDet][v_hits[iLay]].size();iFib++) {
                                        if (fabs(m_pV[iDet][v_hits[iLay]][iFib]-MeanPos_V)<2.) {
                                                MeanCutPos_V+=m_pV[iDet][v_hits[iLay]][iFib];
                                                cnt_fib_V++;
                                        }
                                }
                        }

                        if (cnt_fib_U > 0) MeanCutPos_U/=float(cnt_fib_U);
                        if (cnt_fib_V > 0) MeanCutPos_V/=float(cnt_fib_V);

                        cnt_lay_U=0;
                        cnt_lay_V=0;

                        for (int iLay=0;iLay<(int)u_hits.size();iLay++) {

                                cnt_fib_U=0;
                                float minDist=2.;
                                for (int iFib=0; iFib<(int)m_pU[iDet][u_hits[iLay]].size();iFib++) {
                                        if (fabs(m_pU[iDet][u_hits[iLay]][iFib]-MeanCutPos_U)<minDist) {
                                                minDist=fabs(m_pU[iDet][u_hits[iLay]][iFib]-MeanCutPos_U);
                                                Closest_Fib_U=m_pU[iDet][u_hits[iLay]][iFib];
                                                if (cnt_fib_U==0) cnt_fib_U++;
                                        }
                                }
                                if (cnt_fib_U==1) {RecPos_U+=Closest_Fib_U; cnt_lay_U++;}
			}

                        for (int iLay=0;iLay<(int)v_hits.size();iLay++) {
                                cnt_fib_V=0;
                                float minDist=2.;
                                for (int iFib=0; iFib<(int)m_pV[iDet][v_hits[iLay]].size();iFib++) {
                                        if (fabs(m_pV[iDet][v_hits[iLay]][iFib]-MeanCutPos_V)<minDist) {
                                                minDist=fabs(m_pV[iDet][v_hits[iLay]][iFib]-MeanCutPos_V);
                                                Closest_Fib_V=m_pV[iDet][v_hits[iLay]][iFib];
                                                if (cnt_fib_V==0) cnt_fib_V++;
                                        }
                                }
                                if (cnt_fib_V==1) {RecPos_V+=Closest_Fib_V; cnt_lay_V++;}
                        }

                        if (cnt_lay_U>3 && cnt_lay_V>3) {

                                if (iDet%2==0) sign=1.;
                                else sign=-1.;

                                RecPos_U/=float(cnt_lay_U);
                                RecPos_V/=float(cnt_lay_V);

                                x_Rec[iDet] = (RecPos_U-RecPos_V)/2.;
                                y_Rec[iDet] = sign*(-(RecPos_V+RecPos_U)/2.-115.);

       		                const std::vector<uint32_t>& itemsBP = resultL1.itemsBeforePrescale();
                                for (std::map<int, int>::iterator it = m_map_TrgItemNumbersToHistGroups.begin(); it != m_map_TrgItemNumbersToHistGroups.end(); ++it) {
                                       int word = it->first>>5;
                                       int offset = (it->first)%32;
                                       if (itemsBP.at(word) & 1<<offset) {
                                             m_hist_ALFA_trig_validated_tracks[it->second][iDet]->Fill(x_Rec[iDet],y_Rec[iDet]);
                                             m_hist_ALFA_trig_validated_tracks_1LB_current[it->second][iDet]->Fill(x_Rec[iDet],y_Rec[iDet]);
                                             m_hist_ALFA_trig_validated_tracks_10LB[it->second][iDet]->Fill(x_Rec[iDet],y_Rec[iDet]);
                                             m_hist_ALFA_trig_validated_tracks_60LB[it->second][iDet]->Fill(x_Rec[iDet],y_Rec[iDet]);
                                             if (m_SBflag) {
                                                m_hist_ALFA_trig_validated_tracks_SB[it->second][iDet]->Fill(x_Rec[iDet],y_Rec[iDet]);
                                             }
                                             //ATH_MSG_INFO ("found track in det: "<<iDet<<" item: "<<it->first<<" in word: "<<word<<" offset: "<<offset);
                    			}
                		}
                                m_nbOfTracksInDetectors[iDet]++;
                                //HitMapAggr[iDet]->Fill(x_Rec[iDet],y_Rec[iDet]);
                        }
                        else {
                                x_Rec[iDet] = -9999.;
                                y_Rec[iDet] = -9999.;
                        }
                }
                else {
                          x_Rec[iDet] = -9999.;
                          y_Rec[iDet] = -9999.;
                }

        }

        const std::vector<uint32_t>& itemsBP = resultL1.itemsBeforePrescale();
        if (itemsBP.at(m_elast15>>5) & (1 <<(m_elast15%32))) {
           m_hist_goodData->Fill(1.);
           m_hist_goodDataLB15->Fill(m_LB, 1.);
           if ((m_nbOfTracksInDetectors[0] <=2) && (m_nbOfTracksInDetectors[2] <=2) && (m_nbOfTracksInDetectors[5]<=2) && (m_nbOfTracksInDetectors[7] <= 2) &&
                 (m_nbOfTracksInDetectors[0]>0) && (m_nbOfTracksInDetectors[2] >0) && (m_nbOfTracksInDetectors[5]>0) && (m_nbOfTracksInDetectors[7] > 0) ) {
              m_hist_goodData->Fill(2.);
              m_hist_goodDataLB15->Fill(m_LB, 2.);
           }
        }
        if (itemsBP.at(m_elast18>>5) & (1 <<(m_elast18%32))) {
           m_hist_goodData->Fill(4.);
           m_hist_goodDataLB18->Fill(m_LB, 1.);
           if ((m_nbOfTracksInDetectors[1] <=2) && (m_nbOfTracksInDetectors[3] <=2) && (m_nbOfTracksInDetectors[4]<=2) && (m_nbOfTracksInDetectors[6] <= 2) &&
                 (m_nbOfTracksInDetectors[1]>0) && (m_nbOfTracksInDetectors[3] >0) && (m_nbOfTracksInDetectors[4]>0) && (m_nbOfTracksInDetectors[6] > 0) ) {
              m_hist_goodData->Fill(5.);
              m_hist_goodDataLB18->Fill(m_LB, 2.);
           }
        }

/*
        if (x_Rec[0]>-9000. && x_Rec[2]>-9000. && x_Rec[5]>-9000. && x_Rec[7]>-9000.) {
                  for (int iDet=0;iDet<4;iDet++) HitMap_Elas[Arm_0[iDet]]->Fill(x_Rec[Arm_0[iDet]],y_Rec[Arm_0[iDet]]);
                  for (int iDet=0;iDet<4;iDet++) HitMap_ElasAggr[Arm_0[iDet]]->Fill(x_Rec[Arm_0[iDet]],y_Rec[Arm_0[iDet]]);
                  Bkg_estim[0]->Fill(x_Rec[0],(x_Rec[0]-x_Rec[2])*1.e6/4130.);
                  Bkg_estim[2]->Fill(x_Rec[2],(x_Rec[0]-x_Rec[2])*1.e6/4130.);
                  Bkg_estim[5]->Fill(x_Rec[5],(x_Rec[7]-x_Rec[5])*1.e6/4130.);
                  Bkg_estim[7]->Fill(x_Rec[7],(x_Rec[7]-x_Rec[5])*1.e6/4130.);
        }

        if (x_Rec[1]>-9000. && x_Rec[3]>-9000. && x_Rec[4]>-9000. && x_Rec[6]>-9000.) {
                  for (int iDet=0;iDet<4;iDet++) HitMap_Elas[Arm_1[iDet]]->Fill(x_Rec[Arm_1[iDet]],y_Rec[Arm_1[iDet]]);
                  for (int iDet=0;iDet<4;iDet++) HitMap_ElasAggr[Arm_1[iDet]]->Fill(x_Rec[Arm_1[iDet]],y_Rec[Arm_1[iDet]]);
                  Bkg_estim[1]->Fill(x_Rec[1],(x_Rec[1]-x_Rec[3])*1.e6/4130.);
                  Bkg_estim[3]->Fill(x_Rec[3],(x_Rec[1]-x_Rec[3])*1.e6/4130.);
                  Bkg_estim[4]->Fill(x_Rec[4],(x_Rec[6]-x_Rec[4])*1.e6/4130.);
                  Bkg_estim[6]->Fill(x_Rec[6],(x_Rec[6]-x_Rec[4])*1.e6/4130.);

	}

	{
		unsigned int localTrigger = alfaTriggerPattern;
                  int mask = 1;
		  int anyTrigger = 0;
                  for(int offset = 0; offset <7; offset++) {
                      if (mask & localTrigger) {
		         alfaTriggersAtCTP->Fill((float)(offset+1));
			 anyTrigger++;
                         for (int iDet=0;iDet<8;iDet++) {
			    if (x_Rec[iDet] > -9000.) {
                                 trigValidatedTracks[offset][iDet]->Fill(x_Rec[iDet],y_Rec[iDet]);
			     }
                         }
                      }
                      mask <<= 1;
                  }
		  if (anyTrigger == 0) {
		        alfaTriggersAtCTP->Fill(-1.);
		  }
	}
*/

   return;
}  

void TrigALFAROBMonitor::findODTracks( ) {

    int Multiplicity[3], FibHit, Index=10;
    bool FoundTrack[2];
    double Pos[2];

    //I don't know if you loop over detectors....
    for (int iStation=0;iStation<4;iStation++){
        for (int iSide=0;iSide<2;iSide++){
            FibHit=-1;

            FoundTrack[0] = false;
            FoundTrack[1] = false;
            if (iSide==0){//If we are in the positive side of the detectors

                //IMPORTANT: Here you have to require a fired trigger in the relevant OD. I do it via the variable TrigPat but I don't know how it works in your code.
                //Loop over Upper and Lower detector of a station. Both detectors should have a track
                for (int iUL=0;iUL<2;iUL++){
                    Pos[iUL]=0;
                    if ( ! (m_triggerHitPatternReady[iStation*2+iUL])) {
                        continue;
                    } else {
                        if ( !(m_triggerHitPattern[iStation*2+iUL] &0x8000))
                            continue;
                    }
                    //ATH_MSG_INFO ("in DO search tracks : "<<m_triggerHitPattern[iStation*2+iUL] );
                    int MinMultipl = 60;

                    //Loop over OD layers to search for tracks where at least one layer has only one hit.
                    for (int iLay=0;iLay<3;iLay++){

                        Multiplicity[iLay]=0;
                        for (int iFib=0;iFib<30;iFib++){
                           if (m_FiberHitsODPos[iStation*2+iUL][iLay][iFib]){
                                FibHit = iFib;
                                Multiplicity[iLay] ++;
                           }
                        }
                        if (Multiplicity[iLay] < MinMultipl) MinMultipl = Multiplicity[iLay];
                           // hMultipl[iStation*2+iUL][iSide][iLay]->Fill(Multiplicity[iLay]);
                        if (FibHit==-1) continue;

                        //Go through the LookUpTable for this fiber hit: (the multiplicity for the layer MUST be equal to one.
                        //In all the following places where I have "sFiberHitsODPos" we need to rewrite it with the fiber_to_maroc variables
                        if (FoundTrack[iUL]) continue;
                        for (int iFib1 = (FibHit>0 ? FibHit-1 : 0); iFib1< (FibHit<29 ? FibHit+2 : 30) && Multiplicity[iLay]==1;iFib1++){
                            for (int iFib2 = (FibHit>0 ? FibHit-1 : 0); iFib2< (FibHit<29 ? FibHit+2 : 30);iFib2++){
                                if (abs(iFib1-iFib2)>1) continue;
                                if((FibHit-iFib1 == -1) && (FibHit-iFib2 == -1)) Index = 6;
                                if((FibHit-iFib1 == -1) && (FibHit-iFib2 == 0)) Index = 5;
                                if((FibHit-iFib1 == 0) && (FibHit-iFib2 == -1)) Index = 4;
                                if((FibHit-iFib1 == 0) && (FibHit-iFib2 == 0)) Index = 3;
                                if((FibHit-iFib1 == 0) && (FibHit-iFib2 == 1)) Index = 2;
                                if((FibHit-iFib1 == 1) && (FibHit-iFib2 == 0)) Index = 1;
                                if((FibHit-iFib1 == 1) && (FibHit-iFib2 == 1)) Index = 0;
                                if (m_FiberHitsODPos[iStation*2+iUL][(iLay+1)%3][iFib1] && m_FiberHitsODPos[iStation*2+iUL][(iLay+2)%3][iFib2]){
                                    if (iFib1<29 && m_FiberHitsODPos[iStation*2+iUL][(iLay+1)%3][iFib1+1]){
                                        if (iFib2<29 && m_FiberHitsODPos[iStation*2+iUL][(iLay+2)%3][iFib2+1]){
                                            Pos[iUL] = LUT[(iStation*2+iUL)*6+iSide*3+iLay][FibHit*28+3*7+Index];
                                            if (Pos[iUL]>-10){
                                                Pos[iUL] = LUT[(iStation*2+iUL)*6+iSide*3+iLay][FibHit*28+2*7+Index];
                                                if (Pos[iUL]>-10){
                                                    Pos[iUL] = LUT[(iStation*2+iUL)*6+iSide*3+iLay][FibHit*28+1*7+Index];
                                                    if (Pos[iUL]>-10){
                                                        Pos[iUL] = LUT[(iStation*2+iUL)*6+iSide*3+iLay][FibHit*28+0*7+Index];
                                                    }
                                                }
                                            }
                                        }
                                        else{
                                            Pos[iUL] = LUT[(iStation*2+iUL)*6+iSide*3+iLay][FibHit*28+1*7+Index];
                                            if (Pos[iUL]>-10) Pos[iUL] = LUT[(iStation*2+iUL)*6+iSide*3+iLay][FibHit*28+0*7+Index];
                                        }
                                    }
                                    else{
                                        if (iFib2<29 && m_FiberHitsODPos[iStation*2+iUL][(iLay+2)%3][iFib2+1]){
                                            Pos[iUL] = LUT[(iStation*2+iUL)*6+iSide*3+iLay][FibHit*28+2*7+Index];
                                            if (Pos[iUL]>-10) Pos[iUL] = LUT[(iStation*2+iUL)*6+iSide*3+iLay][FibHit*28+0*7+Index];
                                        }
                                        else Pos[iUL] = LUT[(iStation*2+iUL)*6+iSide*3+iLay][FibHit*28+0*7+Index];
                                    }
                                }
                                if (Pos[iUL]<-10) { 
                                    FoundTrack[iUL]=true;
                                    break;
                                }
                                if (FoundTrack[iUL]) break;
                            }
                            if (FoundTrack[iUL]) break;
                        }
                    }//end of iLay-loop
                    // hMinMultipl[iStation*2+iUL][iSide]->Fill(MinMultipl);
                    // hMultipl2D[iStation*2+iUL][iSide][0]->Fill(Multiplicity[0],Multiplicity[1]);
                    // hMultipl2D[iStation*2+iUL][iSide][1]->Fill(Multiplicity[0],Multiplicity[2]);
                    // hMultipl2D[iStation*2+iUL][iSide][2]->Fill(Multiplicity[1],Multiplicity[2]);
                }//end of iUL-loop

            } else {//The same just for the negative side
                   
                for (int iUL=0;iUL<2;iUL++){
                    Pos[iUL]=0;
                 FoundTrack[iUL] = false;
                    if ( ! (m_triggerHitPatternReady[iStation*2+iUL])) {
                        continue;
                    } else {
                       if ( !(m_triggerHitPattern[iStation*2+iUL] &0x2000))
                        continue;
                       }
                    int MinMultipl = 60;
                    for (int iLay=0;iLay<3;iLay++){
                        Multiplicity[iLay]=0;
                        for (int iFib=0;iFib<30;iFib++){
                            if (m_FiberHitsODNeg[iStation*2+iUL][iLay][iFib]){
                                FibHit = iFib;
                                Multiplicity[iLay] ++;
                            }
                        }

                        if (Multiplicity[iLay] < MinMultipl) MinMultipl = Multiplicity[iLay];
                               //           hMultipl[iStation*2+iUL][iSide][iLay]->Fill(Multiplicity[iLay]);
                        if (FibHit==-1) continue;

                        if (FoundTrack[iUL]) continue;
                        for (int iFib1 = (FibHit>0 ? FibHit-1 : 0); iFib1< (FibHit<29 ? FibHit+2 : 30) && Multiplicity[iLay]==1;iFib1++){
                            for (int iFib2 = (FibHit>0 ? FibHit-1 : 0); iFib2< (FibHit<29 ? FibHit+2 : 30);iFib2++){
                                if (abs(iFib1-iFib2)>1) continue;
                                if((FibHit-iFib1 == -1) && (FibHit-iFib2 == -1)) Index = 6;
                                if((FibHit-iFib1 == -1) && (FibHit-iFib2 == 0)) Index = 5;
                                if((FibHit-iFib1 == 0) && (FibHit-iFib2 == -1)) Index = 4;
                                if((FibHit-iFib1 == 0) && (FibHit-iFib2 == 0)) Index = 3;
                                if((FibHit-iFib1 == 0) && (FibHit-iFib2 == 1)) Index = 2;
                                if((FibHit-iFib1 == 1) && (FibHit-iFib2 == 0)) Index = 1;
                                if((FibHit-iFib1 == 1) && (FibHit-iFib2 == 1)) Index = 0;
                                if (m_FiberHitsODNeg[iStation*2+iUL][(iLay+1)%3][iFib1] && m_FiberHitsODNeg[iStation*2+iUL][(iLay+2)%3][iFib2]){
                                    if (iFib1<29 && m_FiberHitsODNeg[iStation*2+iUL][(iLay+1)%3][iFib1+1]){
                                        if (iFib2<29 && m_FiberHitsODNeg[iStation*2+iUL][(iLay+2)%3][iFib2+1]){
                                            Pos[iUL] = LUT[(iStation*2+iUL)*6+iSide*3+iLay][FibHit*28+3*7+Index];
                                            if (Pos[iUL]>-10){
                                                Pos[iUL] = LUT[(iStation*2+iUL)*6+iSide*3+iLay][FibHit*28+2*7+Index];
                                                if (Pos[iUL]>-10){
                                                    Pos[iUL] = LUT[(iStation*2+iUL)*6+iSide*3+iLay][FibHit*28+1*7+Index];
                                                    if (Pos[iUL]>-10){
                                                        Pos[iUL] = LUT[(iStation*2+iUL)*6+iSide*3+iLay][FibHit*28+0*7+Index];
                                                    }
                                                }
                                            }
                                        }
                                        else{
                                            Pos[iUL] = LUT[(iStation*2+iUL)*6+iSide*3+iLay][FibHit*28+1*7+Index];
                                            if (Pos[iUL]>-10) Pos[iUL] = LUT[(iStation*2+iUL)*6+iSide*3+iLay][FibHit*28+0*7+Index];
                                        }
                                    }
                                    else{
                                        if (iFib2<29 && m_FiberHitsODNeg[iStation*2+iUL][(iLay+2)%3][iFib2+1]){
                                            Pos[iUL] = LUT[(iStation*2+iUL)*6+iSide*3+iLay][FibHit*28+2*7+Index];
                                            if (Pos[iUL]>-10) Pos[iUL] = LUT[(iStation*2+iUL)*6+iSide*3+iLay][FibHit*28+0*7+Index];
                                        }
                                        else Pos[iUL] = LUT[(iStation*2+iUL)*6+iSide*3+iLay][FibHit*28+0*7+Index];
                                    }
                                }
                                if (Pos[iUL]<-10) {
                                   FoundTrack[iUL]=true;
                                   break;
                                }
                            }
                            if (FoundTrack[iUL]) break;
                        }
                        if (FoundTrack[iUL]) break;
                    }//end of iLay-loop
                    // hMinMultipl[iStation*2+iUL][iSide]->Fill(MinMultipl);
                    // hMultipl2D[iStation*2+iUL][iSide][0]->Fill(Multiplicity[0],Multiplicity[1]);
                    // hMultipl2D[iStation*2+iUL][iSide][1]->Fill(Multiplicity[0],Multiplicity[2]);
                    // hMultipl2D[iStation*2+iUL][iSide][2]->Fill(Multiplicity[1],Multiplicity[2]);
                }//end of iUL-loop
            }

            //If we have a track in both upper and lower detector, we fill the histograms
            if (FoundTrack[0]) { 
                 m_hist_PosDetector[iStation*2][iSide]->Fill(Pos[0]);
                 m_ODtracks[iStation*2][iSide] = Pos[0];
            }
            if (FoundTrack[1]) {
                 m_hist_PosDetector[iStation*2+1][iSide]->Fill(Pos[1]);
                 m_ODtracks[iStation*2+1][iSide] = Pos[1];
            }
            //if (FoundTrack[0] && FoundTrack[1]){
            if( (m_ODtracks[iStation*2][iSide] < 0) && (m_ODtracks[iStation*2+1][iSide] < 0) ) {
                 m_hist_DistStation[2*iStation][iSide]->Fill(-m_ODtracks[iStation*2][iSide] - m_ODtracks[iStation*2+1][iSide] + alfa_edge[iStation*2] + alfa_edge[iStation*2+1]);
            }

        }//end of iSide-loop
    }//end of iStation-loop
    ATH_MSG_DEBUG ("end of findOD tracks");
}




