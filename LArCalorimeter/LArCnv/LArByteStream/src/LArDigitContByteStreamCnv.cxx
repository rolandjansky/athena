/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArByteStream/LArDigitContByteStreamCnv.h"
#include "LArByteStream/LArRawDataContByteStreamTool.h"

#include "CaloIdentifier/CaloGain.h"

#include "ByteStreamCnvSvc/ByteStreamCnvSvc.h"
#include "ByteStreamCnvSvcBase/ByteStreamCnvSvcBase.h" 
#include "ByteStreamCnvSvcBase/ByteStreamAddress.h" 
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h" 
#include "ByteStreamData/RawEvent.h" 

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IRegistry.h"

//#include "LArRawEvent/LArRawChannelContainer.h"
#include "LArRawEvent/LArDigitContainer.h"

#include "StoreGate/StoreGateSvc.h"
#include "CLIDSvc/CLASS_DEF.h"

// Tool 
#include "GaudiKernel/IToolSvc.h"

//STL-Stuff
#include <map> 
#include <iostream>


LArDigitContByteStreamCnv::LArDigitContByteStreamCnv(ISvcLocator* svcloc) :
  Converter(ByteStream_StorageType, classID(),svcloc), m_log(NULL),m_tool(NULL),m_ByteStreamEventAccess(NULL),m_rdpSvc(NULL),m_storeGate(NULL){}
    
LArDigitContByteStreamCnv::~LArDigitContByteStreamCnv() {
 if (m_log) 
    delete m_log;
}

const CLID& LArDigitContByteStreamCnv::classID(){
  return ClassID_traits<LArDigitContainer>::ID() ;
}


StatusCode
LArDigitContByteStreamCnv::initialize()
{
   StatusCode sc = Converter::initialize(); 
   if(StatusCode::SUCCESS!=sc) 
   { 
    return sc; 
   } 
   m_log = new MsgStream(msgSvc(),"LArDigitBSCnv");
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
      (*m_log) <<MSG::ERROR<< "  LArDigitContByteStreamCnv: Can't cast to  ByteStreamCnvSvc " <<endmsg; 
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
      (*m_log) <<MSG::ERROR<< "  LArDigitContByteStreamCnv: Can't cast to  ByteStreamInputSvc " <<endmsg; 
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
LArDigitContByteStreamCnv::createObj(IOpaqueAddress* pAddr, DataObject*& pObj) 
{//Convert Digits from ByteStream to StoreGate
  (*m_log) << MSG::VERBOSE << "Executing CreateObj method for LArDigitContainer " << endmsg;
  

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

  // Convert the RawEvent to  LArDigitContainer
  (*m_log) << MSG::DEBUG << "Converting LArDigits (from ByteStream). key=" << key << " ,gain=" << gain << endmsg; 
  LArDigitContainer *DigitContainer=new LArDigitContainer;
  StatusCode sc=m_tool->convert(re,DigitContainer,gain);
  if (sc!=StatusCode::SUCCESS)
    (*m_log) << MSG::WARNING << "Conversion tool returned an error. LArDigitContainer might be empty." << endmsg;
    
  pObj = SG::asStorable(DigitContainer) ;

  return StatusCode::SUCCESS;
}

 StatusCode 
LArDigitContByteStreamCnv::createRep(DataObject* pObj, IOpaqueAddress*& pAddr) 
{// convert LArDigits from StoreGate into ByteStream
  (*m_log) << MSG::VERBOSE << "Execute CreateRep method of LArDigitContainer " << endmsg;
  StatusCode sc;

  // Get the already existing part of the RawEvent 
  //RawEvent* re = m_ByteStreamEventAccess->getRawEvent();
  // Get Full Event Assembler
  FullEventAssembler<Hid2RESrcID> *fea = 0;
  std::string key("LAr");
  sc=m_ByteStreamEventAccess->getFullEventAssembler(fea,key);
  if (sc.isFailure())
    {(*m_log) << MSG::ERROR << "Cannot get full event assember with key \"LAr\" from ByteStreamEventAccess." << endmsg;
     return sc;
    }
//  fea=new  FullEventAssembler<Hid2RESrcID>;
  if (!fea->idMap().isInitialized()) 
  {
   sc = fea->idMap().initialize(); 
   if (sc.isFailure())
    {(*m_log) << MSG::ERROR << "Cannot initialize Hid2RESrcID " << endmsg;
     return sc;
    }
  }
  
  LArDigitContainer* DigitContainer=0;
  ATH_CHECK( m_storeGate->fromStorable(pObj, DigitContainer) );
  if(!DigitContainer){
     (*m_log) << MSG::ERROR << "Cannot get LArDigitContainer for DataObject. Key=" << pObj->registry()->name() << endmsg ;
     return StatusCode::FAILURE;    
  }
   std::string nm = pObj->registry()->name(); 
   ByteStreamAddress* addr = new
       ByteStreamAddress(classID(),nm,""); 

   pAddr = addr; 

   //(*m_log) << MSG::VERBOSE << "Start conversion of LArDigitContainer requested. Writing channels and digits. " << endmsg;
   //return m_tool->convert(ChannelContainer, DigitContainer, DigitContainerArray, re, log); 
   sc=m_tool->WriteLArDigits(DigitContainer, fea);
   if (sc.isFailure())
     return sc;
   //fea->fill(re,log);
   //delete fea;
   return StatusCode::SUCCESS;
}
