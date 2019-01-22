/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// Gaudi includes
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IToolSvc.h"


// Athena includes 
#include "AthenaKernel/StorableConversions.h"
#include "AthenaKernel/errorcheck.h"

#include "ByteStreamCnvSvc/ByteStreamCnvSvc.h"
#include "ByteStreamCnvSvcBase/ByteStreamCnvSvcBase.h" 
#include "ByteStreamCnvSvcBase/ByteStreamAddress.h" 
#include "ByteStreamCnvSvcBase/ROBDataProviderSvc.h"
#include "ByteStreamData/RawEvent.h" 

#include "StoreGate/ActiveStoreSvc.h"
#include "StoreGate/StoreClearedIncident.h"
#include "AthenaKernel/CLASS_DEF.h"


// Tile includes
#include "TileByteStream/TileDigitsContByteStreamCnv.h"
#include "TileByteStream/TileDigitsContByteStreamTool.h"
#include "TileByteStream/TileROD_Decoder.h"
#include "TileByteStream/TileHid2RESrcID.h"
#include "TileEvent/TileDigitsContainer.h"
#include "TileByteStream/TileROD_Decoder.h"

#include <vector> 
#include <string> 
#include <stdint.h>


TileDigitsContByteStreamCnv::TileDigitsContByteStreamCnv(ISvcLocator* svcloc) 
  : Converter(ByteStream_StorageType, classID(), svcloc)
  , ::AthMessaging(msgSvc(), "TileDigitsContByteStreamCnv")
  , m_name("TileDigitsContByteStreamCnv")
  , m_tool("TileDigitsContByteStreamTool")
  , m_byteStreamEventAccess("ByteStreamCnvSvc", m_name)
  , m_byteStreamCnvSvc(0)
  , m_activeStore("ActiveStoreSvc", m_name)
  , m_robSvc("ROBDataProviderSvc", m_name)
  , m_decoder("TileROD_Decoder")
  , m_hid2re(0)
{
}

const CLID& TileDigitsContByteStreamCnv::classID(){ return ClassID_traits<TileDigitsContainer>::ID();}


StatusCode TileDigitsContByteStreamCnv::initialize() {
  
  ATH_CHECK(Converter::initialize());

  ATH_MSG_DEBUG(" initialize ");

  // Get ByteStreamCnvSvc
  ATH_CHECK( m_byteStreamEventAccess.retrieve() );
  m_byteStreamCnvSvc = dynamic_cast<ByteStreamCnvSvc*>(&*m_byteStreamEventAccess);

  // retrieve Tool
  ATH_CHECK( m_decoder.retrieve() );
  m_hid2re = m_decoder->getHid2re();

  ATH_CHECK( m_tool.retrieve() );

  ATH_CHECK( m_robSvc.retrieve() );

  ATH_CHECK( m_activeStore.retrieve() );
  
  return StatusCode::SUCCESS;
}

StatusCode TileDigitsContByteStreamCnv::createObj(IOpaqueAddress* pAddr, DataObject*& pObj) {

  ATH_MSG_DEBUG(" Executing createObj method ");

  ByteStreamAddress *pRE_Addr;
  pRE_Addr = dynamic_cast<ByteStreamAddress*>(pAddr); 
  if(!pRE_Addr) {
    ATH_MSG_ERROR(" Can not cast to ByteStreamAddress ");
    return StatusCode::FAILURE;    
  }

  uint32_t newrob = 0x0;

  for (int icnt = 0; icnt < 2; ++icnt) {

    bool isTMDB(icnt == 1);

    std::vector<uint32_t> robid(1); 
    robid[0] = 0;
    std::vector<const ROBDataProviderSvc::ROBF*> robf;

    TileMutableDigitsContainer* cont = m_queue.get (true);
    ATH_CHECK( cont->status() );
    
    // iterate over all collections in a container and fill them
    //
    for (IdentifierHash hash : cont->GetAllCurrentHashes()) {
      TileDigitsCollection* digitsCollection = cont->indexFindPtr (hash);
      digitsCollection->clear();
      TileDigitsCollection::ID collID = digitsCollection->identify();  
      
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
      
      if (robf.size() > 0 ) {
        if (isTMDB) {// reid for TMDB 0x5x010x
          ATH_MSG_DEBUG(" Decoding TMDB digits in ROD fragment ");
          m_decoder->fillCollection_TileMuRcv_Digi(robf[0], *digitsCollection);
        } else {
          m_decoder->fillCollection(robf[0], *digitsCollection);
        }
      } else {
        digitsCollection->setFragBCID((TileROD_Decoder::NO_ROB)<<16);
      }
    }

    ATH_MSG_DEBUG( "Creating digits container " << *(pRE_Addr->par()) );

    TileDigitsContainer* basecont = cont;
    if (isTMDB) {
      ATH_CHECK( m_activeStore->activeStore()->record( basecont, "MuRcvDigitsCnt" ) );
    } else {
      pObj = SG::asStorable( basecont ) ;
    }
  }


  return StatusCode::SUCCESS;  
}


StatusCode TileDigitsContByteStreamCnv::createRep(DataObject* pObj, IOpaqueAddress*& pAddr) {
  // convert TileDigitsContainer in the container into ByteStream

  ATH_MSG_DEBUG(" Executing createRep method ");

  // uint32_t runnum = re->header()->specific_part()->run_no(); 

  // get Full Event Assembler
  FullEventAssembler<TileHid2RESrcID>* fea = 0;
  std::string key("Tile");
  ATH_CHECK( m_byteStreamCnvSvc->getFullEventAssembler(fea, key) );

  // create TileDigitsContainer
  TileDigitsContainer* digicont(0) ; 
  SG::fromStorable(pObj, digicont ); 

  if(!digicont){
    ATH_MSG_ERROR(" Can not cast to TileDigitsContainer ");
   return StatusCode::FAILURE;    
  } 

  std::string name = pObj->registry()->name(); 

  ByteStreamAddress* addr = new ByteStreamAddress(classID(), name, ""); 

  pAddr = addr; 

  // call TileDigitsContByteStreamTool

  return m_tool->convert(digicont, fea);
}

StatusCode TileDigitsContByteStreamCnv::finalize()
{
  return Converter::finalize(); 
}
