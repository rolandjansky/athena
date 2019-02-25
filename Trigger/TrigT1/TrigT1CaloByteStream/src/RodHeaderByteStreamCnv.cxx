/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include <vector>
#include <stdint.h>

#include "ByteStreamCnvSvcBase/ByteStreamAddress.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"

#include "ByteStreamData/RawEvent.h"
#include "ByteStreamData/ROBData.h"

#include "AthContainers/DataVector.h"

#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/StatusCode.h"

#include "AthenaKernel/ClassID_traits.h"
#include "AthenaKernel/StorableConversions.h"

#include "TrigT1CaloEvent/RODHeader.h"

#include "RodHeaderByteStreamTool.h"

#include "RodHeaderByteStreamCnv.h"

namespace LVL1BS {

RodHeaderByteStreamCnv::RodHeaderByteStreamCnv( ISvcLocator* svcloc )
    : Converter( storageType(), classID(), svcloc ),
      m_name("RodHeaderByteStreamCnv"),
      m_tool("LVL1BS::RodHeaderByteStreamTool/RodHeaderByteStreamTool"),
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

long RodHeaderByteStreamCnv::storageType()
{
  return ByteStreamAddress::storageType();
}

//  Init method gets all necessary services etc.


StatusCode RodHeaderByteStreamCnv::initialize()
{
  m_debug = msgSvc()->outputLevel(m_name) <= MSG::DEBUG;
  m_log << MSG::DEBUG << "Initializing " << m_name << " - package version "
                      << PACKAGE_VERSION << endmsg;

  StatusCode sc = Converter::initialize();
  if ( sc.isFailure() )
    return sc;

  // Retrieve Tool
  sc = m_tool.retrieve();
  if ( sc.isFailure() ) {
    m_log << MSG::ERROR << "Failed to retrieve tool " << m_tool << endmsg;
    return sc;
  } else m_log << MSG::DEBUG << "Retrieved tool " << m_tool << endmsg;

  return StatusCode::SUCCESS;
}

// createObj should create the RDO from bytestream.

StatusCode RodHeaderByteStreamCnv::createObj( IOpaqueAddress* pAddr,
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
  // size check
  DataVector<LVL1::RODHeader>* const rhCollection =
                                           new DataVector<LVL1::RODHeader>;
  StatusCode sc = m_tool->convert(nm, rhCollection);
  if ( sc.isFailure() ) {
    m_log << MSG::ERROR << " Failed to create Objects   " << nm << endmsg;
    delete rhCollection;
    return sc;
  }

  pObj = SG::asStorable(rhCollection);

  return sc;
}

} // end namespace
