/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//author Renato Febbraro
//renato.febbraro@cern.ch
//date February 2008

// Gaudi includes
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/IToolSvc.h"

// Athena includes
#include "SGTools/StorableConversions.h"
#include "AthenaKernel/errorcheck.h"

#include "ByteStreamCnvSvc/ByteStreamCnvSvc.h"
#include "ByteStreamCnvSvcBase/ByteStreamCnvSvcBase.h" 
#include "ByteStreamCnvSvcBase/ByteStreamAddress.h" 
#include "ByteStreamCnvSvcBase/ROBDataProviderSvc.h" 
#include "ByteStreamData/RawEvent.h" 

#include "CLIDSvc/CLASS_DEF.h"

// Tile includes
#include "TileByteStream/TileLaserObjByteStreamCnv.h"
#include "TileByteStream/TileLaserObjByteStreamTool.h"
#include "TileByteStream/TileROD_Decoder.h"
#include "TileByteStream/TileHid2RESrcID.h"
#include "TileIdentifier/TileTBFrag.h"
#include "TileEvent/TileLaserObject.h"
#include "TileByteStream/TileROD_Decoder.h"



TileLaserObjByteStreamCnv::TileLaserObjByteStreamCnv(ISvcLocator* svcloc)
  : Converter(ByteStream_StorageType, classID(), svcloc)
  , ::AthMessaging(msgSvc(), "TileLaserObjByteStreamCnv")
  , m_name("TileLaserObjByteStreamCnv")
  , m_robSvc("ROBDataProviderSvc", m_name)
  , m_decoder("TileROD_Decoder")
  , m_robFrag(0)
  , m_ROBID()
  , m_hid2re(0)
  , m_container(0)
{
}

const CLID& TileLaserObjByteStreamCnv::classID(){ return ClassID_traits<TileLaserObject>::ID();}


StatusCode TileLaserObjByteStreamCnv::initialize() {

  CHECK(Converter::initialize());

  ATH_MSG_DEBUG(" initialize ");

  CHECK( m_robSvc.retrieve() );

  // retrieve Tool
  CHECK( m_decoder.retrieve() );

  m_hid2re = m_decoder->getHid2re();

  m_ROBID.clear();
  // m_ROBID.push_back( 0x500000 );
  // m_ROBID.push_back( 0x520010 );
  m_ROBID.push_back( m_hid2re->getRobFromFragID(LASER_OBJ_FRAG) );

  return StatusCode::SUCCESS ;
}


StatusCode TileLaserObjByteStreamCnv::createObj(IOpaqueAddress* pAddr, DataObject*& pObj) {

  ATH_MSG_DEBUG( " Executing createObj method" );

  ByteStreamAddress *pRE_Addr;
  pRE_Addr = dynamic_cast<ByteStreamAddress*>(pAddr); 
  if(!pRE_Addr) {
    ATH_MSG_ERROR( " Can not cast to ByteStreamAddress " );
    return StatusCode::FAILURE;    
  }

  /*FIND ROB*/
  std::vector<const ROBDataProviderSvc::ROBF*> robf;
  m_robSvc->getROBData(m_ROBID, robf);

  // create TileLaserObject
  m_container = new TileLaserObject() ; 

  if (robf.size() > 0 ) {
    m_robFrag = robf[0];
    m_decoder->fillTileLaserObj(m_robFrag, *m_container);
  } else {
    ATH_MSG_DEBUG( " No LASTROD fragment in BS, TileLaserObject will be empty." );
    m_robFrag = 0;
  }

  pObj = SG::asStorable( m_container ) ; 

  return StatusCode::SUCCESS;  
}

StatusCode TileLaserObjByteStreamCnv::createRep(DataObject* /* pObj */, IOpaqueAddress*& /* pAddr */) {
  // No conversion from TileLaserObj to BS 

  ATH_MSG_ERROR( " Can not create BS from TileLaserObject " );

  return   StatusCode::FAILURE ; 
}

