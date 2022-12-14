/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "ByteStreamData/RawEvent.h"

// Trigger include(s):
#include "TrigT1Interfaces/RecMuonRoI.h"
#include "TrigT1Result/MuCTPI_RIO.h"
#include "TrigConfData/L1Menu.h"

// Local include(s):
#include "RecMuCTPIByteStreamTool.h"

/// Unique interface ID of the tool that identifies it to the framweork
static const InterfaceID IID_IRecMuCTPIByteStreamTool( "RecMuCTPIByteStreamTool", 1, 1 );

/**
 * This function is needed by the framework to know what kind of tool
 * this is.
 */
const InterfaceID& RecMuCTPIByteStreamTool::interfaceID() {

  return IID_IRecMuCTPIByteStreamTool;

}

/**
 * The constructor takes care of correctly constructing the base class and
 * declaring the tool's interface to the framework.
 */
RecMuCTPIByteStreamTool::RecMuCTPIByteStreamTool( const std::string& type, const std::string& name,
                                                  const IInterface* parent )
  : AthAlgTool( type, name, parent )

{
  declareInterface< RecMuCTPIByteStreamTool >( this );
}


/**
 * The function connects to all services being used, resets the internally used
 * variables, and initialises the base class.
 */
StatusCode RecMuCTPIByteStreamTool::initialize() {

  StatusCode sc = m_rpcRoITool.retrieve();
  if( sc.isFailure() ) {
    ATH_MSG_WARNING("Couldn't access RPC RecMuonRoISvc");
  } else {
    ATH_MSG_DEBUG("Connected to RPC RecMuonRoISvc");
  }

  sc = m_tgcRoITool.retrieve();
  if( sc.isFailure() ) {
    ATH_MSG_WARNING("Couldn't access TGC RecMuonRoISvc");
  } else {
    ATH_MSG_DEBUG("Connected to TGC RecMuonRoISvc");
  }

  return StatusCode::SUCCESS;
  
}


/**
 * Conversion from eformat::ROBFragment to RIO.
 * This is called from the RecMuCTPIByteStreamCnv::createObj method.
 */
StatusCode RecMuCTPIByteStreamTool::convert( const ROBF* rob, MuCTPI_RIO*& result ) {

  ATH_MSG_DEBUG("executing convert() from ROBFragment to RIO");

  // L1Menu
  const TrigConf::L1Menu * l1menu = nullptr;
  ATH_CHECK( detStore()->retrieve(l1menu) );

  // Source ID of MIROD
  const uint32_t miRodId = m_srcIdMap.getRodID();

  /* get ROD source ID */
  uint32_t rodId = rob->rod_source_id();

  if( rodId == miRodId ) {
    
    // result
    MuCTPI_RIO* muCTPI_RIO = new MuCTPI_RIO;

    // get the ROD header data and store it
    const uint32_t bcid = rob->rod_bc_id();
    const uint32_t ndata = rob->rod_ndata();
    const uint32_t detev_type = rob->rod_detev_type();
    const uint32_t lvl1_type = rob->rod_lvl1_trigger_type();
    const uint32_t lvl1_id = rob->rod_lvl1_id();
    const uint32_t run_no = rob->rod_run_no();
    const uint32_t nstatus = rob->rod_nstatus();
    OFFLINE_FRAGMENTS_NAMESPACE::PointerType it_statusData;
    rob->rod_status( it_statusData );
    
    ATH_MSG_VERBOSE(" Dumping CTP ROD fragment Header data:");
    ATH_MSG_VERBOSE("Source ID            0x" << MSG::hex << rodId << MSG::dec);
    ATH_MSG_VERBOSE("BCID                 0x" << MSG::hex << bcid << MSG::dec 
        << " (dec " << bcid << ")");
    ATH_MSG_VERBOSE("Detector event type  0x" << MSG::hex << detev_type 
        << MSG::dec << " (dec " << detev_type << ")");
    ATH_MSG_VERBOSE("Level-1 ID           0x" << MSG::hex << lvl1_id << MSG::dec 
        << " (dec " << lvl1_id << ")");
    ATH_MSG_VERBOSE("Level-1 type         0x" << MSG::hex << lvl1_type << MSG::dec 
        << " (dec " << lvl1_type << ")");
    ATH_MSG_VERBOSE("Run number             " << run_no);
    ATH_MSG_VERBOSE("Number of data words   " << ndata);
    ATH_MSG_VERBOSE("Number of status words ");
    for ( uint32_t i = 0; i < nstatus; ++i , ++it_statusData) {
      ATH_MSG_VERBOSE("Status word # " << i << ": 0x" << MSG::hex << (*it_statusData) << MSG::dec
          << " (dec " << (*it_statusData) << ")");      
      muCTPI_RIO->headerStatusWords().push_back( static_cast<uint32_t>( *it_statusData ) );
    }
    muCTPI_RIO->headerSourceId(rodId);
    muCTPI_RIO->headerRunNumber(run_no);
    muCTPI_RIO->headerLVL1ID(lvl1_id);
    muCTPI_RIO->headerBCID(bcid);
    muCTPI_RIO->headerLVL1TriggerType(lvl1_type);
    muCTPI_RIO->headerDetectorEventType(detev_type);
    muCTPI_RIO->headerNumberDataWords(ndata);
    muCTPI_RIO->headerNumberStatusWords(nstatus);

    ATH_MSG_DEBUG("  Dumping MuCTPI words:");

    OFFLINE_FRAGMENTS_NAMESPACE::PointerType it_data;
    rob->rod_data( it_data );

    //First count mult words
    int nMultWords = 0;
    for( uint32_t i = 0; i < ndata; ++i ) {
      if( ((*it_data) & 0x20000000) != 0 ) nMultWords++;
      ++it_data;
    }

    rob->rod_data( it_data );
    
    it_data+=(nMultWords-1)/2;//Goes to the central BC of the RO window

    ATH_MSG_DEBUG("     0x" << MSG::hex << std::setw( 8 ) << *it_data
        << " (candidate multiplicity)");

    // candidate multiplicities
    uint16_t sum[ MuCTPI_RIO::N_SUM ];
    for ( int i = 0; i < MuCTPI_RIO::N_SUM; ++i )
      sum[ i ] = ( *it_data >> ( i * 3 ) ) & 0x1F;
    muCTPI_RIO->setSum( sum );

    // BCID
    uint16_t g_bcId = ( (*it_data) & 0x001C0000 ) >> 18;
    muCTPI_RIO->setBCID( g_bcId );

    it_data+=(nMultWords+1)/2;//Goes to the first candidate word;

    // data words
    for( uint32_t i = nMultWords; i < ndata; ++i, ++it_data ) {

      ATH_MSG_DEBUG("     0x" << MSG::hex << std::setw( 8 ) << *it_data);

      // extract BCID
      const uint32_t tmpWord = static_cast<uint32_t>( *it_data ); // The two types are the same right now, but it might change.
      uint16_t bcId = ( tmpWord & 0x0001C000 ) >> 14;

      // accepted bit
      bool accepted = true;
      if ( ( tmpWord & 0x04000000 ) == 0 )
        accepted = false;

      // make RoI word
      uint32_t roiWord = ( tmpWord & 0x00003FFF ) + ( ( tmpWord & 0x03FE0000 ) >> 3 );

      ATH_MSG_DEBUG(MSG::hex << std::setw( 8 ) << *it_data
          << " : ROI=" << std::setw( 8 ) << roiWord);

      // reconstruct
      LVL1::RecMuonRoI thisRoI( roiWord, m_rpcRoITool.isSet() ? m_rpcRoITool.operator->() : 0,
                                m_tgcRoITool.isSet() ? m_tgcRoITool.operator->() : 0, l1menu );

      uint16_t pTVal = thisRoI.getThresholdValue();
      uint16_t pTNumber = thisRoI.getThresholdNumber();
      uint16_t roiNumber = thisRoI.getRoINumber();
      uint16_t secId = thisRoI.sectorID();
      uint16_t sysId = thisRoI.sysID();
      uint16_t hemisphere = thisRoI.subsysID();
      double eta = thisRoI.eta();
      double phi = thisRoI.phi();
      bool first = thisRoI.firstCandidate();
      bool duplicatedRoI = thisRoI.padOverflow();
      bool duplicatedSector = thisRoI.sectorOverflow();

      // fill
      muCTPI_RIO->setRoI( bcId, pTVal,pTNumber, eta, phi, secId, sysId, hemisphere, roiNumber,
                          accepted, first, duplicatedRoI, duplicatedSector );
    }

    /* set return value */
    result = muCTPI_RIO;

    return StatusCode::SUCCESS;

  } else {
  
    ATH_MSG_WARNING("Expected source ID 0x" << MSG::hex << miRodId << " but found 0x" 
        << rodId << MSG::dec);
    return StatusCode::FAILURE;
  
  }
}
