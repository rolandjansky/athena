/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonByteStream/RpcPadContByteStreamCnv.h"
#include "MuonRPC_CnvTools/IRPC_RDOtoByteStreamTool.h"

#include "ByteStreamCnvSvcBase/ByteStreamCnvSvcBase.h" 
#include "ByteStreamCnvSvcBase/ByteStreamAddress.h" 
#include "ByteStreamData/RawEvent.h" 

//using namespace EventFormat::RawMemory ; 

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IRegistry.h"

#include "MuonRDO/RpcPadContainer.h"
#include "MuonRDO/RpcSectorLogicContainer.h"
//#include "MuonRDO/RpcPadIdHash.h"
#include "RPCcablingInterface/RpcPadIdHash.h"

#include "MuonIdHelpers/RpcIdHelper.h"
#include "RPCcablingInterface/IRPCcablingServerSvc.h"

#include "StoreGate/StoreGateSvc.h"

// Tool 
#include "GaudiKernel/IToolSvc.h"

#include <sstream>
#include <map> 

RpcPadContByteStreamCnv::RpcPadContByteStreamCnv(ISvcLocator* svcloc) :
    Converter(storageType(), classID(),svcloc),
    m_tool("Muon::RpcPadContByteStreamTool"),
    m_byteStreamEventAccess("ByteStreamCnvSvc", "RpcPadContByteStreamCnv"),
    m_storeGate("StoreGateSvc", "RpcPadContByteStreamCnv")
{}

const CLID& RpcPadContByteStreamCnv::classID(){
return ClassID_traits<RpcPadContainer>::ID() ;
}

long RpcPadContByteStreamCnv::storageType(){
  return ByteStreamAddress::storageType();
}

StatusCode RpcPadContByteStreamCnv::initialize() {
      MsgStream log(msgSvc(), "RpcPadContByteStreamCnv");
      log << MSG::DEBUG<< " initialize " <<endmsg; 

    // initialize base class
      StatusCode sc = Converter::initialize(); 
      if (StatusCode::SUCCESS != sc) 
          return sc; 

    // get ByteStreamEventAccess interface
      if (m_byteStreamEventAccess.retrieve().isFailure())
      {
          log << MSG::ERROR << " Can't get ByteStreamEventAccess interface" << endmsg;
          return StatusCode::FAILURE;
      }

    // retrieve Tool
      if(m_tool.retrieve().isFailure())
      {
          log << MSG::ERROR << " Can't get ByteStreamTool " << endmsg;
          return StatusCode::FAILURE;
      }

      if(m_storeGate.retrieve().isFailure())
      {
          log << MSG::ERROR << " Can't get StoreGateSvc" << endmsg;
          return StatusCode::FAILURE;
      }

      return StatusCode::SUCCESS;
}

StatusCode 
RpcPadContByteStreamCnv::createRep(DataObject* pObj, IOpaqueAddress*& pAddr) {
   // convert Rpc pads in the container into ByteStream

   MsgStream log(msgSvc(), "RpcPadContByteStreamCnv");

   RawEventWrite* re = m_byteStreamEventAccess->getRawEvent();  

   RpcPadContainer* cont=NULL ; 
   StoreGateSvc::fromStorable(pObj, cont ); 
   if(!cont) {
     log << MSG::ERROR << " Can not cast to RpcPadContainer " << endmsg ; 
     return StatusCode::FAILURE;    
   } 

   std::string nm = pObj->registry()->name(); 

   ByteStreamAddress* addr = new ByteStreamAddress(classID(),nm,""); 

   pAddr = addr; 

   return m_tool->convert(cont, re, log); 

}
