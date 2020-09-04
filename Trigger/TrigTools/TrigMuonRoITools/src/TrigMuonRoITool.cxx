/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonRoITool.h"
#include "TrigT1Result/MuCTPI_RDO.h"
#include "TrigT1Result/MuCTPI_MultiplicityWord_Decoder.h"
#include "TrigT1Result/MuCTPI_DataWord_Decoder.h"


TrigMuonRoITool::TrigMuonRoITool(const std::string& type, 
				 const std::string& name,
				 const IInterface*  parent): 
  base_class(type,name,parent),
  m_robDataProviderSvc( "ROBDataProviderSvc", name ),
  m_decodeMuCTPiFromROB(false)
{
  // Declare the properties
  declareProperty("DaqMuCTPiROBid",m_daqMuCTPiROBid=0x760000);
  declareProperty("DecodeMuCTPiFromROB",m_decodeMuCTPiFromROB=false);
}


StatusCode TrigMuonRoITool::initialize()
{
  ATH_CHECK( AthAlgTool::initialize() );
  ATH_CHECK( m_robDataProviderSvc.retrieve() );

  // Print the property values
  ATH_MSG_DEBUG(" ROB ID: DAQ muCTPi = " << m_daqMuCTPiROBid
		<< std::setw(6) << " (=0x" << MSG::hex << m_daqMuCTPiROBid.value() << MSG::dec << ")");

  // Build the vector with ROB Ids to retrieve
  m_muCTPiRobIds.push_back(m_daqMuCTPiROBid.value());

  //Retrieve the MuCTPi RDO is we aren't decoding them ourselves
  ATH_CHECK(m_muCTPIKey.initialize(!m_decodeMuCTPiFromROB));
  
  return StatusCode::SUCCESS; 
}


/// helper to decode MuCTPi information
std::unique_ptr<TrigMuonRoITool::MuonRois> TrigMuonRoITool::decodeMuCTPi() {

  // save input stream flags
  char log_fill_char_save = (msg().stream()).fill();


  /////////////////////////////////////
  // decode MUCTPI
  /////////////////////////////////////

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
      ATH_MSG_DEBUG(" decodeMuCTPi: No muCTPi ROB found.");
      return nullptr;
    }
      
    // check for received ROB id and return if it doesn't match the DAQ MuCTPI ROB
    ATH_MSG_DEBUG(" decodeMuCTPi: ROB id = 0x" << std::setw(6)  << MSG::hex << muCTPiRobFragmentVec[0]->source_id() << MSG::dec);
    
    if (muCTPiRobFragmentVec[0]->source_id() != (uint32_t)m_daqMuCTPiROBid.value()) {
      ATH_MSG_DEBUG(" decodeMuCTPi: received ROB id = 0x" << std::setw(6) << MSG::hex << muCTPiRobFragmentVec[0]->source_id() << MSG::dec
		    << " does not match requested ROB id = 0x"
		    << std::setw(6) << MSG::hex << m_daqMuCTPiROBid.value() << MSG::dec);
      return nullptr; 
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
    
    ATH_MSG_DEBUG("ROB ID 0x" << MSG::hex << robId <<  " ROD ID 0x"
		  << rodId << MSG::dec << " ROB fragment size "
		  << robFragSize << " ROD fragment size " << rodFragSize);
		    
    ATH_MSG_DEBUG(" ROD Header L1 ID " << evtNum
		  << " ROD Header BCID " << bcId
		  << " ROD Header Format version " << formatVersion);
    
    OFFLINE_FRAGMENTS_NAMESPACE::PointerType it_data;
    muCTPiRobFragmentVec[0]->rod_data( it_data );
    const uint32_t ndata = muCTPiRobFragmentVec[0]->rod_ndata();
    ATH_MSG_DEBUG( " Dumping RoI Words:");
    ATH_MSG_DEBUG( " number of data words: " << ndata);
    // candidate multiplicity
    std::vector< uint32_t > candidateMultiplicity;
    // data words
    std::vector< uint32_t > dataWord;
    for( uint32_t i = 0; i < ndata; ++i, ++it_data ) {
      if( *it_data >> MuCTPI_RDO::MULT_WORD_FLAG_SHIFT ) {
	candidateMultiplicity.push_back( static_cast< uint32_t >( *it_data ) );
	ATH_MSG_DEBUG("     0x" << MSG::hex << std::setw( 8 ) << std::setfill( '0' )
		      << ( *it_data ) << " (candidate multiplicity)" << std::setfill( log_fill_char_save ));
      } else {
	dataWord.push_back( static_cast< uint32_t >( *it_data ) );
	ATH_MSG_DEBUG("     0x" << MSG::hex << std::setw( 8 ) << std::setfill( '0' )
		      << ( *it_data ) << " (candidate word)" 
		      << " (--> RoI word = 0x" << MSG::hex << std::setw( 8 ) << std::setfill( '0' ) 
		      << mirodToRoIBDataWord( *it_data ) << ")" 
		      << std::setfill( log_fill_char_save ));
      }
    }
    
    // create MuCTPi RDO (DAQ  muCTPi Result)
    daqmuCTPIResult = new MuCTPI_RDO( std::move(candidateMultiplicity), std::move(dataWord) );
  }
  
  // Retrieve the MuCTPi RDO via data handle
  //////////////////////////////////////////////
  else{
    SG::ReadHandle<MuCTPI_RDO> rdoHandle(m_muCTPIKey);
    if(rdoHandle.isValid()){
	daqmuCTPIResult = rdoHandle.cptr();
      }
    else{
      ATH_MSG_WARNING("Can't get MUCTPI_RDO object");
      return nullptr;
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

  // reset the containers and fill them with new data
  auto inTime_muCTPIRoIs=new std::vector<ROIB::MuCTPIRoI>();
  auto outOfTime_muCTPIRoIs=new std::vector< std::pair<ROIB::MuCTPIRoI,int> >();

  inTime_muCTPIRoIs->reserve( roiEventNCan );
  outOfTime_muCTPIRoIs->reserve( roiEventNCan );

  for(std::vector< uint32_t >::const_iterator it = daqmuCTPIResult->dataWord().begin();
      it != daqmuCTPIResult->dataWord().end(); ++it) {

    // decode the RDO data word
    MuCTPI_DataWord_Decoder daqRoI(*it);

    // create a L1 RoI
    ROIB::MuCTPIRoI roI( mirodToRoIBDataWord(*it) );

    if (roiEventBCID == daqRoI.getBCID()) { // RoI matches event BCID
      inTime_muCTPIRoIs->push_back(roI);
    } else {
      outOfTime_muCTPIRoIs->push_back( std::pair<ROIB::MuCTPIRoI,int>(roI,(int(daqRoI.getBCID())-int(roiEventBCID))) );
    }
  } // end loop over data words
  
  // print contents of RoI arrays
  if (msgLvl(MSG::DEBUG)) {
    ATH_MSG_DEBUG(" RoIs in time with event BCID:  Number of RoIs = " << inTime_muCTPIRoIs->size());
    for (std::vector< ROIB::MuCTPIRoI >::iterator it = inTime_muCTPIRoIs->begin(); it != inTime_muCTPIRoIs->end(); ++it) {
      dumpRoIBDataWord((*it).roIWord());
    }

    ATH_MSG_DEBUG(" RoIs out of time with event BCID:  Number of RoIs = " << outOfTime_muCTPIRoIs->size());
    for (std::vector< std::pair<ROIB::MuCTPIRoI,int> >::iterator it = outOfTime_muCTPIRoIs->begin(); it != outOfTime_muCTPIRoIs->end(); ++it) {
      ATH_MSG_DEBUG( " Difference(RoI(BCID) - Event(BCID)) = " << (*it).second);
      dumpRoIBDataWord(((*it).first).roIWord());
    }
  }

  std::unique_ptr<TrigMuonRoITool::MuonRois> rois(new TrigMuonRoITool::MuonRois(std::move(inTime_muCTPIRoIs), std::move(outOfTime_muCTPIRoIs)));


  if(m_decodeMuCTPiFromROB)
    delete daqmuCTPIResult;

  return rois; 
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

  ATH_MSG_DEBUG( "RoIB word               : 0x" << MSG::hex << roI.roIWord() );
  ATH_MSG_DEBUG( "Threshold               :  pt" << roI.pt() );
  ATH_MSG_DEBUG( "Sector location         :  " << loc );
  std::string sectorOffset("");  
  if ((roI.getSectorAddress() & MuCTPI_RDO::SECTOR_HEMISPHERE_MASK) &&
      (roI.getSectorLocation() == MuCTPI_RDO::BARREL)) sectorOffset = " + 32 for Hemisphere = 1 "; 
  ATH_MSG_DEBUG( "Sector ID               :  " << roI.getSectorID() << sectorOffset );
  ATH_MSG_DEBUG( "Sector addr             :  0x" << MSG::hex << roI.getSectorAddress());
  ATH_MSG_DEBUG( "Sector overflow         :  " << roI.getSectorOverflow() );
  ATH_MSG_DEBUG( "RoI overflow            :  " << roI.getRoiOverflow() );
  ATH_MSG_DEBUG( "RoI number              :  " << roI.getRoiNumber() );
  ATH_MSG_DEBUG( "IsHighestPt             :  " << roI.getCandidateIsHighestPt() );
  ATH_MSG_DEBUG( "Overlap                 :  " << roI.getOverlapBits() );
  ATH_MSG_DEBUG( "Hemisphere              :  " << (roI.getSectorAddress() & MuCTPI_RDO::SECTOR_HEMISPHERE_MASK) );
  return;
}
