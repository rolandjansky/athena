/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonRoITool.h"
#include "GaudiKernel/ToolFactory.h"
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
  AthAlgTool(type,name,parent),
  m_run_no(0),
  m_event_number(0),
  m_bunch_crossing_id(0),
  m_time_stamp(0),
  m_time_stamp_ns_offset(0),
  m_lumi_block(0),
  m_robDataProviderSvc( "ROBDataProviderSvc", name ),
  m_decodeMuCTPiFromROB(false)
{
  declareInterface <ITrigMuonRoITool> ( this );

  // Declare the properties
  declareProperty("DaqMuCTPiROBid",m_daqMuCTPiROBid=0x760000);
  declareProperty("DecodeMuCTPiFromROB",m_decodeMuCTPiFromROB=false);
}


TrigMuonRoITool::~TrigMuonRoITool() 
{
}


StatusCode TrigMuonRoITool::initialize()
{
  msg(MSG::INFO) << "Initializing TrigMuonRoITool - package version " << PACKAGE_VERSION << endreq ;
  
  ATH_CHECK( AthAlgTool::initialize() );
  ATH_CHECK( evtStore().retrieve() );
  ATH_CHECK( m_robDataProviderSvc.retrieve() );

  // Print the property values
  msg(MSG::INFO) << " ROB ID: DAQ muCTPi                         = " << m_daqMuCTPiROBid
	      << std::setw(6) << " (=0x" << MSG::hex << m_daqMuCTPiROBid.value() << MSG::dec << ")" << endreq;

  // Build the vector with ROB Ids to retrieve
  m_muCTPiRobIds.push_back(m_daqMuCTPiROBid.value());
  
  return StatusCode::SUCCESS; 
}


StatusCode TrigMuonRoITool::finalize()
{
  msg(MSG::INFO) << "Finalizing TrigMuonRoITool - package version " << PACKAGE_VERSION << endreq ;

  ATH_CHECK( AthAlgTool::finalize() );
  return StatusCode::SUCCESS; 
}


/// iterator over muon RoIs in time with event in MuCTPi format
std::vector<ROIB::MuCTPIRoI>::const_iterator TrigMuonRoITool::begin_InTimeRoIs() {
  // check that data are valid and eventually decode information
  decodeMuCTPi();
  return m_inTime_muCTPIRoIs.begin();
}
std::vector<ROIB::MuCTPIRoI>::const_iterator TrigMuonRoITool::end_InTimeRoIs() {
  // check that data are valid and eventually decode information
  decodeMuCTPi();
  return m_inTime_muCTPIRoIs.end();
}

/// iterator over muon RoIs out of time with event in MuCTPi format
/// pair < Muon RoI in RoIB format, difference: RoI(BCID)-event(BCID) >
std::vector< std::pair<ROIB::MuCTPIRoI,int> >::const_iterator TrigMuonRoITool::begin_OutOfTimeRoIs() {
  // check that data are valid and eventually decode information
  decodeMuCTPi();
  return m_outOfTime_muCTPIRoIs.begin();
}
std::vector< std::pair<ROIB::MuCTPIRoI,int> >::const_iterator TrigMuonRoITool::end_OutOfTimeRoIs() {
  // check that data are valid and eventually decode information
  decodeMuCTPi();
  return m_outOfTime_muCTPIRoIs.end();
}

/// helper to decode MuCTPi information
void TrigMuonRoITool::decodeMuCTPi() {

  // save input stream flags
  std::ios_base::fmtflags log_flags_save = (msg().stream()).flags();
  char log_fill_char_save = (msg().stream()).fill();

  /////////////////////////////////////
  // check if data are still valid
  /////////////////////////////////////

  // retrieve the event info information and check its validity
  const EventInfo* p_EventInfo(0);
  StatusCode sc = evtStore()->retrieve(p_EventInfo);
  if ((sc.isFailure()) || (p_EventInfo == 0)) {
    msg(MSG::WARNING) << "Can't get EventInfo object for checking data validity." << endreq;
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
      if (msgLvl(MSG::DEBUG)) {
  	msg(MSG::DEBUG) << " decodeMuCTPi: Stored information is still valid. Complete EventID = " 
  	    << *(p_EventInfo->event_ID()) << endreq; 
      }
      return; // no need to decode again MUCTPI, already done for current event
    } else {
      // information is outdated
      if (msgLvl(MSG::DEBUG)) {
  	msg(MSG::DEBUG) << " decodeMuCTPi: Stored information needs to be updated. Complete EventID = " 
  	    << *(p_EventInfo->event_ID()) << endreq; 
      }
    }
  }

  /////////////////////////////////////
  // decode MUCTPI
  /////////////////////////////////////

  // reset the containers and fill them with new data
  m_inTime_muCTPIRoIs.clear();
  m_outOfTime_muCTPIRoIs.clear();

  // save event parameters
  m_run_no               = p_EventInfo->event_ID()->run_number();
  m_event_number         = p_EventInfo->event_ID()->event_number();
  m_bunch_crossing_id    = p_EventInfo->event_ID()->bunch_crossing_id();
  m_time_stamp           = p_EventInfo->event_ID()->time_stamp();
  m_time_stamp_ns_offset = p_EventInfo->event_ID()->time_stamp_ns_offset();
  m_lumi_block           = p_EventInfo->event_ID()->lumi_block();

  // MuCTPi pointer
  const MuCTPI_RDO* daqmuCTPIResult = 0;
  
  // Build muCTPi from DAQ ROB
  ///////////////////////////////
  if(m_decodeMuCTPiFromROB) {
  
    // in L2 the DAQ muCTPi ROB needs to be retreived first from the ROS
    m_robDataProviderSvc->addROBData(m_muCTPiRobIds);

    // get the muCTPi ROB fragment
    std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*> muCTPiRobFragmentVec;
    muCTPiRobFragmentVec.reserve(m_muCTPiRobIds.size());
    m_robDataProviderSvc->getROBData(m_muCTPiRobIds,muCTPiRobFragmentVec);
    
    if (muCTPiRobFragmentVec.size()==0) {
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " decodeMuCTPi: No muCTPi ROB found." << endmsg;
      return;
    }
      
    // check for received ROB id and return if it doesn't match the DAQ MuCTPI ROB
    if (msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG)
	<< " decodeMuCTPi: ROB id = 0x" 
	<< std::setw(6)  << MSG::hex << muCTPiRobFragmentVec[0]->source_id() << MSG::dec 
	<< endreq;
    }
    
    if (muCTPiRobFragmentVec[0]->source_id() != (uint32_t)m_daqMuCTPiROBid.value()) {
      if (msgLvl(MSG::DEBUG)) {
	msg(MSG::DEBUG)
	  << " decodeMuCTPi: received ROB id = 0x"
	  << std::setw(6) << MSG::hex << muCTPiRobFragmentVec[0]->source_id() << MSG::dec
	  << " does not match requested ROB id = 0x"
	  << std::setw(6) << MSG::hex << m_daqMuCTPiROBid.value() << MSG::dec
	  << endreq;
      }
      // reset msg() stream flags to original values
      msg().flags(log_flags_save);
      return; 
    }
    
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
    // nstatus, errorStat not used, cause compilation warning
    //uint32_t nstatus = muCTPiRobFragmentVec[0]->rod_nstatus();
    //uint32_t errorStat( 0 );
    //if( nstatus > 0 ) errorStat = static_cast< uint32_t >( *status );
    //
    
    if (msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << "ROB ID 0x" << MSG::hex << robId <<  " ROD ID 0x"
		      << rodId << MSG::dec << " ROB fragment size "
		      << robFragSize << " ROD fragment size " << rodFragSize 
		      << endreq;
      msg(MSG::DEBUG) 
	<< " ROD Header L1 ID " << evtNum
	<< " ROD Header BCID " << bcId
	<< " ROD Header Format version " << formatVersion
	<< endreq;
    }
    
    OFFLINE_FRAGMENTS_NAMESPACE::PointerType it_data;
    muCTPiRobFragmentVec[0]->rod_data( it_data );
    const uint32_t ndata = muCTPiRobFragmentVec[0]->rod_ndata();
    if (msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << " Dumping RoI Words:" << endreq;
      msg(MSG::DEBUG) << " number of data words: " << ndata << endreq;
    }
    
    // candidate multiplicity
    std::vector< uint32_t > candidateMultiplicity;
    // data words
    std::vector< uint32_t > dataWord;
    for( uint32_t i = 0; i < ndata; ++i, ++it_data ) {
      if( *it_data >> MuCTPI_RDO::MULT_WORD_FLAG_SHIFT ) {
	candidateMultiplicity.push_back( static_cast< uint32_t >( *it_data ) );
	if (msgLvl(MSG::DEBUG)) {
	  msg(MSG::DEBUG) << "     0x" << MSG::hex << std::setw( 8 ) << std::setfill( '0' )
			  << ( *it_data ) << " (candidate multiplicity)" << std::setfill( log_fill_char_save ) << endreq;
	}
      } else {
	dataWord.push_back( static_cast< uint32_t >( *it_data ) );
	if (msgLvl(MSG::DEBUG)) {
	  msg(MSG::DEBUG) << "     0x" << MSG::hex << std::setw( 8 ) << std::setfill( '0' )
			  << ( *it_data ) << " (candidate word)" 
			  << " (--> RoI word = 0x" << MSG::hex << std::setw( 8 ) << std::setfill( '0' ) 
			  << mirodToRoIBDataWord( *it_data ) << ")" 
			  << std::setfill( log_fill_char_save ) << endreq;
	}
      }
    }
    
    // create MuCTPi RDO (DAQ  muCTPi Result)
    daqmuCTPIResult = new MuCTPI_RDO( candidateMultiplicity, dataWord );
  }
  
  // Retrieve the MuCTPi RDO from event store
  //////////////////////////////////////////////
  else{
    sc = evtStore()->retrieve(daqmuCTPIResult,"MUCTPI_RDO");
    if ((sc.isFailure()) || (daqmuCTPIResult == 0)) {
      msg(MSG::WARNING) << "Can't get MUCTPI_RDO object from event store." << endreq;
      m_inTime_muCTPIRoIs.clear();
      m_outOfTime_muCTPIRoIs.clear();
      return;
    }
  }
  
  // print contents
  if (msgLvl(MSG::DEBUG)) {
    MuCTPI_MultiplicityWord_Decoder(daqmuCTPIResult->candidateMultiplicity()).dumpData(msg());
    for(std::vector< uint32_t >::const_iterator it = daqmuCTPIResult->dataWord().begin();
	it != daqmuCTPIResult->dataWord().end(); ++it) {
      MuCTPI_DataWord_Decoder(*it).dumpData(msg());
      dumpRoIBDataWord(mirodToRoIBDataWord(*it));
    }
  }

  // now select out the RoI candidates for the BCID which triggered the event and save them in 
  // a special list indexed by RoI ID
  uint16_t roiEventBCID = MuCTPI_MultiplicityWord_Decoder(daqmuCTPIResult->candidateMultiplicity()).getBCID();
  uint16_t roiEventNCan = MuCTPI_MultiplicityWord_Decoder(daqmuCTPIResult->candidateMultiplicity()).getNCandidates();

  m_inTime_muCTPIRoIs.reserve( roiEventNCan );
  m_outOfTime_muCTPIRoIs.reserve( roiEventNCan );

  for(std::vector< uint32_t >::const_iterator it = daqmuCTPIResult->dataWord().begin();
      it != daqmuCTPIResult->dataWord().end(); ++it) {

    // decode the RDO data word
    MuCTPI_DataWord_Decoder daqRoI(*it);

    // create a L1 RoI
    ROIB::MuCTPIRoI roI( mirodToRoIBDataWord(*it) );

    if (roiEventBCID == daqRoI.getBCID()) { // RoI matches event BCID
      m_inTime_muCTPIRoIs.push_back(roI);
      // The line below is meant to enable testing late particles trigger chains with in-time RoIs
      /////m_outOfTime_muCTPIRoIs.push_back( std::pair<ROIB::MuCTPIRoI,int>(roI,(int(daqRoI.getBCID())-int(roiEventBCID))) );
    } else {
      m_outOfTime_muCTPIRoIs.push_back( std::pair<ROIB::MuCTPIRoI,int>(roI,(int(daqRoI.getBCID())-int(roiEventBCID))) );
    }
  } // end loop over data words
  
  // print contents of RoI arrays
  if (msgLvl(MSG::DEBUG)) {
    msg(MSG::DEBUG) << "=================================================" << endreq;
    msg(MSG::DEBUG) << " RoIs in time with event BCID:  Number of RoIs = " << m_inTime_muCTPIRoIs.size() << endreq;
    msg(MSG::DEBUG) << "=================================================" << endreq;
    for (std::vector< ROIB::MuCTPIRoI >::iterator it = m_inTime_muCTPIRoIs.begin(); it != m_inTime_muCTPIRoIs.end(); ++it) {
      dumpRoIBDataWord((*it).roIWord());
    }

    msg(MSG::DEBUG) << "=====================================================" << endreq;
    msg(MSG::DEBUG) << " RoIs out of time with event BCID:  Number of RoIs = " << m_outOfTime_muCTPIRoIs.size() << endreq;
    msg(MSG::DEBUG) << "=====================================================" << endreq;
    for (std::vector< std::pair<ROIB::MuCTPIRoI,int> >::iterator it = m_outOfTime_muCTPIRoIs.begin(); it != m_outOfTime_muCTPIRoIs.end(); ++it) {
      msg(MSG::DEBUG) << " Difference(RoI(BCID) - Event(BCID)) = " << (*it).second << endreq;
      msg(MSG::DEBUG) << " ------------------------------------- " << endreq;
      dumpRoIBDataWord(((*it).first).roIWord());
    }
  }

  //reset msg() stream flags to original values
  msg().flags(log_flags_save);

  if(m_decodeMuCTPiFromROB)
    delete daqmuCTPIResult;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
uint32_t TrigMuonRoITool::mirodToRoIBDataWord( uint32_t data_word ) {
  return ( ( ( data_word & 0x8000000 ) >> 4 ) | ( ( data_word & 0x3fe0000 ) >> 3 ) |
	   ( data_word & 0x3fff ) );
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
void TrigMuonRoITool::dumpRoIBDataWord( uint32_t data_word ) {

  ROIB::MuCTPIRoI roI(data_word);

  std::string loc = "UNDEFINED"; 
  if( roI.getSectorLocation() == MuCTPI_RDO::ENDCAP )
    loc = "ENDCAP";
  else if( roI.getSectorLocation() == MuCTPI_RDO::FORWARD )
    loc = "FORWARD";
  else if( roI.getSectorLocation() == MuCTPI_RDO::BARREL )
    loc = "BARREL";

  msg(MSG::DEBUG) << "RoIB word               : 0x"
      << MSG::hex << roI.roIWord() << MSG::dec << endreq;
  msg(MSG::DEBUG) << "Threshold               :  pt" << roI.pt() << endreq;
  msg(MSG::DEBUG) << "Sector location         :  " << loc << endreq;
  std::string sectorOffset("");  
  if ((roI.getSectorAddress() & MuCTPI_RDO::SECTOR_HEMISPHERE_MASK) &&
      (roI.getSectorLocation() == MuCTPI_RDO::BARREL)) sectorOffset = " + 32 for Hemisphere = 1 "; 
  msg(MSG::DEBUG) << "Sector ID               :  " << roI.getSectorID() << sectorOffset << endreq;
  msg(MSG::DEBUG) << "Sector addr             :  0x" << MSG::hex
      << roI.getSectorAddress() << MSG::dec << endreq;
  msg(MSG::DEBUG) << "Sector overflow         :  " << roI.getSectorOverflow() << endreq;
  msg(MSG::DEBUG) << "RoI overflow            :  " << roI.getRoiOverflow() << endreq;
  msg(MSG::DEBUG) << "RoI number              :  " << roI.getRoiNumber() << endreq;
  msg(MSG::DEBUG) << "IsHighestPt             :  " << roI.getCandidateIsHighestPt() << endreq;
  msg(MSG::DEBUG) << "Overlap                 :  " << roI.getOverlapBits() << endreq;
  msg(MSG::DEBUG) << "Hemisphere              :  " << (roI.getSectorAddress() & MuCTPI_RDO::SECTOR_HEMISPHERE_MASK) << endreq;
  msg(MSG::DEBUG) << "=================================================" << endreq;
  return;
}
