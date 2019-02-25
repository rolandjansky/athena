/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArByteStream/LArFebHeaderContByteStreamCnv.h"
#include "LArByteStream/LArRawDataContByteStreamTool.h"

#include "CaloIdentifier/CaloGain.h"

#include "ByteStreamCnvSvc/ByteStreamCnvSvc.h"
#include "ByteStreamCnvSvcBase/ByteStreamCnvSvcBase.h" 
#include "ByteStreamCnvSvcBase/ByteStreamAddress.h" 
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h" 
#include "ByteStreamData/RawEvent.h" 

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IRegistry.h"

//#include "LArRawEvent/LArRawChannelContainer.h"
#include "LArRawEvent/LArFebHeaderContainer.h"

#include "StoreGate/StoreGateSvc.h"
#include "AthenaKernel/CLASS_DEF.h"

// Tool 
#include "GaudiKernel/IToolSvc.h"

//STL-Stuff
#include <map> 
#include <iostream>


LArFebHeaderContByteStreamCnv::LArFebHeaderContByteStreamCnv(ISvcLocator* svcloc) :
  Converter(storageType(), classID(),svcloc),m_tool(NULL),m_ByteStreamEventAccess(NULL),m_rdpSvc(NULL),m_storeGate(NULL){}

const CLID& LArFebHeaderContByteStreamCnv::classID(){
  return ClassID_traits<LArFebHeaderContainer>::ID() ;
}


StatusCode
LArFebHeaderContByteStreamCnv::initialize()
{
   StatusCode sc = Converter::initialize(); 
   if(StatusCode::SUCCESS!=sc) 
   { 
    return sc; 
   } 
   MsgStream log(msgSvc(), "LArFebHeaderContByteStreamCnv");
   log << MSG::DEBUG<< " initialize " <<endmsg; 
   IService* svc;
  //Get ByteStreamCnvSvc
  if(StatusCode::SUCCESS != serviceLocator()->getService("ByteStreamCnvSvc",svc)){
    log << MSG::ERROR << " Can't get ByteStreamEventAccess interface " << endmsg;
    return StatusCode::FAILURE;
  }
  m_ByteStreamEventAccess=dynamic_cast<ByteStreamCnvSvc*>(svc);
  if (m_ByteStreamEventAccess==NULL)
    {
      log<<MSG::ERROR<< "  LArFebHeaderContByteStreamCnv: Can't cast to  ByteStreamCnvSvc " <<endmsg; 
      return StatusCode::FAILURE ;
    }

  //Get ByteStreamInputSvc (only necessary for reading of digits, not for writing and for channels)
  
  if(StatusCode::SUCCESS != serviceLocator()->getService("ROBDataProviderSvc",svc)){
    log << MSG::WARNING << " Can't get ByteStreamInputSvc interface Reading of ByteStream Data not possible. " << endmsg;
    m_rdpSvc=0;
  }
  else {
    m_rdpSvc=dynamic_cast<IROBDataProviderSvc*>(svc);
    if(m_rdpSvc == 0 ) {
      log<<MSG::ERROR<< "  LArFebHeaderContByteStreamCnv: Can't cast to  ByteStreamInputSvc " <<endmsg; 
      return StatusCode::FAILURE;
    }
  }

  // retrieve ToolSvc
  IToolSvc* toolSvc;

  if(StatusCode::SUCCESS != service("ToolSvc",toolSvc)){
    log << MSG::ERROR << " Can't get ToolSvc " << endmsg;
    return StatusCode::FAILURE;
  }

  //Get LArByteStreamTool
  std::string toolType = "LArRawDataContByteStreamTool" ; 
  if(StatusCode::SUCCESS !=toolSvc->retrieveTool(toolType,m_tool))
  {
    log << MSG::ERROR << " Can't get LArRawDataByteStreamTool " << endmsg;
    return StatusCode::FAILURE;
  }

  //Get StoreGateSvc
  return service("StoreGateSvc", m_storeGate); 

}

StatusCode LArFebHeaderContByteStreamCnv::createObj(IOpaqueAddress* pAddr, DataObject*& pObj) 
{//Convert Digits from ByteStream to StoreGate
  MsgStream log(msgSvc(), "LArFebHeaderContByteStreamCnv");
  log << MSG::VERBOSE << "Executing CreateObj method for LArFebHeaderContainer " << endmsg;
  

  if (!m_rdpSvc)
    {log << MSG::ERROR << " ROBDataProviderSvc not loaded. Can't read ByteStream." << endmsg;
     return StatusCode::FAILURE;
    }
  ByteStreamAddress *pRE_Addr;
  pRE_Addr = dynamic_cast<ByteStreamAddress*>(pAddr); //Cast from OpaqueAddress to ByteStreamAddress
  if (!pRE_Addr)
    {log << MSG::ERROR << "dynamic_cast of OpaqueAdress to ByteStreamAddress failed!" << endmsg;
     return StatusCode::FAILURE;
    }

  const RawEvent* re = m_rdpSvc->getEvent();
  if (!re)
    {log << MSG::ERROR << "Could not get raw event from ByteStreamInputSvc" << endmsg;
     return StatusCode::FAILURE;
    }

  // Convert the RawEvent to  LArFebHeaderContainer
  log << MSG::DEBUG << "Converting LArFebHeaders (from ByteStream)." << endmsg;
  LArFebHeaderContainer *febHeaderContainer=new LArFebHeaderContainer;
  StatusCode sc=m_tool->convert(re,febHeaderContainer,(CaloGain::CaloGain)0);
  if (sc!=StatusCode::SUCCESS)
    log << MSG::WARNING << "Conversion tool returned an error. LArFebHeaderContainer might be empty." << endmsg;
    
  pObj = SG::asStorable(febHeaderContainer) ;

  return StatusCode::SUCCESS;
}

StatusCode LArFebHeaderContByteStreamCnv::createRep(DataObject* /*pObj*/, IOpaqueAddress*& /*pAddr*/) 
{// convert LArFebHeaders from StoreGate into ByteStream
  MsgStream log(msgSvc(), "LArFebHeaderContByteStreamCnv");
  log<< MSG::ERROR << "CreateRep method of LArFebHeaderContainer not implemented!" << endmsg;
  return StatusCode::SUCCESS;
}
