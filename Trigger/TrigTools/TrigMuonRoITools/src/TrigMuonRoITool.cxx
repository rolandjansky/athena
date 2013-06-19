/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonRoITool.h"
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "TrigT1Result/RoIBResult.h"
#include "TrigT1Result/MuCTPI_RDO.h"
#include "TrigT1Result/MuCTPI_MultiplicityWord_Decoder.h"
#include "TrigT1Result/MuCTPI_DataWord_Decoder.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventType.h"


TrigMuonRoITool::TrigMuonRoITool(const std::string& type, 
				 const std::string& name,
				 const IInterface*  parent): 
  AlgTool(type,name,parent),
  m_msg(0),
  m_run_no(0),
  m_event_number(0),
  m_bunch_crossing_id(0),
  m_time_stamp(0),
  m_time_stamp_ns_offset(0),
  m_lumi_block(0),
  m_storeGateSvc( "StoreGateSvc", name ),
  m_robDataProviderSvc( "ROBDataProviderSvc", name )
{
  declareInterface <ITrigMuonRoITool> ( this );

  // Declare the properties
  declareProperty("DaqMuCTPiROBid",m_daqMuCTPiROBid=0x760000);
}


TrigMuonRoITool::~TrigMuonRoITool() 
{
}


StatusCode TrigMuonRoITool::initialize()
{
  // Get a message stream instance
  m_msg = new MsgStream( msgSvc(), name() );
  logStream() << MSG::INFO << "Initializing TrigMuonRoITool - package version " << PACKAGE_VERSION << endreq ;
  
  StatusCode sc;
  sc = AlgTool::initialize();
  if (!sc.isSuccess()) {
    logStream() << MSG::ERROR << "Could not initialize the AlgTool base class." << endreq;
    return sc;
  }

  // Locate the StoreGateSvc
  sc =  m_storeGateSvc.retrieve();
  if (!sc.isSuccess()) {
    logStream() << MSG::ERROR << "Could not find StoreGateSvc" << endreq;
    return sc;
  }

  // Locate the ROBDataProviderSvc
  sc = m_robDataProviderSvc.retrieve();
  if (!sc.isSuccess()) {
    logStream() << MSG::ERROR << "Could not find ROBDataProviderSvc" << endreq;
    return sc;
  }

  // Print the property values
  logStream() << MSG::INFO << " ROB ID: DAQ muCTPi                         = " << m_daqMuCTPiROBid
	      << std::setw(6) << " (=0x" << MSG::hex << m_daqMuCTPiROBid.value() << MSG::dec << ")" << endreq;

  // Build the vector with ROB Ids to retrieve
  m_muCTPiRobIds.push_back(m_daqMuCTPiROBid.value());
  
  return StatusCode::SUCCESS; 
}


StatusCode TrigMuonRoITool::finalize()
{
  logStream() << MSG::INFO << "Finalizing TrigMuonRoITool - package version " << PACKAGE_VERSION << endreq ;

  // delete message stream
  if ( m_msg ) delete m_msg;

  StatusCode sc = AlgTool::finalize(); 
  return sc;
}


/// iterator over muon RoIs in time with event in MuCTPi format
std::vector<ROIB::MuCTPIRoI>::const_iterator TrigMuonRoITool::begin_InTimeRoIs() {
  // check that data are valid and eventually decode information
  decodeMuCTPi(logStream());
  return m_inTime_muCTPIRoIs.begin();
}
std::vector<ROIB::MuCTPIRoI>::const_iterator TrigMuonRoITool::end_InTimeRoIs() {
  // check that data are valid and eventually decode information
  decodeMuCTPi(logStream());
  return m_inTime_muCTPIRoIs.end();
}

/// iterator over muon RoIs out of time with event in MuCTPi format
/// pair < Muon RoI in RoIB format, difference: RoI(BCID)-event(BCID) >
std::vector< std::pair<ROIB::MuCTPIRoI,int> >::const_iterator TrigMuonRoITool::begin_OutOfTimeRoIs() {
  // check that data are valid and eventually decode information
  decodeMuCTPi(logStream());
  return m_outOfTime_muCTPIRoIs.begin();
}
std::vector< std::pair<ROIB::MuCTPIRoI,int> >::const_iterator TrigMuonRoITool::end_OutOfTimeRoIs() {
  // check that data are valid and eventually decode information
  decodeMuCTPi(logStream());
  return m_outOfTime_muCTPIRoIs.end();
}

/// helper to decode MuCTPi information
void TrigMuonRoITool::decodeMuCTPi(MsgStream& log) {
  // save input stream flags
  std::ios_base::fmtflags log_flags_save = (log.stream()).flags();
  char log_fill_char_save = (log.stream()).fill();

  // check if data are still valid
  // retrieve the event info information and check its validity
  // get EventInfo
  const EventInfo* p_EventInfo(0);
  StatusCode sc = m_storeGateSvc->retrieve(p_EventInfo);
  if ((sc.isFailure()) || (p_EventInfo == 0)) {
    log << MSG::WARNING << "Can't get EventInfo object for checking data validity." << endreq;
    m_inTime_muCTPIRoIs.clear();
    m_outOfTime_muCTPIRoIs.clear();
    return;
  } else {
    // check event parameters for validity. The check for run/event number is not sufficient since
    // in L2 the event number = L1 ID, which may be identical to a previous one after an ECR.
    // Only if all parameters match the present info is still valid
    if ( (m_run_no               == p_EventInfo->event_ID()->run_number()) &&
	 (m_event_number         == p_EventInfo->event_ID()->event_number()) &&
	 (m_bunch_crossing_id    == p_EventInfo->event_ID()->bunch_crossing_id()) &&
	 (m_time_stamp           == p_EventInfo->event_ID()->time_stamp()) &&
	 (m_time_stamp_ns_offset == p_EventInfo->event_ID()->time_stamp_ns_offset()) &&
	 (m_lumi_block           == p_EventInfo->event_ID()->lumi_block()) ) {
      // information is still valid
      if (log.level() <= MSG::DEBUG) {
	log << MSG::DEBUG << " decodeMuCTPi: Stored information is still valid. Complete EventID = " 
	    << *(p_EventInfo->event_ID()) << endreq; 
      }
      return ;
    } else {
      // information is outdated
      if (log.level() <= MSG::DEBUG) {
	log << MSG::DEBUG << " decodeMuCTPi: Stored information needs to be updated. Complete EventID = " 
	    << *(p_EventInfo->event_ID()) << endreq; 
      }
    }
  }

  //
  // --- Update information from DAQ ROB
  //
  // reset the containers and fill them with new data
  m_inTime_muCTPIRoIs.clear();
  m_outOfTime_muCTPIRoIs.clear();

  // in L2 the DAQ muCTPi ROB needs to be retreived first from the ROS
  m_robDataProviderSvc->addROBData(m_muCTPiRobIds);

  // get the muCTPi ROB fragment
  std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*> muCTPiRobFragmentVec;
  muCTPiRobFragmentVec.reserve(m_muCTPiRobIds.size());
  m_robDataProviderSvc->getROBData(m_muCTPiRobIds,muCTPiRobFragmentVec);
  if (muCTPiRobFragmentVec.size()==0) {
    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << " decodeMuCTPi: No muCTPi ROB found." << endreq;
    return;
  }

  // check for received ROB id and return if it doesn't match the DAQ MuCTPI ROB
  if (log.level() <= MSG::DEBUG) {
    log << MSG::DEBUG
	<< " decodeMuCTPi: ROB id = 0x" 
	<< std::setw(6)  << MSG::hex << muCTPiRobFragmentVec[0]->source_id() << MSG::dec 
	<< endreq;
  }

  if (muCTPiRobFragmentVec[0]->source_id() != (uint32_t)m_daqMuCTPiROBid.value()) {
    if (log.level() <= MSG::DEBUG) {
      log << MSG::DEBUG
	  << " decodeMuCTPi: received ROB id = 0x"
	  << std::setw(6) << MSG::hex << muCTPiRobFragmentVec[0]->source_id() << MSG::dec
	  << " does not match requested ROB id = 0x"
	  << std::setw(6) << MSG::hex << m_daqMuCTPiROBid.value() << MSG::dec
	  << endreq;
    }
    // reset log stream flags to original values
    log.flags(log_flags_save);
    return; 
  }

  //
  // --- Save event parameters
  //
  m_run_no               = p_EventInfo->event_ID()->run_number();
  m_event_number         = p_EventInfo->event_ID()->event_number();
  m_bunch_crossing_id    = p_EventInfo->event_ID()->bunch_crossing_id();
  m_time_stamp           = p_EventInfo->event_ID()->time_stamp();
  m_time_stamp_ns_offset = p_EventInfo->event_ID()->time_stamp_ns_offset();
  m_lumi_block           = p_EventInfo->event_ID()->lumi_block();

  // decode now the ROB
  uint32_t formatVersion = muCTPiRobFragmentVec[0]->rod_version();
  uint32_t evtNum        = muCTPiRobFragmentVec[0]->rod_lvl1_id();
  uint32_t robFragSize   = muCTPiRobFragmentVec[0]->fragment_size_word();
  uint32_t rodFragSize   = muCTPiRobFragmentVec[0]->rod_fragment_size_word();
  uint32_t robId         = muCTPiRobFragmentVec[0]->source_id();
  uint32_t rodId         = muCTPiRobFragmentVec[0]->rod_source_id();
  const uint32_t bcId    = muCTPiRobFragmentVec[0]->rod_bc_id();

  const uint32_t* status;
  muCTPiRobFragmentVec[0]->rod_status( status );
  /* nstatus, errorStat not used, cause compilation warning
  uint32_t nstatus = muCTPiRobFragmentVec[0]->rod_nstatus();
  uint32_t errorStat( 0 );
  if( nstatus > 0 ) errorStat = static_cast< uint32_t >( *status );
  */

  if (log.level() <= MSG::DEBUG) {
    log << MSG::DEBUG << "ROB ID 0x" << MSG::hex << robId <<  " ROD ID 0x"
	<< rodId << MSG::dec << " ROB fragment size "
	<< robFragSize << " ROD fragment size " << rodFragSize 
	<< endreq;
    log << MSG::DEBUG 
	<< " ROD Header L1 ID " << evtNum
	<< " ROD Header BCID " << bcId
	<< " ROD Header Format version " << formatVersion
     	<< endreq;
  }
 
  OFFLINE_FRAGMENTS_NAMESPACE::PointerType it_data;
  muCTPiRobFragmentVec[0]->rod_data( it_data );
  const uint32_t ndata = muCTPiRobFragmentVec[0]->rod_ndata();
  if (log.level() <= MSG::DEBUG) {
    log << MSG::DEBUG << " Dumping RoI Words:" << endreq;
    log << MSG::DEBUG << " number of data words: " << ndata << endreq;
  }

  // candidate multiplicity
  std::vector< uint32_t > candidateMultiplicity;
  // data words
  std::vector< uint32_t > dataWord;
  for( uint32_t i = 0; i < ndata; ++i, ++it_data ) {
    if( *it_data >> MuCTPI_RDO::MULT_WORD_FLAG_SHIFT ) {
      candidateMultiplicity.push_back( static_cast< uint32_t >( *it_data ) );
      if (log.level() <= MSG::DEBUG) {
	log << MSG::DEBUG << "     0x" << MSG::hex << std::setw( 8 ) << std::setfill( '0' )
	    << ( *it_data ) << " (candidate multiplicity)" << std::setfill( log_fill_char_save ) << endreq;
      }
    } else {
      dataWord.push_back( static_cast< uint32_t >( *it_data ) );
      if (log.level() <= MSG::DEBUG) {
	log << MSG::DEBUG << "     0x" << MSG::hex << std::setw( 8 ) << std::setfill( '0' )
	    << ( *it_data ) << " (candidate word)" 
	    << " (--> RoI word = 0x" << MSG::hex << std::setw( 8 ) << std::setfill( '0' ) 
	    << mirodToRoIBDataWord( *it_data ) << ")" 
	    << std::setfill( log_fill_char_save ) << endreq;
      }
    }
  }

  // create MuCTPI RDO
  MuCTPI_RDO daqmuCTPIResult( candidateMultiplicity, dataWord );       // DAQ  muCTPi Result

  // print contents
  if (log.level() <= MSG::DEBUG) {
    MuCTPI_MultiplicityWord_Decoder(daqmuCTPIResult.candidateMultiplicity()).dumpData(log);
    for(std::vector< uint32_t >::const_iterator it = daqmuCTPIResult.dataWord().begin();
	it != daqmuCTPIResult.dataWord().end(); ++it) {
      MuCTPI_DataWord_Decoder(*it).dumpData(log);
      dumpRoIBDataWord(log, mirodToRoIBDataWord(*it));
    }
  }

  // now select out the RoI candidates for the BCID which triggered the event and save them in 
  // a special list indexed by RoI ID
  uint16_t roiEventBCID = MuCTPI_MultiplicityWord_Decoder(daqmuCTPIResult.candidateMultiplicity()).getBCID();
  uint16_t roiEventNCan = MuCTPI_MultiplicityWord_Decoder(daqmuCTPIResult.candidateMultiplicity()).getNCandidates();

  m_inTime_muCTPIRoIs.reserve( roiEventNCan );
  m_outOfTime_muCTPIRoIs.reserve( roiEventNCan );

  for(std::vector< uint32_t >::const_iterator it = daqmuCTPIResult.dataWord().begin();
      it != daqmuCTPIResult.dataWord().end(); ++it) {
    // decode the RDO data word
    MuCTPI_DataWord_Decoder daqRoI(*it);

    // create a L1 RoI
    ROIB::MuCTPIRoI roI( mirodToRoIBDataWord(*it) );

    if (roiEventBCID == daqRoI.getBCID()) { // RoI matches event BCID
      m_inTime_muCTPIRoIs.push_back(roI);
    } else {
      m_outOfTime_muCTPIRoIs.push_back( std::pair<ROIB::MuCTPIRoI,int>(roI,(int(daqRoI.getBCID())-int(roiEventBCID))) );
    }
  } // end loop over data words

  // print contents of RoI arrays
  if (log.level() <= MSG::DEBUG) {
    log << MSG::DEBUG << "=================================================" << endreq;
    log << MSG::DEBUG << " RoIs in time with event BCID:  Number of RoIs = " << m_inTime_muCTPIRoIs.size() << endreq;
    log << MSG::DEBUG << "=================================================" << endreq;
    for (std::vector< ROIB::MuCTPIRoI >::iterator it = m_inTime_muCTPIRoIs.begin(); it != m_inTime_muCTPIRoIs.end(); ++it) {
      dumpRoIBDataWord(log,(*it).roIWord());
    }

    log << MSG::DEBUG << "=====================================================" << endreq;
    log << MSG::DEBUG << " RoIs out of time with event BCID:  Number of RoIs = " << m_outOfTime_muCTPIRoIs.size() << endreq;
    log << MSG::DEBUG << "=====================================================" << endreq;
    for (std::vector< std::pair<ROIB::MuCTPIRoI,int> >::iterator it = m_outOfTime_muCTPIRoIs.begin(); it != m_outOfTime_muCTPIRoIs.end(); ++it) {
      log << MSG::DEBUG << " Difference(RoI(BCID) - Event(BCID)) = " << (*it).second << endreq;
      log << MSG::DEBUG << " ------------------------------------- " << endreq;
      dumpRoIBDataWord(log,((*it).first).roIWord());
    }
  }

  // reset log stream flags to original values
  log.flags(log_flags_save);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
uint32_t TrigMuonRoITool::mirodToRoIBDataWord( uint32_t data_word ) {
  return ( ( ( data_word & 0x8000000 ) >> 4 ) | ( ( data_word & 0x3fe0000 ) >> 3 ) |
	   ( data_word & 0x3fff ) );
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
void TrigMuonRoITool::dumpRoIBDataWord( MsgStream& log, uint32_t data_word ) {

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
  return;
}
