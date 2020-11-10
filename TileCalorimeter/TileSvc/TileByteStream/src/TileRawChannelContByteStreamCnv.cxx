/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
  : AthConstConverter(storageType(), classID(), svcloc, "TileRawChannelContByteStreamCnv")
  , m_tool("TileRawChannelContByteStreamTool")
  , m_byteStreamEventAccess("ByteStreamCnvSvc", name())
  , m_byteStreamCnvSvc(0)
  , m_robSvc("ROBDataProviderSvc", name())
  , m_decoder("TileROD_Decoder")
  , m_hid2re(0)
{
}

const CLID& TileRawChannelContByteStreamCnv::classID() { return ClassID_traits<TileRawChannelContainer>::ID();}

long TileRawChannelContByteStreamCnv::storageType() { return ByteStreamAddress::storageType(); }

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

StatusCode TileRawChannelContByteStreamCnv::createObjConst(IOpaqueAddress* pAddr, DataObject*& pObj) const
{
  ATH_MSG_DEBUG(" Executing createObj method ");

  ByteStreamAddress *pRE_Addr;
  pRE_Addr = dynamic_cast<ByteStreamAddress*>(pAddr); 

  if(!pRE_Addr) {
    ATH_MSG_ERROR(" Can not cast to ByteStreamAddress ");
    return StatusCode::FAILURE;    
  }

  const std::string containerName(*(pRE_Addr->par()));
  bool isTMDB(containerName == std::string("MuRcvRawChCnt"));

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

  std::unordered_map<uint32_t,int> bsflags;
  uint32_t flag;

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
      flag = cont->get_bsflags();
      auto result = bsflags.insert(std::pair<uint32_t, int>(flag, 1));
      if (result.second == false) result.first->second++;
    } else {
      ATH_MSG_DEBUG( "ROB  for " << ((isTMDB)?"TMDB ":"") << "drawer 0x" << MSG::hex << collID << MSG::dec << " not found in BS" );
      uint32_t status = TileROD_Decoder::NO_ROB | TileROD_Decoder::CRC_ERR;
      rawChannelCollection->setFragGlobalCRC(status);
      ATH_MSG_DEBUG( "Status for " << ((isTMDB)?"TMDB ":"") << "drawer 0x" << MSG::hex << collID << " is 0x" << status << MSG::dec);
    }
  }

  if (bsflags.size() > 1) {
    int n=0;
    for (const auto & elem : bsflags) {
      if (elem.second > n) {
        n = elem.second;
        flag = elem.first;
      }
    }

    if (flag != cont->get_bsflags()) {

      uint32_t unit = ((flag & 0xC0000000) >> 30);
      if ((flag & 0x30000000) < 0x30000000) unit += TileRawChannelUnit::OnlineOffset; // Online units in real data

      ATH_MSG_DEBUG( "Changing units for " << ((isTMDB)?"TMDB ":"") << "RawChannelContainer from "
                     << cont->get_unit() << " to " << unit << MSG::hex
                     << " and BS flags from 0x" << cont->get_bsflags() << " to 0x" << flag << MSG::dec);

      cont->set_unit((TileRawChannelUnit::UNIT)unit);
      cont->set_bsflags(flag);
    }
  }

  ATH_MSG_DEBUG( "Creating raw channel container: " << containerName );

  TileRawChannelContainer* basecont = cont;
  pObj = SG::asStorable( basecont );


  return StatusCode::SUCCESS;  
}

StatusCode TileRawChannelContByteStreamCnv::createRepConst(DataObject* pObj, IOpaqueAddress*& pAddr) const
{
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

