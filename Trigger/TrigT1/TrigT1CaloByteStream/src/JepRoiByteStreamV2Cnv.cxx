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

#include "TrigT1CaloEvent/JEPRoIBSCollectionV2.h"

#include "JepRoiByteStreamV2Cnv.h"
#include "JepRoiByteStreamV2Tool.h"

namespace LVL1BS {

JepRoiByteStreamV2Cnv::JepRoiByteStreamV2Cnv( ISvcLocator* svcloc )
    : AthConstConverter( storageType(), classID(), svcloc, "JepRoiByteStreamV2Cnv" ),
      m_tool("LVL1BS::JepRoiByteStreamV2Tool/JepRoiByteStreamV2Tool")
{
}

JepRoiByteStreamV2Cnv::~JepRoiByteStreamV2Cnv()
{
}

// CLID

const CLID& JepRoiByteStreamV2Cnv::classID()
{
  return ClassID_traits<LVL1::JEPRoIBSCollectionV2>::ID();
}

long JepRoiByteStreamV2Cnv::storageType()
{
  return ByteStreamAddress::storageType();
}

//  Init method gets all necessary services etc.


StatusCode JepRoiByteStreamV2Cnv::initialize()
{
  ATH_CHECK( Converter::initialize() );
  ATH_CHECK(  m_tool.retrieve() );

  return StatusCode::SUCCESS;
}

// createRep should create the bytestream from RDOs.

StatusCode JepRoiByteStreamV2Cnv::createRepConst ( DataObject* pObj,
                                                   IOpaqueAddress*& pAddr ) const
{
  LVL1::JEPRoIBSCollectionV2* jep = 0;
  if( !SG::fromStorable( pObj, jep ) ) {
    ATH_MSG_ERROR( " Cannot cast to JEPRoIBSCollectionV2" );
    return StatusCode::FAILURE;
  }

  const std::string nm = pObj->registry()->name();

  pAddr = new ByteStreamAddress( classID(), nm, "" );

  // Convert to ByteStream
  return m_tool->convert( jep );
}

} // end namespace
