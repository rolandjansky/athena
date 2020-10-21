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

#include "TrigT1CaloEvent/CPMTower.h"

#include "CpReadByteStreamV1V2Cnv.h"
#include "CpByteStreamV1Tool.h"
#include "CpByteStreamV2Tool.h"

namespace LVL1BS {

CpReadByteStreamV1V2Cnv::CpReadByteStreamV1V2Cnv( ISvcLocator* svcloc )
    : Converter( storageType(), classID(), svcloc ),
      m_name("CpReadByteStreamV1V2Cnv"),
      m_tool1("LVL1BS::CpByteStreamV1Tool/CpByteStreamV1Tool"),
      m_tool2("LVL1BS::CpByteStreamV2Tool/CpByteStreamV2Tool"),
      m_robDataProvider("ROBDataProviderSvc", m_name),
      m_debug(false)
{
}

CpReadByteStreamV1V2Cnv::~CpReadByteStreamV1V2Cnv()
{
}

// CLID

const CLID& CpReadByteStreamV1V2Cnv::classID()
{
  return ClassID_traits<DataVector<LVL1::CPMTower> >::ID();
}

long CpReadByteStreamV1V2Cnv::storageType()
{
  return ByteStreamAddress::storageType();
}

//  Init method gets all necessary services etc.


StatusCode CpReadByteStreamV1V2Cnv::initialize()
{
  m_debug = msgSvc()->outputLevel(m_name) <= MSG::DEBUG;

  ATH_CHECK( Converter::initialize() );
  ATH_CHECK( m_tool1.retrieve() );
  ATH_CHECK( m_tool2.retrieve() );
  ATH_CHECK( m_robDataProvider.retrieve() );

  return StatusCode::SUCCESS;
}

// createObj should create the RDO from bytestream.

StatusCode CpReadByteStreamV1V2Cnv::createObj( IOpaqueAddress* pAddr,
                                               DataObject*& pObj )
{
  ByteStreamAddress *pBS_Addr;
  pBS_Addr = dynamic_cast<ByteStreamAddress *>( pAddr );
  if ( !pBS_Addr ) {
    REPORT_ERROR (StatusCode::FAILURE) << " Can not cast to ByteStreamAddress ";
    return StatusCode::FAILURE;
  }

  const std::string nm = *( pBS_Addr->par() );

  if (m_debug) {
    REPORT_MESSAGE (MSG::DEBUG) << " Creating Objects " << nm;
  }

  // get SourceIDs
  const std::vector<uint32_t>& vID1(m_tool1->sourceIDs());
  const std::vector<uint32_t>& vID2(m_tool2->sourceIDs());

  // get ROB fragments
  IROBDataProviderSvc::VROBFRAG robFrags1;
  m_robDataProvider->getROBData( vID1, robFrags1 );
  IROBDataProviderSvc::VROBFRAG robFrags2;
  m_robDataProvider->getROBData( vID2, robFrags2 );

  // size check
  auto towerCollection = std::make_unique<DataVector<LVL1::CPMTower> >();
  if (m_debug) {
    REPORT_MESSAGE (MSG::DEBUG) << " Number of ROB fragments is " << robFrags1.size()
                                << ", " << robFrags2.size();
  }
  if (robFrags1.size() == 0 && robFrags2.size() == 0) {
    pObj = SG::asStorable(std::move(towerCollection)) ;
    return StatusCode::SUCCESS;
  }

  // Pre-LS1 data
  if (robFrags1.size() > 0) {
    ATH_CHECK( m_tool1->convert(nm, robFrags1, towerCollection.get()) );
  }
  // Post-LS1 data
  if (robFrags2.size() > 0) {
    ATH_CHECK( m_tool2->convert(nm, robFrags2, towerCollection.get()) ); 
 }

  pObj = SG::asStorable(std::move(towerCollection));

  return StatusCode::SUCCESS;
}

} // end namespace
