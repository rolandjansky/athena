/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonByteStream/TgcRdoContByteStreamCnv.h"
#include "MuonRDO/TgcRdoContainer.h"

#include "ByteStreamCnvSvcBase/ByteStreamCnvSvcBase.h" 
#include "ByteStreamCnvSvcBase/ByteStreamAddress.h" 
#include "ByteStreamData/RawEvent.h" 

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IToolSvc.h"

#include "MuonRDO/TgcRdoIdHash.h"

#include "StoreGate/StoreGateSvc.h"

#include <sstream>
#include <inttypes.h>

const std::string const_cnvName = "TgcRdoContByteStreamCnv";


// constructor
TgcRdoContByteStreamCnv::TgcRdoContByteStreamCnv(ISvcLocator* svcloc) 
  : Converter(storageType(), classID(), svcloc),
  m_tool("Muon::TgcRdoContByteStreamTool"),
  m_byteStreamEventAccess("ByteStreamCnvSvc", const_cnvName),
  m_storeGate("StoreGateSvc", const_cnvName)
{}


// class ID
const CLID& TgcRdoContByteStreamCnv::classID()
{
  return ClassID_traits<TgcRdoContainer>::ID();
}

long TgcRdoContByteStreamCnv::storageType()
{
  return ByteStreamAddress::storageType();
}

// initialize
StatusCode TgcRdoContByteStreamCnv::initialize() 
{
      MsgStream log(msgSvc(), const_cnvName);
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

// convert TGC RDOs in the container into ByteStream
StatusCode
TgcRdoContByteStreamCnv::createRep(DataObject* pObj, IOpaqueAddress*& pAddr)
{
   MsgStream log(msgSvc(), const_cnvName);

   // get Raw Event data
   RawEventWrite* re = m_byteStreamEventAccess->getRawEvent();  

   // retrieve TGC RDO container
   TgcRdoContainer * cont(NULL); 
   StoreGateSvc::fromStorable(pObj, cont);
   if (!cont)
     {
       log << MSG::ERROR << " Can not cast to TgcRdoContainer" << endmsg; 
       return StatusCode::FAILURE;    
     } 

   // create address
   std::string nm = pObj->registry()->name(); 
   ByteStreamAddress* addr = new ByteStreamAddress(classID(),nm,""); 

   pAddr = addr; 

   // convert
   return m_tool->convert(cont, re, log); 
}
