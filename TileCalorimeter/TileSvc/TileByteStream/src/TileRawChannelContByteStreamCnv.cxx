/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Gaudi includes
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ServiceHandle.h"

// Athena includes
#include "AthenaKernel/StorableConversions.h"
#include "AthenaKernel/errorcheck.h"

#include "ByteStreamCnvSvc/ByteStreamCnvSvc.h"
#include "ByteStreamCnvSvcBase/ByteStreamCnvSvcBase.h" 
#include "ByteStreamCnvSvcBase/ByteStreamAddress.h" 
#include "ByteStreamCnvSvcBase/ROBDataProviderSvc.h"
#include "ByteStreamData/RawEvent.h" 

#include "StoreGate/StoreClearedIncident.h"
#include "AthenaKernel/CLASS_DEF.h"

// Tile includes
#include "TileByteStream/TileRawChannelContByteStreamCnv.h"
#include "TileByteStream/TileRawChannelContByteStreamTool.h"
#include "TileByteStream/TileROD_Decoder.h"
#include "TileByteStream/TileHid2RESrcID.h"
#include "TileEvent/TileRawChannelContainer.h"


#include <string> 
#include <stdint.h>

TileRawChannelContByteStreamCnv::TileRawChannelContByteStreamCnv(ISvcLocator* svcloc) 
  : Converter(ByteStream_StorageType, classID(), svcloc)
  , ::AthMessaging(msgSvc(), "TileRawChannelContByteStreamCnv")
  , m_name("TileRawChannelContByteStreamCnv")
  , m_tool("TileRawChannelContByteStreamTool")
  , m_byteStreamEventAccess("ByteStreamCnvSvc", m_name)
  , m_byteStreamCnvSvc(0)
  , m_robSvc("ROBDataProviderSvc", m_name)
  , m_decoder("TileROD_Decoder")
  , m_hid2re(0)
{
}

const CLID& TileRawChannelContByteStreamCnv::classID() { return ClassID_traits<TileRawChannelContainer>::ID();}

StatusCode TileRawChannelContByteStreamCnv::initialize() {

  ATH_MSG_DEBUG(" initialize ");

  ATH_CHECK(Converter::initialize());

  // Get ByteStreamCnvSvc
  ATH_CHECK( m_byteStreamEventAccess.retrieve() );
  m_byteStreamCnvSvc = dynamic_cast<ByteStreamCnvSvc*>(&*m_byteStreamEventAccess);

  // retrieve Tool
  ATH_CHECK( m_decoder.retrieve() );
  m_hid2re = m_decoder->getHid2re();

  ATH_CHECK( m_tool.retrieve() );

  ATH_CHECK( m_robSvc.retrieve() );

  return StatusCode::SUCCESS;
}

StatusCode TileRawChannelContByteStreamCnv::createObj(IOpaqueAddress* pAddr, DataObject*& pObj) {

  ATH_MSG_DEBUG(" Executing createObj method ");

  ByteStreamAddress *pRE_Addr;
  pRE_Addr = dynamic_cast<ByteStreamAddress*>(pAddr); 

  if(!pRE_Addr) {
    ATH_MSG_ERROR(" Can not cast to ByteStreamAddress ");
    return StatusCode::FAILURE;    
  }

  const std::string containerName(*(pRE_Addr->par()));
  bool isTMDB(containerName == "MuRcvRawChCnt");

  uint32_t newrob = 0x0;

    
  std::vector<uint32_t> robid(1);
  robid[0] = 0;
  std::vector<const ROBDataProviderSvc::ROBF*> robf;

  TileMutableRawChannelContainer* cont = m_queue.get (true);
  ATH_CHECK( cont->status() );

  if (isTMDB) {
    cont->set_type (TileFragHash::MF);
  } else {
    cont->set_type (TileFragHash::OptFilterDsp);
  }

  // iterate over all collections in a container and fill them
  for (IdentifierHash hash : cont->GetAllCurrentHashes()) {
    TileRawChannelCollection* rawChannelCollection = cont->indexFindPtr (hash);
    rawChannelCollection->clear();
    TileRawChannelCollection::ID collID = rawChannelCollection->identify();

    // find ROB
    if (isTMDB) {
      newrob = m_hid2re->getRobFromTileMuRcvFragID(collID);
    } else {
      newrob = m_hid2re->getRobFromFragID(collID);
    }

    if (newrob != robid[0]) {
      robid[0] = newrob;
      robf.clear();
      m_robSvc->getROBData(robid, robf);
    }
    
    // unpack ROB data
    if (robf.size() > 0 ) {
      if (isTMDB) {// reid for TMDB 0x5x010x
        m_decoder->fillCollection_TileMuRcv_RawChannel(robf[0], *rawChannelCollection);
      } else {
        m_decoder->fillCollection(robf[0], *rawChannelCollection, cont);
      }
    } else {
      rawChannelCollection->setFragGlobalCRC(TileROD_Decoder::NO_ROB);
    }
  }

  ATH_MSG_DEBUG( "Creating raw channel container: " << containerName );

  TileRawChannelContainer* basecont = cont;
  pObj = SG::asStorable( basecont );


  return StatusCode::SUCCESS;  
}

StatusCode TileRawChannelContByteStreamCnv::createRep(DataObject* pObj, IOpaqueAddress*& pAddr) {
  // convert TileRawChannels in the container into ByteStream

  ATH_MSG_DEBUG(" Executing createRep method ");

  // uint32_t runnum = re->header()->specific_part()->run_no(); 

  std::string key("Tile");
  
  // get Full Event Assembler 
  FullEventAssembler<TileHid2RESrcID> *fea = 0;
  ATH_CHECK( m_byteStreamCnvSvc->getFullEventAssembler(fea, key) );

  // create TileRawChannelContainer
  TileRawChannelContainer* rccont(0) ; 
  SG::fromStorable(pObj, rccont ); 
  if(!rccont){
    ATH_MSG_ERROR(" Can not cast to TileRawChannelContainer ");
    return StatusCode::FAILURE;    
  } 

  std::string name = pObj->registry()->name(); 
  ByteStreamAddress* addr = new ByteStreamAddress(classID(), name, ""); 
  pAddr = addr; 

  ATH_CHECK( m_tool->convert(rccont, fea) );

  return StatusCode::SUCCESS;
}

StatusCode TileRawChannelContByteStreamCnv::finalize()
{
  return Converter::finalize(); 
}

