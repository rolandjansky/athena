/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "ByteStreamData/ROBData.h"
#include "ByteStreamCnvSvcBase/ByteStreamCnvSvcBase.h"
#include "ByteStreamCnvSvcBase/ByteStreamAddress.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IRegistry.h"

#include "TrigFTKByteStream/TrigFTKByteStreamCnv.h"

#include "StoreGate/StoreGate.h"
//#include "CLIDSvc/CLASS_DEF.h"

// Tool
#include "GaudiKernel/IToolSvc.h"

//------------------------------------------------------------------------------
FTK::TrigFTKByteStreamCnv::TrigFTKByteStreamCnv(ISvcLocator* svcloc) :
  Converter(storageType(), classID(),svcloc),
  m_log(0),
  m_tool("FTK::TrigFTKByteStreamTool"),
  m_ByteStreamEventAccess("ByteStreamCnvSvc", "TrigFTKByteStreamCnv"),
  m_robDataProvider("ROBDataProviderSvc", "TrigFTKByteStreamCnv")
{ }

FTK::TrigFTKByteStreamCnv::~TrigFTKByteStreamCnv() {
  delete m_log;
}

//------------------------------------------------------------------------------
const CLID& FTK::TrigFTKByteStreamCnv::classID()
{
  return ClassID_traits<FTK_RawTrackContainer>::ID() ;
}

//------------------------------------------------------------------------------
StatusCode  FTK::TrigFTKByteStreamCnv::finalize()
{ 
  return StatusCode::SUCCESS;
}

//------------------------------------------------------------------------------
StatusCode FTK::TrigFTKByteStreamCnv::initialize()
{
   StatusCode sc = Converter::initialize();
   if(StatusCode::SUCCESS!=sc)
   {
    return sc;
   }

   m_log = new MsgStream(msgSvc(), "TrigFTKByteStreamCnv");
   (*m_log) << MSG::DEBUG << "TrigFTKByteStreamCnv in initialize() " <<endmsg;

   //Get ByteStreamCnvSvc
   if ( m_ByteStreamEventAccess.retrieve().isFailure() ) {
     (*m_log) << MSG::FATAL << "failed to retrieve ByteStreamEventAccess service: "
        << m_ByteStreamEventAccess << endmsg;
     return  StatusCode::FAILURE;
   } else {
     (*m_log) << MSG::DEBUG << "successfully retrieved ByteStreamEventAccess service: "
        << m_ByteStreamEventAccess << endmsg;
   }

   // get the converter tool:
   if ( m_tool.retrieve().isFailure() ) {
     (*m_log) << MSG::FATAL << "failed to retrieve converter tool : "
        << m_tool << endmsg;
     return  StatusCode::FAILURE;
   } else {
     (*m_log) << MSG::DEBUG << "successfully retrieved converter tool: "
        << m_tool << endmsg;
   }


   // Get ROBDataProvider
   if (m_robDataProvider.retrieve().isFailure()) {
    (*m_log) << MSG::FATAL << "failed to retrieve ROBDataProviderSvc service: "
       << m_robDataProvider << endmsg;
    return StatusCode::FAILURE;
   }

   (*m_log) << MSG::DEBUG << "successfully retrieved ROBDataProviderSvc service: "
      << m_robDataProvider << endmsg;

   return StatusCode::SUCCESS;
}

//------------------------------------------------------------------------------
StatusCode FTK::TrigFTKByteStreamCnv::createObj(IOpaqueAddress* pAddr, DataObject*& pObj)
{
  FTK_RawTrackContainer* result(0);

  ByteStreamAddress *pBS_Addr;
  pBS_Addr = dynamic_cast<ByteStreamAddress*>(pAddr);
  if(!pBS_Addr) {
    (*m_log) << MSG::ERROR << " Can not cast to ByteStreamAddress " << endmsg ;
    return StatusCode::FAILURE;
  }

  const std::string object_name = *(pBS_Addr->par()) ; //< Params for tool converter
  std::vector<ROBData> vRobData;



  StatusCode sc = m_tool->convert(*m_robDataProvider, result, object_name); //<Actual converstion
  if (sc != StatusCode::SUCCESS) {
    (*m_log) << MSG::ERROR << "Failed to convert object " << object_name << endmsg;
    return sc;
  }

  pObj = StoreGateSvc::asStorable(result);
  return sc;
}

//------------------------------------------------------------------------------
StatusCode FTK::TrigFTKByteStreamCnv::createRep(DataObject* pObj, IOpaqueAddress*& pAddr)
{

   FTK_RawTrackContainer* result = 0;
   StoreGateSvc::fromStorable(pObj, result);

   if (!result) {
     (*m_log) << MSG::ERROR << " Cannot cast to FTK_RawTrackContainer " << endmsg ;
     return StatusCode::FAILURE;
   }

   std::string nm = pObj->registry()->name();

   RawEventWrite* re = m_ByteStreamEventAccess->getRawEvent();
  // Convert to ByteStream
   StatusCode sc = m_tool->convert(result, re, nm);

   ByteStreamAddress* addr = new ByteStreamAddress(classID(), nm, "");
   pAddr = addr;

   return sc;

}
