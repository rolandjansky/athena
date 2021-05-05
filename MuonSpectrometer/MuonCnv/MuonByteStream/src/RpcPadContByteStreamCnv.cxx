/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonByteStream/RpcPadContByteStreamCnv.h"
#include "ByteStreamCnvSvcBase/ByteStreamAddress.h" 
#include "MuonRDO/RpcPadContainer.h"
#include "AthenaBaseComps/AthCheckMacros.h"
#include "AthenaKernel/StorableConversions.h"
#include <string> 


RpcPadContByteStreamCnv::RpcPadContByteStreamCnv(ISvcLocator* svcloc) :
    AthConstConverter(storageType(), classID(),svcloc,"RpcPadContByteStreamCnv"),
    m_tool("Muon::RpcPadContByteStreamTool")
{}

const CLID& RpcPadContByteStreamCnv::classID(){
  return ClassID_traits<RpcPadContainer>::ID() ;
}

long RpcPadContByteStreamCnv::storageType(){
  return ByteStreamAddress::storageType();
}

StatusCode RpcPadContByteStreamCnv::initialize() {
  ATH_MSG_DEBUG( " initialize " );
  ATH_CHECK( AthConstConverter::initialize() );
  ATH_CHECK( m_tool.retrieve() );
  return StatusCode::SUCCESS;
}

StatusCode 
RpcPadContByteStreamCnv::createRepConst(DataObject* pObj, IOpaqueAddress*& pAddr) const {
  RpcPadContainer* cont = nullptr;
  SG::fromStorable(pObj, cont ); 
  if(!cont) {
    ATH_MSG_ERROR( " Can not cast to RpcPadContainer " );
    return StatusCode::FAILURE;    
  } 

  std::string nm = pObj->registry()->name(); 
  pAddr = new ByteStreamAddress(classID(),nm,""); 
  return m_tool->convert(cont);
}
