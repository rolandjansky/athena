/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include <vector>

#include "ByteStreamCnvSvcBase/ByteStreamAddress.h"

#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/StatusCode.h"

#include "AthenaKernel/ClassID_traits.h"
#include "SGTools/StlVectorClids.h"
#include "AthenaKernel/StorableConversions.h"

#include "L1CaloErrorByteStreamTool.h"

#include "L1CaloErrorByteStreamCnv.h"

namespace LVL1BS {

L1CaloErrorByteStreamCnv::L1CaloErrorByteStreamCnv( ISvcLocator* svcloc )
    : Converter( storageType(), classID(), svcloc ),
      m_name("L1CaloErrorByteStreamCnv"),
      m_tool("LVL1BS::L1CaloErrorByteStreamTool/L1CaloErrorByteStreamTool"),
      m_log(msgSvc(), m_name), m_debug(false)
{
}

L1CaloErrorByteStreamCnv::~L1CaloErrorByteStreamCnv()
{
}

// CLID

const CLID& L1CaloErrorByteStreamCnv::classID()
{
  return ClassID_traits<std::vector<unsigned int> >::ID();
}

long L1CaloErrorByteStreamCnv::storageType()
{
  return ByteStreamAddress::storageType();
}

//  Init method gets all necessary services etc.

#ifndef PACKAGE_VERSION
#define PACKAGE_VERSION "unknown"
#endif

StatusCode L1CaloErrorByteStreamCnv::initialize()
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

StatusCode L1CaloErrorByteStreamCnv::createObj( IOpaqueAddress* pAddr,
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

  std::vector<unsigned int>* const errCollection =
                                             new std::vector<unsigned int>;

  StatusCode sc = m_tool->errors(errCollection);
  if ( sc.isFailure() ) {
    m_log << MSG::ERROR << " Failed to create Objects   " << nm << endmsg;
    delete errCollection;
    return sc;
  }

  pObj = SG::asStorable(errCollection);

  return sc;
}

} // end namespace
