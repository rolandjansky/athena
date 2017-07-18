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
#include "TileByteStream/TileL2ContByteStreamCnv.h"
#include "TileByteStream/TileL2ContByteStreamTool.h"
#include "TileByteStream/TileROD_Decoder.h"

#include "TileL2Algs/TileL2Builder.h"


#include <string> 


TileL2ContByteStreamCnv::TileL2ContByteStreamCnv(ISvcLocator* svcloc)
  : Converter(ByteStream_StorageType, classID(), svcloc)
  , ::AthMessaging(msgSvc(), "TileL2ContByteStreamCnv")
  , m_name("TileL2ContByteStreamCnv")
  , m_tool("TileL2ContByteStreamTool")
  , m_byteStreamEventAccess("ByteStreamCnvSvc", m_name)
  , m_byteStreamCnvSvc(0)
  , m_storeGate("StoreGateSvc", m_name)
  , m_robSvc("ROBDataProviderSvc", m_name)
  , m_decoder("TileROD_Decoder")
  , m_container(0)
{
}

const CLID& TileL2ContByteStreamCnv::classID(){ return ClassID_traits<TileL2Container>::ID(); }


StatusCode TileL2ContByteStreamCnv::initialize() {

  CHECK(Converter::initialize());

  ATH_MSG_DEBUG(" initialize ");

  // Get ByteStreamCnvSvc
  CHECK( m_byteStreamEventAccess.retrieve() );
  m_byteStreamCnvSvc = dynamic_cast<ByteStreamCnvSvc*>(&*m_byteStreamEventAccess);

  // retrieve Tool
  CHECK( m_decoder.retrieve() );

  CHECK( m_tool.retrieve() );

  CHECK( m_robSvc.retrieve() );
  
  // create empty TileL2Container and all TileL2 inside
  m_container = new TileL2Container(); 
  m_container->reserve(256);

  for(int i = 0; i < 256; ++i) {
    int collId = m_decoder->hashFunc()->identifier(i);
    TileL2 *l2 = new TileL2(collId);
    m_container->push_back(l2);
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


StatusCode TileL2ContByteStreamCnv::createObj(IOpaqueAddress* pAddr, DataObject*& pObj) {

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

  if (!m_decoder->convert(re, m_container).isSuccess()) {
    ATH_MSG_WARNING( "Conversion tool returned an error. TileL2Container might be empty." );
  }

  // new container will not own elements, i.e. TileL2 will not be deleted
  TileL2Container* new_container = new TileL2Container(*m_container);
  
  pObj = SG::asStorable( new_container ) ; 

  return StatusCode::SUCCESS;  
}

StatusCode TileL2ContByteStreamCnv::createRep(DataObject* pObj, IOpaqueAddress*& pAddr) {
  // convert TileL2s in the container into ByteStream

  ATH_MSG_DEBUG( " Executing createRep method" );

  // get Full Event Assembler
  FullEventAssembler<TileHid2RESrcID>* fea = 0;
  std::string key("Tile");
  
  CHECK( m_byteStreamCnvSvc->getFullEventAssembler(fea, key) );

  // create TileL2Container
  TileL2Container* l2cont(0);
  SG::fromStorable(pObj, l2cont);
  if(!l2cont){
    ATH_MSG_ERROR( " Can not cast to TileL2Container " );
    return StatusCode::FAILURE;
  }

  std::string name = pObj->registry()->name();
  ByteStreamAddress* addr = new ByteStreamAddress(classID(), name, "");

  pAddr = addr;

  // call TileL2ContByteStreamTool
  CHECK( m_tool->convert(l2cont, fea) );

  return StatusCode::SUCCESS;
}


StatusCode TileL2ContByteStreamCnv::finalize() {

  ATH_MSG_DEBUG( " Clearing TileL2 Container " );

  m_container->clear();

  delete m_container; 

  return Converter::finalize(); 
}

void TileL2ContByteStreamCnv::handle(const Incident& incident) {

  if (incident.type() == "StoreCleared") {
    if (const StoreClearedIncident* inc = dynamic_cast<const StoreClearedIncident*> (&incident)) {
      if (inc->store() == &*m_storeGate) {
        for (const TileL2* tileL2 : *m_container) {
          const_cast<TileL2*>(tileL2)->clear();
        }
      }
    }
  }

}
