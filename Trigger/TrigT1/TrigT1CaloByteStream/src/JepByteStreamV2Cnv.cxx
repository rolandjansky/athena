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
    : Converter( storageType(), classID(), svcloc ),
      m_name("JepByteStreamV2Cnv"),
      m_tool("LVL1BS::JepByteStreamV2Tool/JepByteStreamV2Tool"),
      m_ByteStreamEventAccess("ByteStreamCnvSvc", m_name)
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
  ATH_CHECK( m_ByteStreamEventAccess.retrieve() );
  ATH_CHECK( m_tool.retrieve() );

  return StatusCode::SUCCESS;
}

// createRep should create the bytestream from RDOs.

StatusCode JepByteStreamV2Cnv::createRep( DataObject* pObj,
                                          IOpaqueAddress*& pAddr )
{
  RawEventWrite* re = m_ByteStreamEventAccess->getRawEvent();

  LVL1::JEPBSCollectionV2* jep = 0;
  if( !SG::fromStorable( pObj, jep ) ) {
    REPORT_ERROR (StatusCode::FAILURE) << " Cannot cast to JEPBSCollectionV2";
    return StatusCode::FAILURE;
  }

  const std::string nm = pObj->registry()->name();

  ByteStreamAddress* addr = new ByteStreamAddress( classID(), nm, "" );

  pAddr = addr;

  // Convert to ByteStream
  return m_tool->convert( jep, re );
}

} // end namespace
