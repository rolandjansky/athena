/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IRegistry.h"

#include "ByteStreamCnvSvcBase/ByteStreamAddress.h"
#include "ByteStreamData/RawEvent.h"
#include "ByteStreamData/ROBData.h"

#include "SGTools/StorableConversions.h"

// Trigger include(s):
#include "TrigT1Result/CTP_RDO.h"

// Local include(s):
#include "TrigT1ResultByteStream/CTPByteStreamCnv.h"
#include "TrigT1ResultByteStream/CTPSrcIdMap.h"

/**
 * The constructor sets up all the ToolHandle and ServiceHandle objects and initialises the
 * base class in the correct way.
 */
CTPByteStreamCnv::CTPByteStreamCnv( ISvcLocator* svcloc )
  : Converter( ByteStream_StorageType, classID(), svcloc ),
    m_tool( "CTPByteStreamTool" ), m_srcIdMap( 0 ),
    m_robDataProvider( "ROBDataProviderSvc", "CTPByteStreamCnv" ),
    m_ByteStreamEventAccess( "ByteStreamCnvSvc", "CTPByteStreamCnv" ) {

}

/**
 * The destructor actually does some cleanup, it deletes the CTPSrcIdMap
 * object that is created in the initialize() function.
 */
CTPByteStreamCnv::~CTPByteStreamCnv() {

  if( m_srcIdMap ) {
    delete m_srcIdMap;
    m_srcIdMap = 0;
  }

}

/**
 * Function telling the framework the Class ID of the object that this converter
 * is for (CTP_RDO).
 */
const CLID& CTPByteStreamCnv::classID() {

  return ClassID_traits< CTP_RDO >::ID();

}

/**
 * Init method gets all necessary services etc.
 */
StatusCode CTPByteStreamCnv::initialize() {

  //
  // Initialise the base class:
  //
  StatusCode sc = Converter::initialize();
  if( sc.isFailure() ) {
    return sc;
  }

  MsgStream log( messageService(), "CTPByteStreamCnv" );
  log << MSG::DEBUG << "CTPByteStreamCnv in initialize() " << endreq;

  //
  // Get ByteStreamCnvSvc:
  //
  sc = m_ByteStreamEventAccess.retrieve();
  if( sc.isFailure() ) {
    log << MSG::FATAL << "Can't get ByteStreamEventAccess interface" << endreq;
    return sc;
  } else {
    log << MSG::DEBUG << "Connected to ByteStreamEventAccess interface" << endreq;
  }

  //
  // Get CTPByteStreamTool:
  //
  sc = m_tool.retrieve();
  if( sc.isFailure() ) {
    log << MSG::FATAL << "Can't get CTPByteStreamTool" << endreq;
    return sc;
  } else {
    log << MSG::DEBUG << "Connected to CTPByteStreamTool" << endreq;
  }

  //
  // Get ROBDataProvider:
  //
  sc = m_robDataProvider.retrieve();
  if( sc.isFailure() ) {
    log << MSG::WARNING << "Can't get ROBDataProviderSvc" << endreq;
    // return is disabled for Write BS which does not requre ROBDataProviderSvc
  } else {
    log << MSG::DEBUG << "Connected to ROBDataProviderSvc" << endreq;
  }

  //
  // Create CTPSrcIdMap:
  //
  m_srcIdMap = new CTPSrcIdMap();

  return StatusCode::SUCCESS;
}

/**
 * This function creates the CTP_RDO object from the BS data. It requests
 * the ROB fragment with the ROB Id of the CTP and gives this fragment to the
 * CTPByteStreamTool for conversion.
 */
StatusCode CTPByteStreamCnv::createObj( IOpaqueAddress* pAddr, DataObject*& pObj ) {

  MsgStream log( messageService(), "CTPByteStreamCnv" );

  log << MSG::DEBUG << "createObj() called" << endreq;

  ByteStreamAddress *pBS_Addr;
  pBS_Addr = dynamic_cast< ByteStreamAddress* >( pAddr );
  if( ! pBS_Addr ) {
    log << MSG::ERROR << "Can not cast input to ByteStreamAddress" << endreq ;
    return StatusCode::FAILURE;
  }

  log << MSG::DEBUG << "Creating Objects: " << *( pBS_Addr->par() ) << endreq;

  //
  // Get the SourceID:
  //
  const uint32_t robId = m_srcIdMap->getRobID( m_srcIdMap->getRodID() );

  log << MSG::DEBUG << "expected ROB sub-detector ID: " << std::hex 
      << robId << std::dec << endreq;  

  std::vector< uint32_t > vID;
  vID.push_back( robId );

  //
  // Get the ROB fragment:
  //
  IROBDataProviderSvc::VROBFRAG robFrags;
  m_robDataProvider->getROBData( vID, robFrags );

  //
  // Size check:
  //
  if( robFrags.size() != 1 ) {
    log << MSG::WARNING << "Number of ROB fragments is " << robFrags.size() << endreq;
    log << MSG::WARNING << "Creating empty CTP_RDO object" << endreq;

    CTP_RDO* result = new CTP_RDO(3,0,0);
    pObj = SG::asStorable( result );
    return StatusCode::SUCCESS;
  }

  //
  // Do the actual conversion:
  //
  IROBDataProviderSvc::VROBFRAG::const_iterator it = robFrags.begin();
  CTP_RDO* result = 0;

  StatusCode sc = m_tool->convert( ROBData( *it ).getROBFragment(), result );
  if ( sc.isFailure() ) {
    log << MSG::ERROR << " Failed to create Objects: " << *( pBS_Addr->par() ) << endreq;
    return sc;
  }
  pObj = SG::asStorable( result ) ;

  return sc;
}

/**
 * This function receives a CTP_RDO object as input, and adds the ROB fragment
 * of the CTP to the current raw event using the IByteStreamEventAccess
 * interface.
 */
StatusCode CTPByteStreamCnv::createRep( DataObject* pObj, IOpaqueAddress*& pAddr ) {

  MsgStream log( messageService(), "CTPByteStreamCnv" );

  log << MSG::DEBUG << "createRep() called" << endreq;

  RawEventWrite* re = m_ByteStreamEventAccess->getRawEvent();

  CTP_RDO* result;
  if( ! SG::fromStorable( pObj, result ) ) {
    log << MSG::ERROR << " Cannot cast to CTP_RDO" << endreq;
    return StatusCode::FAILURE;
  }

  ByteStreamAddress* addr = new ByteStreamAddress( classID(), pObj->registry()->name(), "" );

  pAddr = addr;

  // Convert to ByteStream
  return m_tool->convert( result, re );
}
