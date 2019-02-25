// -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHLTResultByteStream/HLTResultByteStreamCnv.h"
#include "TrigHLTResultByteStream/HLTResultByteStreamTool.h"

#include "ByteStreamData/ROBData.h"
#include "ByteStreamCnvSvcBase/ByteStreamCnvSvcBase.h"
#include "ByteStreamCnvSvcBase/ByteStreamAddress.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IRegistry.h"

#include "TrigSteeringEvent/HLTResult.h"

#include "StoreGate/StoreGate.h"
#include "AthenaKernel/CLASS_DEF.h"

// Tool
#include "GaudiKernel/IToolSvc.h"

#include <map>

/**
    Constructor
*/

HLT::HLTResultByteStreamCnv::HLTResultByteStreamCnv(ISvcLocator* svcloc) :
  Converter(storageType(), classID(),svcloc),
  m_log(0),
  m_tool("HLT::HLTResultByteStreamTool"),
  m_ByteStreamEventAccess("ByteStreamCnvSvc", "HLTResultByteStreamCnv"),
  m_robDataProvider("ROBDataProviderSvc", "HLTResultByteStreamCnv")
{ }

/**
   CLID
*/
const CLID& HLT::HLTResultByteStreamCnv::classID()
{
  return ClassID_traits<HLT::HLTResult>::ID() ;
}

long HLT::HLTResultByteStreamCnv::storageType()
{
  return ByteStreamAddress::storageType();
}

StatusCode  HLT::HLTResultByteStreamCnv::finalize()
{
  delete m_log;
  return StatusCode::SUCCESS;
}


/**
    Init method gets all necessary services etc.
*/
StatusCode HLT::HLTResultByteStreamCnv::initialize()
{
   StatusCode sc = Converter::initialize();
   if(StatusCode::SUCCESS!=sc)
   {
    return sc;
   }

   m_log = new MsgStream(msgSvc(), "HLTResultByteStreamCnv");

   (*m_log) << MSG::DEBUG << "HLTResultByteStreamCnv in initialize() " <<endmsg;

   //Get ByteStreamCnvSvc
   if ( m_ByteStreamEventAccess.retrieve().isFailure() ) {
     (*m_log) << MSG::FATAL << "failed to retrieve ByteStreamEventAccess service: "
	      << m_ByteStreamEventAccess << endmsg;
     return  StatusCode::FAILURE;
   } else {
     (*m_log) << MSG::DEBUG << "successfully retrieved ByteStreamEventAccess service: "
	      << m_ByteStreamEventAccess << endmsg;
   }
//    if(StatusCode::SUCCESS != service("ByteStreamCnvSvc",m_ByteStreamEventAccess)){
//      (*m_log) << MSG::ERROR << " Can't get ByteStreamEventAccess interface " << endmsg;
//      return StatusCode::FAILURE;
//    }



   // Retrieve Tool
   //   IToolSvc* toolSvc;
   //   if(StatusCode::SUCCESS != service("ToolSvc",toolSvc)){
   //     (*m_log) << MSG::ERROR << " Can't get ToolSvc " << endmsg;
   //     return StatusCode::FAILURE;
   //   }

//    std::string toolType = "HLT::HLTResultByteStreamTool" ;
//    if(StatusCode::SUCCESS !=toolSvc->retrieveTool(toolType,m_tool))
//    {
//      (*m_log) << MSG::ERROR << " Can't get ByteStreamTool " << endmsg;
//      return StatusCode::FAILURE;
//    }
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

//    IService* robSvc ;
//    sc = serviceLocator()->getService("ROBDataProviderSvc", robSvc);
//    if(sc != StatusCode::SUCCESS ) {
//      (*m_log)<<MSG::ERROR << " Cant get ROBDataProviderSvc " <<endmsg;

//      //DBG:
//      //     return sc ;
//      return StatusCode::SUCCESS;
//    }

//    m_robDataProvider = dynamic_cast<IROBDataProviderSvc*> (robSvc);
//    if(m_robDataProvider == 0 ) {
//      (*m_log)<<MSG::ERROR<< " Cant cast to ROBDataProviderSvc " <<endmsg;
//      // return StatusCode::FAILURE ;
//    }

   return StatusCode::SUCCESS;
}

/**
   createObj should create the RDO from bytestream.
 */
StatusCode HLT::HLTResultByteStreamCnv::createObj(IOpaqueAddress* pAddr, DataObject*& pObj)
{
  HLTResult* result = new HLTResult();

  ByteStreamAddress *pBS_Addr;
  pBS_Addr = dynamic_cast<ByteStreamAddress*>(pAddr);
  if(!pBS_Addr) {
    (*m_log) << MSG::ERROR << " Can not cast to ByteStreamAddress " << endmsg ;
    return StatusCode::FAILURE;
  }

  const std::string nm = *(pBS_Addr->par()) ;
  std::vector<ROBData> vRobData;

  StatusCode sc = m_tool->convert(*m_robDataProvider, result, nm);
  if (sc != StatusCode::SUCCESS) {
    (*m_log) << MSG::ERROR << "Failed to create object " << nm << endmsg;
    return sc;
  }

  pObj = StoreGateSvc::asStorable(result);
  return sc;
}

/**
   createRep should create the bytestream from RDOs.
 */
StatusCode HLT::HLTResultByteStreamCnv::createRep(DataObject* pObj, IOpaqueAddress*& pAddr)
{

   HLTResult* result = 0;
   StoreGateSvc::fromStorable(pObj, result);

   if (!result) {
     (*m_log) << MSG::ERROR << " Cannot cast to HLTResult " << endmsg ;
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
