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
    : AthConstConverter( storageType(), classID(), svcloc, "CpmRoiByteStreamV2Cnv" ),
      m_tool("LVL1BS::CpmRoiByteStreamV2Tool/CpmRoiByteStreamV2Tool"),
      m_robDataProvider("ROBDataProviderSvc", name())
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
  ATH_CHECK( Converter::initialize() );
  ATH_CHECK( m_tool.retrieve() );

  // Get ROBDataProvider
  StatusCode sc = m_robDataProvider.retrieve();
  if ( sc.isFailure() ) {
    ATH_MSG_WARNING( "Failed to retrieve service " << m_robDataProvider );
    // return is disabled for Write BS which does not require ROBDataProviderSvc
    // return sc ;
  }

  return StatusCode::SUCCESS;
}

// createObj should create the RDO from bytestream.

StatusCode CpmRoiByteStreamV2Cnv::createObjConst ( IOpaqueAddress* pAddr,
                                                   DataObject*& pObj ) const
{
  ByteStreamAddress *pBS_Addr;
  pBS_Addr = dynamic_cast<ByteStreamAddress *>( pAddr );
  if ( !pBS_Addr ) {
    ATH_MSG_ERROR( " Can not cast to ByteStreamAddress " );
    return StatusCode::FAILURE;
  }

  const std::string nm = *( pBS_Addr->par() );

  ATH_MSG_DEBUG( " Creating Objects " << nm );

  // get SourceIDs
  const std::vector<uint32_t>& vID(m_tool->sourceIDs(nm));

  // get ROB fragments
  IROBDataProviderSvc::VROBFRAG robFrags;
  m_robDataProvider->getROBData( vID, robFrags );

  // size check
  auto roiCollection = std::make_unique<DataVector<LVL1::CPMTobRoI> >();
  ATH_MSG_DEBUG( " Number of ROB fragments is " << robFrags.size() );

  if (robFrags.size() == 0) {
    pObj = SG::asStorable(std::move(roiCollection)) ;
    return StatusCode::SUCCESS;
  }

  ATH_CHECK( m_tool->convert(robFrags, roiCollection.get()) );

  pObj = SG::asStorable(std::move(roiCollection));

  return StatusCode::SUCCESS;
}

// createRep should create the bytestream from RDOs.

StatusCode CpmRoiByteStreamV2Cnv::createRepConst( DataObject* pObj,
                                                  IOpaqueAddress*& pAddr ) const
{
  DataVector<LVL1::CPMTobRoI>* roiCollection = 0;
  if( !SG::fromStorable( pObj, roiCollection ) ) {
    ATH_MSG_ERROR( " Cannot cast to DataVector<CPMTobRoI>" );
    return StatusCode::FAILURE;
  }

  const std::string nm = pObj->registry()->name();

  pAddr = new ByteStreamAddress( classID(), nm, "" );

  // Convert to ByteStream
  return m_tool->convert( roiCollection );
}

} // end namespace
