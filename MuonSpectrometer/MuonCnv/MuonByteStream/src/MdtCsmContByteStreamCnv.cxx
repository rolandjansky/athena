/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonByteStream/MdtCsmContByteStreamCnv.h"
#include "MuonMDT_CnvTools/IMDT_RDOtoByteStreamTool.h"

#include "ByteStreamCnvSvcBase/ByteStreamCnvSvcBase.h" 
#include "ByteStreamCnvSvcBase/ByteStreamAddress.h" 
#include "ByteStreamData/RawEvent.h" 

//using namespace EventFormat::RawMemory ; 

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IRegistry.h"

#include "MuonRDO/MdtCsmContainer.h"
//#include "MuonRDO/MdtCsmIdHash.h"

#include "StoreGate/StoreGateSvc.h"
//#include "StoreGate/tools/ClassID_traits.h"

// Tool 
#include "GaudiKernel/IToolSvc.h"

#include <sstream>
#include <map> 

MdtCsmContByteStreamCnv::MdtCsmContByteStreamCnv(ISvcLocator* svcloc) :
    Converter(storageType(), classID(),svcloc),
    m_tool("Muon::MdtCsmContByteStreamTool"),
    m_byteStreamEventAccess("ByteStreamCnvSvc", "MdtCsmContByteStreamCnv"),
    m_storeGate("StoreGateSvc", "MdtCsmContByteStreamCnv")
{
}

const CLID& MdtCsmContByteStreamCnv::classID(){
return ClassID_traits<MdtCsmContainer>::ID() ;
}

long MdtCsmContByteStreamCnv::storageType(){
  return ByteStreamAddress::storageType();
}

StatusCode MdtCsmContByteStreamCnv::initialize() {
   MsgStream log(msgSvc(), "MdtCsmContByteStreamCnv");
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
MdtCsmContByteStreamCnv::createRep(DataObject* pObj, IOpaqueAddress*& pAddr) {
   // convert Mdt Csm in the container into ByteStream
   MsgStream log(msgSvc(), "MdtCsmContByteStreamCnv");

   RawEventWrite* re = m_byteStreamEventAccess->getRawEvent();  

   MdtCsmContainer* cont=NULL ; 
   StoreGateSvc::fromStorable(pObj, cont ); 
   if(!cont) {
     log << MSG::ERROR << " Can not cast to MdtCsmContainer " << endmsg ; 
     return StatusCode::FAILURE;    
   } 

   std::string nm = pObj->registry()->name(); 

   ByteStreamAddress* addr = new ByteStreamAddress(classID(),nm,""); 

   pAddr = addr; 

   return m_tool->convert(cont, re, log); 
}
