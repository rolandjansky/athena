/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArByteStream/LArCalibDigitContByteStreamCnv.h"
#include "LArByteStream/LArRawDataContByteStreamTool.h"

#include "ByteStreamCnvSvc/ByteStreamCnvSvc.h"
#include "ByteStreamCnvSvcBase/ByteStreamCnvSvcBase.h" 
#include "ByteStreamCnvSvcBase/ByteStreamAddress.h" 
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h" 
#include "ByteStreamData/RawEvent.h" 

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IRegistry.h"

#include "LArRawEvent/LArCalibDigitContainer.h"

#include "StoreGate/StoreGateSvc.h"
#include "AthenaKernel/CLASS_DEF.h"

// Tool 
#include "GaudiKernel/IToolSvc.h"

//STL-Stuff
#include <map> 
#include <iostream>


LArCalibDigitContByteStreamCnv::LArCalibDigitContByteStreamCnv(ISvcLocator* svcloc) :
    Converter(storageType(), classID(),svcloc),
    m_log(NULL),m_tool(NULL),m_ByteStreamEventAccess(NULL),m_rdpSvc(NULL),m_storeGate(NULL) {}

LArCalibDigitContByteStreamCnv::~LArCalibDigitContByteStreamCnv() {
 if (m_log) 
    delete m_log;
}

const CLID& LArCalibDigitContByteStreamCnv::classID(){
  return ClassID_traits<LArCalibDigitContainer>::ID() ;
}


StatusCode
LArCalibDigitContByteStreamCnv::initialize()
{
   StatusCode sc = Converter::initialize(); 
   if(StatusCode::SUCCESS!=sc) 
   { 
    return sc; 
   } 
   m_log = new MsgStream(msgSvc(),"LArCalibDigitBSCnv");
   (*m_log) << MSG::DEBUG<< " initialize " <<endmsg; 

   IService* svc;
  //Get ByteStreamCnvSvc
  if(StatusCode::SUCCESS != serviceLocator()->getService("ByteStreamCnvSvc",svc)){
    (*m_log) << MSG::ERROR << " Can't get ByteStreamEventAccess interface " << endmsg;
    return StatusCode::FAILURE;
  }
  m_ByteStreamEventAccess=dynamic_cast<ByteStreamCnvSvc*>(svc);
  if (m_ByteStreamEventAccess==NULL)
    {
      (*m_log) <<MSG::ERROR<< "  LArCalibDigitContByteStreamCnv: Can't cast to  ByteStreamCnvSvc " <<endmsg; 
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
      (*m_log) <<MSG::ERROR<< "Can't cast to  ByteStreamInputSvc " <<endmsg; 
      return StatusCode::FAILURE;
    }
  }

  // retrieve ToolSvc
  IToolSvc* toolSvc=0;
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
LArCalibDigitContByteStreamCnv::createObj(IOpaqueAddress* pAddr, DataObject*& pObj) 
{//Convert Digits from ByteStream to StoreGate
  (*m_log) << MSG::VERBOSE << "Executing CreateObj method for LArCalibDigitContainer " << endmsg;
  
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
  // get gain an pass to convert function.
  // get gain and pass to convert function.
  CaloGain::CaloGain gain=CaloGain::LARNGAIN; //At this place, LARNGAINS means Automatic gain.
  if (key=="HIGH")
    gain=CaloGain::LARHIGHGAIN;
  else if (key=="MEDIUM")
    gain=CaloGain::LARMEDIUMGAIN;
  else if (key=="LOW")
    gain=CaloGain::LARLOWGAIN;
  // Convert the RawEvent to  LArCalibDigitContainer
  (*m_log) << MSG::DEBUG << "Converting LArCalibDigits (from ByteStream). key=" << key << " ,gain=" << gain << endmsg;
 
  LArCalibDigitContainer *CalibDigitContainer=new LArCalibDigitContainer;
  StatusCode sc=m_tool->convert(re,CalibDigitContainer,gain);
  if (sc!=StatusCode::SUCCESS)
    (*m_log) << MSG::WARNING << "Conversion tool returned an error. LArCalibDigitContainer might be empty." << endmsg;
    
  pObj = SG::asStorable(CalibDigitContainer) ;
  return StatusCode::SUCCESS;
}

 StatusCode 
LArCalibDigitContByteStreamCnv::createRep(DataObject* pObj, IOpaqueAddress*& pAddr) 
{// convert LArDigits from StoreGate into ByteStream
  (*m_log) << MSG::VERBOSE << "Execute CreateRep method of LArCalibDigitContainer " << endmsg;
  StatusCode sc;
  // Get Full Event Assembler
  FullEventAssembler<Hid2RESrcID> *fea = 0;
  std::string key("LAr");
  sc=m_ByteStreamEventAccess->getFullEventAssembler(fea,key);
  if (sc.isFailure())
    {(*m_log) << MSG::ERROR << "Cannot get full event assember with key \"LAr\" from ByteStreamEventAccess." << endmsg;
     return sc;
    }

  if (!fea->idMap().isInitialized())
  {
   sc = fea->idMap().initialize();
   if (sc.isFailure())
    {(*m_log) << MSG::ERROR << "Cannot initialize Hid2RESrcID " << endmsg;
     return sc;
    }
  }



  LArCalibDigitContainer* CalibDigitContainer=0;
  ATH_CHECK( m_storeGate->fromStorable(pObj, CalibDigitContainer) );
  if(!CalibDigitContainer){
     (*m_log) << MSG::ERROR << "Cannot get LArCalibDigitContainer for DataObject. Key=" << pObj->registry()->name() << endmsg ;
     return StatusCode::FAILURE;    
  }
   std::string nm = pObj->registry()->name(); 
   ByteStreamAddress* addr = new
       ByteStreamAddress(classID(),nm,""); 

   pAddr = addr; 

   sc=m_tool->WriteLArCalibDigits(CalibDigitContainer, fea);
   if (sc.isFailure())
     return sc;

   return StatusCode::SUCCESS;
}
