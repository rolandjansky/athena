//dear emacs, this is -*-c++-*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArOnOffMappingAlg.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "CoralBase/Blob.h"

#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include "LArIdentifier/LArOnlineID.h"
#include "LArIdentifier/LArOnline_SuperCellID.h"


LArOnOffMappingAlg::LArOnOffMappingAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_readKey("/LAr/Identifier/OnOnffMap"),
  m_writeKey("LArOnOffIdMap","LArOnOffIdMap"),
  m_condSvc("CondSvc",name),
  m_isSuperCell(false)
{
  declareProperty("ReadKey",m_readKey);
  declareProperty("WriteKey",m_writeKey);
  declareProperty("isSuperCell",m_isSuperCell,"switch to true to use the SuperCell Identfier helper");
}

LArOnOffMappingAlg::~LArOnOffMappingAlg() {}

StatusCode LArOnOffMappingAlg::initialize() {

  ATH_MSG_DEBUG("initializing");

  // CondSvc
  ATH_CHECK( m_condSvc.retrieve() );
  // Read Handles
  ATH_CHECK( m_readKey.initialize() );
  ATH_CHECK( m_writeKey.initialize() );
  // Register write handle
  if (m_condSvc->regHandle(this, m_writeKey).isFailure()) {
    ATH_MSG_ERROR("unable to register WriteCondHandle " << m_writeKey.fullKey() << " with CondSvc");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}


StatusCode LArOnOffMappingAlg::execute() {
    

  ATH_MSG_DEBUG("executing");

  SG::WriteCondHandle<LArOnOffIdMapping> writeHandle{m_writeKey};
  
  if (writeHandle.isValid()) {
    writeHandle.updateStore(); //????
    ATH_MSG_WARNING("Found valid write handle");
    return StatusCode::SUCCESS;
  }  

  SG::ReadCondHandle<AthenaAttributeList> readHandle{m_readKey};
  const AthenaAttributeList* attr{*readHandle};

  if (attr==nullptr) {
    ATH_MSG_ERROR("Failed to retrieve CondAttributeListCollection with key " << m_readKey.key());
    return StatusCode::FAILURE;
  }

  const LArOnlineID_Base* larOnlineID=nullptr;
  const CaloCell_Base_ID* calocellID=nullptr;

  //Identifier helper:
  if (m_isSuperCell) {
    //SG::ReadCondHandle<LArOnline_SuperCellID> larOnlineHdl{m_larSCOnlineIDKey}
    const LArOnline_SuperCellID* scidhelper;
    ATH_CHECK(detStore()->retrieve(scidhelper,"LArOnline_SuperCellID"));
    larOnlineID=scidhelper; //cast to base-class
    
    const CaloCell_SuperCell_ID* sccaloid;
    ATH_CHECK(detStore()->retrieve(sccaloid,"CaloCell_SuperCell_ID"));
    calocellID=sccaloid; //cast to base-class

  }
  else {//regular cells
    //SG::ReadCondHandle<LArOnlineID> larOnlineHdl{m_larOnlineIDKey};
    const LArOnlineID* idhelper;
    ATH_CHECK(detStore()->retrieve(idhelper,"LArOnlineID"));
    larOnlineID=idhelper; //cast to base-class

    const CaloCell_ID* sccaloid;
    ATH_CHECK(detStore()->retrieve(sccaloid,"CaloCell_ID"));
    calocellID=sccaloid; //cast to base-class

  }


  std::unique_ptr<LArOnOffIdMapping> onOffMap=std::make_unique<LArOnOffIdMapping>(larOnlineID,calocellID);

  const coral::Blob& blobOnOff=(*attr)["OnlineHashToOfflineId"].data<coral::Blob>();
  unsigned nChan=blobOnOff.size()/sizeof(uint32_t);
  const uint32_t* pBlobOnOff=static_cast<const uint32_t*>(blobOnOff.startingAddress());  

  if (nChan!=larOnlineID->channelHashMax()) {
    ATH_MSG_ERROR("Number of channels read from DB (" << nChan << ") does not match online hash max ("
		  <<larOnlineID->channelHashMax() << ")");
    return StatusCode::FAILURE;
  }


   unsigned nConnected=0;

  for (unsigned i=0;i<nChan;++i) {
    const Identifier id=Identifier(Identifier32(pBlobOnOff[i]));
    //std::cout << "id[" << i <<"] " << std::hex << id.get_compact() << std::dec << std::endl;
    if (id.is_valid()) {
      const IdentifierHash oflHash=calocellID->calo_cell_hash(id);
      const HWIdentifier hwid=larOnlineID->channel_Id(IdentifierHash(i));
      ++nConnected;
      onOffMap->m_onlHashToOffline[i]=id;
      onOffMap->m_oflHashToOnline[oflHash]=hwid;
    }
  }//end loop over channels

  ATH_MSG_INFO("Done reading online/offline identifier mapping");
  ATH_MSG_INFO("Found " << nChan << " online identifier and " << nConnected << " offline identifier. "
	       << nChan-nConnected << " disconnected channels.");

  
  // Define validity of the output cond object and record it
  EventIDRange rangeW;
  if(!readHandle.range(rangeW)) {
    ATH_MSG_ERROR("Failed to retrieve validity range for " << readHandle.key());
    return StatusCode::FAILURE;
  }

  if(writeHandle.record(rangeW,onOffMap.release()).isFailure()) {
    ATH_MSG_ERROR("Could not record LArOnOffMapping object with " 
		  << writeHandle.key() 
		  << " with EventRange " << rangeW
		  << " into Conditions Store");
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("recorded new " << writeHandle.key() << " with range " << rangeW << " into Conditions Store");

 
  return StatusCode::SUCCESS;
}

