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

#include "TrigT1CaloEvent/JEPBSCollection.h"

#include "JepByteStreamCnv.h"
#include "JepByteStreamTool.h"

namespace LVL1BS {

JepByteStreamCnv::JepByteStreamCnv( ISvcLocator* svcloc )
    : AthConstConverter( storageType(), classID(), svcloc, "JepByteStreamCnv" ),
      m_tool("LVL1BS::JepByteStreamTool/JepByteStreamTool")
{
}

JepByteStreamCnv::~JepByteStreamCnv()
{
}

// CLID

const CLID& JepByteStreamCnv::classID()
{
  return ClassID_traits<LVL1::JEPBSCollection>::ID();
}

long JepByteStreamCnv::storageType()
{
  return ByteStreamAddress::storageType();
}

//  Init method gets all necessary services etc.


StatusCode JepByteStreamCnv::initialize()
{
  ATH_CHECK( Converter::initialize() );
  ATH_CHECK(  m_tool.retrieve() );

  return StatusCode::SUCCESS;
}

// createRep should create the bytestream from RDOs.

StatusCode JepByteStreamCnv::createRepConst ( DataObject* pObj,
                                              IOpaqueAddress*& pAddr ) const
{
  LVL1::JEPBSCollection* jep = 0;
  if( !SG::fromStorable( pObj, jep ) ) {
    ATH_MSG_ERROR( " Cannot cast to JEPBSCollection" );
    return StatusCode::FAILURE;
  }

  const std::string nm = pObj->registry()->name();

  pAddr = new ByteStreamAddress( classID(), nm, "" );

  // Convert to ByteStream
  return m_tool->convert( jep );
}

} // end namespace
