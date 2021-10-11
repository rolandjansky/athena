/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonByteStream/MdtCsmContByteStreamCnv.h"
#include "MuonMDT_CnvTools/IMDT_RDOtoByteStreamTool.h"
#include "ByteStreamCnvSvcBase/ByteStreamAddress.h" 
#include "MuonRDO/MdtCsmContainer.h"
#include "AthenaBaseComps/AthCheckMacros.h"
#include "AthenaKernel/StorableConversions.h"

MdtCsmContByteStreamCnv::MdtCsmContByteStreamCnv(ISvcLocator* svcloc) :
  AthConstConverter(storageType(), classID(),svcloc, "MdtCsmContByteStreamCnv"),
    m_tool("Muon::MdtCsmContByteStreamTool")
{
}

const CLID& MdtCsmContByteStreamCnv::classID(){
  return ClassID_traits<MdtCsmContainer>::ID() ;
}

long MdtCsmContByteStreamCnv::storageType(){
  return ByteStreamAddress::storageType();
}

StatusCode MdtCsmContByteStreamCnv::initialize() {
  ATH_MSG_DEBUG( " initialize " );
  ATH_CHECK( AthConstConverter::initialize() );
  ATH_CHECK( m_tool.retrieve() );
  return StatusCode::SUCCESS;
}


StatusCode 
MdtCsmContByteStreamCnv::createRepConst(DataObject* pObj, IOpaqueAddress*& pAddr) const
{
  MdtCsmContainer* cont = nullptr;
  SG::fromStorable(pObj, cont ); 
  if(!cont) {
    ATH_MSG_ERROR( " Can not cast to MdtCsmContainer " );
    return StatusCode::FAILURE;    
  } 

  std::string nm = pObj->registry()->name(); 
  pAddr = new ByteStreamAddress(classID(),nm,""); 
  return m_tool->convert(cont, msg());
}
