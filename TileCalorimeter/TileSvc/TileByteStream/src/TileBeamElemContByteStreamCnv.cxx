/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// Gaudi includes
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IRegistry.h"

// Athena includes
#include "AthenaKernel/StorableConversions.h"
#include "AthenaKernel/errorcheck.h"

#include "ByteStreamCnvSvcBase/ByteStreamCnvSvcBase.h" 
#include "ByteStreamCnvSvcBase/ByteStreamAddress.h" 
#include "ByteStreamCnvSvcBase/ROBDataProviderSvc.h"
#include "ByteStreamData/RawEvent.h" 

#include "StoreGate/StoreGate.h"
#include "AthenaKernel/CLASS_DEF.h"


// Tile includes
#include "TileByteStream/TileBeamElemContByteStreamCnv.h"
#include "TileByteStream/TileROD_Decoder.h"
#include "TileByteStream/TileHid2RESrcID.h"
#include "TileIdentifier/TileTBFrag.h"
#include "TileEvent/TileBeamElemContainer.h"



TileBeamElemContByteStreamCnv::TileBeamElemContByteStreamCnv(ISvcLocator* svcloc)
  : Converter(ByteStream_StorageType, classID(), svcloc)
  , ::AthMessaging(msgSvc(), "TileBeamElemContByteStreamCnv")
  , m_name("TileBeamElemContByteStreamCnv")
  , m_robSvc("ROBDataProviderSvc", m_name)
  , m_decoder("TileROD_Decoder")
  , m_ROBID()
  , m_hid2re(nullptr)
{
}

const CLID& TileBeamElemContByteStreamCnv::classID() {return ClassID_traits<TileBeamElemContainer>::ID();}


StatusCode TileBeamElemContByteStreamCnv::initialize() {

  ATH_CHECK( Converter::initialize() );

  ATH_MSG_DEBUG( " initialize " );

  // retrieve Tool
  ATH_CHECK( m_decoder.retrieve() );
  m_hid2re = m_decoder->getHid2re();

  ATH_CHECK( m_robSvc.retrieve() );

  m_ROBID.clear();
  m_ROBID.push_back( m_hid2re->getRobFromFragID(DIGI_PAR_FRAG) );
  m_ROBID.push_back( m_hid2re->getRobFromFragID(LASER_OBJ_FRAG) );

  return StatusCode::SUCCESS;
}


StatusCode TileBeamElemContByteStreamCnv::createObj(IOpaqueAddress* pAddr, DataObject*& pObj) {

  ATH_MSG_DEBUG( " Executing createObj method" );

  ByteStreamAddress* pRE_Addr;
  pRE_Addr = dynamic_cast<ByteStreamAddress*>(pAddr); 
  if(!pRE_Addr) {
    ATH_MSG_ERROR( " Can not cast to ByteStreamAddress " );
    return StatusCode::FAILURE;    
  }

  std::vector<uint32_t> robid(1);
  robid[0] = 0;
  std::vector<const ROBDataProviderSvc::ROBF*> robf;

  // keep pointer to whole event and to CIS PAR frag internally
  m_event = m_robSvc->getEvent();
  m_robSvc->getROBData(m_ROBID, robf);
  m_robFrag = (robf.size() > 0 ) ? robf[0] : 0;

  TileMutableBeamElemContainer* cont = m_queue.get (true);
  ATH_CHECK( cont->status() );

  // iterate over all collections in a container and fill them
  for (IdentifierHash hash : cont->GetAllCurrentHashes()) {
    TileBeamElemCollection* beamCollection = cont->indexFindPtr (hash);
    beamCollection->clear();
    TileBeamElemCollection::ID collID = beamCollection->identify();  

    // find ROB
    uint32_t newrob = m_hid2re->getRobFromFragID(collID);
    if (newrob != robid[0]) {
      robid[0] = newrob;
      robf.clear();
      m_robSvc->getROBData(robid, robf);
    }
    
    // unpack ROB data
    if (robf.size() > 0 ) {
      m_decoder->fillCollection(robf[0], *beamCollection);
    }
  }

  ATH_MSG_DEBUG( " Creating Container " << *(pRE_Addr->par()) );

  TileBeamElemContainer* basecont = cont;
  pObj = SG::asStorable( basecont ) ; 
  return StatusCode::SUCCESS;  
}

StatusCode TileBeamElemContByteStreamCnv::createRep(DataObject* /* pObj */, IOpaqueAddress*& /* pAddr */) {
  // No conversion from TileBeamElem to BS 

  ATH_MSG_ERROR( " Can not create BS from TileBeamElem   " );

  return   StatusCode::FAILURE ; 
}

StatusCode TileBeamElemContByteStreamCnv::finalize()
{
  return Converter::finalize(); 
}
