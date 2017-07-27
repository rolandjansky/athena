/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Gaudi includes
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IToolSvc.h"

// Athena includes
#include "AthenaKernel/errorcheck.h"

#include "ByteStreamCnvSvc/ByteStreamCnvSvc.h"
#include "ByteStreamCnvSvcBase/ByteStreamCnvSvcBase.h"
#include "ByteStreamCnvSvcBase/ByteStreamAddress.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "ByteStreamData/RawEvent.h"

#include "StoreGate/StoreGate.h"
#include "StoreGate/StoreClearedIncident.h"
#include "CLIDSvc/CLASS_DEF.h"

// Tile includes
#include "TileByteStream/TileMuRcvContByteStreamCnv.h"
#include "TileByteStream/TileMuRcvContByteStreamTool.h"
#include "TileByteStream/TileROD_Decoder.h"


#include <string> 


TileMuRcvContByteStreamCnv::TileMuRcvContByteStreamCnv(ISvcLocator* svcloc)
  : Converter(ByteStream_StorageType, classID(),svcloc)
  , ::AthMessaging(msgSvc(), "TileMuRcvContByteStreamCnv")
  , m_name("TileMuRcvContByteStreamCnv")
  , m_tool("TileMuRcvContByteStreamTool")
  , m_byteStreamEventAccess("ByteStreamCnvSvc", m_name)
  , m_byteStreamCnvSvc(0)
  , m_storeGate("StoreGateSvc", m_name)
  , m_robSvc("ROBDataProviderSvc", m_name)
  , m_decoder("TileROD_Decoder")
  , m_container(0)
{
}

const CLID& TileMuRcvContByteStreamCnv::classID(){
return ClassID_traits<TileMuonReceiverContainer>::ID() ;
}

StatusCode TileMuRcvContByteStreamCnv::initialize()
{
  CHECK(Converter::initialize());

  ATH_MSG_DEBUG(" initialize ");

  // Get ByteStreamCnvSvc
  CHECK( m_byteStreamEventAccess.retrieve() );
  m_byteStreamCnvSvc = dynamic_cast<ByteStreamCnvSvc*>(&*m_byteStreamEventAccess);

  // retrieve Tool
  CHECK( m_decoder.retrieve() );

  CHECK( m_tool.retrieve() );

  CHECK( m_robSvc.retrieve() );

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
  ServiceHandle<IIncidentSvc> incSvc("IncidentSvc", m_name);
  if ( !incSvc.retrieve().isSuccess() ) {
    ATH_MSG_WARNING( "Unable to retrieve the IncidentSvc" );
  } else {
    incSvc->addListener(this, "StoreCleared");
  }

  CHECK( m_storeGate.retrieve() );

  return StatusCode::SUCCESS ;
}


StatusCode TileMuRcvContByteStreamCnv::createObj(IOpaqueAddress* pAddr, DataObject*& pObj) 
{

  ATH_MSG_DEBUG( " Executing createObj method" );

  ByteStreamAddress* pRE_Addr;
  pRE_Addr = dynamic_cast<ByteStreamAddress*>(pAddr);
  if(!pRE_Addr) {
    ATH_MSG_ERROR( " Can not cast to ByteStreamAddress " );
    return StatusCode::FAILURE;
  }

  const RawEvent* re = m_robSvc->getEvent();
  if (!re) {
    ATH_MSG_ERROR( "Could not get raw event from ByteStreamInputSvc" );
    return StatusCode::FAILURE;
  }

  StatusCode sc=m_decoder->convertTMDBDecision(re,m_container);
  if (sc!=StatusCode::SUCCESS) {
    ATH_MSG_WARNING( "Conversion tool returned an error. TileMuonReceiverContainer might be empty." );
  }

  // new container will not own elements, i.e. TileMuonReceiverContainer will not be deleted
  TileMuonReceiverContainer * new_container = new TileMuonReceiverContainer(*m_container);

  pObj = SG::asStorable( new_container );

  return StatusCode::SUCCESS;
}

StatusCode TileMuRcvContByteStreamCnv::createRep(DataObject* pObj, IOpaqueAddress*& pAddr)
{
  ATH_MSG_DEBUG( " Executing createRep method" );

  // get Full Event Assembler
  FullEventAssembler<TileHid2RESrcID>* fea = 0;
  std::string key("Tile");

  CHECK( m_byteStreamCnvSvc->getFullEventAssembler(fea, key) );

  // create TileMuonReceiverContainer
  //
  TileMuonReceiverContainer* muRcvCont(0);
  SG::fromStorable(pObj, muRcvCont);
  if(!muRcvCont){
    ATH_MSG_ERROR( " Can not cast to TileMuonReceiverContainer " );
    return StatusCode::FAILURE;
  }

  std::string name = pObj->registry()->name();
  ByteStreamAddress* addr = new ByteStreamAddress(classID(), name, "");

  pAddr = addr;

  // call TileMuRcvContByteStreamTool
  CHECK( m_tool->convert(muRcvCont,fea) );

  return StatusCode::SUCCESS;
}

StatusCode TileMuRcvContByteStreamCnv::finalize()
{
  ATH_MSG_DEBUG( " Clearing TileMuonReceiverContainer Container " );

  m_container->clear();

  delete m_container;

  return Converter::finalize();
}

void TileMuRcvContByteStreamCnv::handle(const Incident& incident)
{
  if (incident.type() == "StoreCleared") {
    if (const StoreClearedIncident* inc = dynamic_cast<const StoreClearedIncident*> (&incident)) {
      if (inc->store() == &*m_storeGate) {
        for (const TileMuonReceiverObj* tileMuObj : *m_container) {
          const_cast<TileMuonReceiverObj*>(tileMuObj)->clear();
        }
      }
    }
  }
  return;
}
