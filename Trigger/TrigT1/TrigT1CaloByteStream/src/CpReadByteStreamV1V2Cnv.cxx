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

#include "TrigT1CaloEvent/CPMTower.h"

#include "CpReadByteStreamV1V2Cnv.h"
#include "CpByteStreamV1Tool.h"
#include "CpByteStreamV2Tool.h"

namespace LVL1BS {

CpReadByteStreamV1V2Cnv::CpReadByteStreamV1V2Cnv( ISvcLocator* svcloc )
    : Converter( storageType(), classID(), svcloc ),
      m_name("CpReadByteStreamV1V2Cnv"),
      m_tool1("LVL1BS::CpByteStreamV1Tool/CpByteStreamV1Tool"),
      m_tool2("LVL1BS::CpByteStreamV2Tool/CpByteStreamV2Tool"),
      m_robDataProvider("ROBDataProviderSvc", m_name),
      m_log(msgSvc(), m_name), m_debug(false)
{
}

CpReadByteStreamV1V2Cnv::~CpReadByteStreamV1V2Cnv()
{
}

// CLID

const CLID& CpReadByteStreamV1V2Cnv::classID()
{
  return ClassID_traits<DataVector<LVL1::CPMTower> >::ID();
}

long CpReadByteStreamV1V2Cnv::storageType()
{
  return ByteStreamAddress::storageType();
}

//  Init method gets all necessary services etc.

#ifndef PACKAGE_VERSION
#define PACKAGE_VERSION "unknown"
#endif

StatusCode CpReadByteStreamV1V2Cnv::initialize()
{
  m_debug = msgSvc()->outputLevel(m_name) <= MSG::DEBUG;
  m_log << MSG::DEBUG << "Initializing " << m_name << " - package version "
                      << PACKAGE_VERSION << endmsg;

  StatusCode sc = Converter::initialize();
  if ( sc.isFailure() )
    return sc;

  // Retrieve Tools
  sc = m_tool1.retrieve();
  if ( sc.isFailure() ) {
    m_log << MSG::ERROR << "Failed to retrieve tool " << m_tool1 << endmsg;
    return StatusCode::FAILURE;
  } else m_log << MSG::DEBUG << "Retrieved tool " << m_tool1 << endmsg;
  sc = m_tool2.retrieve();
  if ( sc.isFailure() ) {
    m_log << MSG::ERROR << "Failed to retrieve tool " << m_tool2 << endmsg;
    return StatusCode::FAILURE;
  } else m_log << MSG::DEBUG << "Retrieved tool " << m_tool2 << endmsg;

  // Get ROBDataProvider
  sc = m_robDataProvider.retrieve();
  if ( sc.isFailure() ) {
    m_log << MSG::WARNING << "Failed to retrieve service "
          << m_robDataProvider << endmsg;
    return sc ;
  } else {
    m_log << MSG::DEBUG << "Retrieved service "
          << m_robDataProvider << endmsg;
  }

  return StatusCode::SUCCESS;
}

// createObj should create the RDO from bytestream.

StatusCode CpReadByteStreamV1V2Cnv::createObj( IOpaqueAddress* pAddr,
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
  const std::vector<uint32_t>& vID1(m_tool1->sourceIDs(nm));
  const std::vector<uint32_t>& vID2(m_tool2->sourceIDs(nm));

  // get ROB fragments
  IROBDataProviderSvc::VROBFRAG robFrags1;
  m_robDataProvider->getROBData( vID1, robFrags1 );
  IROBDataProviderSvc::VROBFRAG robFrags2;
  m_robDataProvider->getROBData( vID2, robFrags2 );

  // size check
  DataVector<LVL1::CPMTower>* const towerCollection = new DataVector<LVL1::CPMTower>;
  if (m_debug) {
    m_log << MSG::DEBUG << " Number of ROB fragments is " << robFrags1.size()
          << ", " << robFrags2.size() << endmsg;
  }
  if (robFrags1.size() == 0 && robFrags2.size() == 0) {
    pObj = SG::asStorable(towerCollection) ;
    return StatusCode::SUCCESS;
  }

  // Pre-LS1 data
  if (robFrags1.size() > 0) {
    StatusCode sc = m_tool1->convert(robFrags1, towerCollection);
    if ( sc.isFailure() ) {
      m_log << MSG::ERROR << " Failed to create Objects   " << nm << endmsg;
      delete towerCollection;
      return sc;
    }
  }
  // Post-LS1 data
  if (robFrags2.size() > 0) {
    StatusCode sc = m_tool2->convert(robFrags2, towerCollection);
    if ( sc.isFailure() ) {
      m_log << MSG::ERROR << " Failed to create Objects   " << nm << endmsg;
      delete towerCollection;
      return sc;
    }
  }

  pObj = SG::asStorable(towerCollection);

  return StatusCode::SUCCESS;
}

} // end namespace
