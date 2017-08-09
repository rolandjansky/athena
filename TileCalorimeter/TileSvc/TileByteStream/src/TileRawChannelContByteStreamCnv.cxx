/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Gaudi includes
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/ServiceHandle.h"

// Athena includes
#include "SGTools/StorableConversions.h"
#include "AthenaKernel/errorcheck.h"

#include "ByteStreamCnvSvc/ByteStreamCnvSvc.h"
#include "ByteStreamCnvSvcBase/ByteStreamCnvSvcBase.h" 
#include "ByteStreamCnvSvcBase/ByteStreamAddress.h" 
#include "ByteStreamCnvSvcBase/ROBDataProviderSvc.h"
#include "ByteStreamData/RawEvent.h" 

#include "StoreGate/ActiveStoreSvc.h"
#include "StoreGate/StoreClearedIncident.h"
#include "CLIDSvc/CLASS_DEF.h"

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
  , m_activeStore("ActiveStoreSvc", m_name)
  , m_robSvc("ROBDataProviderSvc", m_name)
  , m_decoder("TileROD_Decoder")
  , m_hid2re(0)
  , m_containers(2,0)

{
}

const CLID& TileRawChannelContByteStreamCnv::classID() { return ClassID_traits<TileRawChannelContainer>::ID();}

StatusCode TileRawChannelContByteStreamCnv::initialize() {

  ATH_MSG_DEBUG(" initialize ");

  CHECK(Converter::initialize());

  // Get ByteStreamCnvSvc
  CHECK( m_byteStreamEventAccess.retrieve() );
  m_byteStreamCnvSvc = dynamic_cast<ByteStreamCnvSvc*>(&*m_byteStreamEventAccess);

  // retrieve Tool
  CHECK( m_decoder.retrieve() );
  m_hid2re = m_decoder->getHid2re();

  CHECK( m_tool.retrieve() );

  CHECK( m_robSvc.retrieve() );

  TileFragHash::TYPE type;
  TileRawChannelUnit::UNIT unit;

  // create empty TileRawChannelContainer and all collections inside
  type = TileFragHash::OptFilterDsp;
  unit = TileRawChannelUnit::ADCcounts;
  m_containers[0] = new TileRawChannelContainer(true, type, unit); 
  m_containers[0]->addRef(); // make sure it's not deleted at the end of event
  m_decoder->PtrRChContainer( m_containers[0] );

  type = TileFragHash::MF;
  unit = TileRawChannelUnit::ADCcounts;
  m_containers[1] = new TileRawChannelContainer(true, type, unit);
  m_containers[1]->addRef(); // make sure it's not deleted at the end of event

  // Register incident handler
  ServiceHandle<IIncidentSvc> incSvc("IncidentSvc", m_name);
  if ( !incSvc.retrieve().isSuccess() ) {
    ATH_MSG_WARNING("Unable to retrieve the IncidentSvc");
  } else {
    incSvc->addListener(this, "StoreCleared");
  }

  CHECK( m_activeStore.retrieve() );

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

  uint32_t newrob = 0x0;

  for (int icnt = 0; icnt < 2; ++icnt) {

    bool isTMDB = (icnt == 1);
    
    std::vector<uint32_t> robid(1); 
    robid[0] = 0;
    std::vector<const ROBDataProviderSvc::ROBF*> robf;

    // iterate over all collections in a container and fill them
    for (const TileRawChannelCollection* constRawChCollection : *m_containers[icnt]) {
      TileRawChannelCollection* rawChannelCollection = const_cast<TileRawChannelCollection*>(constRawChCollection);

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
          m_decoder->fillCollection(robf[0], *rawChannelCollection);
        }
      } else {
        rawChannelCollection->setFragGlobalCRC(TileROD_Decoder::NO_ROB);
      }
    }

    ATH_MSG_DEBUG( "Creating Container " << *(pRE_Addr->par()) );  

    if (isTMDB) {
      CHECK( m_activeStore->activeStore()->record( m_containers[icnt], "MuRcvRawChCnt" ) );
    } else {
      pObj = SG::asStorable( m_containers[icnt] ) ;
    }
  }

  return StatusCode::SUCCESS;  
}

StatusCode TileRawChannelContByteStreamCnv::createRep(DataObject* pObj, IOpaqueAddress*& pAddr) {
  // convert TileRawChannels in the container into ByteStream

  ATH_MSG_DEBUG(" Executing createRep method ");

  // uint32_t runnum = re->header()->specific_part()->run_no(); 

  std::string key("Tile");
  
  // get Full Event Assembler 
  FullEventAssembler<TileHid2RESrcID> *fea = 0;
  CHECK( m_byteStreamCnvSvc->getFullEventAssembler(fea, key) );

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

  CHECK( m_tool->convert(rccont, fea) ); 

  return StatusCode::SUCCESS;
}

StatusCode TileRawChannelContByteStreamCnv::finalize() {

  ATH_MSG_DEBUG(" Clearing Container ");

  for (TileRawChannelContainer* rawChannelContainer : m_containers){
    for (const TileRawChannelCollection* rawChannelCollection : *rawChannelContainer) {
      const_cast<TileRawChannelCollection*>(rawChannelCollection)->clear();
    }
    
    rawChannelContainer->release(); 
  }

  return Converter::finalize(); 
}

void TileRawChannelContByteStreamCnv::handle(const Incident& incident) {

  if (incident.type() == "StoreCleared") {
    if (const StoreClearedIncident* inc = dynamic_cast<const StoreClearedIncident*> (&incident)) {
      if (inc->store() == m_activeStore->activeStore()) {
        for (TileRawChannelContainer* rawChannelContainer : m_containers){
          for (const TileRawChannelCollection* rawChannelCollection : *rawChannelContainer) {
            const_cast<TileRawChannelCollection*>(rawChannelCollection)->clear();
          }
        }
      }
    }
  }
}
