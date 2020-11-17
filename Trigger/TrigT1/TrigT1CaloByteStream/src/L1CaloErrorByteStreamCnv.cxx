/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#include <vector>

#include "ByteStreamCnvSvcBase/ByteStreamAddress.h"

#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/StatusCode.h"

#include "AthenaKernel/ClassID_traits.h"
#include "SGTools/StlVectorClids.h"
#include "AthenaKernel/StorableConversions.h"
#include "AthenaKernel/errorcheck.h"

#include "L1CaloErrorByteStreamTool.h"

#include "L1CaloErrorByteStreamCnv.h"

namespace LVL1BS {

L1CaloErrorByteStreamCnv::L1CaloErrorByteStreamCnv( ISvcLocator* svcloc )
    : AthConstConverter( storageType(), classID(), svcloc, "L1CaloErrorByteStreamCnv" ),
      m_tool("LVL1BS::L1CaloErrorByteStreamTool/L1CaloErrorByteStreamTool")
{
}

L1CaloErrorByteStreamCnv::~L1CaloErrorByteStreamCnv()
{
}

// CLID

const CLID& L1CaloErrorByteStreamCnv::classID()
{
  return ClassID_traits<std::vector<unsigned int> >::ID();
}

long L1CaloErrorByteStreamCnv::storageType()
{
  return ByteStreamAddress::storageType();
}

//  Init method gets all necessary services etc.


StatusCode L1CaloErrorByteStreamCnv::initialize()
{
  ATH_CHECK( Converter::initialize() );
  ATH_CHECK( m_tool.retrieve() );

  return StatusCode::SUCCESS;
}

// createObj should create the RDO from bytestream.

StatusCode L1CaloErrorByteStreamCnv::createObjConst ( IOpaqueAddress* pAddr,
                                                      DataObject*& pObj ) const
{
  ByteStreamAddress *pBS_Addr;
  pBS_Addr = dynamic_cast<ByteStreamAddress *>( pAddr );
  if ( !pBS_Addr ) {
    ATH_MSG_ERROR( " Can not cast to ByteStreamAddress " );
    return StatusCode::FAILURE;
  }

  const std::string nm = *( pBS_Addr->par() );

  ATH_MSG_DEBUG( " Creating Objects " << nm );

  auto errCollection = std::make_unique<std::vector<unsigned int> >();

  ATH_CHECK( m_tool->errors(errCollection.get()) );

  pObj = SG::asStorable(std::move(errCollection));

  return StatusCode::SUCCESS;
}

} // end namespace
