/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include <vector>

#include "ByteStreamCnvSvcBase/ByteStreamAddress.h"
#include "ByteStreamCnvSvcBase/IByteStreamEventAccess.h"

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

#include "TrigT1CaloEvent/CPBSCollectionV2.h"

#include "CpByteStreamV2Cnv.h"
#include "CpByteStreamV2Tool.h"

namespace LVL1BS {

CpByteStreamV2Cnv::CpByteStreamV2Cnv( ISvcLocator* svcloc )
    : Converter( storageType(), classID(), svcloc ),
      m_name("CpByteStreamV2Cnv"),
      m_tool("LVL1BS::CpByteStreamV2Tool/CpByteStreamV2Tool"),
      m_ByteStreamEventAccess("ByteStreamCnvSvc", m_name),
      m_log(msgSvc(), m_name), m_debug(false)
{
}

CpByteStreamV2Cnv::~CpByteStreamV2Cnv()
{
}

// CLID

const CLID& CpByteStreamV2Cnv::classID()
{
  return ClassID_traits<LVL1::CPBSCollectionV2>::ID();
}

long CpByteStreamV2Cnv::storageType()
{
  return ByteStreamAddress::storageType();
}

//  Init method gets all necessary services etc.


StatusCode CpByteStreamV2Cnv::initialize()
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
    return sc;
  } else m_log << MSG::DEBUG << "Retrieved tool " << m_tool << endmsg;

  return StatusCode::SUCCESS;
}

// createRep should create the bytestream from RDOs.

StatusCode CpByteStreamV2Cnv::createRep( DataObject* pObj,
                                         IOpaqueAddress*& pAddr )
{
  if (m_debug) m_log << MSG::DEBUG << "createRep() called" << endmsg;

  RawEventWrite* re = m_ByteStreamEventAccess->getRawEvent();

  LVL1::CPBSCollectionV2* cp = 0;
  if( !SG::fromStorable( pObj, cp ) ) {
    m_log << MSG::ERROR << " Cannot cast to CPBSCollectionV2" << endmsg;
    return StatusCode::FAILURE;
  }

  const std::string nm = pObj->registry()->name();

  ByteStreamAddress* addr = new ByteStreamAddress( classID(), nm, "" );

  pAddr = addr;

  // Convert to ByteStream
  return m_tool->convert( cp, re );
}

} // end namespace
