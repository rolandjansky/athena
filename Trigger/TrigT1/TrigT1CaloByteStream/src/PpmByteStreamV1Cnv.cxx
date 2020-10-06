/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
#include "AthenaKernel/errorcheck.h"

#include "TrigT1CaloEvent/TriggerTower.h"

#include "PpmByteStreamV1Cnv.h"
#include "PpmByteStreamV1Tool.h"

namespace LVL1BS {

PpmByteStreamV1Cnv::PpmByteStreamV1Cnv( ISvcLocator* svcloc )
    : Converter( storageType(), classID(), svcloc ),
      m_name("PpmByteStreamV1Cnv"),
      m_tool("LVL1BS::PpmByteStreamV1Tool/PpmByteStreamV1Tool"),
      m_robDataProvider("ROBDataProviderSvc", m_name),
      m_ByteStreamEventAccess("ByteStreamCnvSvc", m_name),
      m_debug(false)
{
}

PpmByteStreamV1Cnv::~PpmByteStreamV1Cnv()
{
}

// CLID

const CLID& PpmByteStreamV1Cnv::classID()
{
  return ClassID_traits<DataVector<LVL1::TriggerTower> >::ID();
}

long PpmByteStreamV1Cnv::storageType()
{
  return ByteStreamAddress::storageType();
}

//  Init method gets all necessary services etc.


StatusCode PpmByteStreamV1Cnv::initialize()
{
  m_debug = msgSvc()->outputLevel(m_name) <= MSG::DEBUG;

  ATH_CHECK( Converter::initialize() );
  ATH_CHECK( m_ByteStreamEventAccess.retrieve() );
  ATH_CHECK( m_tool.retrieve() );

  // Get ROBDataProvider
  StatusCode sc = m_robDataProvider.retrieve();
  if ( sc.isFailure() ) {
    REPORT_MESSAGE (MSG::WARNING) << "Failed to retrieve service "
                                  << m_robDataProvider;
    // return is disabled for Write BS which does not require ROBDataProviderSvc
    // return sc ;
  }
  return StatusCode::SUCCESS;
}

// createObj should create the RDO from bytestream.

StatusCode PpmByteStreamV1Cnv::createObj( IOpaqueAddress* pAddr,
                                        DataObject*& pObj )
{
  ByteStreamAddress *pBS_Addr;
  pBS_Addr = dynamic_cast<ByteStreamAddress *>( pAddr );
  if ( !pBS_Addr ) {
    REPORT_ERROR (StatusCode::FAILURE) << " Can not cast to ByteStreamAddress ";
    return StatusCode::FAILURE;
  }

  const std::string nm = *( pBS_Addr->par() );

  if (m_debug) {
    REPORT_MESSAGE (MSG::DEBUG) << " Creating Objects " << nm;
  }

  // get SourceIDs
  const std::vector<uint32_t>& vID(m_tool->sourceIDs(nm));

  // get ROB fragments
  IROBDataProviderSvc::VROBFRAG robFrags;
  m_robDataProvider->getROBData( vID, robFrags );

  // size check
  auto ttCollection = std::make_unique<DataVector<LVL1::TriggerTower> >(SG::VIEW_ELEMENTS);
  if (m_debug) {
    REPORT_MESSAGE (MSG::DEBUG) << " Number of ROB fragments is " << robFrags.size();
  }
  if (robFrags.size() == 0) {
    pObj = SG::asStorable(std::move(ttCollection)) ;
    return StatusCode::SUCCESS;
  }

  ATH_CHECK( m_tool->convert(robFrags, ttCollection.get()) );

  pObj = SG::asStorable(std::move(ttCollection));

  return StatusCode::SUCCESS;
}

// createRep should create the bytestream from RDOs.

StatusCode PpmByteStreamV1Cnv::createRep( DataObject* pObj,
                                        IOpaqueAddress*& pAddr )
{
  RawEventWrite* re = m_ByteStreamEventAccess->getRawEvent();

  DataVector<LVL1::TriggerTower>* ttCollection = 0;
  if( !SG::fromStorable( pObj, ttCollection ) ) {
    REPORT_ERROR (StatusCode::FAILURE) << " Cannot cast to DataVector<TriggerTower>";
    return StatusCode::FAILURE;
  }

  const std::string nm = pObj->registry()->name();

  ByteStreamAddress* addr = new ByteStreamAddress( classID(), nm, "" );

  pAddr = addr;

  // Convert to ByteStream
  return m_tool->convert( ttCollection, re );
}

} // end namespace
