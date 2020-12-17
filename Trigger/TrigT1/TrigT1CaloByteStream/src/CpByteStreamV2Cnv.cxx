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

#include "TrigT1CaloEvent/CPBSCollectionV2.h"

#include "CpByteStreamV2Cnv.h"
#include "CpByteStreamV2Tool.h"

namespace LVL1BS {

CpByteStreamV2Cnv::CpByteStreamV2Cnv( ISvcLocator* svcloc )
    : AthConstConverter( storageType(), classID(), svcloc, "CpByteStreamV2Cnv" ),
      m_tool("LVL1BS::CpByteStreamV2Tool/CpByteStreamV2Tool")
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
  StatusCode sc = Converter::initialize();
  ATH_CHECK( sc.isFailure() );
  ATH_CHECK( m_tool.retrieve() );

  return StatusCode::SUCCESS;
}

// createRep should create the bytestream from RDOs.

StatusCode CpByteStreamV2Cnv::createRepConst ( DataObject* pObj,
                                               IOpaqueAddress*& pAddr ) const
{
  LVL1::CPBSCollectionV2* cp = 0;
  if( !SG::fromStorable( pObj, cp ) ) {
    ATH_MSG_ERROR( " Cannot cast to CPBSCollectionV2" );
    return StatusCode::FAILURE;
  }

  const std::string nm = pObj->registry()->name();

  ByteStreamAddress* addr = new ByteStreamAddress( classID(), nm, "" );

  pAddr = addr;

  // Convert to ByteStream
  return m_tool->convert( cp );
}

} // end namespace
