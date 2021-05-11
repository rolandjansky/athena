/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonByteStream/CscRdoContByteStreamCnv.h"
#include "MuonRDO/CscRawDataContainer.h"
#include "ByteStreamCnvSvcBase/ByteStreamAddress.h" 
#include "AthenaKernel/StorableConversions.h"


CscRdoContByteStreamCnv::CscRdoContByteStreamCnv(ISvcLocator* svcloc) 
  : AthConstConverter(storageType(), classID(), svcloc, "CscRdoContByteStreamCnv"),
  m_tool("Muon::CscRdoContByteStreamTool")
{}


// class ID
const CLID& CscRdoContByteStreamCnv::classID()
{
  return ClassID_traits<CscRawDataContainer>::ID();
}

long CscRdoContByteStreamCnv::storageType()
{
  return ByteStreamAddress::storageType();
}

// initialize
StatusCode CscRdoContByteStreamCnv::initialize() 
{
  ATH_MSG_DEBUG( " initialize " );

  ATH_CHECK( AthConstConverter::initialize() );
  ATH_CHECK( m_tool.retrieve() );
  return StatusCode::SUCCESS;
}

// convert CSC RDOs in the container into ByteStream
StatusCode
CscRdoContByteStreamCnv::createRepConst(DataObject* pObj,
                                        IOpaqueAddress*& pAddr) const
{
  // retrieve CSC RDO container
  CscRawDataContainer* cont = nullptr;
  SG::fromStorable(pObj, cont);
  if (!cont) {
    ATH_MSG_ERROR( " Can not cast to CscRawDataContainer" );
    return StatusCode::FAILURE;    
  } 

  // create address
  std::string nm = pObj->registry()->name(); 
  pAddr = new ByteStreamAddress(classID(),nm,""); 

  return m_tool->convert(cont, msg()); 
}
