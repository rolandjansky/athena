/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TileByteStream/TileRawChannelContByteStreamCnv.h"
#include "TileByteStream/TileRawChannelContByteStreamTool.h"
#include "TileByteStream/TileROD_Decoder.h"
#include "TileByteStream/TileHid2RESrcID.h"

#include "ByteStreamCnvSvc/ByteStreamCnvSvc.h"
#include "ByteStreamCnvSvcBase/ByteStreamCnvSvcBase.h" 
#include "ByteStreamCnvSvcBase/ByteStreamAddress.h" 
#include "ByteStreamCnvSvcBase/ROBDataProviderSvc.h"
#include "ByteStreamData/RawEvent.h" 

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/ServiceHandle.h"

#include "TileEvent/TileRawChannelContainer.h"

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/StoreClearedIncident.h"
#include "CLIDSvc/CLASS_DEF.h"

#include <vector> 
#include <string> 
#include <stdint.h>


TileRawChannelContByteStreamCnv::TileRawChannelContByteStreamCnv(ISvcLocator* svcloc) :
    Converter(ByteStream_StorageType, classID(),svcloc),
    m_tool(0),
    m_ByteStreamEventAccess(0),
    m_container(0),
    m_storeGate(0),
    m_RobSvc(0),
    m_decoder(0),
    m_hid2re(0)
{
}

const CLID& TileRawChannelContByteStreamCnv::classID(){
return ClassID_traits<TileRawChannelContainer>::ID() ;
}


StatusCode
TileRawChannelContByteStreamCnv::initialize()
{
  StatusCode sc = Converter::initialize(); 
  if (sc.isFailure()) {
    return sc; 
  } 

  MsgStream log(msgSvc(), "TileRawChannelContByteStreamCnv");
  log << MSG::DEBUG<< " initialize " <<endreq; 

  StoreGateSvc* detStore;
  sc = service("DetectorStore",detStore);
  if (sc.isFailure()) {
    log << MSG::FATAL << "Unable to get pointer to Detector Store Service"
	<< endreq;
    return sc;
  }

  // Get ByteStreamCnvSvc
  IService* svc;
  if(StatusCode::SUCCESS != serviceLocator()->getService("ByteStreamCnvSvc",svc)){
    log << MSG::ERROR << " Can't get ByteStreamEventAccess interface " << endreq;
    return StatusCode::FAILURE;
  }
  m_ByteStreamEventAccess=dynamic_cast<ByteStreamCnvSvc*>(svc);
  if (m_ByteStreamEventAccess==NULL)
    {
      log << MSG::ERROR << "  TileRawChannelContByteStreamCnv: Can't cast to  ByteStreamCnvSvc " << endreq; 
      return StatusCode::FAILURE ;
    }

  // retrieve Tool

  IToolSvc* toolSvc;
  if(StatusCode::SUCCESS != service("ToolSvc",toolSvc)){
    log << MSG::ERROR << " Can't get ToolSvc " << endreq;
    return StatusCode::FAILURE;
  }
  std::string toolType = "TileRawChannelContByteStreamTool" ; 
  if(StatusCode::SUCCESS !=toolSvc->retrieveTool(toolType,m_tool))
  {
    log << MSG::ERROR << " Can't get ByteStreamTool " << endreq;
    return StatusCode::FAILURE;
  }
  
  if(StatusCode::SUCCESS !=toolSvc->retrieveTool("TileROD_Decoder",m_decoder)) {
    log << MSG::ERROR << " Can't get TileROD_Decoder " << endreq;
    return StatusCode::FAILURE;
  }

  m_hid2re = m_decoder->getHid2re();

  sc = serviceLocator()->service("ROBDataProviderSvc",m_RobSvc);
  if (sc.isFailure()) {
    log << MSG::ERROR <<" Can't get ROBDataProviderSvc " << endreq;
  }

  // create empty TileRawChannelContainer and all collections inside
  TileFragHash::TYPE type = TileFragHash::OptFilterDsp;
  TileRawChannelUnit::UNIT unit = TileRawChannelUnit::ADCcounts;
  m_container = new TileRawChannelContainer(true, type, unit); 
  m_container->addRef(); // make sure it's not deleted at the end of event
  m_decoder->PtrRChContainer( m_container );

  // Register incident handler
  ServiceHandle<IIncidentSvc> incSvc("IncidentSvc",
                                     "TileRawChannelContByteStreamCnv");
  if ( !incSvc.retrieve().isSuccess() ) {
    log << MSG::WARNING << "Unable to retrieve the IncidentSvc" << endreq;
  }
  else {
    incSvc->addListener(this,"StoreCleared");
  }

  return service("StoreGateSvc", m_storeGate);
}


StatusCode
TileRawChannelContByteStreamCnv::createObj(IOpaqueAddress* pAddr, DataObject*& pObj) 
{
  MsgStream log(msgSvc(), "TileRawChannelContByteStreamCnv");
  log << MSG::DEBUG << " Executing createObj method" << endreq;

  ByteStreamAddress *pRE_Addr;
  pRE_Addr = dynamic_cast<ByteStreamAddress*>(pAddr); 
  if(!pRE_Addr) {
    log << MSG::ERROR << " Can not cast to ByteStreamAddress " << endreq ; 
    return StatusCode::FAILURE;    
  }

  std::vector<uint32_t> robid(1); robid[0] = 0;
  std::vector<const ROBDataProviderSvc::ROBF*> robf;

  // iterate over all collections in a container and fill them
  TileRawChannelContainer::const_iterator collItr=m_container->begin();
  TileRawChannelContainer::const_iterator lastColl=m_container->end();

  for(; collItr!=lastColl; ++collItr) {

    const TileRawChannelCollection * constColl = (*collItr);
    TileRawChannelCollection * coll = (TileRawChannelCollection *)constColl;
    coll->clear();
    TileRawChannelCollection::ID collID = coll->identify();  

    // find ROB
    uint32_t newrob = m_hid2re->getRobFromFragID(collID);
    if (newrob != robid[0]) {
      robid[0] = m_hid2re->getRobFromFragID(collID);
      robf.clear();
      m_RobSvc->getROBData(robid, robf);
    }
    
    // unpack ROB data
    if (robf.size() > 0 ) {
      m_decoder->fillCollection(robf[0],*coll);
    }
  }

  log << MSG::DEBUG<<" Creating Container " << *(pRE_Addr->par()) <<endreq; 
  pObj = StoreGateSvc::asStorable( m_container ) ; 
  return StatusCode::SUCCESS;  
}

StatusCode 
TileRawChannelContByteStreamCnv::createRep(DataObject* pObj, IOpaqueAddress*& pAddr) 
{
  // convert TileRawChannels in the container into ByteStream
  MsgStream log(msgSvc(), "TileRawChannelContByteStreamCnv");
  log << MSG::DEBUG << " Executing createRep method" << endreq;

  StatusCode sc;

  // uint32_t runnum = re->header()->specific_part()->run_no(); 

  // get Full Event Assembler
  FullEventAssembler<TileHid2RESrcID> *fea = 0;
  std::string key("Tile");
  sc=m_ByteStreamEventAccess->getFullEventAssembler(fea,key);
  if (sc.isFailure()) {
    log << MSG::ERROR << "Cannot get full event assember with key \"Tile\" from ByteStreamEventAccess." << endreq;
    return sc;
  }

  // create TileRawChannelContainer
  TileRawChannelContainer* rccont(0) ; 
  StoreGateSvc::fromStorable(pObj, rccont ); 
  if(!rccont){
   log << MSG::ERROR << " Can not cast to TileRawChannelContainer " << endreq ; 
   return StatusCode::FAILURE;    
  } 

  std::string nm = pObj->registry()->name(); 

  ByteStreamAddress* addr = new
      ByteStreamAddress(classID(),nm,""); 

  pAddr = addr; 

  // call TileRawChannelContByteStreamTool
  sc=m_tool->convert(rccont,fea);
  if (sc.isFailure())
    return sc;

  return StatusCode::SUCCESS;
}

StatusCode
TileRawChannelContByteStreamCnv::finalize()
{
  MsgStream log(msgSvc(), "TileRCContBSCnv");
  log << MSG::DEBUG<<" Clearing Container " <<endreq; 

  // iterate over all collections in a container and clear them
  TileRawChannelContainer::const_iterator collItr=m_container->begin();
  TileRawChannelContainer::const_iterator lastColl=m_container->end();

  for(; collItr!=lastColl; ++collItr) {

    const TileRawChannelCollection * constColl = (*collItr);
    TileRawChannelCollection * coll = (TileRawChannelCollection *)constColl;
    coll->clear();
  }
  m_container->release(); 

  return Converter::finalize(); 

}


void TileRawChannelContByteStreamCnv::handle(const Incident& incident)
{
  if (incident.type() == "StoreCleared")
  {
    if (const StoreClearedIncident* inc =
        dynamic_cast<const StoreClearedIncident*> (&incident))
    {
      if (inc->store() == m_storeGate) {
        TileRawChannelContainer::const_iterator collItr=m_container->begin();
        TileRawChannelContainer::const_iterator lastColl=m_container->end();

        for(; collItr!=lastColl; ++collItr) {
          const TileRawChannelCollection * constColl = (*collItr);
          TileRawChannelCollection * coll = (TileRawChannelCollection *)constColl;
          coll->clear();
        }
      }
    }
  }
}
