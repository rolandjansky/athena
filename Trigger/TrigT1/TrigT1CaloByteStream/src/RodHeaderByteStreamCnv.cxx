/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
#include "AthenaKernel/errorcheck.h"

#include "TrigT1CaloEvent/RODHeader.h"

#include "RodHeaderByteStreamTool.h"

#include "RodHeaderByteStreamCnv.h"

namespace LVL1BS {

RodHeaderByteStreamCnv::RodHeaderByteStreamCnv( ISvcLocator* svcloc )
    : Converter( storageType(), classID(), svcloc ),
      m_name("RodHeaderByteStreamCnv"),
      m_tool("LVL1BS::RodHeaderByteStreamTool/RodHeaderByteStreamTool")
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
  ATH_CHECK( Converter::initialize() );
  ATH_CHECK( m_tool.retrieve() );

  return StatusCode::SUCCESS;
}

// createObj should create the RDO from bytestream.

StatusCode RodHeaderByteStreamCnv::createObj( IOpaqueAddress* pAddr,
                                        DataObject*& pObj )
{
  ByteStreamAddress *pBS_Addr;
  pBS_Addr = dynamic_cast<ByteStreamAddress *>( pAddr );
  if ( !pBS_Addr ) {
    REPORT_ERROR (StatusCode::FAILURE) << " Can not cast to ByteStreamAddress ";
    return StatusCode::FAILURE;
  }

  const std::string nm = *( pBS_Addr->par() );
  // size check
  auto rhCollection = std::make_unique<DataVector<LVL1::RODHeader> >();
  ATH_CHECK( m_tool->convert(nm, rhCollection.get()) );

  pObj = SG::asStorable(std::move(rhCollection));

  return StatusCode::SUCCESS;
}

} // end namespace
