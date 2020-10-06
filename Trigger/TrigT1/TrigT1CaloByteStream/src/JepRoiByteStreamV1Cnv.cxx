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

#include "TrigT1CaloEvent/JEPRoIBSCollectionV1.h"

#include "JepRoiByteStreamV1Cnv.h"
#include "JepRoiByteStreamV1Tool.h"

namespace LVL1BS {

JepRoiByteStreamV1Cnv::JepRoiByteStreamV1Cnv( ISvcLocator* svcloc )
    : Converter( storageType(), classID(), svcloc ),
      m_name("JepRoiByteStreamV1Cnv"),
      m_tool("LVL1BS::JepRoiByteStreamV1Tool/JepRoiByteStreamV1Tool"),
      m_ByteStreamEventAccess("ByteStreamCnvSvc", m_name)
{
}

JepRoiByteStreamV1Cnv::~JepRoiByteStreamV1Cnv()
{
}

// CLID

const CLID& JepRoiByteStreamV1Cnv::classID()
{
  return ClassID_traits<LVL1::JEPRoIBSCollectionV1>::ID();
}

long JepRoiByteStreamV1Cnv::storageType()
{
  return ByteStreamAddress::storageType();
}

//  Init method gets all necessary services etc.


StatusCode JepRoiByteStreamV1Cnv::initialize()
{
  ATH_CHECK( Converter::initialize() );
  ATH_CHECK( m_ByteStreamEventAccess.retrieve() );
  ATH_CHECK( m_tool.retrieve() );

  return StatusCode::SUCCESS;
}

// createRep should create the bytestream from RDOs.

StatusCode JepRoiByteStreamV1Cnv::createRep( DataObject* pObj,
                                             IOpaqueAddress*& pAddr )
{
  RawEventWrite* re = m_ByteStreamEventAccess->getRawEvent();

  LVL1::JEPRoIBSCollectionV1* jep = 0;
  if( !SG::fromStorable( pObj, jep ) ) {
    REPORT_ERROR (StatusCode::FAILURE) << " Cannot cast to JEPRoIBSCollectionV1";
    return StatusCode::FAILURE;
  }

  const std::string nm = pObj->registry()->name();

  ByteStreamAddress* addr = new ByteStreamAddress( classID(), nm, "" );

  pAddr = addr;

  // Convert to ByteStream
  return m_tool->convert( jep, re );
}

} // end namespace
