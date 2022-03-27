/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//  BCM_RawContByteStreamCnv.cxx
//   Implementation file for class BCM_RawContByteStreamCnv
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
// classID() - returning BCM_RDO_Container ID
// createRep() - convert BCM_RawData in the container into ByteStream
///////////////////////////////////////////////////////////////////
//  Version 00-00-39 05/03/2007 Daniel Dobos for pixels
///////////////////////////////////////////////////////////////////

#include "BCM_RawDataByteStreamCnv/BCM_RawContByteStreamCnv.h"

#include "BCM_RawDataByteStreamCnv/BCM_RawContByteStreamTool.h"
#include "ByteStreamCnvSvcBase/ByteStreamCnvSvcBase.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthenaKernel/StorableConversions.h"

#include "ByteStreamCnvSvcBase/ByteStreamAddress.h" 
#include "InDetBCM_RawData/InDetBCM_RawDataCLASS_DEF.h"

////////////////////////
// constructor
////////////////////////
BCM_RawContByteStreamCnv::BCM_RawContByteStreamCnv(ISvcLocator* svcloc) :
  AthConstConverter(storageType(), classID(),svcloc,"BCM_RawContByteStreamCnv"),
  m_BCMRawContBSTool{}
{
}

// ////////////////////////
// // destructor
// ////////////////////////
// BCM_RawContByteStreamCnv::~BCM_RawContByteStreamCnv() {
// }

////////////////////////
// initialize()
////////////////////////
StatusCode BCM_RawContByteStreamCnv::initialize() {

  ATH_CHECK( AthConstConverter::initialize() );

  // retrieve Tool
  IToolSvc* toolSvc = nullptr;
  ATH_CHECK( service("ToolSvc",toolSvc) );
  ATH_CHECK( toolSvc->retrieveTool("BCM_RawContByteStreamTool", m_BCMRawContBSTool) );
  return StatusCode::SUCCESS;
}

////////////////////////
// classID() - returning BCM_RDO_Container ID
////////////////////////
const CLID& BCM_RawContByteStreamCnv::classID() {
  return ClassID_traits<BCM_RDO_Container>::ID();
}

long BCM_RawContByteStreamCnv::storageType() {
  return ByteStreamAddress::storageType();
}

////////////////////////
// createRep() - convert BCM_RawData in the container into ByteStream
////////////////////////
StatusCode
BCM_RawContByteStreamCnv::createRepConst(DataObject* pObj, IOpaqueAddress*& pAddr) const
{
  BCM_RDO_Container* cont = nullptr;
  SG::fromStorable(pObj,cont);
  if(!cont) {
    ATH_MSG_ERROR( "Can not cast to BCM_RDO_Container" );
    return StatusCode::FAILURE;
  }

  std::string nm = pObj->registry()->name();
  pAddr = new ByteStreamAddress(classID(),nm,"");

  ATH_CHECK( m_BCMRawContBSTool->convert(cont) );

  return StatusCode::SUCCESS;
}
