/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TileByteStream/TileMuRcvContByteStreamCnv.h"
#include "TileByteStream/TileMuRcvContByteStreamTool.h"
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

#include "TileEvent/TileMuonReceiverObj.h"
#include "TileEvent/TileContainer.h"

#include "StoreGate/StoreGate.h"
#include "StoreGate/StoreClearedIncident.h"
#include "CLIDSvc/CLASS_DEF.h"

#include <string> 

TileMuRcvContByteStreamCnv::TileMuRcvContByteStreamCnv(ISvcLocator* svcloc) :
    Converter(ByteStream_StorageType, classID(),svcloc),
    m_tool(0),
    m_decoder(0),
    m_ByteStreamEventAccess(0),
    m_container(0),
    m_storeGate(0),
    m_rdpSvc(0)
{
}

const CLID& TileMuRcvContByteStreamCnv::classID(){
return ClassID_traits<TileMuonReceiverContainer>::ID() ;
}

StatusCode TileMuRcvContByteStreamCnv::initialize()
{
  StatusCode sc = Converter::initialize(); 

  if (sc.isFailure()) {
    return sc; 
  } 

  MsgStream log(msgSvc(), "TileMuRcvContByteStreamCnv");

  log << MSG::DEBUG << " initialize() " << endmsg; 

  // Get ByteStreamInputSvc
  // 
  IService* svc;
  if(StatusCode::SUCCESS != serviceLocator()->getService("ROBDataProviderSvc",svc)){
    log << MSG::WARNING << " Can't get ByteStreamInputSvc interface Reading of ByteStream Data not possible. " << endmsg;
    m_rdpSvc=0;
  }
  else {
    m_rdpSvc=dynamic_cast<IROBDataProviderSvc*>(svc);
    if(m_rdpSvc == 0 ) {
      log<<MSG::ERROR<< " Can't cast to  ByteStreamInputSvc " <<endmsg; 
      return StatusCode::FAILURE;
    }
  }

  // Get ByteStreamCnvSvc  [svc]
  //
  if(StatusCode::SUCCESS != serviceLocator()->getService("ByteStreamCnvSvc",svc)){
    log << MSG::ERROR << " Can't get ByteStreamEventAccess interface " << endmsg;
    return StatusCode::FAILURE;
  }
  m_ByteStreamEventAccess=dynamic_cast<ByteStreamCnvSvc*>(svc);
  if (m_ByteStreamEventAccess==NULL)
    {
      log << MSG::ERROR << "  TileMuRcvContByteStreamCnv: Can't cast to  ByteStreamCnvSvc " << endmsg; 
      return StatusCode::FAILURE ;
    }

  // retrieve Tools        [m_tool,m_decoder]
  //  
  IToolSvc* toolSvc;

  if(StatusCode::SUCCESS != service("ToolSvc",toolSvc)){
    log << MSG::ERROR << " Can't get ToolSvc " << endmsg;
    return StatusCode::FAILURE;
  }

  if(StatusCode::SUCCESS != toolSvc->retrieveTool("TileMuRcvContByteStreamTool",m_tool)) {
    log << MSG::ERROR << " Can't get ByteStreamTool " << endmsg;
    return StatusCode::FAILURE;
  }

  if(StatusCode::SUCCESS != toolSvc->retrieveTool("TileROD_Decoder",m_decoder)) {
    log << MSG::ERROR << "Can't get TileROD_Decoder" << endmsg;
    return StatusCode::FAILURE; 
  }

  // per event there is only one object per EB module 128
  //
  int cntsize = 128;
  m_container = new TileMuonReceiverContainer(); 
  m_container->reserve(cntsize);

  for( int i=0; i<cntsize; i++) {
    int objId = m_decoder->hashFunc()->identifier(i);
    TileMuonReceiverObj *muRcv = new TileMuonReceiverObj(objId);
    m_container->push_back(muRcv);
  }

  // Register incident handler
  //
  ServiceHandle<IIncidentSvc> incSvc( "IncidentSvc", "TileMuRcvContByteStreamCnv");
  if ( !incSvc.retrieve().isSuccess() ) {
    log << MSG::WARNING << "Unable to retrieve the IncidentSvc" << endmsg;
  }
  else {
    incSvc->addListener(this,"StoreCleared");
  }

  log << MSG::DEBUG << " End of initialize() " << endmsg;

  return service("StoreGateSvc",m_storeGate) ;
}


StatusCode TileMuRcvContByteStreamCnv::createObj(IOpaqueAddress* pAddr, DataObject*& pObj) 
{

  MsgStream log(msgSvc(), "TileMuRcvContByteStreamCnv");

  log << MSG::DEBUG << " Executing createObj method" << endmsg;

  if (!m_rdpSvc) {
    log << MSG::ERROR << " ROBDataProviderSvc not loaded. Can't read ByteStream." << endmsg;
    return StatusCode::FAILURE;
  }

  ByteStreamAddress *pRE_Addr;
  pRE_Addr = dynamic_cast<ByteStreamAddress*>(pAddr);
  if(!pRE_Addr) {
    log << MSG::ERROR << " Can't cast to ByteStreamAddress " << endmsg ;
    return StatusCode::FAILURE;
  }

  const RawEvent* re = m_rdpSvc->getEvent();
  if (!re) {
    log << MSG::ERROR << "Could not get raw event from ByteStreamInputSvc" << endmsg;
    return StatusCode::FAILURE;
  }

  StatusCode sc=m_decoder->convertTMDBDecision(re,m_container);
  if (sc!=StatusCode::SUCCESS) {
    log << MSG::WARNING << "Conversion tool returned an error. TileMuonReceiverContainer might be empty." << endmsg;
  }

  // new container will not own elements, i.e. TileMuonReceiverContainer will not be deleted
  TileMuonReceiverContainer * new_container = new TileMuonReceiverContainer(*m_container);
  pObj = StoreGateSvc::asStorable( new_container );

  return StatusCode::SUCCESS;
}

StatusCode TileMuRcvContByteStreamCnv::createRep(DataObject* pObj, IOpaqueAddress*& pAddr)
{
  // convert TileMuonsReceiverObj's in the container into ByteStream
  //
  MsgStream log(msgSvc(), "TileMuRcvContByteStreamCnv");
  log << MSG::DEBUG << " Executing createRep method " << endmsg;

  StatusCode sc;

  // get Full Event Assembler
  //
  FullEventAssembler<TileHid2RESrcID> *fea = 0;
  std::string key("Tile");
  sc=m_ByteStreamEventAccess->getFullEventAssembler(fea,key);

  if (sc.isFailure()) {
    log << MSG::ERROR << "Can't get full event assember with key \"Tile\" from ByteStreamEventAccess." << endmsg;
    return sc;
  }

  // create TileMuonReceiverContainer
  //
  TileMuonReceiverContainer* muRcvCont(0);
  StoreGateSvc::fromStorable(pObj, muRcvCont );

  if(!muRcvCont){
    log << MSG::ERROR << " Can't cast to TileMuonReceiverContainer " << endmsg;
    return StatusCode::FAILURE;
  }

  std::string          nm = pObj->registry()->name();

  ByteStreamAddress* addr = new ByteStreamAddress(classID(),nm,"");

  pAddr                   = addr;

  // call TileMuRcvContByteStreamTool
  //
  sc = m_tool->convert(muRcvCont,fea);
  if (sc.isFailure()) return sc;

  return StatusCode::SUCCESS;
}

StatusCode TileMuRcvContByteStreamCnv::finalize()
{
  MsgStream log(msgSvc(), "TileMuRcvContByteStreamCnv");
  log << MSG::DEBUG << " Clearing TileMuonReceiverContainer " << endmsg; 

  m_container->clear();

  delete m_container; 

  return Converter::finalize(); 
}

void TileMuRcvContByteStreamCnv::handle(const Incident& incident)
{
  if (incident.type() == "StoreCleared")
  {
    if (const StoreClearedIncident* inc =
        dynamic_cast<const StoreClearedIncident*> (&incident))
    {
      if (inc->store() == m_storeGate) {
        TileMuonReceiverContainer::const_iterator collItr=m_container->begin();
        TileMuonReceiverContainer::const_iterator lastColl=m_container->end();

        for(; collItr!=lastColl; ++collItr) {
          const TileMuonReceiverObj * constColl = (*collItr);
          TileMuonReceiverObj * coll = (TileMuonReceiverObj *)constColl;
          coll->clear();
        }
      }
    }
  }
  return;
}
