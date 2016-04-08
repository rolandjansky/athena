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
#include "TrigT1Result/MuCTPI_RDO.h"

// Local include(s):
#include "TrigT1ResultByteStream/MuCTPIByteStreamCnv.h"
#include "TrigT1ResultByteStream/MuCTPIByteStreamTool.h"

// Hack for early data, where CTP and MuCTPI were plugged into the
// same ROS PC
#ifdef CTP_MUCTPI_HAVE_SAME_ROS
#  include "TrigT1ResultByteStream/CTPByteStreamTool.h"
#  include "TrigT1ResultByteStream/RecCTPByteStreamTool.h"
#  include "TrigT1Result/CTP_RDO.h"
#  include "TrigT1Result/CTP_RIO.h"
#endif

/**
 * The constructor sets up all the ToolHandle and ServiceHandle objects and initialises the
 * base class in the correct way.
 */
MuCTPIByteStreamCnv::MuCTPIByteStreamCnv( ISvcLocator* svcloc )
  : Converter( ByteStream_StorageType, classID(), svcloc ),
    m_tool( "MuCTPIByteStreamTool" ), m_srcIdMap( 0 ),
#ifdef CTP_MUCTPI_HAVE_SAME_ROS
    m_ctp_tool( "CTPByteStreamTool" ),
    m_ctp_rec_tool( "RecCTPByteStreamTool" ),
    m_storeGate( "StoreGateSvc", "MuCTPIByteStreamCnv" ),
#endif
    m_robDataProvider( "ROBDataProviderSvc", "MuCTPIByteStreamCnv" ),
    m_ByteStreamEventAccess( "ByteStreamCnvSvc", "MuCTPIByteStreamCnv" ) {

}

/**
 * The destructor actually does some cleanup, it deletes the CTPSrcIdMap
 * object that is created in the initialize() function.
 */
MuCTPIByteStreamCnv::~MuCTPIByteStreamCnv() {

  if( m_srcIdMap ) {
    delete m_srcIdMap;
    m_srcIdMap = 0;
  }

}

/**
 * Function telling the framework the Class ID of the object that this converter
 * is for (MuCTPI_RDO).
 */
const CLID& MuCTPIByteStreamCnv::classID() {

  return ClassID_traits<MuCTPI_RDO>::ID();

}

/**
 * Init method gets all necessary services etc.
 */
StatusCode MuCTPIByteStreamCnv::initialize() {

  //
  // Initialise the base class:
  //
  StatusCode sc = Converter::initialize();
  if( sc.isFailure() ) {
    return sc;
  }

  MsgStream log( messageService(), "MuCTPIByteStreamCnv" );
  log << MSG::DEBUG << "MuCTPIByteStreamCnv in initialize() " << endreq;

  //
  // Get ByteStreamCnvSvc:
  //
  sc = m_ByteStreamEventAccess.retrieve();
  if( sc.isFailure() ) {
    log << MSG::FATAL << "Can't get ByteStreamEventAccess interface" << endreq;
    return sc;
  }

  //
  // Get MuCTPIByteStreamTool:
  //
  sc = m_tool.retrieve();
  if( sc.isFailure() ) {
    log << MSG::FATAL << "Can't get MuCTPIByteStreamTool" << endreq;
    return sc;
  } else {
    log << MSG::DEBUG << "Connected to MuCTPIByteStreamTool" << endreq;
  }

#ifdef CTP_MUCTPI_HAVE_SAME_ROS
  //
  // Get CTPByteStreamTool:
  //
  sc = m_ctp_tool.retrieve();
  if( sc.isFailure() ) {
    log << MSG::FATAL << "Can't get CTPByteStreamTool" << endreq;
    return sc;
  } else {
    log << MSG::DEBUG << "Connected to CTPByteStreamTool" << endreq;
  }

  //
  // Get RecCTPByteStreamTool:
  //
  sc = m_ctp_rec_tool.retrieve();
  if( sc.isFailure() ) {
    log << MSG::FATAL << "Can't get RecCTPByteStreamTool" << endreq;
    return sc;
  } else {
    log << MSG::DEBUG << "Connected to RecCTPByteStreamTool" << endreq;
  }
#endif

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
  // Create MuCTPISrcIdMap:
  //
  m_srcIdMap = new MuCTPISrcIdMap();

  return StatusCode::SUCCESS;
}

/**
 * This function creates the MuCTPI_RDO object from the BS data. It requests
 * the ROB fragment with the ROB Id of the MuCTPI and gives this fragment to the
 * MuCTPIByteStreamTool for conversion.
 */
StatusCode MuCTPIByteStreamCnv::createObj( IOpaqueAddress* pAddr, DataObject*& pObj ) {

  MsgStream log( messageService(), "MuCTPIByteStreamCnv" );

  log << MSG::DEBUG << "createObj() called" << endreq;

  ByteStreamAddress *pBS_Addr;
  pBS_Addr = dynamic_cast< ByteStreamAddress* >( pAddr );
  if ( !pBS_Addr ) {
    log << MSG::ERROR << " Can not cast to ByteStreamAddress " << endreq ;
    return StatusCode::FAILURE;
  }

  log << MSG::DEBUG << " Creating Objects " << *( pBS_Addr->par() ) << endreq;

  //
  // Get SourceID:
  //
  const uint32_t robId = m_srcIdMap->getRobID( m_srcIdMap->getRodID() );

  log << MSG::DEBUG << " expected ROB sub-detector ID: " << std::hex 
      << robId << std::dec << endreq;  

  std::vector< uint32_t > vID;
  vID.push_back( robId );

  //
  // Get ROB fragment:
  //
  IROBDataProviderSvc::VROBFRAG robFrags;
  m_robDataProvider->getROBData( vID, robFrags );

  //
  // Size check:
  //
  if( robFrags.size() == 0 ) {
    
    // This is a hack - for some time CTP and MUCTPI were plugged in
    // the same ROB of the same ROS PC ...
    // 0x76000b : MUCTPI
    // 0x76000a : CTP
    // Note that the MUCTPI ROBID must come first, cos further down we
    // just use the first element of robFrags to create the RDO
    vID[0] = ( robId | 0x00000b );
#ifdef CTP_MUCTPI_HAVE_SAME_ROS
    vID.push_back( robId | 0x00000a );
#endif
    m_robDataProvider->getROBData( vID, robFrags );

    if( robFrags.size() == 0 ) {
      // yet another temporary hack!!! - for some time the ROB ID was 0!
      vID[0] = 0x00000b;
#ifdef CTP_MUCTPI_HAVE_SAME_ROS
      vID[1] = 0x00000a;
#endif
      m_robDataProvider->getROBData( vID, robFrags );
      
      if( robFrags.size() == 0 ) {
        log << MSG::WARNING << "No MuCTPI ROB fragments found!" << endreq;

        MuCTPI_RDO* result = new MuCTPI_RDO;
        pObj = SG::asStorable( result );
        return StatusCode::SUCCESS;
      }
    }
  }

  IROBDataProviderSvc::VROBFRAG::const_iterator it = robFrags.begin();
  MuCTPI_RDO* result = 0;
  StatusCode sc = m_tool->convert( ROBData( *it ).getROBFragment(), result );
  if( sc.isFailure() ) {
    log << MSG::ERROR << " Failed to create Objects: " << *( pBS_Addr->par() ) << endreq;
    if (result!=0){
      delete result;
      result=0;
    }
    return sc;
  }
  pObj = SG::asStorable( result ) ;

#ifdef CTP_MUCTPI_HAVE_SAME_ROS
  // If CTP and MuCTPI send to the same ROS, the MuCTPI ROS, we get
  // the CTP fragment in the MuCTPI ROS fragment. In order to be able
  // to produce a CTP_RDO, we call the CTP ByteStream AlgTool from
  // here whenever we think we have a CTP fragment.

  if( robFrags.size() > 1 ) {

    ++it;
    if( it != robFrags.end() ) {
    
      CTP_RDO* ctp_result = 0;
      CTP_RIO* ctp_rio_result = 0;

      sc = m_ctp_tool->convert( ROBData( *it ).getROBFragment(), ctp_result );
      if ( sc.isFailure() ) {
        log << MSG::ERROR << "Failed to create CTP_RDO " << endreq;
        return sc;
      }

      SG::asStorable( ctp_result ) ;
      sc = m_storeGate->record( ctp_result, "CTP_RDO" );
      if( sc.isFailure() ) {
        log << MSG::ERROR << "Cannot record CTP_RDO " << endreq;
        return sc;
      }

      sc = m_ctp_rec_tool->convert( ROBData( *it ).getROBFragment(), ctp_rio_result, log );
      if( sc.isFailure() ) {
        log << MSG::ERROR << "Failed to create CTP_RIO object" << endreq;
        return sc;
      }

      SG::asStorable( ctp_rio_result ) ;
      sc = m_storeGate->record( ctp_rio_result, "CTP_RIO" );
      if( sc.isFailure() ) {
        log << MSG::ERROR << "Cannot record CTP_RIO " << endreq;
        return sc;
      }

    }

  }
#endif

  return sc;
}

/**
 * This function receives a MuCTPI_RDO object as input, and adds the ROB fragment
 * of the MuCTPI to the current raw event using the IByteStreamEventAccess
 * interface.
 */
StatusCode MuCTPIByteStreamCnv::createRep( DataObject* pObj, IOpaqueAddress*& pAddr ) {

  MsgStream log( messageService(), "MuCTPIByteStreamCnv" );
  log << MSG::DEBUG << "createRep() called" << endreq;

  RawEventWrite* re = m_ByteStreamEventAccess->getRawEvent();

  MuCTPI_RDO* result;
  if( ! SG::fromStorable( pObj, result ) ) {
    log << MSG::ERROR << " Cannot cast to MuCTPI_RDO" << endreq;
    return StatusCode::FAILURE;
  }

  ByteStreamAddress* addr = new ByteStreamAddress( classID(), pObj->registry()->name(), "" );

  pAddr = addr;

  // Convert to ByteStream
  return m_tool->convert( result, re );
}
