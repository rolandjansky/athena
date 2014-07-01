/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IRegistry.h"

#include "ByteStreamCnvSvcBase/ByteStreamAddress.h"
#include "ByteStreamData/ROBData.h"

#include "SGTools/StorableConversions.h"

// Trigger include(s):
#include "TrigT1Result/MuCTPI_RIO.h"

// Local include(s):
#include "TrigT1ResultByteStream/RecMuCTPIByteStreamCnv.h"
#include "TrigT1ResultByteStream/MuCTPISrcIdMap.h"

/**
 * The constructor sets up all the ToolHandle and ServiceHandle objects and initialises the
 * base class in the correct way.
 */
RecMuCTPIByteStreamCnv::RecMuCTPIByteStreamCnv( ISvcLocator* svcloc )
  : Converter( ByteStream_StorageType, classID(), svcloc ),
    m_tool( "RecMuCTPIByteStreamTool" ), m_srcIdMap( 0 ),
    m_robDataProvider( "ROBDataProviderSvc", "RecMuCTPIByteStreamCnv" ) {

}

/**
 * The destructor actually does some cleanup, it deletes the CTPSrcIdMap
 * object that is created in the initialize() function.
 */
RecMuCTPIByteStreamCnv::~RecMuCTPIByteStreamCnv() {

  if( m_srcIdMap ) {
    delete m_srcIdMap;
    m_srcIdMap = 0;
  }

}

/**
 * Function telling the framework the Class ID of the object that this converter
 * is for (CTP_RIO).
 */
const CLID& RecMuCTPIByteStreamCnv::classID() {

  return ClassID_traits< MuCTPI_RIO >::ID();

}

/**
 * Init method gets all necessary services etc.
 */
StatusCode RecMuCTPIByteStreamCnv::initialize() {

  //
  // Initialise the base class:
  //
  StatusCode sc = Converter::initialize();
  if ( sc.isFailure() ) {
    return sc;
  }

  MsgStream log( messageService(), "RecMuCTPIByteStreamCnv" );
  log << MSG::DEBUG << "RecMuCTPIByteStreamCnv in initialize()" << endreq;

  //
  // Get RecMuCTPIByteStreamTool:
  //
  sc = m_tool.retrieve();
  if( sc.isFailure() ) {
    log << MSG::ERROR << "Can't get RecMuCTPIByteStreamTool" << endreq;
    return sc;
  } else {
    log << MSG::DEBUG << "Connected to RecMuCTPIByteStreamTool" << endreq;
  }

  //
  // Get ROBDataProvider:
  //
  sc = m_robDataProvider.retrieve();
  if( sc.isFailure() ) {
    log << MSG::ERROR << "Can't get ROBDataProviderSvc" << endreq;
    return sc;
  } else {
    log << MSG::DEBUG << "Connected to ROBDataProviderSvc" << endreq;
  }

  //
  // Create MuCTPISrcIdMap:
  //
  m_srcIdMap = new MuCTPISrcIdMap();

  return StatusCode::SUCCESS;

}

/**
 * This function creates the MuCTPI_RIO object from the BS data. It requests
 * the ROB fragment with the ROB Id of the MuCTPI and gives this fragment to the
 * RecMuCTPIByteStreamTool for conversion.
 */
StatusCode RecMuCTPIByteStreamCnv::createObj( IOpaqueAddress* pAddr, DataObject*& pObj ) {

  MsgStream log( messageService(), "RecMuCTPIByteStreamCnv" );
  log << MSG::DEBUG << "executing createObj()" << endreq;

  ByteStreamAddress *pBS_Addr;
  pBS_Addr = dynamic_cast< ByteStreamAddress* >( pAddr );
  if ( !pBS_Addr ) {
    log << MSG::ERROR << " Cannot cast to ByteStreamAddress " << endreq ;
    return StatusCode::FAILURE;
  }

  log << MSG::DEBUG << " Creating Objects  " << *( pBS_Addr->par() ) << endreq;

  // get SourceID
  const uint32_t robId = m_srcIdMap->getRobID( m_srcIdMap->getRodID() );

  std::vector< uint32_t > vID;
  vID.push_back( robId );

  // get ROB fragment
  IROBDataProviderSvc::VROBFRAG robFrags ;
  m_robDataProvider->getROBData( vID, robFrags );

  // size check
  if ( robFrags.size() == 0 ) {    
    // This is a hack - in early data the rob id of the MuCTPI was
    // wrong, i.e. 0x76000b
    uint32_t newRobId = robId;
    newRobId |= 0x00000b;
    vID[0] = ( newRobId );
    m_robDataProvider->getROBData( vID, robFrags );
    // size check
    if ( robFrags.size() != 1 ) {
      log << MSG::WARNING << " Number of ROB fragments for source ROB ID " << MSG::hex << newRobId << " (ROD ID " 
          << m_srcIdMap->getRodID() << MSG::dec << ") is " << robFrags.size() << endreq;
      MuCTPI_RIO * result = new MuCTPI_RIO;
      pObj = SG::asStorable( result ) ;
      return StatusCode::SUCCESS;
    }
  }

  IROBDataProviderSvc::VROBFRAG::const_iterator it = robFrags.begin();
  MuCTPI_RIO* result;
  // Convert to Object
  StatusCode sc = m_tool->convert( ROBData( *it ).getROBFragment(), result );
  if ( sc.isFailure() ) {
    log << MSG::ERROR << " Failed to create Objects   " << *( pBS_Addr->par() ) << endreq;
    return sc;
  }
  pObj = SG::asStorable( result ) ;
  
  return StatusCode::SUCCESS;
}
