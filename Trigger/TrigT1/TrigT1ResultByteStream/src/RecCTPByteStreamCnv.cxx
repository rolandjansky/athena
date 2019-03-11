/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"
#include "ByteStreamCnvSvcBase/ByteStreamAddress.h"
#include "ByteStreamData/RawEvent.h"
#include "ByteStreamData/ROBData.h"

#include "SGTools/StorableConversions.h"

// Trigger include(s):
#include "TrigT1Result/CTP_RIO.h"

// Local include(s):
#include "TrigT1ResultByteStream/RecCTPByteStreamCnv.h"
#include "TrigT1ResultByteStream/CTPSrcIdMap.h"

/**
 * The constructor sets up all the ToolHandle and ServiceHandle objects and initialises the
 * base class in the correct way.
 */
RecCTPByteStreamCnv::RecCTPByteStreamCnv( ISvcLocator* svcloc )
  : Converter( storageType(), classID(), svcloc ),
    m_tool( "RecCTPByteStreamTool" ), m_srcIdMap( 0 ),
    m_robDataProvider( "ROBDataProviderSvc", "RecCTPByteStreamCnv" ) {

}

/**
 * The destructor actually does some cleanup, it deletes the CTPSrcIdMap
 * object that is created in the initialize() function.
 */
RecCTPByteStreamCnv::~RecCTPByteStreamCnv() {

  if( m_srcIdMap ) {
    delete m_srcIdMap;
    m_srcIdMap = 0;
  }

}

/**
 * Function telling the framework the Class ID of the object that this converter
 * is for (CTP_RIO).
 */
const CLID& RecCTPByteStreamCnv::classID() {

  return ClassID_traits< CTP_RIO >::ID();

}

long RecCTPByteStreamCnv::storageType() {
  return ByteStreamAddress::storageType();
}

/**
 * Init method gets all necessary services etc.
 */
StatusCode RecCTPByteStreamCnv::initialize() {

  //
  // Initialise the base class:
  //
  ATH_CHECK( Converter::initialize() );

  MsgStream log( msgSvc(), "RecCTPByteStreamCnv" );
  log << MSG::DEBUG << "RecCTPByteStreamCnv in initialize()" << endmsg;

  //
  // Get RecCTPByteStreamTool:
  //
  ATH_CHECK(  m_tool.retrieve() );
  log << MSG::DEBUG << "Connected to RecCTPByteStreamTool" << endmsg;

  //
  // Get ROBDataProvider:
  //
  ATH_CHECK( m_robDataProvider.retrieve() );
  log << MSG::DEBUG << "Connected to ROBDataProviderSvc" << endmsg;

  //
  // Create CTPSrcIdMap:
  //
  m_srcIdMap = new CTPSrcIdMap();

  return StatusCode::SUCCESS;
}

/**
 * This function creates the CTP_RIO object from the BS data. It requests
 * the ROB fragment with the ROB Id of the CTP and gives this fragment to the
 * RecCTPByteStreamTool for conversion.
 */
StatusCode RecCTPByteStreamCnv::createObj( IOpaqueAddress* pAddr, DataObject*& pObj ) {

  MsgStream log( msgSvc(), "RecCTPByteStreamCnv" );
  log << MSG::DEBUG << "RecCTPByteStreamCnv::createObj() called" << endmsg;

  ByteStreamAddress *pBS_Addr;
  pBS_Addr = dynamic_cast< ByteStreamAddress* >( pAddr );
  if( !pBS_Addr ) {
    log << MSG::ERROR << " Can not cast to ByteStreamAddress " << endmsg ;
    return StatusCode::FAILURE;
  }

  log << MSG::DEBUG << " Creating Objects: " << *( pBS_Addr->par() ) << endmsg;

  //
  // Get SourceID:
  //
  const uint32_t robId = m_srcIdMap->getRobID( m_srcIdMap->getRodID() );

  log << MSG::DEBUG << "expected ROB sub-detector ID: " << std::hex 
      << robId << std::dec << endmsg;  

  std::vector< uint32_t > vID;
  vID.push_back( robId );

  // get ROB fragment
  IROBDataProviderSvc::VROBFRAG robFrags;
  m_robDataProvider->getROBData( vID, robFrags );

  // size check
  if ( robFrags.size() != 1 ) {
    log << MSG::WARNING << " Number of ROB fragments for source ID 0x" << MSG::hex << robId
        << " is " << robFrags.size() << endmsg;

    CTP_RIO* result = new CTP_RIO;
    pObj = SG::asStorable( result ) ;
    return StatusCode::SUCCESS;
  }

  IROBDataProviderSvc::VROBFRAG::const_iterator it = robFrags.begin();
  CTP_RIO * result = 0;

  ATH_CHECK(  m_tool->convert( ROBData( *it ).getROBFragment(), result ) );
  pObj = SG::asStorable( result );

  return StatusCode::SUCCESS;
}
