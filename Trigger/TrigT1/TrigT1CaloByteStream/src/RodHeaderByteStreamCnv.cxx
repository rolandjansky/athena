/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include <vector>
#include <stdint.h>

#include "ByteStreamCnvSvcBase/ByteStreamAddress.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"

#include "ByteStreamData/RawEvent.h"
#include "ByteStreamData/ROBData.h"

#include "DataModel/DataVector.h"

#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/StatusCode.h"

#include "SGTools/ClassID_traits.h"
#include "SGTools/StorableConversions.h"

#include "TrigT1CaloEvent/RODHeader.h"

#include "RodHeaderByteStreamTool.h"

#include "RodHeaderByteStreamCnv.h"

namespace LVL1BS {

RodHeaderByteStreamCnv::RodHeaderByteStreamCnv( ISvcLocator* svcloc )
    : Converter( ByteStream_StorageType, classID(), svcloc ),
      m_name("RodHeaderByteStreamCnv"),
      m_tool("LVL1BS::RodHeaderByteStreamTool/RodHeaderByteStreamTool"),
      m_robDataProvider("ROBDataProviderSvc", m_name),
      m_log(msgSvc(), m_name), m_debug(false)
{
}

RodHeaderByteStreamCnv::~RodHeaderByteStreamCnv()
{
}

// CLID

const CLID& RodHeaderByteStreamCnv::classID()
{
  return ClassID_traits<DataVector<LVL1::RODHeader> >::ID();
}

//  Init method gets all necessary services etc.

#ifndef PACKAGE_VERSION
#define PACKAGE_VERSION "unknown"
#endif

StatusCode RodHeaderByteStreamCnv::initialize()
{
  m_debug = msgSvc()->outputLevel(m_name) <= MSG::DEBUG;
  m_log << MSG::DEBUG << "Initializing " << m_name << " - package version "
                      << PACKAGE_VERSION << endreq;

  StatusCode sc = Converter::initialize();
  if ( sc.isFailure() )
    return sc;

  // Retrieve Tool
  sc = m_tool.retrieve();
  if ( sc.isFailure() ) {
    m_log << MSG::ERROR << "Failed to retrieve tool " << m_tool << endreq;
    return sc;
  } else m_log << MSG::DEBUG << "Retrieved tool " << m_tool << endreq;

  // Get ROBDataProvider
  sc = m_robDataProvider.retrieve();
  if ( sc.isFailure() ) {
    m_log << MSG::ERROR << "Failed to retrieve service "
          << m_robDataProvider << endreq;
    return sc ;
  } else {
    m_log << MSG::DEBUG << "Retrieved service "
          << m_robDataProvider << endreq;
  }

  return StatusCode::SUCCESS;
}

// createObj should create the RDO from bytestream.

StatusCode RodHeaderByteStreamCnv::createObj( IOpaqueAddress* pAddr,
                                        DataObject*& pObj )
{
  if (m_debug) m_log << MSG::DEBUG << "createObj() called" << endreq;

  ByteStreamAddress *pBS_Addr;
  pBS_Addr = dynamic_cast<ByteStreamAddress *>( pAddr );
  if ( !pBS_Addr ) {
    m_log << MSG::ERROR << " Can not cast to ByteStreamAddress " << endreq;
    return StatusCode::FAILURE;
  }

  const std::string nm = *( pBS_Addr->par() );

  if (m_debug) m_log << MSG::DEBUG << " Creating Objects " << nm << endreq;

  // get SourceIDs
  const std::vector<uint32_t>& vID(m_tool->sourceIDs(nm));

  // get ROB fragments
  IROBDataProviderSvc::VROBFRAG robFrags;
  m_robDataProvider->getROBData( vID, robFrags );

  // size check
  DataVector<LVL1::RODHeader>* const rhCollection =
                                           new DataVector<LVL1::RODHeader>;
  if (m_debug) {
    m_log << MSG::DEBUG << " Number of ROB fragments is " << robFrags.size()
          << endreq;
  }
  if (robFrags.size() == 0) {
    pObj = SG::asStorable(rhCollection) ;
    return StatusCode::SUCCESS;
  }

  StatusCode sc = m_tool->convert(robFrags, rhCollection);
  if ( sc.isFailure() ) {
    m_log << MSG::ERROR << " Failed to create Objects   " << nm << endreq;
    delete rhCollection;
    return sc;
  }

  pObj = SG::asStorable(rhCollection);

  return sc;
}

} // end namespace
