/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TRTRawContByteStreamCnv.h"

#include "ByteStreamCnvSvcBase/IByteStreamEventAccess.h"
#include "ByteStreamData/RawEvent.h" 
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/MsgStream.h"

// ------------------------------------------------------
// constructor

TRTRawContByteStreamCnv::TRTRawContByteStreamCnv(ISvcLocator* svcloc) :
    Converter(storageType(), classID(),svcloc),
    m_tool                 ("TRTRawContByteStreamTool"),                  // init tool handles
    m_byteStreamEventAccess("ByteStreamCnvSvc","TRTRawContByteStreamCnv") // init service handle
{}
 
// ------------------------------------------------------
// initialize

StatusCode
TRTRawContByteStreamCnv::initialize()
{
   StatusCode sc = Converter::initialize(); 
   if(StatusCode::SUCCESS!=sc) return sc; 

   MsgStream log(msgSvc(), "TRTRawContByteStreamCnv");
   log << MSG::DEBUG<< " initialize " <<endmsg; 

   // Retrieve ByteStreamCnvSvc
   if (m_byteStreamEventAccess.retrieve().isFailure()) {
     log << MSG::FATAL << "Failed to retrieve service " << m_byteStreamEventAccess << endmsg;
     return StatusCode::FAILURE;
   } else 
     log << MSG::INFO << "Retrieved service " << m_byteStreamEventAccess << endmsg;

   // Retrieve byte stream tool
   if (m_tool.retrieve().isFailure()) {
     log << MSG::FATAL << "Failed to retrieve tool " << m_tool << endmsg;
     return StatusCode::FAILURE;
   } else 
     log << MSG::INFO << "Retrieved tool " << m_tool << endmsg;

   log << MSG::INFO << "Leaving TRTRawContByteStreamCnv::initialize()" << endmsg;
   return StatusCode::SUCCESS; 

}

// ------------------------------------------------------
// convert TRT Raw Data in the container into ByteStream

StatusCode 
TRTRawContByteStreamCnv::createRep(DataObject* pObj, IOpaqueAddress*& pAddr) 
{
  // message stream
  MsgStream log(msgSvc(), "TRTRawContByteStreamCnv");
  
  // get RawEvent pointer
  RawEventWrite* re = m_byteStreamEventAccess->getRawEvent(); 
  
  // get IDC for TRT Raw Data
  TRT_RDO_Container* cont=0; 
  StoreGateSvc::fromStorable(pObj, cont); 
  if(!cont){
    log << MSG::ERROR << " Can not cast to TRTRawContainer " << endmsg ; 
    return StatusCode::FAILURE;    
  } 
  
  // set up the IOpaqueAddress for Storegate
  std::string nm = pObj->registry()->name(); 
  pAddr = new ByteStreamAddress(classID(),nm,""); 

  // now use the tool to do the conversion
  StatusCode sc = m_tool->convert(cont, re );
  if(sc.isFailure()){
    log << MSG::ERROR
	<< " Could not convert rdo with TRTRawContByteStreamTool " << endmsg;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS ;
}
