/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
#include "AthenaKernel/errorcheck.h"

#include "TrigT1CaloEvent/CPBSCollectionV1.h"

#include "CpByteStreamV1Cnv.h"
#include "CpByteStreamV1Tool.h"

namespace LVL1BS {

CpByteStreamV1Cnv::CpByteStreamV1Cnv( ISvcLocator* svcloc )
    : AthConstConverter( storageType(), classID(), svcloc, "CpByteStreamV1Cnv" ),
      m_tool("LVL1BS::CpByteStreamV1Tool/CpByteStreamV1Tool")
{
}

CpByteStreamV1Cnv::~CpByteStreamV1Cnv()
{
}

// CLID

const CLID& CpByteStreamV1Cnv::classID()
{
  return ClassID_traits<LVL1::CPBSCollectionV1>::ID();
}

long CpByteStreamV1Cnv::storageType()
{
  return ByteStreamAddress::storageType();
}

//  Init method gets all necessary services etc.


StatusCode CpByteStreamV1Cnv::initialize()
{
  ATH_CHECK( Converter::initialize() );
  ATH_CHECK( m_tool.retrieve() );

  return StatusCode::SUCCESS;
}

// createRep should create the bytestream from RDOs.

StatusCode CpByteStreamV1Cnv::createRepConst( DataObject* pObj,
                                              IOpaqueAddress*& pAddr ) const
{
  LVL1::CPBSCollectionV1* cp = 0;
  if( !SG::fromStorable( pObj, cp ) ) {
    ATH_MSG_ERROR( "Cannot cast to CPBSCollectionV1" );
    return StatusCode::FAILURE;
  }

  const std::string nm = pObj->registry()->name();

  ByteStreamAddress* addr = new ByteStreamAddress( classID(), nm, "" );

  pAddr = addr;

  // Convert to ByteStream
  return m_tool->convert( cp );
}

} // end namespace
