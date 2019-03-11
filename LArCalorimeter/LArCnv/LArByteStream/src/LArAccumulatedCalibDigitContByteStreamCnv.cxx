/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArByteStream/LArAccumulatedCalibDigitContByteStreamCnv.h"
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

#include "LArRawEvent/LArAccumulatedCalibDigitContainer.h"

#include "StoreGate/StoreGateSvc.h"
#include "AthenaKernel/CLASS_DEF.h"

// Tool 
#include "GaudiKernel/IToolSvc.h"

//STL-Stuff
#include <map> 
#include <iostream>


LArAccumulatedCalibDigitContByteStreamCnv::LArAccumulatedCalibDigitContByteStreamCnv(ISvcLocator* svcloc) :
  Converter(storageType(), classID(),svcloc),m_log(NULL),m_tool(NULL),m_ByteStreamEventAccess(NULL),m_rdpSvc(NULL),m_storeGate(NULL){}

const CLID& LArAccumulatedCalibDigitContByteStreamCnv::classID(){
  return ClassID_traits<LArAccumulatedCalibDigitContainer>::ID() ;
}

LArAccumulatedCalibDigitContByteStreamCnv::~LArAccumulatedCalibDigitContByteStreamCnv() {
  if (m_log) 
    delete m_log;
}


StatusCode
LArAccumulatedCalibDigitContByteStreamCnv::initialize()
{
   StatusCode sc = Converter::initialize(); 
   if(StatusCode::SUCCESS!=sc) 
   { 
    return sc; 
   } 
   m_log = new MsgStream(msgSvc(),"LArAccumulatedCalibDigitBSCnv");
   IService* svc;
  //Get ByteStreamCnvSvc
  if(StatusCode::SUCCESS != serviceLocator()->getService("ByteStreamCnvSvc",svc)){
    (*m_log) << MSG::ERROR << " Can't get ByteStreamEventAccess interface " << endmsg;
    return StatusCode::FAILURE;
  }
  m_ByteStreamEventAccess=dynamic_cast<ByteStreamCnvSvc*>(svc);
  if (m_ByteStreamEventAccess==NULL)
    {
      (*m_log) <<MSG::ERROR<< "  LArAccumulatedCalibDigitContByteStreamCnv: Can't cast to  ByteStreamCnvSvc " <<endmsg; 
      return StatusCode::FAILURE ;
    }

  //Get ByteStreamInputSvc (only necessary for reading of digits, not for writing and for channels)
  
  if(StatusCode::SUCCESS != serviceLocator()->getService("ROBDataProviderSvc",svc)){
    (*m_log) << MSG::WARNING << " Can't get ByteStreamInputSvc interface Reading of ByteStream Data not possible. " << endmsg;
    m_rdpSvc=0;
  }
  else {
    m_rdpSvc=dynamic_cast<IROBDataProviderSvc*>(svc);
    if(m_rdpSvc == 0 ) {
      (*m_log) <<MSG::ERROR<< "  LArAccumulatedCalibDigitContByteStreamCnv: Can't cast to  ByteStreamInputSvc " <<endmsg; 
      return StatusCode::FAILURE;
    }
  }

  // retrieve ToolSvc
  IToolSvc* toolSvc;

  if(StatusCode::SUCCESS != service("ToolSvc",toolSvc)){
    (*m_log) << MSG::ERROR << " Can't get ToolSvc " << endmsg;
    return StatusCode::FAILURE;
  }

  //Get LArByteStreamTool
  std::string toolType = "LArRawDataContByteStreamTool" ; 
  if(StatusCode::SUCCESS !=toolSvc->retrieveTool(toolType,m_tool))
  {
    (*m_log) << MSG::ERROR << " Can't get LArRawDataByteStreamTool " << endmsg;
    return StatusCode::FAILURE;
  }

  //Get StoreGateSvc
  return service("StoreGateSvc", m_storeGate); 

}

StatusCode
LArAccumulatedCalibDigitContByteStreamCnv::createObj(IOpaqueAddress* pAddr, DataObject*& pObj) 
{//Convert Accumulated Calib Digits from ByteStream to StoreGate
  MsgStream log(msgSvc(), "LArAccumulatedCalibDigitContByteStreamCnv");
  (*m_log) << MSG::VERBOSE << "Executing CreateObj method for LArAccumulatedCalibDigitContainer " << endmsg;
  

  if (!m_rdpSvc)
    {(*m_log) << MSG::ERROR << " ROBDataProviderSvc not loaded. Can't read ByteStream." << endmsg;
     return StatusCode::FAILURE;
    }
  ByteStreamAddress *pRE_Addr;
  pRE_Addr = dynamic_cast<ByteStreamAddress*>(pAddr); //Cast from OpaqueAddress to ByteStreamAddress
  if (!pRE_Addr)
    {(*m_log) << MSG::ERROR << "dynamic_cast of OpaqueAdress to ByteStreamAddress failed!" << endmsg;
     return StatusCode::FAILURE;
    }

  const RawEvent* re = m_rdpSvc->getEvent();
  if (!re)
    {(*m_log) << MSG::ERROR << "Could not get raw event from ByteStreamInputSvc" << endmsg;
     return StatusCode::FAILURE;
    }
  const std::string& key = *(pAddr->par()); // Get key used in the StoreGateSvc::retrieve function
  // get gain and pass to convert function.
  CaloGain::CaloGain gain=CaloGain::LARNGAIN; //At this place, LARNGAINS means Automatic gain.
  if (key=="HIGH")
    gain=CaloGain::LARHIGHGAIN;
  else if (key=="MEDIUM")
    gain=CaloGain::LARMEDIUMGAIN;
  else if (key=="LOW")
    gain=CaloGain::LARLOWGAIN;

  // Convert the RawEvent to  LArAccumulatedCalibDigitContainer
  (*m_log) << MSG::DEBUG << "Converting LArAccumulatedCalibDigits (from ByteStream). key=" << key << " ,gain=" << gain << endmsg; 
  LArAccumulatedCalibDigitContainer *DigitContainer=new LArAccumulatedCalibDigitContainer;
  StatusCode sc=m_tool->convert(re,DigitContainer,gain);
  if (sc!=StatusCode::SUCCESS)
    (*m_log) << MSG::WARNING << "Conversion tool returned an error. LArAccumulatedCalibDigitContainer might be empty." << endmsg;
  DigitContainer->setDelayScale(25./240.);  //FIXME should not be hardcoded! 
  pObj = SG::asStorable(DigitContainer) ;

  return StatusCode::SUCCESS;
}

// IWS 19.07.2005 
 StatusCode 
 LArAccumulatedCalibDigitContByteStreamCnv::createRep(DataObject* /*pObj*/, IOpaqueAddress*& /*pAddr*/) 
{// convert LArAccumulatedCalibDigits from StoreGate into ByteStream
  (*m_log) << MSG::ERROR << "CreateRep method of LArAccumulatedCalibDigitContainer not implemented" << endmsg;
  return StatusCode::SUCCESS;
}

