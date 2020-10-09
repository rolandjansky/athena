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

#include "TrigT1CaloEvent/CPMTobRoI.h"

#include "CpmRoiByteStreamV2Cnv.h"
#include "CpmRoiByteStreamV2Tool.h"

namespace LVL1BS {

CpmRoiByteStreamV2Cnv::CpmRoiByteStreamV2Cnv( ISvcLocator* svcloc )
    : Converter( storageType(), classID(), svcloc ),
      m_name("CpmRoiByteStreamV2Cnv"),
      m_tool("LVL1BS::CpmRoiByteStreamV2Tool/CpmRoiByteStreamV2Tool"),
      m_robDataProvider("ROBDataProviderSvc", m_name),
      m_ByteStreamEventAccess("ByteStreamCnvSvc", m_name),
      m_debug(false)
{
}

CpmRoiByteStreamV2Cnv::~CpmRoiByteStreamV2Cnv()
{
}

// CLID

const CLID& CpmRoiByteStreamV2Cnv::classID()
{
  return ClassID_traits<DataVector<LVL1::CPMTobRoI> >::ID();
}

long CpmRoiByteStreamV2Cnv::storageType()
{
  return ByteStreamAddress::storageType();
}

//  Init method gets all necessary services etc.


StatusCode CpmRoiByteStreamV2Cnv::initialize()
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

StatusCode CpmRoiByteStreamV2Cnv::createObj( IOpaqueAddress* pAddr,
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
  auto roiCollection = std::make_unique<DataVector<LVL1::CPMTobRoI> >();
  if (m_debug) {
    REPORT_MESSAGE (MSG::DEBUG) <<" Number of ROB fragments is " << robFrags.size();
  }
  if (robFrags.size() == 0) {
    pObj = SG::asStorable(std::move(roiCollection)) ;
    return StatusCode::SUCCESS;
  }

  ATH_CHECK( m_tool->convert(robFrags, roiCollection.get()) );

  pObj = SG::asStorable(std::move(roiCollection));

  return StatusCode::SUCCESS;
}

// createRep should create the bytestream from RDOs.

StatusCode CpmRoiByteStreamV2Cnv::createRep( DataObject* pObj,
                                        IOpaqueAddress*& pAddr )
{
  RawEventWrite* re = m_ByteStreamEventAccess->getRawEvent();

  DataVector<LVL1::CPMTobRoI>* roiCollection = 0;
  if( !SG::fromStorable( pObj, roiCollection ) ) {
    REPORT_ERROR (StatusCode::FAILURE) << " Cannot cast to DataVector<CPMTobRoI>";
    return StatusCode::FAILURE;
  }

  const std::string nm = pObj->registry()->name();

  ByteStreamAddress* addr = new ByteStreamAddress( classID(), nm, "" );

  pAddr = addr;

  // Convert to ByteStream
  return m_tool->convert( roiCollection, re );
}

} // end namespace
