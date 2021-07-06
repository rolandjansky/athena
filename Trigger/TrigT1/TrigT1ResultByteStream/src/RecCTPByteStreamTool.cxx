/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// Trigger include(s):
#include "TrigT1Result/CTP_RIO.h"

// Local include(s):
#include "RecCTPByteStreamTool.h"

/// Unique interface ID of the tool that identifies it to the framweork
static const InterfaceID IID_IRecCTPByteStreamTool( "RecCTPByteStreamTool", 1, 0 );

/**
 * This function is needed by the framework to know what kind of tool
 * this is.
 */
const InterfaceID & RecCTPByteStreamTool::interfaceID() {
  return IID_IRecCTPByteStreamTool;
}

/**
 * The constructor takes care of correctly constructing the base class and
 * declaring the tool's interface to the framework.
 */
RecCTPByteStreamTool::RecCTPByteStreamTool( const std::string& type, const std::string& name,
                                            const IInterface* parent )
  : AthAlgTool( type, name, parent ) {

  declareInterface< RecCTPByteStreamTool >( this );
}

/**
 * Conversion from eformat::ROBFragment to RIO.
 * This is called from the RecCTPByteStreamCnv::createObj method.
 */
StatusCode RecCTPByteStreamTool::convert( const ROBF* rob, CTP_RIO*& result ) {

   ATH_MSG_DEBUG("executing convert() from ROBFragment to RIO");

   const uint32_t ctpRodId = m_srcIdMap.getRodID();
   const uint32_t rodId = rob->rod_source_id();

   ATH_MSG_DEBUG(" expected ROD sub-detector ID: 0x" << std::hex << ctpRodId 
       << " ID found: 0x" << std::hex << rodId << std::dec);  

   if( rodId == ctpRodId ) {
      // create CTP RIO
      result = new CTP_RIO();

      const uint32_t bcid = rob->rod_bc_id();
      const uint32_t ndata = rob->rod_ndata();
      const uint32_t detev_type = rob->rod_detev_type();
      const uint32_t lvl1_type = rob->rod_lvl1_trigger_type();
      const uint32_t lvl1_id = rob->rod_lvl1_id();
      const uint32_t run_no = rob->rod_run_no();
      const uint32_t nstatus = rob->rod_nstatus();

      OFFLINE_FRAGMENTS_NAMESPACE::PointerType it_data;
      rob->rod_status( it_data );

      ATH_MSG_VERBOSE(" Dumping CTP ROD fragment Header data:");
      ATH_MSG_VERBOSE("  Source ID            0x" << MSG::hex << rodId << MSG::dec);
      ATH_MSG_VERBOSE("  BCID                 0x" << MSG::hex << bcid << MSG::dec 
          << " (dec " << bcid << ")");
      ATH_MSG_VERBOSE("  Detector event type  0x" << MSG::hex << detev_type 
          << MSG::dec << " (dec " << detev_type << ")");
      ATH_MSG_VERBOSE("  Level-1 ID           0x" << MSG::hex << lvl1_id << MSG::dec 
          << " (dec " << lvl1_id << ")");
      ATH_MSG_VERBOSE("  Level-1 type         0x" << MSG::hex << lvl1_type << MSG::dec 
          << " (dec " << lvl1_type << ")");
      ATH_MSG_VERBOSE("  Run number             " << run_no);
      ATH_MSG_VERBOSE("  Number of data words   " << ndata);
      ATH_MSG_VERBOSE("  Number of status words ");
      for ( uint32_t i = 0; i < nstatus; ++i , ++it_data) {
         ATH_MSG_VERBOSE("  Status word # " << i << ": 0x" << MSG::hex << (*it_data) << MSG::dec
             << " (dec " << (*it_data) << ")");      
         result->statusWords().push_back( static_cast< uint32_t >( *it_data ) );
      }
      result->sourceId( rodId );
      result->runNumber( run_no );
      result->lvl1Id( lvl1_id );
      result->bcid( bcid );
      result->lvl1TriggerType( lvl1_type );
      result->detectorEventType( detev_type );
      result->numberDataWords( ndata );
      result->numberStatusWords( nstatus );

      return StatusCode::SUCCESS;
   }

   ATH_MSG_ERROR("Wrong ROD ID found in the CTP ROB fragment!");
   return StatusCode::FAILURE;
}
