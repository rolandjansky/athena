/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArByteStream/LArRawChannelContByteStreamCnv.h"
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

#include "LArRawEvent/LArRawChannelContainer.h"
#include "LArRawEvent/LArDigitContainer.h"

#include "StoreGate/StoreGateSvc.h"
#include "AthenaKernel/CLASS_DEF.h"

// Tool 
#include "GaudiKernel/IToolSvc.h"

//STL-Stuff
#include <map> 
#include <iostream>
#include <string>


LArRawChannelContByteStreamCnv::LArRawChannelContByteStreamCnv(ISvcLocator* svcloc) :
  Converter(storageType(), classID(),svcloc), m_log(NULL),
  m_tool(NULL),m_ByteStreamEventAccess(0),m_rdpSvc(0),m_storeGate(0),m_contSize(0)
{}

const CLID& LArRawChannelContByteStreamCnv::classID(){
  return ClassID_traits<LArRawChannelContainer>::ID() ;
}

LArRawChannelContByteStreamCnv::~LArRawChannelContByteStreamCnv() {
 if (m_log) 
    delete m_log;
}


StatusCode
LArRawChannelContByteStreamCnv::initialize()
{
   StatusCode sc = Converter::initialize(); 
   if(StatusCode::SUCCESS!=sc) 
   { 
    return sc; 
   } 
    m_log = new MsgStream(msgSvc(),"LArRawChannelBSCnv");
    (*m_log) << MSG::DEBUG<< " initialize " <<endmsg; 
   
  //Get ByteStreamCnvSvc
   IService* svc;
  if(StatusCode::SUCCESS != serviceLocator()->getService("ByteStreamCnvSvc",svc)){
    (*m_log) << MSG::ERROR << " Can't get ByteStreamEventAccess interface " << endmsg;
    return StatusCode::FAILURE;
  }
  m_ByteStreamEventAccess=dynamic_cast<ByteStreamCnvSvc*>(svc);
  if (m_ByteStreamEventAccess==NULL)
    {
      (*m_log) << MSG::ERROR<< "  LArDigitContByteStreamCnv: Can't cast to  ByteStreamCnvSvc " <<endmsg; 
      return StatusCode::FAILURE ;
    }

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
  // retrieve ConversionTool
  if(StatusCode::SUCCESS !=toolSvc->retrieveTool("LArRawDataContByteStreamTool",m_tool))
  {
    (*m_log) << MSG::ERROR << " Can't get ByteStreamTool " << endmsg;
    return StatusCode::FAILURE;
  }
  //retrieve StoreGateSvc
  return service("StoreGateSvc", m_storeGate); 

}

StatusCode
LArRawChannelContByteStreamCnv::createObj(IOpaqueAddress* pAddr, DataObject*& pObj) 
{ //Convert RawChannels from ByteStream to StoreGate
  //(*m_log) << MSG::VERBOSE << "Executing CreateObj method for LArRawChannelContainer " << endmsg;
    
  ByteStreamAddress *pRE_Addr;
  pRE_Addr = dynamic_cast<ByteStreamAddress*>(pAddr); 
  if(!pRE_Addr) {
    (*m_log) << MSG::ERROR << " Can not cast to ByteStreamAddress " << endmsg ; 
    return StatusCode::FAILURE;    
  }
  
  const RawEvent* re = m_rdpSvc->getEvent();
  if (!re)
    {(*m_log) << MSG::ERROR << "Could not get raw event from ByteStreamInputSvc" << endmsg;
     return StatusCode::FAILURE;
    }

  // create LArRawChannelContainer
  LArRawChannelContainer* channelContainer = new LArRawChannelContainer() ;
  if (m_contSize) channelContainer->reserve(m_contSize);

  StatusCode sc=m_tool->convert(re,channelContainer,CaloGain::CaloGain(0)); //Gain is a dummy variable in this case...
  if (sc!=StatusCode::SUCCESS)
    (*m_log) << MSG::WARNING << "Conversion tool returned an error. LArRawChannelContainer might be empty." << endmsg;
    
  pObj = StoreGateSvc::asStorable( channelContainer ) ; 

  if (m_log->level()<=MSG::DEBUG) {
    (*m_log) << MSG::DEBUG << "Created a LArRawChannelContainer of size" << channelContainer->size() << endmsg;
  }
  if (!m_contSize) {
    m_contSize=channelContainer->size();
    (*m_log) << MSG::DEBUG << "For the following events, we will reserve space for " 
	     << m_contSize << " LArRawchannels" << endmsg;
  }
    
  return StatusCode::SUCCESS;  
    
  
}

 StatusCode 
LArRawChannelContByteStreamCnv::createRep(DataObject* pObj, IOpaqueAddress*& pAddr) 
{ // convert LArRawChannels in the container into ByteStream
  //(*m_log) << MSG::VERBOSE << "Executing CreateRep method of LArRawChannelContainer" << endmsg;
   StatusCode sc;
   
   // Get Full Event Assembler
   FullEventAssembler<Hid2RESrcID> *fea = 0;
   sc=m_ByteStreamEventAccess->getFullEventAssembler(fea,"LAr");
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


   LArRawChannelContainer* ChannelContainer=0; 
   StoreGateSvc::fromStorable(pObj, ChannelContainer ); 
   if(!ChannelContainer){
    (*m_log) << MSG::ERROR << " Can not cast to LArRawChannelContainer " << endmsg ;
    return StatusCode::FAILURE;    
   } 

   const std::string& nm = pObj->registry()->name(); 

   ByteStreamAddress* addr = new
       ByteStreamAddress(classID(),nm,""); 

   pAddr = addr; 
   sc=m_tool->WriteLArRawChannels(ChannelContainer,fea);
   if (sc.isFailure())
     return sc;
   //fea->fill(re,log);
   return StatusCode::SUCCESS;
   
}
