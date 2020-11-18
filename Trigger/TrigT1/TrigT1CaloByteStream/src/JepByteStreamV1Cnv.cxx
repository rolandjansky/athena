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

#include "TrigT1CaloEvent/JEPBSCollectionV1.h"

#include "JepByteStreamV1Cnv.h"
#include "JepByteStreamV1Tool.h"

namespace LVL1BS {

JepByteStreamV1Cnv::JepByteStreamV1Cnv( ISvcLocator* svcloc )
    : AthConstConverter( storageType(), classID(), svcloc, "JepByteStreamV1Cnv" ),
      m_tool("LVL1BS::JepByteStreamV1Tool/JepByteStreamV1Tool")
{
}

JepByteStreamV1Cnv::~JepByteStreamV1Cnv()
{
}

// CLID

const CLID& JepByteStreamV1Cnv::classID()
{
  return ClassID_traits<LVL1::JEPBSCollectionV1>::ID();
}

long JepByteStreamV1Cnv::storageType()
{
  return ByteStreamAddress::storageType();
}

//  Init method gets all necessary services etc.


StatusCode JepByteStreamV1Cnv::initialize()
{
  ATH_CHECK( Converter::initialize() );
  ATH_CHECK( m_tool.retrieve() );

  return StatusCode::SUCCESS;
}

// createRep should create the bytestream from RDOs.

StatusCode JepByteStreamV1Cnv::createRepConst( DataObject* pObj,
                                               IOpaqueAddress*& pAddr ) const
{
  LVL1::JEPBSCollectionV1* jep = 0;
  if( !SG::fromStorable( pObj, jep ) ) {
    ATH_MSG_ERROR( " Cannot cast to JEPBSCollectionV1" );
    return StatusCode::FAILURE;
  }

  const std::string nm = pObj->registry()->name();

  pAddr = new ByteStreamAddress( classID(), nm, "" );

  // Convert to ByteStream
  return m_tool->convert( jep );
}

} // end namespace
