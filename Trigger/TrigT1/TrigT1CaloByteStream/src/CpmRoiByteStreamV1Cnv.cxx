/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include <vector>
#include <stdint.h>

#include "ByteStreamCnvSvcBase/ByteStreamAddress.h"
#include "ByteStreamCnvSvcBase/IByteStreamEventAccess.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"

#include "ByteStreamData/RawEvent.h"
#include "ByteStreamData/ROBData.h"

#include "AthContainers/DataVector.h"

#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/StatusCode.h"

#include "AthenaKernel/ClassID_traits.h"
#include "AthenaKernel/StorableConversions.h"

#include "TrigT1CaloEvent/CPMRoI.h"

#include "CpmRoiByteStreamV1Cnv.h"
#include "CpmRoiByteStreamV1Tool.h"

namespace LVL1BS {

CpmRoiByteStreamV1Cnv::CpmRoiByteStreamV1Cnv( ISvcLocator* svcloc )
    : Converter( storageType(), classID(), svcloc ),
      m_name("CpmRoiByteStreamV1Cnv"),
      m_tool("LVL1BS::CpmRoiByteStreamV1Tool/CpmRoiByteStreamV1Tool"),
      m_robDataProvider("ROBDataProviderSvc", m_name),
      m_ByteStreamEventAccess("ByteStreamCnvSvc", m_name),
      m_log(msgSvc(), m_name), m_debug(false)
{
}

CpmRoiByteStreamV1Cnv::~CpmRoiByteStreamV1Cnv()
{
}

// CLID

const CLID& CpmRoiByteStreamV1Cnv::classID()
{
  return ClassID_traits<DataVector<LVL1::CPMRoI> >::ID();
}

long CpmRoiByteStreamV1Cnv::storageType()
{
  return ByteStreamAddress::storageType();
}

//  Init method gets all necessary services etc.


StatusCode CpmRoiByteStreamV1Cnv::initialize()
{
  m_debug = msgSvc()->outputLevel(m_name) <= MSG::DEBUG;
  m_log << MSG::DEBUG << "Initializing " << m_name << " - package version "
                      << PACKAGE_VERSION << endmsg;

  StatusCode sc = Converter::initialize();
  if ( sc.isFailure() )
    return sc;

  //Get ByteStreamCnvSvc
  sc = m_ByteStreamEventAccess.retrieve();
  if ( sc.isFailure() ) {
    m_log << MSG::ERROR << "Failed to retrieve service "
          << m_ByteStreamEventAccess << endmsg;
    return sc;
  } else {
    m_log << MSG::DEBUG << "Retrieved service "
          << m_ByteStreamEventAccess << endmsg;
  }

  // Retrieve Tool
  sc = m_tool.retrieve();
  if ( sc.isFailure() ) {
    m_log << MSG::ERROR << "Failed to retrieve tool " << m_tool << endmsg;
    return StatusCode::FAILURE;
  } else m_log << MSG::DEBUG << "Retrieved tool " << m_tool << endmsg;

  // Get ROBDataProvider
  sc = m_robDataProvider.retrieve();
  if ( sc.isFailure() ) {
    m_log << MSG::WARNING << "Failed to retrieve service "
          << m_robDataProvider << endmsg;
    // return is disabled for Write BS which does not require ROBDataProviderSvc
    // return sc ;
  } else {
    m_log << MSG::DEBUG << "Retrieved service "
          << m_robDataProvider << endmsg;
  }

  return StatusCode::SUCCESS;
}

// createObj should create the RDO from bytestream.

StatusCode CpmRoiByteStreamV1Cnv::createObj( IOpaqueAddress* pAddr,
                                             DataObject*& pObj )
{
  if (m_debug) m_log << MSG::DEBUG << "createObj() called" << endmsg;

  ByteStreamAddress *pBS_Addr;
  pBS_Addr = dynamic_cast<ByteStreamAddress *>( pAddr );
  if ( !pBS_Addr ) {
    m_log << MSG::ERROR << " Can not cast to ByteStreamAddress " << endmsg;
    return StatusCode::FAILURE;
  }

  const std::string nm = *( pBS_Addr->par() );

  if (m_debug) m_log << MSG::DEBUG << " Creating Objects " << nm << endmsg;

  // get SourceIDs
  const std::vector<uint32_t>& vID(m_tool->sourceIDs(nm));

  // get ROB fragments
  IROBDataProviderSvc::VROBFRAG robFrags;
  m_robDataProvider->getROBData( vID, robFrags );

  // size check
  DataVector<LVL1::CPMRoI>* const roiCollection = new DataVector<LVL1::CPMRoI>;
  if (m_debug) {
    m_log << MSG::DEBUG << " Number of ROB fragments is " << robFrags.size()
          << endmsg;
  }
  if (robFrags.size() == 0) {
    pObj = SG::asStorable(roiCollection) ;
    return StatusCode::SUCCESS;
  }

  StatusCode sc = m_tool->convert(robFrags, roiCollection);
  if ( sc.isFailure() ) {
    m_log << MSG::ERROR << " Failed to create Objects   " << nm << endmsg;
    delete roiCollection;
    return sc;
  }

  pObj = SG::asStorable(roiCollection);

  return sc;
}

// createRep should create the bytestream from RDOs.

StatusCode CpmRoiByteStreamV1Cnv::createRep( DataObject* pObj,
                                             IOpaqueAddress*& pAddr )
{
  if (m_debug) m_log << MSG::DEBUG << "createRep() called" << endmsg;

  RawEventWrite* re = m_ByteStreamEventAccess->getRawEvent();

  DataVector<LVL1::CPMRoI>* roiCollection = 0;
  if( !SG::fromStorable( pObj, roiCollection ) ) {
    m_log << MSG::ERROR << " Cannot cast to DataVector<CPMRoI>" << endmsg;
    return StatusCode::FAILURE;
  }

  const std::string nm = pObj->registry()->name();

  ByteStreamAddress* addr = new ByteStreamAddress( classID(), nm, "" );

  pAddr = addr;

  // Convert to ByteStream
  return m_tool->convert( roiCollection, re );
}

} // end namespace
