/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Gaudi includes
#include "GaudiKernel/MsgStream.h"
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
#include "AthenaKernel/CLASS_DEF.h"

// Tile includes
#include "TileByteStream/TileL2ContByteStreamCnv.h"
#include "TileByteStream/TileL2ContByteStreamTool.h"
#include "TileByteStream/TileROD_Decoder.h"

#include "TileL2Algs/TileL2Builder.h"


#include <string>


TileRecyclableL2Container::TileRecyclableL2Container (const TileROD_Decoder& decoder)
{
  this->reserve(256);
  for(int i = 0; i < 256; ++i) {
    int collId = decoder.hashFunc()->identifier(i);
    this->push_back (std::make_unique<TileL2> (collId));
  }
}


/**
 * @brief Recycle this object for use in another event.
 *
 * This is called from AthenaKernel/RecyclableDataObject when this object
 * is released by StoreGate.  Unlock the object so that non-const access
 * is again possible, and clear out the contents if the collections.
 */
void TileRecyclableL2Container::recycle()
{
  for (TileL2* elt : *this) {
    elt->clear();
  }
}


TileL2ContByteStreamCnv::TileL2ContByteStreamCnv(ISvcLocator* svcloc)
  : Converter(storageType(), classID(), svcloc)
  , ::AthMessaging(msgSvc(), "TileL2ContByteStreamCnv")
  , m_name("TileL2ContByteStreamCnv")
  , m_tool("TileL2ContByteStreamTool")
  , m_byteStreamEventAccess("ByteStreamCnvSvc", m_name)
  , m_byteStreamCnvSvc(0)
  , m_storeGate("StoreGateSvc", m_name)
  , m_robSvc("ROBDataProviderSvc", m_name)
  , m_decoder("TileROD_Decoder")
{
}

const CLID& TileL2ContByteStreamCnv::classID(){ return ClassID_traits<TileL2Container>::ID(); }

long TileL2ContByteStreamCnv::storageType() { return ByteStreamAddress::storageType(); }

StatusCode TileL2ContByteStreamCnv::initialize() {

  ATH_CHECK(Converter::initialize());

  ATH_MSG_DEBUG(" initialize ");

  // Get ByteStreamCnvSvc
  ATH_CHECK( m_byteStreamEventAccess.retrieve() );
  m_byteStreamCnvSvc = dynamic_cast<ByteStreamCnvSvc*>(&*m_byteStreamEventAccess);

  // retrieve Tool
  ATH_CHECK( m_decoder.retrieve() );

  ATH_CHECK( m_tool.retrieve() );

  ATH_CHECK( m_robSvc.retrieve() );

  ATH_CHECK( m_storeGate.retrieve() );

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

  TileL2Container* cont = m_queue.get (*m_decoder);

  if (!m_decoder->convert(re, cont).isSuccess()) {
    ATH_MSG_WARNING( "Conversion tool returned an error. TileL2Container might be empty." );
  }

  pObj = SG::asStorable( cont ) ; 

  return StatusCode::SUCCESS;  
}

StatusCode TileL2ContByteStreamCnv::createRep(DataObject* pObj, IOpaqueAddress*& pAddr) {
  // convert TileL2s in the container into ByteStream

  ATH_MSG_DEBUG( " Executing createRep method" );

  // get Full Event Assembler
  FullEventAssembler<TileHid2RESrcID>* fea = 0;
  std::string key("Tile");
  
  ATH_CHECK( m_byteStreamCnvSvc->getFullEventAssembler(fea, key) );

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
  ATH_CHECK( m_tool->convert(l2cont, fea) );

  return StatusCode::SUCCESS;
}


StatusCode TileL2ContByteStreamCnv::finalize()
{
  return Converter::finalize(); 
}
