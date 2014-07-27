/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TileByteStream/TileL2ContByteStreamCnv.h"
#include "TileByteStream/TileL2ContByteStreamTool.h"
#include "TileByteStream/TileROD_Decoder.h"

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
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IToolSvc.h"

#include "TileEvent/TileContainer.h"
#include "TileL2Algs/TileL2Builder.h"

#include "StoreGate/StoreGate.h"
#include "StoreGate/StoreClearedIncident.h"
#include "CLIDSvc/CLASS_DEF.h"

#include <string> 


TileL2ContByteStreamCnv::TileL2ContByteStreamCnv(ISvcLocator* svcloc) :
    Converter(ByteStream_StorageType, classID(),svcloc),
    m_tool(0),
    m_decoder(0),
    m_ByteStreamEventAccess(0),
    m_container(0),
    m_storeGate(0),
    m_rdpSvc(0)
{
}

const CLID& TileL2ContByteStreamCnv::classID(){
return ClassID_traits<TileL2Container>::ID() ;
}


StatusCode
TileL2ContByteStreamCnv::initialize()
{
  StatusCode sc = Converter::initialize(); 
  if (sc.isFailure()) {
    return sc; 
  } 

  MsgStream log(msgSvc(), "TileL2ContByteStreamCnv");
  log << MSG::DEBUG<< " initialize " <<endreq; 

  //Get ByteStreamInputSvc 
  IService* svc;
  if(StatusCode::SUCCESS != serviceLocator()->getService("ROBDataProviderSvc",svc)){
    log << MSG::WARNING << " Can't get ByteStreamInputSvc interface Reading of ByteStream Data not possible. " << endreq;
    m_rdpSvc=0;
  }
  else {
    m_rdpSvc=dynamic_cast<IROBDataProviderSvc*>(svc);
    if(m_rdpSvc == 0 ) {
      log<<MSG::ERROR<< " Can't cast to  ByteStreamInputSvc " <<endreq; 
      return StatusCode::FAILURE;
    }
  }

  // Get ByteStreamCnvSvc
  if(StatusCode::SUCCESS != serviceLocator()->getService("ByteStreamCnvSvc",svc)){
    log << MSG::ERROR << " Can't get ByteStreamEventAccess interface " << endreq;
    return StatusCode::FAILURE;
  }
  m_ByteStreamEventAccess=dynamic_cast<ByteStreamCnvSvc*>(svc);
  if (m_ByteStreamEventAccess==NULL)
    {
      log << MSG::ERROR << "  TileL2ContByteStreamCnv: Can't cast to  ByteStreamCnvSvc " << endreq; 
      return StatusCode::FAILURE ;
    }

  // retrieve Tool
  
  IToolSvc* toolSvc;
  if(StatusCode::SUCCESS != service("ToolSvc",toolSvc)){
    log << MSG::ERROR << " Can't get ToolSvc " << endreq;
    return StatusCode::FAILURE;
  }

  std::string toolType = "TileL2ContByteStreamTool" ;
  if(StatusCode::SUCCESS !=toolSvc->retrieveTool(toolType,m_tool))
  {
    log << MSG::ERROR << " Can't get ByteStreamTool " << endreq;
    return StatusCode::FAILURE;
  }

  if(StatusCode::SUCCESS !=toolSvc->retrieveTool("TileROD_Decoder",m_decoder)) {
    log << MSG::ERROR << "Can't get TileROD_Decoder"
	<< endreq;
    return StatusCode::FAILURE; 
  }

  // create empty TileL2Container and all TileL2 inside
  m_container = new TileL2Container(); 
  m_container->reserve(256);
  for(int i=0;i<256;i++) {
    int collId = m_decoder->hashFunc()->identifier(i);
    TileL2 *l2 = new TileL2(collId);
    m_container->push_back(l2);
  }

  // Register incident handler
  ServiceHandle<IIncidentSvc> incSvc("IncidentSvc",
                                     "TileL2ContByteStreamCnv");
  if ( !incSvc.retrieve().isSuccess() ) {
    log << MSG::WARNING << "Unable to retrieve the IncidentSvc" << endreq;
  }
  else {
    incSvc->addListener(this,"StoreCleared");
  }

  return service("StoreGateSvc",m_storeGate) ;
}


StatusCode
TileL2ContByteStreamCnv::createObj(IOpaqueAddress* pAddr, DataObject*& pObj) 
{
  MsgStream log(msgSvc(), "TileL2ContByteStreamCnv");
  log << MSG::DEBUG << " Executing createObj method" << endreq;

  if (!m_rdpSvc) {
    log << MSG::ERROR << " ROBDataProviderSvc not loaded. Can't read ByteStream." << endreq;
    return StatusCode::FAILURE;
  }

  ByteStreamAddress *pRE_Addr;
  pRE_Addr = dynamic_cast<ByteStreamAddress*>(pAddr); 
  if(!pRE_Addr) {
    log << MSG::ERROR << " Can not cast to ByteStreamAddress " << endreq ; 
    return StatusCode::FAILURE;    
  }

  const RawEvent* re = m_rdpSvc->getEvent();
  if (!re) {
    log << MSG::ERROR << "Could not get raw event from ByteStreamInputSvc" << endreq;
    return StatusCode::FAILURE;
  }

  StatusCode sc=m_decoder->convert(re,m_container);
  if (sc!=StatusCode::SUCCESS)
    log << MSG::WARNING << "Conversion tool returned an error. TileL2Container might be empty." << endreq;

  // new container will not own elements, i.e. TileL2 will not be deleted
  TileL2Container * new_container = new TileL2Container(*m_container);
  
  pObj = StoreGateSvc::asStorable( new_container ) ; 
  return StatusCode::SUCCESS;  
}

StatusCode 
TileL2ContByteStreamCnv::createRep(DataObject* pObj, IOpaqueAddress*& pAddr)
{
  // convert TileL2s in the container into ByteStream
  MsgStream log(msgSvc(), "TileL2ContByteStreamCnv");
  log << MSG::DEBUG << " Executing createRep method" << endreq;

  StatusCode sc;

  // get Full Event Assembler
  FullEventAssembler<TileHid2RESrcID> *fea = 0;
  std::string key("Tile");
  sc=m_ByteStreamEventAccess->getFullEventAssembler(fea,key);
  if (sc.isFailure()) {
    log << MSG::ERROR << "Cannot get full event assember with key \"Tile\" from ByteStreamEventAccess." << endreq;
    return sc;
  }

  // create TileL2Container
  TileL2Container* l2cont(0);
  StoreGateSvc::fromStorable(pObj, l2cont );
  if(!l2cont){
    log << MSG::ERROR << " Can not cast to TileL2Container " << endreq;
    return StatusCode::FAILURE;
  }

  std::string nm = pObj->registry()->name();

  ByteStreamAddress* addr = new ByteStreamAddress(classID(),nm,"");

  pAddr = addr;

  // call TileL2ContByteStreamTool
  sc=m_tool->convert(l2cont,fea);
  if (sc.isFailure())
    return sc;

  return StatusCode::SUCCESS;
}


StatusCode
TileL2ContByteStreamCnv::finalize()
{
  MsgStream log(msgSvc(), "TileL2ContByteStreamCnv");
  log << MSG::DEBUG<<" Clearing TileL2 Container " <<endreq; 

  m_container->clear();

  delete m_container; 

  return Converter::finalize(); 
}

void TileL2ContByteStreamCnv::handle(const Incident& incident)
{
  if (incident.type() == "StoreCleared")
  {
    if (const StoreClearedIncident* inc =
        dynamic_cast<const StoreClearedIncident*> (&incident))
    {
      if (inc->store() == m_storeGate) {
        TileL2Container::const_iterator collItr=m_container->begin();
        TileL2Container::const_iterator lastColl=m_container->end();

        for(; collItr!=lastColl; ++collItr) {
          const TileL2 * constColl = (*collItr);
          TileL2 * coll = (TileL2 *)constColl;
          coll->clear();
        }
      }
    }
  }
}
