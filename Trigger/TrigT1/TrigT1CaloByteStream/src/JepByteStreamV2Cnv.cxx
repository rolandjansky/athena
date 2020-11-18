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

#include "TrigT1CaloEvent/JEPBSCollectionV2.h"

#include "JepByteStreamV2Cnv.h"
#include "JepByteStreamV2Tool.h"

namespace LVL1BS {

JepByteStreamV2Cnv::JepByteStreamV2Cnv( ISvcLocator* svcloc )
    : AthConstConverter( storageType(), classID(), svcloc, "JepByteStreamV2Cnv" ),
      m_tool("LVL1BS::JepByteStreamV2Tool/JepByteStreamV2Tool")
{
}

JepByteStreamV2Cnv::~JepByteStreamV2Cnv()
{
}

// CLID

const CLID& JepByteStreamV2Cnv::classID()
{
  return ClassID_traits<LVL1::JEPBSCollectionV2>::ID();
}

long JepByteStreamV2Cnv::storageType()
{
  return ByteStreamAddress::storageType();
}

//  Init method gets all necessary services etc.


StatusCode JepByteStreamV2Cnv::initialize()
{
  ATH_CHECK( Converter::initialize() );
  ATH_CHECK( m_tool.retrieve() );

  return StatusCode::SUCCESS;
}

// createRep should create the bytestream from RDOs.

StatusCode JepByteStreamV2Cnv::createRepConst ( DataObject* pObj,
                                                IOpaqueAddress*& pAddr ) const
{
  LVL1::JEPBSCollectionV2* jep = 0;
  if( !SG::fromStorable( pObj, jep ) ) {
    ATH_MSG_ERROR( " Cannot cast to JEPBSCollectionV2" );
    return StatusCode::FAILURE;
  }

  const std::string nm = pObj->registry()->name();

  pAddr = new ByteStreamAddress( classID(), nm, "" );

  // Convert to ByteStream
  return m_tool->convert( jep );
}

} // end namespace
